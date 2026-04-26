set -x
gcc py0i.c -o py0i
./py0i ../tests/hello.py
./py0i ../tests/ifwhile.py 
./py0i ../tests/fact.py 
./py0i py0i.py ../tests/fact.py 