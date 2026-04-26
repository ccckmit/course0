CC = gcc
CFLAGS = -I ../ssl0/include
SRCS = ../ssl0/src/bignum.c ../ssl0/src/rand.c

keygen0: keygen0.c $(SRCS)
	$(CC) $(CFLAGS) -o $@ $< $(SRCS)

all: keygen0

clean:
	rm -f keygen0 id_rsa id_rsa.pub

.PHONY: all clean