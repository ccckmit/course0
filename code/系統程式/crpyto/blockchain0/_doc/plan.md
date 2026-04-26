# Simple Blockchain System Plan

## 1. Project Overview
- **Project Name**: Simple Blockchain
- **Type**: C program with test script
- **Core Functionality**: A minimal blockchain implementation demonstrating block structure, hash calculation, and chain validation
- **Target Users**: Developers learning blockchain concepts

## 2. Files Structure
```
blockchain/
├── _doc/plan.md
├── blockchain.c
└── test.sh
```

## 3. Core Features

### 3.1 Block Structure
- `index`: Block number (0, 1, 2, ...)
- `timestamp`: Creation time
- `data`: Transaction data (string)
- `previous_hash`: Hash of previous block
- `hash`: Current block's hash

### 3.2 Hash Function
- SHA-256 based hash calculation
- Combines: index + timestamp + data + previous_hash

### 3.3 Blockchain Operations
- `create_block()`: Create new block with data
- `calculate_hash()`: Calculate block hash
- `is_chain_valid()`: Validate entire chain
- `print_block()`: Display block info

### 3.4 Demo
- Create genesis block
- Add several blocks
- Validate chain integrity

## 4. Test Script (test.sh)
- Compile blockchain.c
- Run the program
- Verify output contains expected blocks and validation

## 5. Build & Run
```bash
gcc -o blockchain blockchain.c -lcrypto
./blockchain
```

## 6. Acceptance Criteria
- [ ] Compiles without errors
- [ ] Genesis block created automatically
- [ ] New blocks can be added with data
- [ ] Each block contains valid previous_hash reference
- [ ] Chain validation returns true for valid chain
- [ ] Program runs and displays blockchain state