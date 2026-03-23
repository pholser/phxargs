#ifndef PHXARGS_ARG_SOURCE_H
#define PHXARGS_ARG_SOURCE_H

#include <stdio.h>

FILE* arg_source_open(const char* path);

void arg_source_close(FILE* arg_source);

#endif  // PHXARGS_ARG_SOURCE_H

