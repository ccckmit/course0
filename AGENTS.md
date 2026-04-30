# course0 - 陳鍾誠的課程教材

## Structure
```
course0/
├── code/           # 程式碼範例 (每個科目/專案獨立，有自己的 pyproject.toml 或 package.json)
├── wiki/           # LLM 知識庫 (使用 wiki/AGENTS.md 中的 LLM Wiki schema)
├── lecture/        # 課程講稿
└── _more/          # 補充教材
```

## Key Facts
- **NOT a software project**: No root-level build/test/lint commands
- **Each `code/[subject]/[project]/` is independent** - no cross-project dependencies
- **Wiki path is `wiki/`, not `_wiki/`** (wiki/AGENTS.md still contains stale `_wiki/` refs in its architecture diagram)

## Workflow
1. Ask user which subject to work on (don't assume)
2. Check `code/[subject]/` for code, `lecture/[subject]/` for notes, `wiki/` for knowledge base
3. Check for `AGENTS.md` in project subdirectories for specific guidance

## Sub-AGENTS.md Files
- `code/系統程式/os/mini-riscv-os2/` - RISC-V OS tutorial (stages 01-09, uses `riscv64-unknown-elf-gcc`)
- `code/系統程式/os/xv7/` - xv7 OS implementation
- `code/系統程式/database/sql0/` - SQL examples
- `code/系統程式/crpyto/ssl0/` - SSL/TLS (note: directory misspelled "crpyto")
- `code/網站設計/05-form/blog1form/` - Form handling
- `code/網站設計/05-form/blog4form/` - Form handling
- `code/程式與數學/_ai/lean4py/` - Python formal math library (install: `pip install -e .`, test: `pytest tests/`)

## Common Commands
```bash
cd code/[subject]/[project] && python main.py
cd code/[subject]/[project] && python -m pytest
cd code/[subject]/[project] && npm run dev
```

## CI / Deploy (`.github/workflows/`)
| Workflow | Trigger | Purpose |
|----------|---------|---------|
| `astro.yml` | push to main | Deploy Astro site (looks for root package.json) |
| `hugo.yml` | push to main | Deploy Hugo site (requires hugo_extended + Dart Sass) |
| `mdbook.yml` | push to main | Deploy mdBook site (runs `mdbook build`, uploads `./book`) |
| `static.yml` | push to main | Deploy static content (uploads entire repo as-is) |

## Wiki Operations (see wiki/AGENTS.md for full schema)
- **Ingest**: Add source to `wiki/source/`, then ask LLM to ingest
- **Query**: Search `wiki/` for existing knowledge
- **Lint**: Check for stale info, broken links, gaps