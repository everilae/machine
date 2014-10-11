#include <stdio.h>
#include "machine.h"
#include "compiler.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		return 1;
	}

	code_t code[1024] = { HALT };
	if (compile(argv[1], code, sizeof(code)) != 0) {
		return 1;
	}

	stack_t stack[256] = { 0 };
	struct machine_t m = {
		0, (sizeof(stack) / sizeof(stack_t)) - 1,
		code,
		sizeof(code),
		stack,
		sizeof(stack) / sizeof(stack_t),
		NO_ERROR,
	};
	machine_run(&m);
	return 0;
}
