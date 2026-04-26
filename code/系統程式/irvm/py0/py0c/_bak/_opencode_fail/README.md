# py0c

A minimal compiler for `py0` (a simplified Python) that targets LLVM IR (`.ll`).

## v0.1 Scope (current)
The goal of v0.1 is to establish the project skeleton and a clear language spec
for a small, static subset of Python-like syntax. The compiler does not need to
emit LLVM IR yet in this stage.

### Language subset (v0.1)
- Single file program with function definitions
- `def` functions with positional parameters
- Statements: assignment, `return`, `if`/`else`, `while`, `pass`
- Expressions: integer literals, identifiers, arithmetic, comparisons
- Types: only `int` (static, no dynamic typing yet)

See: `doc/ebnf_v0.1_basic.md`

## Planned roadmap (high level)
- v0.2 Lexer/Parser/AST with tests
- v0.3 Type checking and LLVM IR emission (`.ll`)
- v0.4 Builtins (`print`) and sample programs
- v0.5 More control flow (`for`, `break`, `continue`)

## Project layout (planned)
- `doc/` Language specs and notes
- `src/` Compiler source (lexer/parser/ast/typecheck/codegen)
- `tests/` Sample programs and expected outputs

## CLI (planned)
```
py0c <file.py> -o <file.ll>
```
