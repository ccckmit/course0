CC ?= cc
CFLAGS ?= -O0 -g -Wall -Wextra

SRCS = main.c lexer.c parser.c ast.c codegen.c macro.c
OBJS = $(SRCS:.c=.o)

all: c0c

c0c: $(OBJS) c0c_compat.o
	$(CC) $(CFLAGS) -o $@ $(OBJS) c0c_compat.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) c0c c0c_compat.o

.PHONY: all clean
