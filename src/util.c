#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void* safe_calloc(size_t count, size_t size) {
  void* ptr = calloc(count, size);
  if (ptr == NULL) {
    perror("phxargs: calloc");
    exit(EXIT_FAILURE);
  }
  return ptr;
}

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

char* safe_strdup(const char* s) {
  if (s == NULL) {
    return NULL;
  }

  char* copy = strdup(s);
  if (copy == NULL) {
    perror("phxargs: strdup");
    exit(EXIT_FAILURE);
  }

  return copy;
}
