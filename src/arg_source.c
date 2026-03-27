#include "arg_source.h"

#include <stdio.h>
#include <stdlib.h>

FILE* arg_source_open(const char* path) {
  if (path == NULL) {
    return stdin;
  }

  FILE* arg_source = fopen(path, "r");
  if (arg_source == NULL) {
    perror("phxargs: cannot open arg file");
    exit(EXIT_FAILURE);
  }

  return arg_source;
}

void arg_source_close(FILE* arg_source) {
  if (arg_source != stdin) {
    fclose(arg_source);
  }
}
