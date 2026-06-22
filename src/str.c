#include "str.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

static size_t checked_mul(size_t a, size_t b) {
  if (a != 0 && b > SIZE_MAX / a) {
    fprintf(stderr, "phxargs: replacement too large\n");
    exit(EXIT_FAILURE);
  }
  return a * b;
}

static size_t checked_add(size_t a, size_t b) {
  if (a > SIZE_MAX - b) {
    fprintf(stderr, "phxargs: replacement too large\n");
    exit(EXIT_FAILURE);
  }
  return a + b;
}

char* str_replace(
  const char* s,
  const char* placeholder,
  const char* replacement) {

  if (s == NULL) {
    return NULL;
  }
  if (placeholder == NULL || strlen(placeholder) == 0) {
    return phxargs_strdup(s);
  }

  const char* repl = replacement == NULL ? "" : replacement;

  const size_t target_len = strlen(s);
  const size_t placeholder_len = strlen(placeholder);
  const size_t replacement_len = strlen(repl);
  size_t occurrence_count = 0;

  const char* tmp = s;
  while ((tmp = strstr(tmp, placeholder)) != NULL) {
    ++occurrence_count;
    tmp += placeholder_len;
  }

  /* occurrence_count non-overlapping matches each consume placeholder_len
     bytes of s, so removed can never exceed target_len. */
  const size_t removed = checked_mul(occurrence_count, placeholder_len);
  const size_t added = checked_mul(occurrence_count, replacement_len);
  const size_t new_len = checked_add(target_len - removed, added);
  char* result = phxargs_calloc(new_len + 1, sizeof(char));

  const char* pos = s;
  char* new_pos = result;
  while ((tmp = strstr(pos, placeholder)) != NULL) {
    size_t prefix_len = tmp - pos;
    memcpy(new_pos, pos, prefix_len);
    new_pos += prefix_len;

    memcpy(new_pos, repl, replacement_len); // NOLINT(bugprone-not-null-terminated-result): buffer is calloc'd; null is already in place
    new_pos += replacement_len;

    pos = tmp + placeholder_len;
  }

  memcpy(new_pos, pos, strlen(pos) + 1);

  return result;
}
