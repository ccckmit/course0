CC     = gcc
CFLAGS = -w -g -O1 -fno-omit-frame-pointer

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Detect JIT arch flag: macOS arm64 or Linux aarch64 -> ARM64, else x86
ifeq ($(UNAME_M),arm64)
  JIT_ARCH = JIT_ARM64
  JIT_CC   = cc
  JIT_FLAGS = -w -g -O1 -fsanitize=address -fno-omit-frame-pointer
else ifeq ($(UNAME_M),aarch64)
  JIT_ARCH = JIT_ARM64
  JIT_CC   = gcc
  JIT_FLAGS = -w -g -O1 -fno-omit-frame-pointer
else
  JIT_ARCH = JIT_X86
  JIT_CC   = gcc
  JIT_FLAGS = -w -g -O0
endif

.PHONY: all test test_linker selfhost test_jit test_all clean

# Default: build compiler + toolchain
all: c5 c5_tool

# Compiler: frontend (c5.c) + ELF backend (c5_elf.c)
c5: c5.c c5_elf.c
	$(CC) $(CFLAGS) c5.c c5_elf.c -o c5

# Toolchain: VM runner + static linker (standalone)
c5_tool: c5_tool.c
	$(CC) $(CFLAGS) c5_tool.c -o c5_tool

# JIT executor: single source, arch selected at compile time
jit: jit.c jit_common.h
	$(JIT_CC) $(JIT_FLAGS) -D$(JIT_ARCH) jit.c -o jit

# ---- Tests -----------------------------------------------------------------

test: all
	@echo "--- 基本編譯與 VM 測試 ---"
	./c5 -s test/fib.c    || true
	./c5    test/fib.c    || true
	./c5    test/hello.c  || true
	./c5    test/float.c  || true
	./c5 -o test/fib.elf  test/fib.c
	@echo "--- 基本測試完成 ---"

test_linker: all
	@echo "--- 靜態連結測試 ---"
	./c5 -o test/lib_math.elf  test/lib_math.c
	./c5 -o test/main_ext.elf  test/main_ext.c
	./c5_tool link -o test/run.elf test/lib_math.elf test/main_ext.elf
	./c5_tool run  test/run.elf || true
	@echo "--- 靜態連結測試完成 ---"

selfhost: all
	@echo "--- Self-hosting 自我編譯測試 ---"
	./c5 -o c5.elf      c5.c
	./c5 -o c5_elf.elf  c5_elf.c
	./c5_tool link -o c5_final.elf c5.elf c5_elf.elf
	./c5 -o c5_tool.elf c5_tool.c
	./c5_tool run c5_final.elf -o test/fib2.elf test/fib.c      || true
	./c5_tool run c5_tool.elf  run test/fib2.elf            || true
	@echo "--- 自我編譯完成 ---"

test_jit: all jit
	@echo "--- JIT 測試 ($(JIT_ARCH)) ---"
	./c5 -o test/hello.elf test/hello.c
	./c5 -o test/fib.elf   test/fib.c
	./c5 -o test/float.elf test/float.c
	./jit test/hello.elf || true
	./jit test/fib.elf   || true
	./jit test/float.elf || true
	@echo "--- JIT 測試完成 ---"

test_all: test test_linker selfhost test_jit

# ---- Clean -----------------------------------------------------------------
clean:
	rm -rf c5 c5_tool jit *.elf test/*.elf *.dSYM
	@echo "清理完成"
