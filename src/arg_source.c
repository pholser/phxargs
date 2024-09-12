#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "arg_source.h"
#include "options.h"

FILE* init_arg_source(const options* const opts) {
  FILE* arg_source = stdin;

  if (opts->arg_file_path != NULL) {
    arg_source = fopen(opts->arg_file_path, "r");
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
