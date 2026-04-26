# Makefile for elf5_jit test suite
# Usage:
#   make all       - compile all .c tests to .elf with c5
#   make run       - run all tests, compare elf5_jit vs c5tool
#   make clean     - remove .elf files

C5     ?= ../c5
C5TOOL ?= ../c5tool
JIT    ?= ../elf5_jit

SRCS := $(wildcard *.c)
ELFS := $(SRCS:.c=.elf)

.PHONY: all run clean

all: $(ELFS)

%.elf: %.c
	$(C5) -o $@ $<

run: all
	@pass=0; fail=0; \
	for elf in $(ELFS); do \
	  t=$$(basename $$elf .elf); \
	  ref=$$($(C5TOOL) run $$elf 2>/dev/null); \
	  jit=$$($(JIT) $$elf 2>/dev/null); \
	  if [ "$$ref" = "$$jit" ]; then \
	    echo "PASS: $$t"; pass=$$((pass+1)); \
	  else \
	    echo "FAIL: $$t"; \
	    echo "  REF: $$(echo "$$ref" | head -3 | tr '\n' '|')"; \
	    echo "  JIT: $$(echo "$$jit" | head -3 | tr '\n' '|')"; \
	    fail=$$((fail+1)); \
	  fi; \
	done; \
	echo ""; \
	echo "=== $$pass passed, $$fail failed ==="

clean:
	rm -f *.elf
