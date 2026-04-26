set -x
gcc verilog0c.c -o verilog0c
./vrun.sh halfadder
./vrun.sh comparator
./vrun.sh fulladder
./vrun.sh mux2to1

