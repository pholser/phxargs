#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void* safe_malloc(size_t size) {
  void* ptr = malloc(size);
  if (ptr == NULL) {
    perror("phxargs: malloc");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
  ptr = realloc(ptr, size);
  if (ptr == NULL) {
    perror("phxargs: realloc");
    exit(EXIT_FAILURE);
  }
  return ptr;
}
