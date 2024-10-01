#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "tokenizer.h"
#include "space_tokenizer.h"
#include "util.h"

typedef enum {
  NO_TOKEN,
  NO_TOKEN_ESCAPE,
  IN_TOKEN,
  IN_QUOTED_TOKEN,
  IN_TOKEN_ESCAPE
} space_tokenizer_state;

struct _space_tokenizer {
  tokenizer base;

  space_tokenizer_state state;
  int quote_char;
  size_t token_start;

  uint8_t line_mode;
  char* logical_end_of_input_marker;
};

void space_tokenizer_no_token(space_tokenizer* t) {
  t->state = NO_TOKEN;
  t->quote_char = '\0';
  t->token_start = 0;
}

void space_tokenizer_start_quoted_token(
  space_tokenizer* t,
  int quote_char) {

  t->state = IN_QUOTED_TOKEN;
  t->quote_char = quote_char;
  t->token_start = tokenizer_pos(&(t->base));
}

void space_tokenizer_start_no_token_escape(space_tokenizer* t) {
  t->state = NO_TOKEN_ESCAPE;
  t->token_start = tokenizer_pos(&(t->base));
}

void space_tokenizer_start_in_token_escape(space_tokenizer* t) {
  t->state = IN_TOKEN_ESCAPE;
}

void space_tokenizer_end_escape(space_tokenizer* t, int ch) {
  t->state = IN_TOKEN;
  tokenizer_add(&(t->base), (char) ch);
}

void space_tokenizer_start_token(space_tokenizer* t, int ch) {
  t->state = IN_TOKEN;
  t->token_start = tokenizer_pos(&(t->base));
  tokenizer_add(&(t->base), (char) ch);
}

void space_tokenizer_append_to_token(space_tokenizer* t, int ch) {
  tokenizer_add(&(t->base), (char) ch);
}

char* space_tokenizer_end_token(space_tokenizer* t) {
  tokenizer_add(&(t->base), '\0');
  char* token = tokenizer_token(&(t->base), t->token_start);
  space_tokenizer_no_token(t);

  if (t->logical_end_of_input_marker != NULL) {
    if (strcmp(t->logical_end_of_input_marker, token) == 0) {
      return NULL;
    }
  }

  return token;
}

char* next_space_token(
  tokenizer* t,
  FILE* token_source,
  command* cmd) {

  space_tokenizer* self = (space_tokenizer*) t;

  uint8_t line_has_token = 0;

  int last_char = 0;
  int ch;
  while ((ch = getc(token_source)) != EOF) {
    switch (self->state) {
      case NO_TOKEN:
        if (ch == ' ' || ch == '\t') {
          continue;
        } else if (ch == '\n') {
          if (line_has_token && last_char != ' ') {
            ++cmd->line_count;
          }
        } else if (ch == '\'' || ch == '"') {
          space_tokenizer_start_quoted_token(self, ch);
          line_has_token = 1;
        } else if (ch == '\\') {
          space_tokenizer_start_no_token_escape(self);
          line_has_token = 1;
        } else {
          space_tokenizer_start_token(self, ch);
          line_has_token = 1;
        }
        break;

      case NO_TOKEN_ESCAPE:
        space_tokenizer_end_escape(self, ch);
        break;

      case IN_TOKEN:
        if (ch == ' ' || ch == '\t') {
          last_char = ch;
          return space_tokenizer_end_token(self);
        } else if (ch == '\n') {
          if (line_has_token && last_char != ' ') {
            ++cmd->line_count;
          }
          return space_tokenizer_end_token(self);
        } else if (ch == '\\') {
          space_tokenizer_start_in_token_escape(self);
        } else {
          last_char = ch;
          space_tokenizer_append_to_token(self, ch);
        }
        break;

      case IN_QUOTED_TOKEN:
        if (ch == self->quote_char) {
          return space_tokenizer_end_token(self);
        } else if (ch == '\n') {
          fprintf(stderr, "phxargs: unterminated quote\n");
          exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
  } else {
    if (line_has_token) {
      ++cmd->line_count;
      return space_tokenizer_end_token(self);
    }
    return NULL;
  }
}

tokenizer_ops space_tokenizer_ops = {
  .next_token = next_space_token
};

space_tokenizer* space_tokenizer_create(
  size_t buffer_size,
  uint8_t line_mode,
  char* logical_end_of_input_marker) {

  space_tokenizer* t = safe_malloc(sizeof(space_tokenizer));
  tokenizer_init(&(t->base), &space_tokenizer_ops, buffer_size);
  t->line_mode = line_mode;
  t->logical_end_of_input_marker = safe_strdup(logical_end_of_input_marker);
  space_tokenizer_no_token(t);

  return t;
}

void space_tokenizer_destroy(space_tokenizer* t) {
  free(t->logical_end_of_input_marker);
  tokenizer_destroy(&(t->base));
  free(t);
}
