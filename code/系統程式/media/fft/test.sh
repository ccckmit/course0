#!/bin/bash

echo "=== DFT Test Suite ==="
echo

echo "1. Test sine signal (N=16) - magnitude:"
./sft -n 16 -s sine -m
echo

echo "2. Test square signal (N=16) - magnitude:"
./sft -n 16 -s square -m
echo

echo "3. Test sawtooth signal (N=16) - magnitude:"
./sft -n 16 -s sawtooth -m
echo

echo "4. Test impulse signal (N=8) - complex output:"
./sft -n 8 -s impulse -c
echo

echo "5. Test IDFT of impulse (should give delta-like pattern):"
./sft -n 8 -s impulse -i
echo

echo "6. Test with phase output (N=16, sine):"
./sft -n 16 -s sine -p
echo

echo "7. Test IDFT from file (real values only):"
echo "1.0
0.0
-1.0
0.0
1.0
0.0
-1.0
0.0" > /tmp/test_real.txt
./sft -n 8 -f /tmp/test_real.txt -i
echo

echo "8. Compare DC component of different signals:"
echo "Sine DC:   $(./sft -n 64 -s sine -m | head -2 | tail -1)"
echo "Square DC: $(./sft -n 64 -s square -m | head -2 | tail -1)"
echo "Sawtooth DC: $(./sft -n 64 -s sawtooth -m | head -2 | tail -1)"
echo

echo "=== All tests completed ==="