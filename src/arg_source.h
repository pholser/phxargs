#ifndef PHXARGS_ARG_SOURCE_H
#define PHXARGS_ARG_SOURCE_H

#include <stdio.h>

#include "options.h"

FILE* init_arg_source(const options* const opts);

void free_arg_source(FILE* arg_source);

#endif  // PHXARGS_ARG_SOURCE_H
