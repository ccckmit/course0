set -x
make clean
make
# ./c0c.sh c/fact
./c0c.sh c/fact2

./c0c lexer.c -o lexer.ll
./c0c ast.c -o ast.ll
./c0c c0c_compat.c -o c0c_compat.ll
./c0c macro.c -o macro.ll
./c0c parser.c -o parser.ll
./c0c codegen.c -o codegen.ll
./c0c main.c -o main.ll
clang main.ll codegen.ll parser.ll macro.ll c0c_compat.ll lexer.ll -o c0c2
./c0c2.sh c/fact2