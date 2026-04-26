# AGENTS.md - py0i Python Interpreter

## Quick start

```bash
python py0i.py <script.py> [args...]
```

## Key files

- `py0i.py` - Main interpreter (1017 lines)
- `ast.py`, `operator.py` - Local implementations (not Python stdlib)
- `py/fact.py`, `py/hello.py` - Test scripts

## Important notes

- Uses local `ast.py` and `operator.py` instead of Python stdlib: `sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))` at line 14-16
- Supports classes (`PyClass`, `PyInstance`), closures, decorators, exceptions, imports, comprehensions, f-strings
- Entry point: `main()` at line 992, expects `sys.argv[1]` as script path

## Testing

```bash
python3 py0i.py py/fact.py
```

## Architecture

- `Environment` - scope chain management (lines 20-66)
- `Interpreter` - main class with `exec_stmt()` and `eval_expr()` methods
- `_ReturnBox`, `ReturnSignal`, `BreakSignal`, `ContinueSignal` - control flow signals
- `PyFunction`, `PyClass`, `PyInstance` - callable wrappers