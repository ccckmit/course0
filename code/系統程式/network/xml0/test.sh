#!/bin/bash

echo "=== Building XML Parser ==="
gcc -o xml_test main.c xml_parser.c

echo ""
echo "=== Running Tests ==="
echo ""
./xml_test

echo ""
echo "=== Test Complete ==="