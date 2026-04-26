CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -I./src
SRCDIR  = src

.PHONY: all clean index

all: index query

index: $(SRCDIR)/index.c $(SRCDIR)/fulltext0.c $(SRCDIR)/fulltext0.h
	$(CC) $(CFLAGS) -o index $(SRCDIR)/fulltext0.c $(SRCDIR)/index.c

query: $(SRCDIR)/query.c $(SRCDIR)/fulltext0.c $(SRCDIR)/fulltext0.h
	$(CC) $(CFLAGS) -o query $(SRCDIR)/fulltext0.c $(SRCDIR)/query.c

build-index: index
	./index $(CORPUS)

clean:
	rm -f index query _index/data.idx
