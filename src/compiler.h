#ifndef COMPILER_H
#define COMPILER_H 1

#include "machine.h"

extern int compile(const char *fname, code_t *code, size_t code_size);

#endif
