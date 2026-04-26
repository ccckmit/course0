CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra

SRCS = main.c lexer.c parser.c ast.c codegen.c macro.c
OBJS = $(SRCS:.c=.o)

all: c0c

c0c: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) c0c

.PHONY: all clean