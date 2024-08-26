#ifndef PHXARGS_UTIL_H
#define PHXARGS_UTIL_H

#include <stddef.h>

void* safe_malloc(size_t size);

void* safe_realloc(void* ptr, size_t size);

static inline size_t min(size_t a, size_t b) {
  return a < b ? a : b;
}

#endif  // PHXARGS_UTIL_H
