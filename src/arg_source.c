#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "arg_source.h"

FILE* arg_source_init(const char* path) {
  FILE* arg_source = stdin;

  if (path != NULL) {
    arg_source = fopen(path, "r");
    if (arg_source == NULL) {
      perror("phxargs: cannot open arg file");
      exit(EXIT_FAILURE);
    }
  }
  return arg_source;
}

void free_arg_source(FILE* arg_source) {
  if (arg_source != stdin) {
    fclose(arg_source);
  }
}
