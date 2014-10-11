#ifndef MACHINE_H
#define MACHINE_H 1

#include "instructions.h"

enum error_t {
	NO_ERROR,
	STACK_OVERFLOW,
	STACK_UNDERFLOW,
	ILLEGAL_INSTRUCTION,
	HALTED,
};

typedef unsigned char code_t;
typedef int stack_t;

struct machine_t {
	int ip;
	int sp;
	const code_t *code;
	size_t code_size;
	stack_t *stack;
	size_t stack_size;
	enum error_t error;
};

extern void machine_run(struct machine_t *);

#endif
