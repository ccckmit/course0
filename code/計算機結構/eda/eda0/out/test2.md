# EDA Flow Report for `test2.v`

## 1. RTL to Logic Synthesis
Extracting boolean equations from Verilog:
```verilog
z = (a & b) | (a & ~b)
```

## 2. Technology Independent Synthesis
Simplifying boolean equations (using Quine-McCluskey / Sympy):
```text
z = a
```

## 3. Technology Dependent Synthesis
Mapping to Standard Cell Library (AND, OR, NOT, XOR):
```text
Net 'z' requires cells: BUF
```

## 4. Placement
Placing standard cells onto chip rows.

![Placement](images/test2_placement.png)

## 5. Global Routing
Allocating routing resources and determining coarse paths.

![Global Routing](images/test2_global.png)

## 6. Detailed Routing
Drawing exact metal traces, vias, and pin connections.

![Detailed Routing](images/test2_detailed.png)

