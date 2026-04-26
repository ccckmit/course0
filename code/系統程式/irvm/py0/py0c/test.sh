set -x
clang -g -fsanitize=address py0c.c -o py0c
./py0c fact.py -o fact.qd
