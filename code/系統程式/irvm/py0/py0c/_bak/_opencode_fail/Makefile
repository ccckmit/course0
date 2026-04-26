CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g

SRCS = src/main.c src/lexer.c src/parser.c src/ast.c src/quad.c
OBJS = $(SRCS:.c=.o)
TARGET = py0c

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
