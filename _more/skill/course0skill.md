---
name: course0
description: "金門大學資訊工程系-陳鍾誠的課程教材維護技能，涵蓋 lecture、code、wiki 三大部分的規劃與編寫"
license: MIT
metadata:
  author: ccc
  version: "1.0.0"
  domain: 教學教材維護
  source: https://github.com/ccc112b/course0
---

## 專案結構

```
course0/
├── code/           # 課程程式碼範例
│   ├── 演算法/     # Python
│   ├── 系統程式/   # C (主), Python, Rust
│   ├── 計算機結構/ # Verilog
│   ├── 軟體工程/   # Git, 測試框架
│   ├── 網站設計/   # Node.js + SQLite
│   └── 機器學習/   # Python + sklearn + PyTorch + Gym
├── _wiki/          # LLM-based 知識庫 (受 Karpathy Wiki 啟發)
├── lecture/        # 課程講稿
└── README.md       # 2 行描述
```

## 核心原則

1. **不是軟體專案**：沒有 root 層級的 build/test/lint 命令
2. **每個子目錄獨立**：code/ 下的每個專案都是獨立的，可能有自己的 AGENTS.md
3. **交叉引用**：lecture/ 寫作時引用 code/ 範例和 wiki/ 文章，避免重複

## 課程科目

### 1. 計算機結構
- 主要教材：[Nand2Tetris](https://www.nand2tetris.org/)
- 使用技術：Verilog
- 子主題：數位邏輯、計算機組織、硬體描述語言、專案實作

### 2. 系統程式
- 使用技術：C (主), Python, Rust
- 子主題：編譯器、直譯器、IR+VM、OS、網路、密碼學、媒體處理、GUI、文字資料處理、工具

### 3. 機器學習
- 使用技術：Python + scikit-learn + PyTorch + OpenAI Gym
- 子主題：監督式學習、非監督式學習、機率模型、深度學習、強化學習

### 4. 演算法
- 使用技術：Python
- 子主題：基礎概念、演算法設計技巧、排序搜尋、圖形演算法、計算理論 (圖靈機、Lambda、FSM)

### 5. 網站設計
- 使用技術：Node.js + SQLite + HTML/CSS/JavaScript (主), Rust, FastAPI
- 子主題：前端基礎、後端開發、資料庫、REST API

### 6. 軟體工程
- 特色：AI 導向的軟體工程
- 開發模式：單人+AI 模式、大團隊合作模式
- 子主題：Prompt Engineering、Context Engineering、Harness Engineering、Git、敏捷開發、CI/CD

## 寫作流程

### 建立課程 README (每門課程第一個檔案)

1. 課程名稱與簡介
2. 主要教材
3. 使用技術
4. 內容架構（章節目錄）
   - 每章節列出 sub-topic
   - 引用 code/ 中的相關範例（相對路徑）
   - 引用 wiki/ 中的相關文章（相對路徑）

### 交叉引用格式

```markdown
- [code/機器學習/ml/regression/](../code/機器學習/ml/regression/) - 迴歸實作
- [Wiki: _wiki/軟體工程/Prompt工程.md](../_wiki/軟體工程/Prompt工程.md)
```

### 目錄命名

- lecture/ 下的目錄：課程名稱（與 code/ 一致）
  - 機器學習、演算法、系統程式、網站設計、計算機結構、軟體工程

## 常見任務

### 新增課程章節
1. 在 lecture/[課程]/ 下建立 .md 檔案
2. 在 README.md 目錄中加入章節連結
3. 內文引用 code/ 範例和 wiki/ 文章

### 維護 code/ 參考
- code/ 下每個獨立專案可有自己的 AGENTS.md
- 參考現有 AGENTS.md：`code/網站設計/nodejs/blog1form/AGENTS.md`