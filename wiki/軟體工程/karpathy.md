# Andrej Karpathy

## 概述

Andrej Karpathy 是 AI 領域的重要研究者、教育者和工程師。
- Stanford 畢業
- 曾任 Tesla Autopilot 總監
- OpenAI 創始成員（2015-2017）
- 169k followers on GitHub
- Twitter: @karpathy

喜歡在大規模數據集上訓練深度神經網路。

## 主要貢獻

### 1. LLM Wiki 模式

[LLM Wiki](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f)（5k+ stars）

一個讓 LLM 增量構建和維護持久知識庫的模式。

```
核心想法：
- 不是簡單的 RAG（檢索後回答）
- LLM 增量構建和維護持久 wiki
- 知識是編譯一次，持續更新
- wiki 是會累積的工件，跨參考已存在
```

**三層架構**：
```
1. Raw Sources     → 原始文档（不可變）
2. The Wiki      → LLM 生成的 markdown 文件
3. The Schema   → CLAUDE.md / AGENTS.md（配置）
```

**三種操作**：
- **Ingest**：攝取新來源，更新 wiki 頁面
- **Query**：查詢 wiki，綜合答案
- **Lint**：健康檢查，標記矛盾和過時內容

### 2. Karpathy 編碼指南

[andrej-karpathy-skills](https://github.com/forrestchang/andrej-karpathy-skills)（43.9k stars）

基於 Karpathy 對 LLM 編碼陷阱的觀察，四原則解決方案：

| 原則 | 解決問題 |
|------|----------|
| **Think Before Coding** | 錯誤假設、隱藏困惑 |
| **Simplicity First** | 過度設計、膨脹抽象 |
| **Surgical Changes** | 正交編輯、touch 不該動的代碼 |
| **Goal-Driven Execution** | 測試先行、可驗證成功標準 |

**Karpathy 原話**：
> "模型會幫你做錯誤的假設卻不檢查。它們不管理困惑，不尋求澄清，不呈現不一致，不提出取捨，不在應該時反對。"
> "它們喜歡過度複雜化代碼和 API，膨脹抽象，不改變清理死代碼..."

### 3. microGPT

[MicroGPT](https://karpathy.github.io/2026/02/12/microgpt/)

小型 AI 代理的最小實現：
- 簡化的 prompt
- 工具調用
- 循環直到目標達成

## Karpathy 的核心觀點

### 關於 LLM 編碼

```
1. 模型會默默選擇一種解釋然後執行，不聲明假設
2. 模型總是過度設計，代碼膨脹
3. 模型有時會改變它們不夠理解的注釋和代碼
4. 停止時要說明困惑，不要假裝理解
```

### 關於 wiki 作為知識庫

```
- 知識累積而非蒸發
- 每次問問題不需重新發現
- 交叉引用已存在
- 矛盾已被標記
- 綜合反映你看過的一切
```

### 關於人類角色

```
人類工作：策劃來源、引導分析、問好問題、思考意義
LLM 工作：所有瑣事——總結、交叉引用、歸檔、記帳
```

---

## 四原則詳解

### 1. Think Before Coding

**不要假設。不要隱藏困惑。呈現取捨。**

- 明確陳述假設 — 不確定時询问而非猜測
- 呈現多種解釋 — 模糊時不默默選擇
- 在應該時反對 — 如果有更簡單的方法，說出來
- 困惑時停止 — 說明什麼不清楚並請求澄清

### 2. Simplicity First

**最少代碼解決問題。不做推測。**

- 不要功能超出要求
- 單用途代碼不做抽象
- 不要「靈活性」或「可配置性」未被要求
- 不要不可能場景的錯誤處理
- 200 行可變 50 行則重寫

### 3. Surgical Changes

**只 touch 必須的。清理自己的爛攤子。**

- 不要「改進」相鄰代碼、注釋或格式
- 不要重構沒壞的東西
- 匹配現有風格，即使你會不同
- 注意到無關死代碼，提及但不刪除

當你的更改造成孤兒：
- 刪除你的更改造成的未使用 import/變數/函式
- 不要刪除預先存在的死代碼除非被要求

### 4. Goal-Driven Execution

**定義成功標準。循環直到驗證。**

把命令式任務轉換為可驗證目標：

| 不要說... | 轉換為... |
|-----------|-----------|
| "添加驗證" | 為無效輸入寫測試，然後讓它們通過 |
| "修復 bug" | 寫一個复现 bug 的測試，然後讓它通過 |
| "重構 X" | 确保重構前後測試都通過 |

多步驟任務陳述簡短計劃：
```
1. [步驟] → 驗證:[檢查]
2. [步驟] → 驗證:[檢查]
3. [步驟] → 驗證:[檢查]
```

---

## 安裝 Karpathy 指南

### Option A: Claude Code Plugin（推薦）

```
/plugin marketplace add forrestchang/andrej-karpathy-skills
/plugin install andrej-karpathy-skills@karpathy-skills
```

### Option B: CLAUDE.md（每專案）

```bash
# 新專案
curl -o CLAUDE.md https://raw.githubusercontent.com/forrestchang/andrej-karpathy-skills/main/CLAUDE.md

# 現有專案
echo "" >> CLAUDE.md
curl https://raw.githubusercontent.com/forrestchang/andrej-karpathy-skills/main/CLAUDE.md >> CLAUDE.md
```

---

## 運作跡象

這些指南在運作時你會看到：
- **diff 中不必要的更改減少** — 只有請求的更改出現
- **因過度設計導致的重寫減少** — 代碼第一次就簡單
- **澄清問題在實作前到來** — 而非錯誤後
- **最小、乾淨的 PR** — 沒有附带重構或「改進」

---

## 相關資源

- [LLM Wiki Gist](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f)
- [Karpathy Skills](https://github.com/forrestchang/andrej-karpathy-skills)
- [microGPT](https://karpathy.github.io/2026/02/12/microgpt/)
- [Tesla Autopilot](https://github.com/AutoPascal/AutoGPT-Hack)
- [karpathy YouTube](https://www.youtube.com/@AndrejKarpathy)

## 知名專案（Pinned）

| 專案 | Stars | 說明 |
|------|------|------|
| [nanoGPT](https://github.com/karpathy/nanoGPT) | 56.7k | 訓練/微調中大型 GPT 的最簡單、最快代碼庫 |
| [nanochat](https://github.com/karpathy/nanochat) | 51.9k | $100 能買到的最佳 ChatGPT |
| [llm.c](https://github.com/karpathy/llm.c) | 29.6k | 用簡單的 C/CUDA 訓練 LLM |
| [llama2.c](https://github.com/karpathy/llama2.c) | 19.4k | 用純 C 在一個文件中運行 Llama 2 推論 |
| [micrograd](https://github.com/karpathy/micrograd) | 15.5k | 一個微型的 autograd 引擎和神經網路庫 |
| [microgpt](https://gist.github.com/karpathy/8627fe009c40f57531cb18360106ce95) | - | 用純 Python 無依賴訓練 GPT 的最原子化方法 |

## 其他專案

- [GPT-2](https://github.com/karpathy/gpt-2) - GPT-2 簡單實現
- [char-rnn](https://github.com/karpathy/char-rnn) - 字元級 RNN
- [Tesla Autopilot](https://github.com/AutoPascal/AutoGPT-Hack)

## 相關資源

- [GitHub](https://github.com/karpathy)
- [Twitter](https://twitter.com/karpathy)
- [YouTube](https://www.youtube.com/@AndrejKarpathy)
- [LLM Wiki Gist](https://gist.github.com/karpathy/442a6bf555914893e9891c11519de94f)
- [Karpathy Skills](https://github.com/forrestchang/andrej-karpathy-skills)

## Tags

#AndrejKarpathy #AI #LLM #Wiki #Prompt工程 #Claude #nanoGPT #深度學習