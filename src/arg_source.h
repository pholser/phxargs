#ifndef PHXARGS_ARG_SOURCE_H
#define PHXARGS_ARG_SOURCE_H

#include <stdio.h>

FILE* arg_source_init(const char* path);

void free_arg_source(FILE* arg_source);

#endif  // PHXARGS_ARG_SOURCE_H
