# sql0 - Minimalist SQL Database

A lightweight, disk-based SQL database implemented in pure C with B+ Tree indexing.

## Features

- **B+ Tree Indexing**: Efficient range queries with sorted leaf nodes linked in a chain
- **Disk Persistence**: Data survives program restarts via Pager (buffer pool)
- **Buffer Pool**: In-memory cache (MAX_PAGES=100) reduces disk I/O
- **Primary Key Index**: Fast O(log n) lookup by ID
- **ACID-lite**: Inherent transactional properties through atomic page writes

## Build

```bash
# Release build (optimized)
gcc -O3 sql0.c -o sql0

# Debug build (with symbols)
gcc -g -O0 sql0.c -o sql0
```

## Usage

```bash
# Start database
./sql0           # Uses mydb.db
./sql0 <dbfile>  # Specify custom database file
```

### Commands

```
insert <id> <username> <email>    Insert a row
select <id>                       Query by primary key
select_all                        Display all rows
.exit                            Save and exit
```

### Example Session

```
$ ./sql0
sql1> insert 1 alice alice@email.com
Executed.
sql1> insert 2 bob bob@email.com
Executed.
sql1> select 1
(1, 'alice', 'alice@email.com')
sql1> select_all
(1, 'alice', 'alice@email.com')
(2, 'bob', 'bob@email.com')
Total rows: 2
sql1> .exit
Database saved. Bye.
```

## Architecture

```
┌─────────────────────────────────────┐
│           REPL Layer                │  Command parsing & output
├─────────────────────────────────────┤
│         B+ Tree Layer               │  Insert, search, split logic
├─────────────────────────────────────┤
│      Table / Meta Page              │  root_page_num storage
├─────────────────────────────────────┤
│       Pager (Buffer Pool)           │  4KB page cache & disk I/O
├─────────────────────────────────────┤
│         mydb.db                     │  Persistent storage
└─────────────────────────────────────┘
```

### Key Structures

- **Row**: User data (id, username, email)
- **Node**: B+ Tree node (keys, child pointers or row data)
- **Pager**: Manages pages 0-N with buffer pool
- **Table**: Wrapper holding pager + root_page_num

## Test

```bash
# Run all tests
./test.sh

# Manual test
./sql0 <<EOF
insert 1 test test@email.com
select 1
select_all
.exit
EOF
```

### Test Output

```
======================================
    sql0 (B+ Tree Database) Tester    
======================================
=> Compiling sql0.c...
  [PASS] Select specific row.
  [PASS] Total row count.
  [PASS] Data persisted to disk successfully.
  [PASS] B+ Tree kept rows sorted after split.
  [PASS] Duplicate key correctly rejected.
======================================
          All tests finished!           
======================================
```

## Files

| File | Description |
|------|-------------|
| `sql0.c` | Main source (398 lines) |
| `test.sh` | Test script |
| `mydb.db` | Default database file |
| `AGENTS.md` | Developer documentation |

## License

MIT License