#include <stddef.h>
#include <string.h>

#include "str.h"
#include "util.h"

char* str_replace(
  const char* s,
  const char* placeholder,
  const char* replacement) {

  if (s == NULL) {
    return NULL;
  }
  if (placeholder == NULL || strlen(placeholder) == 0) {
    return strdup(s);
  }
  const char* repl = replacement == NULL ? "" : replacement;

  size_t target_len = strlen(s);
  size_t placeholder_len = strlen(placeholder);
  size_t replacement_len = strlen(repl);

  const char* tmp = s;

  size_t occurrence_count = 0;
  while ((tmp = strstr(tmp, placeholder)) != NULL) {
    ++occurrence_count;
    tmp += placeholder_len;
  }

  size_t new_len =
    target_len + (occurrence_count * (replacement_len - placeholder_len));
  char* result = safe_calloc(new_len + 1, sizeof(char));

  const char* pos = s;
  char* new_pos = result;
  while ((tmp = strstr(pos, placeholder)) != NULL) {
    size_t prefix_len = tmp - pos;
    strncpy(new_pos, pos, prefix_len);
    new_pos += prefix_len;

    strncpy(new_pos, repl, replacement_len);
    new_pos += replacement_len;

    pos = tmp + placeholder_len;
  }

  strcpy(new_pos, pos);

  return result;
}
