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
	{ "SUB",	SUB },
	{ "-",		SUB },
	{ "MUL",	MUL },
	{ "*",		MUL },
	{ "DIV",	DIV },
	{ "/",		DIV },
	{ "DUP", 	DUP },
	{ "PUSH", 	PUSH },
	{ "POP",	POP },
	{ NULL, 	HALT }
};

struct compiler_t {
	code_t *code;
	size_t code_size;
	size_t code_pos;
	FILE *fp;
	struct tokenizer_t tokenizer;
};

static struct compiler_t *compiler_init(struct compiler_t *c, const char *path) {
	c->fp = fopen(path, "r");
	if (c->fp == NULL) {
		fprintf(stderr, "Error reading %s: %s\n", path, strerror(errno));
		return NULL;
	}
	c->tokenizer.fp = c->fp;
	tokenizer_init(&c->tokenizer);
	return c;
}

static int compiler_push_number(struct compiler_t *c, stack_t num) {
	if (c->code_pos >= c->code_size - sizeof(stack_t) - 1) {
		fprintf(stderr, "Code buffer too small\n");
		return 1;
	}

	c->code[c->code_pos++] = PUSH;
	*(stack_t *)&c->code[c->code_pos] = num;
	c->code_pos += sizeof(stack_t);
	return 0;
}

static int compiler_parse_token(struct compiler_t *c, const char *token) {
	int result = 1;
	char *endptr;
	/* try integer */
	stack_t num = strtol(token, &endptr, 0);
	if (token != endptr && *endptr == '\0') {
		result = compiler_push_number(c, num);
		goto exit;
	}
	/* try float */
	num = strtod(token, &endptr);
	if (token != endptr && *endptr == '\0') {
		result = compiler_push_number(c, num);
		goto exit;
	}
	/* treat as a symbol */
	for (int i = 0; instruction_lookup_table[i].name != NULL; i++) {
		if (strcmp(token, instruction_lookup_table[i].name) == 0) {
			c->code[c->code_pos++] = instruction_lookup_table[i].code;
			result = 0;
			goto exit;
		}
	}

	fprintf(stderr, "Unknown symbol: %s\n", token);

exit:
	return result;
}

static int compiler_compile(struct compiler_t *c) {
	/* by default in failed state until proven otherwise */
	int result = 1;
	char *token;

	while ((token = tokenizer_next(&c->tokenizer)) != NULL && c->code_pos < c->code_size) {
		if (compiler_parse_token(c, token) != 0) {
			/* did not parse, some sort of error */
			break;
		}
	}

	if (c->code_pos >= c->code_size) {
		fprintf(stderr, "Code buffer too small\n");
	} else { 
		/* finalize */
		c->code[c->code_pos++] = HALT;
		/* success */
		result = 0;
	}

	return result;
}

static void compiler_delete(struct compiler_t *c) {
	if (c->fp && ferror(c->fp)) {
		perror("Compiler error");
	}

	if (c->fp && fclose(c->fp) != 0) {
		perror("Compiler error");
	}
}

int compile(const char *path, code_t *code, size_t code_size) {
	/* default to failed state */
	int result = 1;
	struct compiler_t c = {
		.code = code,
		.code_size = code_size,
		.tokenizer = {
			.delim	= " \t\n",
		},
	};

	if (compiler_init(&c, path) != NULL) {
		result = compiler_compile(&c);
	}

	compiler_delete(&c);
	return result;
}
