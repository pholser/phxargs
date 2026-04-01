#include "util.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void* phxargs_calloc(size_t count, size_t size) {
  void* ptr = calloc(count, size);
  if (ptr == NULL) {
    perror("phxargs: calloc");
    exit(EXIT_FAILURE);
  }

  return ptr;
}

void* phxargs_malloc(size_t size) {
  void* ptr = malloc(size);
  if (ptr == NULL) {
    perror("phxargs: malloc");
    exit(EXIT_FAILURE);
  }

  return ptr;
}

void* phxargs_realloc(void* ptr, size_t size) {
  void* new_ptr = realloc(ptr, size);
  if (new_ptr == NULL) {
    perror("phxargs: realloc");
    exit(EXIT_FAILURE);
  }

  return new_ptr;
}

char* phxargs_strdup(const char* s) {
  if (s == NULL) {
    return NULL;
  }

  const size_t len = strlen(s) + 1;
  char* copy = phxargs_malloc(len);
  memcpy(copy, s, len);

  return copy;
}

long phxargs_sysconf(int name) {
  errno = 0;

  const long result = sysconf(name);
  if (result == -1 && errno != 0) {
    perror("phxargs: sysconf");
    exit(EXIT_FAILURE);
  }

  return result;
}
