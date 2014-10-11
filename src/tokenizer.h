#ifndef TOKENIZER_H
#define TOKENIZER_H 1

#include <stdio.h>
#include <stddef.h>

#define TOKENIZER_BUFLEN 256

struct tokenizer_t {
	FILE *fp;
	const char *delim;
	char *line;
	size_t lineno;
	char *saveptr;
	char *token;
	char buffer[TOKENIZER_BUFLEN];
};

extern void tokenizer_init(struct tokenizer_t *);
extern char *tokenizer_next(struct tokenizer_t *);

#endif
