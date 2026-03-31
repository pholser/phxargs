#include "space_tokenizer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "util.h"

typedef enum {
  NO_TOKEN,
  NO_TOKEN_ESCAPE,
  IN_TOKEN,
  IN_QUOTED_TOKEN,
  IN_TOKEN_QUOTED,
  IN_TOKEN_ESCAPE
} space_tokenizer_state;

struct space_tokenizer_s {
  tokenizer* base;

  space_tokenizer_state state;
  int quote_char;
  size_t token_start;

  char* logical_end_of_input_marker;

  line_count_fn on_line;
  void* on_line_ctx;
};

static void space_tokenizer_no_token(space_tokenizer* t) {
  tokenizer_reset(t->base);
  t->state = NO_TOKEN;
  t->quote_char = '\0';
  t->token_start = 0;
}

static void space_tokenizer_start_quoted_token(
  space_tokenizer* t,
  int quote_char) {

  t->state = IN_QUOTED_TOKEN;
  t->quote_char = quote_char;
  t->token_start = tokenizer_pos(t->base);
}

static void space_tokenizer_start_no_token_escape(space_tokenizer* t) {
  t->state = NO_TOKEN_ESCAPE;
  t->token_start = tokenizer_pos(t->base);
}

static void space_tokenizer_start_in_token_escape(space_tokenizer* t) {
  t->state = IN_TOKEN_ESCAPE;
}

static void space_tokenizer_end_escape(space_tokenizer* t, int ch) {
  t->state = IN_TOKEN;
  tokenizer_add(t->base, (char) ch);
}

static void space_tokenizer_start_token(space_tokenizer* t, int ch) {
  t->state = IN_TOKEN;
  t->token_start = tokenizer_pos(t->base);
  tokenizer_add(t->base, (char) ch);
}

static void space_tokenizer_append_to_token(space_tokenizer* t, int ch) {
  tokenizer_add(t->base, (char) ch);
}

static const char* space_tokenizer_end_token(space_tokenizer* t) {
  tokenizer_add(t->base, '\0');
  const char* token = tokenizer_token(t->base, t->token_start);
  space_tokenizer_no_token(t);

  if (t->logical_end_of_input_marker != NULL) {
    if (strcmp(t->logical_end_of_input_marker, token) == 0) {
      return NULL;
    }
  }

  return token;
}

static const char* next_space_token(tokenizer* t, FILE* token_source) {
  space_tokenizer* self = (space_tokenizer*) tokenizer_impl(t);

  bool line_has_token = false;

  int ch;
  while ((ch = getc(token_source)) != EOF) {
    switch (self->state) {
      case NO_TOKEN:
        if (ch == ' ' || ch == '\t') {
          continue;
        } else if (ch == '\n') {
          if (line_has_token) {
            self->on_line(self->on_line_ctx);
          }
        } else if (ch == '\'' || ch == '"') {
          space_tokenizer_start_quoted_token(self, ch);
          line_has_token = true;
        } else if (ch == '\\') {
          space_tokenizer_start_no_token_escape(self);
          line_has_token = true;
        } else {
          space_tokenizer_start_token(self, ch);
          line_has_token = true;
        }
        break;

      case NO_TOKEN_ESCAPE:
        space_tokenizer_end_escape(self, ch);
        break;

      case IN_TOKEN:
        if (ch == ' ' || ch == '\t') {
          return space_tokenizer_end_token(self);
        } else if (ch == '\n') {
          if (line_has_token) {
            self->on_line(self->on_line_ctx);
          }
          return space_tokenizer_end_token(self);
        } else if (ch == '\'' || ch == '"') {
          self->state = IN_TOKEN_QUOTED;
          self->quote_char = ch;
        } else if (ch == '\\') {
          space_tokenizer_start_in_token_escape(self);
        } else {
          space_tokenizer_append_to_token(self, ch);
        }
        break;

      case IN_QUOTED_TOKEN:
        /* fallthrough */
      case IN_TOKEN_QUOTED:
        if (ch == self->quote_char) {
          self->state = IN_TOKEN;
        } else if (ch == '\n') {
          fprintf(stderr, "phxargs: unterminated quote\n");
          tokenizer_set_error(t, TOKENIZER_ERR_UNTERMINATED_QUOTE);
          return NULL;
        } else {
          space_tokenizer_append_to_token(self, ch);
        }
        break;

      case IN_TOKEN_ESCAPE:
        space_tokenizer_end_escape(self, ch);
        break;
    }
  }

  if (ferror(token_source)) {
    fprintf(stderr, "phxargs: I/O error\n");
    tokenizer_set_error(t, TOKENIZER_ERR_IO);
    return NULL;
  } else if (self->state == IN_TOKEN_ESCAPE || self->state == NO_TOKEN_ESCAPE) {
    fprintf(stderr, "phxargs: backslash at EOF\n");
    tokenizer_set_error(t, TOKENIZER_ERR_BACKSLASH_AT_EOF);
    return NULL;
  } else if (self->state == IN_QUOTED_TOKEN || self->state == IN_TOKEN_QUOTED) {
    fprintf(stderr, "phxargs: unterminated quote\n");
    tokenizer_set_error(t, TOKENIZER_ERR_UNTERMINATED_QUOTE);
    return NULL;
  } else {
    if (line_has_token) {
      self->on_line(self->on_line_ctx);
      return space_tokenizer_end_token(self);
    }
    return NULL;
  }
}

static void space_tokenizer_destroy_impl(void* impl) {
  space_tokenizer* t = (space_tokenizer*) impl;

  free(t->logical_end_of_input_marker);
  free(t);
}

static const tokenizer_ops space_tokenizer_ops = {
  .next_token = next_space_token,
  .destroy_impl = space_tokenizer_destroy_impl
};

space_tokenizer* space_tokenizer_create(
  size_t buffer_size,
  const char* logical_end_of_input_marker,
  line_count_fn on_line,
  void* on_line_ctx) {

  space_tokenizer* t = safe_malloc(sizeof(space_tokenizer));

  t->logical_end_of_input_marker = safe_strdup(logical_end_of_input_marker);
  t->on_line = on_line;
  t->on_line_ctx = on_line_ctx;
  t->base = tokenizer_create(&space_tokenizer_ops, buffer_size, t);
  space_tokenizer_no_token(t);

  return t;
}

tokenizer* space_tokenizer_as_tokenizer(space_tokenizer* t) {
  return t->base;
}

void space_tokenizer_destroy(space_tokenizer* t) {
  tokenizer_destroy(t->base);
}
