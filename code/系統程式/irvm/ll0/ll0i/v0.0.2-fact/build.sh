clang -S -emit-llvm fact.c -o fact.ll
clang fact.ll -o fact
./fact
echo $?
clang ll0i.c -o ll0i
./ll0i fact.ll
