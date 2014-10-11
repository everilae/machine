#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "machine.h"
#include "tokenizer.h"
#include "debug.h"

static const struct {
	const char *name;
	enum instruction_t code;
} instruction_lookup_table[] = {
	{ "HALT", 	HALT },
	{ "PRINT", 	PRINT },
	{ ".",	 	PRINT },
	{ "INC", 	INC },
	{ "DEC", 	DEC },
	{ "ADD", 	ADD },
	{ "+",	 	ADD },
	{ "DUP", 	DUP },
	{ "PUSH", 	PUSH },
	{ "POP",	POP },
	{ "MUL",	MUL },
	{ "*",		MUL },
	{ "DIV",	DIV },
	{ "/",		DIV },
	{ NULL, 	HALT }
};

int compile(const char *fname, code_t *code, size_t code_size) {
	// default to dirty result
	int result = 1;
	FILE *fp = fopen(fname, "r");
	if (!fp) {
		fprintf(stderr, "Error reading %s: %s\n", fname, strerror(errno));
		goto end;
	}

	int cp = 0;
	char *token;
	struct tokenizer_t t = {
		.fp	= fp,
		.delim	= " \t\n",
	};

	tokenizer_init(&t);
	while ((token = tokenizer_tokenize(&t)) != NULL) {
		size_t len = strnlen(token, sizeof(t.buffer));

		if (len == sizeof(t.buffer)) {
			fprintf(stderr, "Token exceeded buffer size, fgets + strtok_r without '\\0'\n");
			goto end;
		}

		char *endptr;
		/* try integer */
		stack_t num = strtol(token, &endptr, 0);
		if (token != endptr && *endptr == '\0') {
			if (cp >= code_size - sizeof(stack_t) - 1) {
				fprintf(stderr, "Code buffer too small\n");
				goto end;
			}

			code[cp++] = PUSH;
			*(stack_t *)&code[cp] = num;
			cp += sizeof(stack_t);
			continue;
		}
		/* try float */
		num = strtod(token, &endptr);
		if (token != endptr && *endptr == '\0') {
			if (cp >= code_size - sizeof(stack_t) - 1) {
				fprintf(stderr, "Code buffer too small\n");
				goto end;
			}

			code[cp++] = PUSH;
			*(stack_t *)&code[cp] = num;
			cp += sizeof(stack_t);
			continue;
		}
		/* treat as a symbol */
		for (int i = 0; instruction_lookup_table[i].name != NULL; i++) {
			if (strcmp(token, instruction_lookup_table[i].name) == 0) {
				code[cp++] = instruction_lookup_table[i].code;
				break;
			}
		}
	}

	if (ferror(fp)) {
		perror("Compiler error");
		goto end;
	}

	if (cp >= code_size) {
		fprintf(stderr, "Code buffer too small\n");
		goto end;
	}

	result = 0;
	code[cp] = HALT;

end:
	if (fp && fclose(fp) != 0) {
		perror("Compiler error");
		result = 1;
	}

	return result;
}
