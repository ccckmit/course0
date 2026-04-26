set -x
gcc diff.c -o diff
./diff test/file1.txt test/file2.txt
