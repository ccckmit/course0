set -x
gcc *.c -o verilog0c
./vrun.sh halfadder
./vrun.sh comparator
./vrun.sh fulladder
./vrun.sh mux2to1
./vrun.sh mcu0m
