 set -x
 ./c0c $1.c -o $1.ll
 clang $1.ll -o $1.o
 ./$1.o
echo $?