# LLM Wiki

**來源類型**: 網頁文章/Gist
**來源路徑**: https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f
**攝取日期**: 2026-04-08

## 摘要

Andrej Karpathy 提出的 LLM Wiki 模式——一種使用 LLMs 構建和維護個人知識庫的模式。核心思想是讓 LLM 增量構建和維護一個持久的 Wiki，而非每次查詢時從頭發現知識。

## 關鍵要點

- Wiki 是持續累積的工件，而非臨時檢索
- 知識編譯一次，持續保持最新
- Schema（AGENTS.md）定義 LLM 的工作方式
- 三層架構：Raw sources → Wiki → Schema
- 操作：Ingest（攝取）、Query（查詢）、Lint（整理）

## 相關頁面

- [AGENTS.md](../../AGENTS.md)
- [index.md](../../index.md)