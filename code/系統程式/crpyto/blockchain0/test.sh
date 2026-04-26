set -x
gcc -o blockchain blockchain.c ../ssl0/src/sha.c -I../ssl0/include
./blockchain
