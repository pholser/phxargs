#ifndef PHXARGS_STR_H
#define PHXARGS_STR_H

char* str_replace(
  const char* s,
  const char* placeholder,
  const char* replacement);

char** strs_replace(
  const char** ss,
  const char* placeholder,
  const char* replacement);

#endif  // PHXARGS_STR_H
