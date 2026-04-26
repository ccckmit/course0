# clang -S -emit-llvm fact.c -o fact.ll
../../c0/c0c/c0c fact.c -o fact.ll
clang fact.ll -o fact
./fact
echo $?
# clang ll0i.c ../ll0.c -o ll0i
./ll0i fact.ll
