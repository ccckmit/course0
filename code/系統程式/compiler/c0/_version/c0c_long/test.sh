#!/bin/sh
set -x

make clean
make

cc -O0 -g -c c0c_compat.c -o c0c_compat.o

./c0c -c main.c    -o main.ll
./c0c -c macro.c   -o macro.ll
./c0c -c parser.c  -o parser.ll
./c0c -c codegen.c -o codegen.ll
./c0c -c lexer.c   -o lexer.ll
./c0c -c ast.c     -o ast.ll

clang -g lexer.ll ast.ll codegen.ll parser.ll macro.ll main.ll \
    c0c_compat.o -o c0c2

echo "=== Stage 2: c0c2 compiles itself ==="

cat > /tmp/lldb_cmds.txt << 'LLDBEOF'
run -c ast.c -o ast2.ll
bt all
frame select 1
frame select 2
frame select 3
register read x0 x1 x2 x3
quit
LLDBEOF

echo "--- ast.c ---"
./c0c2 -c ast.c -o ast2.ll || (lldb --batch -s /tmp/lldb_cmds.txt ./c0c2 2>&1; exit 1)

echo "--- lexer.c ---"
./c0c2 -c lexer.c -o lexer2.ll

echo "--- main.c ---"
./c0c2 -c main.c -o main2.ll

echo "--- macro.c ---"
./c0c2 -c macro.c -o macro2.ll

echo "--- parser.c ---"
./c0c2 -c parser.c -o parser2.ll

echo "--- codegen.c ---"
./c0c2 -c codegen.c -o codegen2.ll

if [ -f ast2.ll ] && [ -f lexer2.ll ] && [ -f main2.ll ] && \
   [ -f macro2.ll ] && [ -f parser2.ll ] && [ -f codegen2.ll ]; then
    clang lexer2.ll ast2.ll codegen2.ll parser2.ll macro2.ll main2.ll \
        c0c_compat.o -o c0c3
    echo "=== Self-hosting test ==="
    ./c0c3 -v
else
    echo "=== Some stage-2 files missing — check errors above ==="
fi
