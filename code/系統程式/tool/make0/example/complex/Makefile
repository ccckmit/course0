CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g
AR = ar
ARFLAGS = rcs

SRCDIR = src
OBJDIR = obj
LIBDIR = lib
BINDIR = bin

SOURCES = $(SRCDIR)/calc.c $(SRCDIR)/utils.c $(SRCDIR)/main.c
OBJECTS = $(OBJDIR)/calc.o $(OBJDIR)/utils.o $(OBJDIR)/main.o
LIB = $(LIBDIR)/libcalc.a
TARGET = $(BINDIR)/calculator

.PHONY: all clean rebuild lib test run makedirs

all: makedirs $(LIB) $(TARGET)

makedirs:
	mkdir -p $(OBJDIR) $(LIBDIR) $(BINDIR)

$(LIB): $(OBJDIR)/calc.o $(OBJDIR)/utils.o
	ar $(ARFLAGS) $@ $^

$(OBJDIR)/calc.o: $(SRCDIR)/calc.c include/calc.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/calc.c -o $@

$(OBJDIR)/utils.o: $(SRCDIR)/utils.c include/utils.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/utils.c -o $@

$(OBJDIR)/main.o: $(SRCDIR)/main.c include/calc.h include/utils.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c -o $@

$(TARGET): $(LIB) $(OBJDIR)/main.o
	$(CC) $(CFLAGS) $(OBJDIR)/main.o $(LIB) -o $@

test: $(TARGET)
	./$(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJDIR)/*.o $(LIB) $(TARGET)

rebuild: clean all
