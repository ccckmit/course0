set -x
clang -Wall -Wno-unused-parameter -o qd0c qd0c.c
./qd0c test.qd
clang test.ll qd0lib.c -o test.o -lm
./test.o
