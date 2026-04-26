# elf5_jit Test Suite

Test programs for the c5 VM JIT compiler (`elf5_jit`).
All tests are written in the subset of C that `c5` understands.

## Test Categories

### Integer & Arithmetic
- `test_arith.c`   — +, -, *, /, % operators
- `test_expr.c`    — operator precedence and complex expressions
- `test_bitops.c`  — &, |, ^, ~, <<, >>
- `test_boundary.c`— INT_MAX, INT_MIN, edge values

### Control Flow
- `test_cmp.c`     — ==, !=, <, >, <=, >= comparisons
- `test_while.c`   — while loops
- `test_for.c`     — nested for loops (emulated with while)
- `test_logical.c` — &&, ||, ! logical operators

### Functions & Recursion
- `test_func.c`    — function calls, fibonacci (recursion)
- `test_multiarg.c`— functions with multiple arguments
- `test_multiret.c`— multiple return points
- `test_nested.c`  — factorial + sum (mutual recursion patterns)
- `test_recursion2.c`— Ackermann function (deep recursion)
- `test_scope.c`   — local vs global variable scoping
- `test_minmax.c`  — nested function calls, ternary operator

### Pointers & Memory
- `test_ptr.c`     — pointer to char, array indexing
- `test_ptr2.c`    — pointer arguments, swap via pointer
- `test_array.c`   — integer arrays
- `test_string.c`  — string operations (strlen, strcmp)
- `test_string2.c` — global char array, character manipulation
- `test_struct_emu.c`— struct emulation via int arrays

### Memory Allocation
- `test_malloc.c`  — malloc, memset, free
- `test_memops.c`  — malloc, memset, memcmp, free

### I/O
- `test_char.c`    — character I/O, printf %c
- `test_open.c`    — open, read, write, close (POSIX I/O)

### Globals & Linkage
- `test_global.c`  — global variable, multiple functions

### Floating Point
- `test_float_min.c`— minimal float: store, load, print
- `test_float.c`   — +, *, /, comparisons with double
- `test_float2.c`  — function returning double, pi * r^2
- `test_float3.c`  — abs, pow (double), all 6 float comparisons
- `test_float4.c`  — global double array

### Stress Tests
- `test_stress.c`  — prime sieve (nested loops, modulo)
- `test_linked.c`  — multi-function program (typical use case)

## Usage

Build `c5`, `c5tool`, and `elf5_jit` in the parent directory, then:

```bash
make all       # compile all tests to .elf
make run       # compare JIT output against c5tool interpreter
make clean     # remove .elf files
```

Or test a single file:
```bash
../c5 -o test_arith.elf test_arith.c
../c5tool run test_arith.elf   # reference output
../elf5_jit test_arith.elf     # JIT output
```
