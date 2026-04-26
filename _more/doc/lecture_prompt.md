本專案 course0 是陳鍾誠在金門大學的課程教材，結構如下

* wiki/ : 
    * 受 [Karpathy LLM Wiki](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f) 啟發，專為課程撰寫的 wiki
* code/ : 
    * 原本稱為 computer0 專案，主要將電腦最重要的一些技術，從作業系統到程式語言，從影像到 web 到 AI 等等，全部實作 DIY 一遍的專案。
    * 主要語言：C + Python，有時會多加上 Rust。
* lecture/ : 
    * 課程講稿。

在撰寫 lecture/ 下的文章 .md 檔案時，請儘量交叉引用，例如寫 lecture/ 時，若 code/ 中有適當的範例，或者 wiki/ 中有適當的文章，就引用進來，不用再重複寫。

現在請在 lecture/ ，寫出這六門課程的 課程 lecture，每門課程 xxx 都先寫 xxx/README.md 讓我確認，架構沒問題之後，再開始寫內容。

## 補充：

1. 計算機結構：
    * 主要採用 nand2tetris https://www.nand2tetris.org/, 搭配 Verilog 實作。
2. 系統程式：
    * 主要採用 C 語言 (偶爾會用 python, rust 實作)
3. 機器學習：
    * 主要採用 python + sklearn + pytorch + gym 。
4. 演算法：
    * 主要採用 python
4. 網站設計：
    * 主要採用 node.js + sqlite + HTML/CSS/JavaScript (有時搭配 rust, fastapi)
5. 軟體工程：
    * 特別強調 AI 導向的軟體工程，包含 git, github, 測試, 敏捷開發
    * 涵蓋：prompt, context, harness engineering
    * 區分：1. 單人+AI 模式 2. 大團隊合作模式