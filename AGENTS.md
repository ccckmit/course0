# course0 - AGENTS.md

Repository for 金門大學資訊工程系-陳鍾誠的課程教材

## Structure

```
course0/
├── code/           # Course code examples (subjects: 演算法, 系統程式, 計算機結構, 軟體工程, 網站設計, 機器學習)
├── wiki/           # LLM-based knowledge wiki
├── lecture/        # Lecture materials
└── README.md       # 2-line description
```

## Key Points

- **NOT a software project**: No build/test/lint commands at root level
- **Each subdirectory is independent**: Code examples are standalone projects, each may have its own `AGENTS.md` and `package.json`
- **Primary purpose**: Educational content storage, not active development
- **Wiki is `wiki/`** (not `_wiki/`)

## Finding Project-Specific Guidance

For code projects in subdirectories, check local `AGENTS.md` files:
- `code/網站設計/nodejs/blog1form/AGENTS.md` - Blog app with Express + SQLite
- `code/網站設計/nodejs/blog4form/AGENTS.md`
- `code/系統程式/textdata/os/xv7/AGENTS.md`

## Working in This Repo

1. Ask user which course/subject to work on
2. Find relevant code in `code/[subject]/` directory
3. Find lecture notes in `lecture/[subject]/` directory
4. Each subdirectory is independent - no cross-project dependencies
5. No root-level CI, tests, or build scripts