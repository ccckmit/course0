---
name: test-script-generator
description: "Generate minimal test.sh scripts using set -x"
risk: low
source: community
date_added: "2026-04-24"
---

## Use this skill when

- User asks to create test.sh for a project

## Test Script Style

```bash
#!/bin/bash
set -x

make clean
make
./[executable] [arguments] || true

ls -la [output files]
```

## Build Options

### If Makefile exists
```bash
make clean
make
```

### If no Makefile
```bash
gcc -o [output] [source.c] -I include
```

## Example

```bash
#!/bin/bash
set -x

make clean
make
./keygen0 -b 512 -f test_key

ls -la test_key test_key.pub
```

## Key Points

- Use `set -x` for trace
- No cd needed, run from project directory
- Minimal output, no echo
- Use `|| true` if exit code doesn't matter
- Keep it simple and short