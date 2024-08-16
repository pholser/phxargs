#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
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

  // If buffer becomes full, we're gonna need to reset it
  // to be able to continue reading token in process, and
  // make room for more input. Buffer full is a nice proxy
  // for "should execute command as was before reading this
  // token.
  buffer_put(t->buf, (char) ch);
}

void tokenizer_append_to_token(tokenizer* t, int ch) {
  buffer_put(t->buf, (char) ch);
}

char* tokenizer_end_token(tokenizer* t) {
  buffer_put(t->buf, '\0');
  char* token = t->buf->buf + t->token_start;
  tokenizer_no_token(t);

  return token;
}

char* next_token(tokenizer* t) {
  int ch;
  while ((ch = getc(stdin)) != EOF) {
    switch (t->state) {
      case NO_TOKEN:
        if (ch == ' ' || ch == '\t' || ch == '\n') {
          // nothing to do, ignore the character
        } else if (ch == '\'' || ch == '"') {
          tokenizer_start_quoted_token(t, ch);
        } else if (ch == '\\') {
          tokenizer_start_escape(t);
        } else {
          tokenizer_start_token(t, ch);
        }
        break;

      case IN_TOKEN:
        if (ch == ' ' || ch == '\t' || ch == '\n') {
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
        tokenizer_append_to_token(t, ch);
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

void free_tokenizer(tokenizer* t) {
  free_buffer(t->buf);
}
