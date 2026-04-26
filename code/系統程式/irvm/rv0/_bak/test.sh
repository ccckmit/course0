#!/bin/bash
set -x

# 1. 定義路徑
LLVM_BIN=/opt/homebrew/opt/llvm/bin
CC=$LLVM_BIN/clang
OBJDUMP=$LLVM_BIN/llvm-objdump
CFLAGS="--target=riscv64 -march=rv64g -mabi=lp64d -g "

# 2. 編譯
$CC $CFLAGS -S fact.c -o fact.s
$CC $CFLAGS -c fact.c -o fact.o

# 3. 反組譯 (常用參數說明如下)
# $OBJDUMP -S fact.o > fact_dis.s
$OBJDUMP -h -d fact.o

clang rv0objdump.c -o rv0objdump
./rv0objdump -h -d fact.o

clang rv0as.c -o rv0as
./rv0as fact.s fact2.o
./rv0objdump -h -d fact2.o

clang rv0vm.c -o rv0vm
./rv0vm fact.o

