set -x
make clean
make
./keygen0 -b 512 -f test_key
ls -la test_key test_key.pub
