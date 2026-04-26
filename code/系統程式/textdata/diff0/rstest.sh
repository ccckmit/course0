set -x
rustc diff.rs -o diff
./diff test/file1.txt test/file2.txt
