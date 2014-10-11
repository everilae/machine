#include <stdio.h>
#include "machine.h"
#include "debug.h"

static const char *error_strs[] = {
	"no error",
	"stack overflow",
	"stack underflow",
	"illegal instruction",
	"halt",
	NULL
};

static const char *machine_strerror(enum error_t error) {
	if (error < sizeof(error_strs) - 1) {
		return error_strs[error];
	} else {
		return "invalid error code";
	}
}

#define PSTATE(m) PDEBUG( \
	"MACHINE:\n" \
	"\tcode_size=%zu, stack_size=%zu\n" \
	"\tip=%i, sp=%i, error=%i(%s)", \
	m->code_size, m->stack_size, \
	m->ip, m->sp, m->error, machine_strerror(m->error) \
)

#define CHECK(m, constraint, error_code) do { \
	if (!(constraint)) { \
		PSTATE(m); \
		m->error = error_code; \
		return; \
	} \
} while (0)

#define INSTRUCTION(name) machine_instruction_ ## name
#define INSTRUCTION_DEF(name) static void INSTRUCTION(name)(struct machine_t *m)

INSTRUCTION_DEF(halt) {
	PDEBUG("HALT");
	m->error = HALTED;
}

INSTRUCTION_DEF(nop) {
	PDEBUG("NOP");
}


INSTRUCTION_DEF(print) {
	PDEBUG("PRINT");
	printf("%i\n", m->stack[m->sp]);
}

INSTRUCTION_DEF(inc) {
	PDEBUG("INC");
	m->stack[m->sp]++;
}

INSTRUCTION_DEF(dec) {
	PDEBUG("DEC");
	m->stack[m->sp]--;
}

INSTRUCTION_DEF(dup) {
	PDEBUG("DUP");
	CHECK(m, m->sp > 0, STACK_OVERFLOW);
	stack_t tmp = m->stack[m->sp];
	--m->sp;
	m->stack[m->sp] = tmp;
}

INSTRUCTION_DEF(push) {
	PDEBUG("PUSH");
	CHECK(m, m->sp > 0, STACK_OVERFLOW);
	m->stack[--m->sp] = *(stack_t *)&m->code[m->ip];
	m->ip += sizeof(stack_t);
	CHECK(m, m->ip < m->code_size, ILLEGAL_INSTRUCTION);
}

INSTRUCTION_DEF(pop) {
	PDEBUG("POP");
	m->sp++;
	CHECK(m, m->sp < m->stack_size, STACK_UNDERFLOW);
}

INSTRUCTION_DEF(add) {
	PDEBUG("ADD");
	stack_t tmp = m->stack[m->sp];
	++m->sp;
	CHECK(m, m->sp < m->stack_size, STACK_UNDERFLOW);
	m->stack[m->sp] += tmp;
}

INSTRUCTION_DEF(sub) {
	PDEBUG("SUB");
	stack_t tmp = m->stack[m->sp];
	++m->sp;
	CHECK(m, m->sp < m->stack_size, STACK_UNDERFLOW);
	m->stack[m->sp] -= tmp;
}

INSTRUCTION_DEF(mul) {
	PDEBUG("MUL");
	stack_t tmp = m->stack[m->sp];
	++m->sp;
	CHECK(m, m->sp < m->stack_size, STACK_UNDERFLOW);
	m->stack[m->sp] *= tmp;
}
INSTRUCTION_DEF(div) {
	PDEBUG("DIV");
	stack_t tmp = m->stack[m->sp];
	++m->sp;
	CHECK(m, m->sp < m->stack_size, STACK_UNDERFLOW);
	m->stack[m->sp] /= tmp;
}

typedef void (*machine_instruction_t)(struct machine_t *);
static const machine_instruction_t instruction_table[1 << sizeof(code_t) * 8] = {
	[HALT]	= &INSTRUCTION(halt),
	[NOP]	= &INSTRUCTION(nop),
	[PRINT]	= &INSTRUCTION(print),
	[INC]	= &INSTRUCTION(inc),
	[DEC]	= &INSTRUCTION(dec),
	[ADD]	= &INSTRUCTION(add),
	[SUB]	= &INSTRUCTION(sub),
	[DUP]	= &INSTRUCTION(dup),
	[PUSH]	= &INSTRUCTION(push),
	[POP]	= &INSTRUCTION(pop),
	[MUL]	= &INSTRUCTION(mul),
	[DIV]	= &INSTRUCTION(div),
};

void machine_run(struct machine_t *m) {
	while (m->ip < m->code_size) {
		machine_instruction_t i = instruction_table[m->code[m->ip++]];

		if (i == ILLEGAL) {
			fprintf(stderr, "ERROR: %s\n", machine_strerror(ILLEGAL_INSTRUCTION));
			PSTATE(m);
			break;
		}

		i(m);

		if (m->error) {
			if (m->error != HALTED) {
				fprintf(stderr, "ERROR: %s\n", machine_strerror(m->error));
				PSTATE(m);
			}
			break;
		}
	}
}
