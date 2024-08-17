#include "buffer.h"
#include "command.h"
#include "null_sep_tokenizer.h"
#include "options.h"
#include "util.h"

void init_tokenizer(
  null_sep_tokenizer* const t,
  const options* const opts) {

  t->buf = safe_malloc(sizeof(buffer));
  init_buffer(t->buf, opts->max_command_length);

  t->terminate_on_too_large_token = opts->terminate_on_too_large_command;
  t->line_mode = options_line_mode(opts);
}

char* next_null_sep_token(null_sep_tokenizer* const t, command* const cmd) {
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
          tokenizer_start_no_token_escape(t);
          line_has_token = 1;
        } else {
          tokenizer_start_token(t, ch);
          line_has_token = 1;
        }
        break;

      case NO_TOKEN_ESCAPE:
        tokenizer_end_escape(t, ch);
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
          tokenizer_start_in_token_escape(t);
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
