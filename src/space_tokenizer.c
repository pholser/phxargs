#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "command.h"
#include "options.h"
#include "space_tokenizer.h"
#include "util.h"

void space_tokenizer_no_token(space_tokenizer* const t) {
  t->state = NO_TOKEN;
  t->quote_char = '\0';
  t->token_start = 0;
}

void init_space_tokenizer(
  space_tokenizer* const t,
  const options* const opts) {

  t->buf = safe_malloc(sizeof(buffer));
  init_buffer(t->buf, opts->max_command_length);

  t->line_mode = options_line_mode(opts);
  t->logical_end_of_input_marker = opts->logical_end_of_input_marker;

  space_tokenizer_no_token(t);
}

void space_tokenizer_start_quoted_token(
  space_tokenizer* const t,
  int quote_char) {

  t->state = IN_QUOTED_TOKEN;
  t->quote_char = quote_char;
  t->token_start = buffer_pos(t->buf);
}

void space_tokenizer_start_no_token_escape(space_tokenizer* const t) {
  t->state = NO_TOKEN_ESCAPE;
  t->token_start = buffer_pos(t->buf);
}

void space_tokenizer_start_in_token_escape(space_tokenizer* const t) {
  t->state = IN_TOKEN_ESCAPE;
}

void space_tokenizer_end_escape(space_tokenizer* const t, int ch) {
  t->state = IN_TOKEN;
  buffer_put(t->buf, (char) ch);
}

void space_tokenizer_start_token(space_tokenizer* const t, int ch) {
  t->state = IN_TOKEN;
  t->token_start = buffer_pos(t->buf);
  buffer_put(t->buf, (char) ch);
}

void space_tokenizer_append_to_token(const space_tokenizer* const t, int ch) {
  buffer_put(t->buf, (char) ch);
}

char* space_tokenizer_end_token(space_tokenizer* const t) {
  buffer_put(t->buf, '\0');
  char* token = t->buf->buf + t->token_start;
  space_tokenizer_no_token(t);

  if (t->logical_end_of_input_marker != NULL) {
    if (strcmp(t->logical_end_of_input_marker, token) == 0) {
      return NULL;
    }
  }

  return token;
}

char* next_space_token(
  space_tokenizer* const t,
  FILE* token_source,
  command* const cmd) {

  uint8_t line_has_token = 0;

  int last_char = 0;
  int ch;
  while ((ch = getc(token_source)) != EOF) {
    switch (t->state) {
      case NO_TOKEN:
        if (ch == ' ' || ch == '\t') {
          continue;
        } else if (ch == '\n') {
          if (line_has_token && last_char != ' ') {
            ++cmd->line_count;
          }
        } else if (ch == '\'' || ch == '"') {
          space_tokenizer_start_quoted_token(t, ch);
          line_has_token = 1;
        } else if (ch == '\\') {
          space_tokenizer_start_no_token_escape(t);
          line_has_token = 1;
        } else {
          space_tokenizer_start_token(t, ch);
          line_has_token = 1;
        }
        break;

      case NO_TOKEN_ESCAPE:
        space_tokenizer_end_escape(t, ch);
        break;

      case IN_TOKEN:
        if (ch == ' ' || ch == '\t') {
          last_char = ch;
          return space_tokenizer_end_token(t);
        } else if (ch == '\n') {
          if (line_has_token && last_char != ' ') {
            ++cmd->line_count;
          }
          return space_tokenizer_end_token(t);
        } else if (ch == '\\') {
          space_tokenizer_start_in_token_escape(t);
        } else {
          last_char = ch;
          space_tokenizer_append_to_token(t, ch);
        }
        break;

      case IN_QUOTED_TOKEN:
        if (ch == t->quote_char) {
          return space_tokenizer_end_token(t);
        } else if (ch == '\n') {
          fprintf(stderr, "phxargs: unterminated quote\n");
          exit(EXIT_FAILURE);
        } else {
          space_tokenizer_append_to_token(t, ch);
        }
        break;

      case IN_TOKEN_ESCAPE:
        space_tokenizer_end_escape(t, ch);
        break;
    }
  }

  if (ferror(token_source)) {
    fprintf(stderr, "phxargs: I/O error\n");
    exit(EXIT_FAILURE);
  } else {
    if (line_has_token) {
      ++cmd->line_count;
      return space_tokenizer_end_token(t);
    }
    return NULL;
  }
}

void free_space_tokenizer(const space_tokenizer* const t) {
  free_buffer(t->buf);
}
