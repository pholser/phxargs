#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "space_tokenizer.h"
#include "tokenizer.h"

static void noop_line_count(void* ctx) {
  (void) ctx;
}

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  FILE* input = fmemopen((void*) data, size, "r");
  if (input == NULL) {
    return 0;
  }

  space_tokenizer* st =
    space_tokenizer_create(16, NULL, noop_line_count, NULL);
  tokenizer* t = space_tokenizer_as_tokenizer(st);

  while (tokenizer_next_token(t, input) != NULL) {
    /* consume */
  }

  fclose(input);
  space_tokenizer_destroy(st);
  return 0;
}
