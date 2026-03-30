#include "arg_source.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* arg_source_open(const char* path) {
  if (path == NULL) {
    return stdin;
  }

  FILE* arg_source = fopen(path, "r");
  if (arg_source == NULL) {
    fprintf(stderr, "phxargs: %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }

  return arg_source;
}

void arg_source_close(FILE* arg_source) {
  if (arg_source != stdin) {
    fclose(arg_source);
  }
}
