#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "command.h"
#include "options.h"
#include "tokenizer.h"

void tokenizer_no_token(tokenizer* t) {
  t->state = NO_TOKEN;
  t->quote_char = '\0';
  t->token_start = 0;
}

void init_tokenizer(tokenizer* t, const options* opts) {
  init_buffer(t->buf, opts->max_command_length);
  tokenizer_no_token(t);
}

void tokenizer_start_quoted_token(tokenizer* t, int quote_char) {
  t->state = IN_QUOTED_TOKEN;
  t->quote_char = quote_char;
  t->token_start = buffer_pos(t->buf);
}

void tokenizer_start_escape(tokenizer* t) {
  t->state = IN_TOKEN_ESCAPE;
  t->token_start = buffer_pos(t->buf);
}

void tokenizer_end_escape(tokenizer* t, int ch) {
  t->state = IN_TOKEN;
  buffer_put(t->buf, ch);
}

void tokenizer_start_token(tokenizer* t, int ch) {
  t->state = IN_TOKEN;
  t->token_start = buffer_pos(t->buf);
  buffer_put(t->buf, (char) ch);
}

void tokenizer_append_to_token(const tokenizer* t, int ch) {
  buffer_put(t->buf, (char) ch);
}

char* tokenizer_end_token(tokenizer* t) {
  buffer_put(t->buf, '\0');
  char* token = t->buf->buf + t->token_start;
  tokenizer_no_token(t);

  return token;
}

char* next_token(tokenizer* t, command* cmd) {
  uint8_t line_has_token = 0;

  int ch;
  while ((ch = getc(stdin)) != EOF) {
    switch (t->state) {
      case NO_TOKEN:
        if (ch == ' ' || ch == '\t') {
          // nothing to do, ignore the character
          // call out to cmd .... when line advances? should_execute/execute
        } else if (ch == '\n') {
          if (line_has_token) {
            ++cmd->line_count;
            line_has_token = 0;
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
          return tokenizer_end_token(t);
        } else if (ch == '\n') {
          if (line_has_token) {
            ++cmd->line_count;
            line_has_token = 0;
          }
          return tokenizer_end_token(t);
        } else if (ch == '\\') {
          tokenizer_start_escape(t);
        } else {
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

      case IN_TOKEN_ESCAPE:
        tokenizer_end_escape(t, ch);
        break;
    }
  }

  if (ferror(stdin)) {
    fprintf(stderr, "phxargs: I/O error\n");
    exit(EXIT_FAILURE);
  } else {
    return NULL;
  }
}

void free_tokenizer(const tokenizer* t) {
  free_buffer(t->buf);
}
