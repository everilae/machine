#include <string.h>
#include "tokenizer.h"
#include "debug.h"

static void tokenizer_readline(struct tokenizer_t *t) {
	/* reset strtok_r */
	t->saveptr = NULL;
	/* read next line (if buffer can hold it) */
	t->line = fgets(t->buffer, sizeof(t->buffer), t->fp);
	PDEBUGIF(t->line, "%s: %s", __FUNCTION__, t->line);
}

void tokenizer_init(struct tokenizer_t *t) {
	tokenizer_readline(t);
}

char *tokenizer_tokenize(struct tokenizer_t *t) {
	if (t->line == NULL) {
		return NULL;
	}

	t->token = strtok_r(
		t->saveptr == NULL ? t->buffer : NULL,
		t->delim,
		&t->saveptr
	);
	if (t->token == NULL) {
		/* line consumed, read next line and recurse */
		tokenizer_readline(t);
		return tokenizer_tokenize(t);
	}

	return t->token;
}
