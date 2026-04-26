set -x
rustc qd0vm.rs
./qd0vm ../tests/hello.qd
./qd0vm ../tests/ifwhile.qd 
./qd0vm ../tests/fact.qd 
#./qd0vm py0i.py ../tests/fact.py 