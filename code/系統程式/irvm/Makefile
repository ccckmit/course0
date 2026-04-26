# ==========================================
# Makefile for c0computer compiler toolchain
# Builds and tests .c and .py files in _data/
# ==========================================

CLANG = clang
RV_CFLAGS = --target=riscv64 -march=rv64g -mabi=lp64d
RV0AS = ./rv0/rv0as
RV0VM = ./rv0/rv0vm
PY0C = ./py0/py0c/py0c
QD0C = ./qd0/qd0c/qd0c
LL0C = ./ll0/ll0c/ll0c
HOST_CC = cc
DATA_DIR = _data

# Get list of .c and .py files
C_FILES := $(wildcard $(DATA_DIR)/*.c)
PY_FILES := $(wildcard $(DATA_DIR)/*.py)
C_TARGETS := $(C_FILES:.c=.o)
PY_HOST_TARGETS := $(PY_FILES:.py=.host)

# Disable built-in rules
.SUFFIXES:

# Default target - build and test all C programs
all: test

# Test C programs
test: $(C_TARGETS)
	@echo ""
	@echo "=========================================="
	@echo "Testing all C programs"
	@echo "=========================================="
	@echo "Running fact (entry=0x6c)..."
	$(RV0VM) -e 0x6c $(DATA_DIR)/fact.o
	@echo "Exit code: $$?"
	@echo ""
	@echo "Running test (entry=0x0)..."
	$(RV0VM) -e 0x0 $(DATA_DIR)/test.o
	@echo "Exit code: $$?"

# Test Python programs (runs on host)
test_py: $(PY_HOST_TARGETS)
	@echo ""
	@echo "=========================================="
	@echo "Testing all Python programs (host)"
	@echo "=========================================="
	@for p in $(PY_FILES); do \
		name=$$(basename $$p .py); \
		echo "Running $$name.py..."; \
		$(DATA_DIR)/$$name.host; \
		echo "Exit code: $$?"; \
		echo ""; \
	done

# Build rule for .c files (to RISC-V)
$(DATA_DIR)/%.o: $(DATA_DIR)/%.c
	@echo "Building $* (C -> RISC-V)..."
	$(CLANG) $(RV_CFLAGS) -S $(DATA_DIR)/$*.c -o $(DATA_DIR)/$*.s
	$(RV0AS) $(DATA_DIR)/$*.s -o $(DATA_DIR)/$*.o

# Build rule for .py files (to qd)
$(DATA_DIR)/%.qd: $(DATA_DIR)/%.py
	@echo "Compiling $* (Python -> qd)..."
	$(PY0C) $< -o $@

# Build rule for .qd files (to LLVM IR)
$(DATA_DIR)/%.ll: $(DATA_DIR)/%.qd
	@echo "Translating $* (qd -> LLVM IR)..."
	$(QD0C) $< -o $@

# Build Python programs for host (for testing)
$(DATA_DIR)/%.host: $(DATA_DIR)/%.ll
	@echo "Compiling $* (LLVM IR -> host)..."
	$(HOST_CC) $< qd0/qd0c/qd0lib.c -o $@ -lm

# Run a specific C program: make run name=fact
run:
	@if [ "$(name)" = "fact" ]; then \
		$(RV0VM) -e 0x6c $(DATA_DIR)/$(name).o; \
	else \
		$(RV0VM) -e 0x0 $(DATA_DIR)/$(name).o; \
	fi

# Run a specific Python program on host
run_py:
	$(DATA_DIR)/$(name).host

# Build specific file: make fact
%:
	@if [ -f $(DATA_DIR)/$@.c ]; then \
		echo "Building $@ (C -> RISC-V)..."; \
		$(CLANG) $(RV_CFLAGS) -S $(DATA_DIR)/$@.c -o $(DATA_DIR)/$@.s; \
		$(RV0AS) $(DATA_DIR)/$@.s -o $(DATA_DIR)/$@.o; \
	elif [ -f $(DATA_DIR)/$@.py ]; then \
		echo "Building $@ (Python -> qd -> LLVM IR -> host)..."; \
		$(PY0C) $(DATA_DIR)/$@.py -o $(DATA_DIR)/$@.qd; \
		$(QD0C) $(DATA_DIR)/$@.qd -o $(DATA_DIR)/$@.ll; \
		$(HOST_CC) $(DATA_DIR)/$@.ll qd0/qd0c/qd0lib.c -o $(DATA_DIR)/$@.host -lm; \
	else \
		echo "No such file: $(DATA_DIR)/$@.c or $(DATA_DIR)/$@.py"; \
		exit 1; \
	fi

# Clean generated files
clean:
	rm -f $(DATA_DIR)/*.ll $(DATA_DIR)/*.s $(DATA_DIR)/*.o $(DATA_DIR)/*.qd $(DATA_DIR)/*.host

.PHONY: all test test_py run run_py clean
