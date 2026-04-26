# AGENTS.md - sql0 Database Project

## Project Overview

- **Language**: C (standard C)
- **Type**: Minimalist SQL database with B+ Tree indexing
- **Persistence**: Disk-based with buffer pool (MAX_PAGES=100)
- **Entry point**: sql0.c → main() in Table* db_open()

## Build Commands

```bash
# Compile (release with optimizations)
gcc -O3 sql0.c -o sql0

# Compile (debug with symbols)
gcc -g -O0 sql0.c -o sql0

# Run the database
./sql0           # Uses mydb.db by default
./sql0 <file.db  # Specify database file
```

## Test Commands

```bash
# Run all tests (bash script)
./test.sh

# Run single test manually
# Use heredoc to feed commands:
./sql0 <<EOF
insert 1 alice alice@email.com
select 1
select_all
.exit
EOF
```

## Code Style Guidelines

### Formatting
- **Indentation**: 4 spaces (no tabs)
- **Line length**: ~80-100 characters max
- **Braces**: K&R style (opening brace on same line)
- **Functions**: Return type on same line as function name

### Naming Conventions
- **Types/Structs**: PascalCase (e.g., `Row`, `Table`, `Pager`)
- **Functions**: snake_case (e.g., `db_open`, `bptree_insert`)
- **Variables**: snake_case (e.g., `page_num`, `root_page_num`)
- **Constants/Macros**: UPPER_SNAKE_CASE (e.g., `PAGE_SIZE`, `MAX_PAGES`)
- **File names**: lowercase with .c extension (e.g., `sql0.c`)

### Imports (sorted alphabetically)
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
```

### Types
- Use `uint32_t` for integers from `<stdint.h>`
- Use `bool` from `<stdbool.h>`
- Use `int` for return codes and loop indices

### Error Handling
- Print errors with `printf("Error: ...\n")`
- Use `exit(EXIT_FAILURE)` for fatal errors
- Return early on error conditions
- No exceptions (C does not support them)

### Comments
- Use Chinese comments in code (as per project convention)
- Section headers: `// ==================`
- Group related code with comment blocks

### Memory Management
- Use `malloc()`/`free()` for dynamic allocation
- Always check `NULL` after `fopen()` and `malloc()`
- Free all allocated memory in close/destroy functions

### Project Structure
- Data structures (typedef structs)
- Pager (disk I/O)
- Database (open/close)
- B+ Tree (insert/search)
- REPL (command execution)

### Constants
```c
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define PAGE_SIZE 4096
#define MAX_PAGES 100
#define MAX_KEYS 3
#define INVALID_PAGE_NUM 0xFFFFFFFF
```

### Database Operations
```
Supported commands:
  insert <id> <username> <email>
  select <id>
  select_all
  .exit
```

### Testing Notes
- Tests verify: insert, select, persistence, B+ tree splitting, duplicate keys
- Database file: mydb.db (clean with `rm -f mydb.db`)
- Test script uses heredoc to feed input

### Key Files
- `/Users/ccc/Desktop/ccc/project/sql0/sql0.c` - Main source
- `/Users/ccc/Desktop/ccc/project/sql0/test.sh` - Test script
- `/Users/ccc/Desktop/ccc/project/sql0/mydb.db` - Default database file
- `/Users/ccc/Desktop/ccc/project/sql0/README.md` - Project readme