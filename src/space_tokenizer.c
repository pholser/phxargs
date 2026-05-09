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

  input_boundary_fn on_input_boundary;
  void* on_input_boundary_ctx;
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

static void handle_no_token_char(
  space_tokenizer* t,
  int ch,
  bool* line_has_token) {

  if (ch == ' ' || ch == '\t') {
    return;
  }

  if (ch == '\n') {
    if (*line_has_token) {
      t->on_input_boundary(t->on_input_boundary_ctx);
    }
    return;
  }

  if (ch == '\'' || ch == '"') {
    space_tokenizer_start_quoted_token(t, ch);
  } else if (ch == '\\') {
    space_tokenizer_start_no_token_escape(t);
  } else {
    space_tokenizer_start_token(t, ch);
  }

  *line_has_token = true;
}

static bool handle_in_token_char(
  space_tokenizer* t,
  int ch,
  bool line_has_token,
  const char** out) {

  if (ch == ' ' || ch == '\t') {
    *out = space_tokenizer_end_token(t);
    return true;
  }

  if (ch == '\n') {
    if (line_has_token) {
      t->on_input_boundary(t->on_input_boundary_ctx);
    }
    *out = space_tokenizer_end_token(t);
    return true;
  }

  if (ch == '\'' || ch == '"') {
    t->state = IN_TOKEN_QUOTED;
    t->quote_char = ch;
  } else if (ch == '\\') {
    space_tokenizer_start_in_token_escape(t);
  } else {
    space_tokenizer_append_to_token(t, ch);
  }

  return false;
}

static bool handle_quoted_char(
  space_tokenizer* self,
  int ch,
  const char** out) {

  if (ch == self->quote_char) {
    self->state = IN_TOKEN;
    return false;
  }

  if (ch == '\n') {
    fprintf(stderr, "phxargs: unterminated quote\n");
    tokenizer_set_error(self->base, TOKENIZER_ERR_UNTERMINATED_QUOTE);
    *out = NULL;
    return true;
  }

  space_tokenizer_append_to_token(self, ch);
  return false;
}

static const char* handle_eof(
  space_tokenizer* self,
  FILE* token_source,
  bool line_has_token) {

  if (ferror(token_source)) {
    fprintf(stderr, "phxargs: I/O error\n");
    tokenizer_set_error(self->base, TOKENIZER_ERR_IO);
    return NULL;
  }

  if (self->state == IN_TOKEN_ESCAPE || self->state == NO_TOKEN_ESCAPE) {
    fprintf(stderr, "phxargs: backslash at EOF\n");
    tokenizer_set_error(self->base, TOKENIZER_ERR_BACKSLASH_AT_EOF);
    return NULL;
  }

  if (self->state == IN_QUOTED_TOKEN || self->state == IN_TOKEN_QUOTED) {
    fprintf(stderr, "phxargs: unterminated quote\n");
    tokenizer_set_error(self->base, TOKENIZER_ERR_UNTERMINATED_QUOTE);
    return NULL;
  }

  if (line_has_token) {
    self->on_input_boundary(self->on_input_boundary_ctx);
    return space_tokenizer_end_token(self);
  }

  return NULL;
}

static const char* next_space_token(tokenizer* t, FILE* token_source) {
  space_tokenizer* self = (space_tokenizer*) tokenizer_impl(t);
  bool line_has_token = false;
  const char* out;

  int ch;
  while ((ch = getc(token_source)) != EOF) {
    switch (self->state) {
      case NO_TOKEN:
        handle_no_token_char(self, ch, &line_has_token);
        break;
      case NO_TOKEN_ESCAPE:
        space_tokenizer_end_escape(self, ch);
        break;
      case IN_TOKEN:
        if (handle_in_token_char(self, ch, line_has_token, &out)) {
          return out;
        }
        break;
      case IN_QUOTED_TOKEN:
        /* fallthrough */
      case IN_TOKEN_QUOTED:
        if (handle_quoted_char(self, ch, &out)) {
          return out;
        }
        break;
      case IN_TOKEN_ESCAPE:
        space_tokenizer_end_escape(self, ch);
        break;
    }
  }

  return handle_eof(self, token_source, line_has_token);
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
  input_boundary_fn on_input_boundary,
  void* on_input_boundary_ctx) {

  space_tokenizer* t = phxargs_malloc(sizeof(space_tokenizer));

  t->logical_end_of_input_marker = phxargs_strdup(logical_end_of_input_marker);
  t->on_input_boundary = on_input_boundary;
  t->on_input_boundary_ctx = on_input_boundary_ctx;
  t->base = tokenizer_create(&space_tokenizer_ops, buffer_size, t);
  space_tokenizer_no_token(t);

  return t;
}

tokenizer* space_tokenizer_as_tokenizer(const space_tokenizer* t) {
  return t->base;
}

void space_tokenizer_destroy(space_tokenizer* t) {
  tokenizer_destroy(t->base);
}
