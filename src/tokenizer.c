#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "command.h"
#include "options.h"
#include "tokenizer.h"
#include "util.h"

void tokenizer_no_token(tokenizer* const t) {
  t->state = NO_TOKEN;
  t->quote_char = '\0';
  t->token_start = 0;
}

void init_tokenizer(tokenizer* const t, const options* opts) {
  t->buf = safe_malloc(sizeof(buffer));
  init_buffer(t->buf, opts->max_command_length);

  t->terminate_on_too_large_token = opts->terminate_on_too_large_command;
  t->line_mode = options_line_mode(opts);
  tokenizer_no_token(t);
}

void tokenizer_start_quoted_token(tokenizer* const t, int quote_char) {
  t->state = IN_QUOTED_TOKEN;
  t->quote_char = quote_char;
  t->token_start = buffer_pos(t->buf);
}

void tokenizer_start_escape(tokenizer* const t) {
  t->state = IN_TOKEN_ESCAPE;
  t->token_start = buffer_pos(t->buf);
}

void tokenizer_end_escape(tokenizer* const t, int ch) {
  t->state = IN_TOKEN;
  buffer_put(t->buf, (char) ch);
}

void tokenizer_start_token(tokenizer* const t, int ch) {
  t->state = IN_TOKEN;
  t->token_start = buffer_pos(t->buf);
  buffer_put(t->buf, (char) ch);
}

void tokenizer_append_to_token(const tokenizer* const t, int ch) {
  if (t->terminate_on_too_large_token && buffer_full(t->buf)) {
    fprintf(stderr, "phxargs: insufficient space for argument\n");
    exit(EXIT_FAILURE);
  }
  buffer_put(t->buf, (char) ch);
}

char* tokenizer_end_token(tokenizer* const t) {
  buffer_put(t->buf, '\0');
  char* token = t->buf->buf + t->token_start;
  tokenizer_no_token(t);

  return token;
}

char* next_token(tokenizer* const t, command* const cmd) {
  uint8_t line_has_token = 0;

  int last_char = 0;
  int ch;
  while ((ch = getc(stdin)) != EOF) {
    switch (t->state) {
      case NO_TOKEN:
        if (ch == ' ' || ch == '\t') {
          continue;
        } else if (ch == '\n') {
          if (line_has_token && last_char != ' ') {
            ++cmd->line_count;
          }
        } else if (ch == '\'' || ch == '"') {
          tokenizer_start_quoted_token(t, ch);
          line_has_token = 1;
        } else if (ch == '\\') {
          tokenizer_start_escape(t);
          line_has_token = 1;
        } else {
          tokenizer_start_token(t, ch);
          line_has_token = 1;
        }
        break;

      case IN_TOKEN:
        if (ch == ' ' || ch == '\t') {
          last_char = ch;
          return tokenizer_end_token(t);
        } else if (ch == '\n') {
          if (line_has_token && last_char != ' ') {
            ++cmd->line_count;
          }
          return tokenizer_end_token(t);
        } else if (ch == '\\') {
          tokenizer_start_escape(t);
        } else {
          last_char = ch;
          tokenizer_append_to_token(t, ch);
        }
        break;

      case IN_QUOTED_TOKEN:
        if (ch == t->quote_char) {
          return tokenizer_end_token(t);
        } else if (ch == '\n') {
          fprintf(stderr, "phxargs: unterminated quote\n");
          exit(EXIT_FAILURE);
        } else {
          tokenizer_append_to_token(t, ch);
        }
        break;

      case IN_TOKEN_ESCAPE:
        tokenizer_end_escape(t, ch);
        break;
    }
  }

  if (ferror(stdin)) {
    fprintf(stderr, "phxargs: I/O error\n");
    exit(EXIT_FAILURE);
  } else {
    if (line_has_token) {
      ++cmd->line_count;
      return tokenizer_end_token(t);
    }
    return NULL;
  }
}

void free_tokenizer(const tokenizer* const t) {
  free_buffer(t->buf);
}
