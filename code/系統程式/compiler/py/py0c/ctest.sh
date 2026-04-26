set -x
gcc qd0vm.c -o qd0vm
./qd0vm ../tests/hello.qd
./qd0vm ../tests/ifwhile.qd 
./qd0vm ../tests/fact.qd 
#./qd0vm py0i.py ../tests/fact.py 