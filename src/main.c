#include <stdio.h>
#include "machine.h"
#include "compiler.h"

#define CODE_SIZE 1024
#define STACK_SIZE 1024
int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		return 1;
	}

	code_t code[CODE_SIZE] = { HALT };
	if (compile(argv[1], code, CODE_SIZE) != 0) {
		return 1;
	}

	stack_t stack[STACK_SIZE] = { 0 };
	struct machine_t m = {
		.ip = 0,
		.sp = STACK_SIZE - 1,
		.code = code,
		.code_size = CODE_SIZE,
		.stack = stack,
		.stack_size = STACK_SIZE,
		.error = NO_ERROR,
	};
	machine_run(&m);
	return 0;
}
