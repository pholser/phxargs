#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "delim_tokenizer.h"
#include "tokenizer.h"

static void noop_line_count(void* ctx) {
  (void) ctx;
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size == 0) {
    return 0;
  }

  /* Use the first byte as the delimiter, the rest as content. */
  char delim = (char) data[0];
  const uint8_t* content = data + 1;
  size_t content_size = size - 1;

  FILE* input = fmemopen((void*) content, content_size, "r");
  if (input == NULL) {
    return 0;
  }

  delim_tokenizer* dt =
    delim_tokenizer_create(content_size + 1, delim, noop_line_count, NULL);
  tokenizer* t = delim_tokenizer_as_tokenizer(dt);

  while (tokenizer_next_token(t, input) != NULL) {
    /* consume */
  }

  fclose(input);
  delim_tokenizer_destroy(dt);
  return 0;
}
