./verilog0c v/$1.v -o v/$1.c
gcc v/$1.c -o v/$1
./v/$1
