CFLAGS	+= -std=c99 -Wall -pedantic -D_XOPEN_SOURCE=700
SRCS	:= machine.c compiler.c tokenizer.c main.c
OBJS	:= $(SRCS:.c=.o)
DEPS	:= machine.h compiler.h
EXEC	:= machine

vpath %.c src
vpath %.h src

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS)

.PHONY: clean
clean:
	rm -f *.o

.PHONY: test
test: all 
	./$(EXEC) masc.txt
