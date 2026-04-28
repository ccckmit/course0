# course0 - 陳鍾誠的課程教材

## Structure
```
course0/
├── code/ # 程式碼範例 (按科目分目錄)
├── wiki/ # LLM 知識庫 (NOT _wiki/ - wiki/AGENTS.md has stale refs)
├── lecture/ # 課程講稿
└── _more/ # 補充教材
```

## Subjects (code/)
| 目錄 | 語言 | 主題 |
|------|------|------|
| `code/計算機結構/` | Verilog | Nand2tetris, CPU, ALU |
| `code/系統程式/` | C, Python | 編譯器, 虛擬機, OS |
| `code/演算法/` | Python | 複雜度, 貪婪, DP |
| `code/網站設計/` | JavaScript | HTML, CSS, Node, React |
| `code/軟體工程/` | Rust, Python | AI 測試, Docker |
| `code/機器學習/` | Python | NN, 強化學習 |
| `code/程式與數學/` | Python | 代數, 微積分, 機率統計 |
| `code/計算理論/` | Python | 圖靈機, Lambda Calculus |

## Key Facts
- **NOT a software project**: No root-level build/test/lint commands
- **Each code/[subject]/ is independent** with own pyproject.toml or package.json
- **No cross-project dependencies** - Each subdirectory is self-contained

## Workflow
1. Ask user which subject to work on (don't assume)
2. Check `code/[subject]/` for code, `lecture/[subject]/` for notes, `wiki/` for knowledge base
3. Check for `AGENTS.md` in project subdirectories for specific guidance

## Sub-AGENTS.md Files
- `code/系統程式/os/mini-riscv-os2/` - RISC-V OS tutorial (stages 01-09)
- `code/系統程式/os/xv7/` - xv7 OS implementation
- `code/系統程式/database/sql0/` - SQL examples
- `code/系統程式/crpyto/ssl0/` - SSL/TLS (note: directory misspelled "crpyto")
- `code/網站設計/05-form/blog1form/` - Form handling
- `code/網站設計/05-form/blog4form/` - Form handling

## Common Commands
```bash
cd code/[subject]/[project] && python main.py
cd code/[subject]/[project] && python -m pytest
cd code/[subject]/[project] && npm run dev
```

## CI / Deploy
- `astro.yml` - Astro site (root-level or code/網站設計)
- `hugo.yml` - Hugo site (lecture/ subdirectories)
- `mdbook.yml` - mdBook site (Rust book style docs)

## Wiki
Uses LLM Wiki schema (see wiki/AGENTS.md):
- **Ingest**: Add source to `wiki/source/`, then ask LLM to ingest
- **Query**: Search `wiki/` for existing knowledge
- **Lint**: Check for stale info, broken links, gaps

## lean4py Sub-project
Located at `code/程式與數學/_ai/lean4py/` - a Python formal math library inspired by Lean/Coq.
- **Version**: 0.2.0
- **Install**: `pip install -e .` from lean4py directory
- **Test**: `pytest tests/`
- **Examples**: `python examples/01_logic.py`, etc.
- **Modules**: logic, sets, algebra, nat, tactics
- **Roadmap**: v0.1=core, v0.2=auto prover+more tactics, v0.3=number theory+linear algebra, v1.0=sympy integration