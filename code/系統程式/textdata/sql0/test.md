
```sh
ccc@teacherdeiMac sql0 % ./test.sh
======================================
    sql0 (B+ Tree Database) Tester    
======================================
=> Compiling sql0.c...
Compilation successful.
=> Cleaning up old database file...

=> Running Test 1: Basic Insert and Select...
  [PASS] Select specific row.
  [PASS] Total row count.

=> Running Test 2: Disk Persistence & B+ Tree Splitting...
  [PASS] Data persisted to disk successfully.
  [PASS] B+ Tree kept rows sorted after split.

=> Running Test 3: Duplicate Primary Key Handling...
  [PASS] Duplicate key correctly rejected.

======================================
          All tests finished!           
======================================
```
