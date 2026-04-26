set -x
rustc py0i.rs
./py0i ../tests/hello.py
./py0i ../tests/ifwhile.py 
./py0i ../tests/fact.py 
./py0i py0i.py ../tests/fact.py 