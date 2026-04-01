#ifndef PHXARGS_UTIL_H
#define PHXARGS_UTIL_H

#include <stddef.h>

void* phxargs_calloc(size_t count, size_t size);

void* phxargs_malloc(size_t size);

void* phxargs_realloc(void* ptr, size_t size);

char* phxargs_strdup(const char* s);

long phxargs_sysconf(int name);

static inline size_t phxargs_min(size_t a, size_t b) {
  return a < b ? a : b;
}

#endif // PHXARGS_UTIL_H
