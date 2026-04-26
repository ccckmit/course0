# Agent 發展史

## 概述

AI Agent 的發展經歷了幾個重要階段，從早期的指令互動到現在的自主系統，工程師的角色也隨之轉變。

```
┌─────────────────────────────────────────────────────────────┐
│                    AI Agent 發展時間線                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  2020        2022        2024        2026                  │
│     │          │          │          │                       │
│  Prompt ← Context ← Harness ← Agent                       │
│  工程     工程      工程      自主                        │
│     │          │          │          │                       │
│  問什麼    給什麼   怎麼跑    自己做                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## 第一階段：Prompt 工程（2020-2022）

### 背景

大型語言模型（LLM）開始普及，工程師學習如何與模型互動。

### 核心問題

```
我應該怎麼問，AI 才能給我想要的答案？
```

### 發展

| 年份 | 里程碑 |
|------|--------|
| 2020 | GPT-3 發布，Prompt 工程概念出現 |
| 2021 | Few-shot、Chain-of-Thought 技術普及 |
| 2022 | Role-playing、Template Patterns 成熟 |

### 角色轉變

```
傳統程式設計：人類 → 程式碼 → 机器
Prompt 時代：   人类 → Prompt → LLM → 回覆
```

### 工程師技能

- 設計清晰具體的指令
- 使用 Few-shot 範例
- Chain-of-Thought 推理
- 限制輸出格式

### OpenCode 範例

```markdown
# Prompt 工程在 OpenCode 中的應用
"請寫一個 TypeScript 函式 calculateTotal：
- 輸入：number[]
- 回傳：加總
- 使用 reduce
- 包含類型註解"
```

### 局限性

Prompt 再長也裝不下整個上下文，模型很快就會「忘記」。

## 第二階段：Context 工程（2022-2024）

### 背景

單輪問答無法滿足複雜任務，需要管理更多資訊。

### 核心問題

```
怎麼給，讓 AI 有足夠的參考資料？
```

### 發展

| 年份 | 里程碑 |
|------|--------|
| 2022 | RAG 技術興起 |
| 2023 | 對話歷史管理、視窗管理 |
| 2024 | 智能分塊、重排演算法 |

### 核心技術

```typescript
// RAG：檢索增強生成
query -> vectorSearch -> relevantDocs -> context -> LLM -> response

// 對話管理
history -> summarize -> compress -> maintainCoherence

// 視窗管理
slidingWindow -> tokenLimit -> dynamicAdjust
```

### OpenCode 範例

```markdown
# Context 工程在 OpenCode 中的應用
"## 現有程式碼
src/services/userService.ts:
[程式碼]

請參考上述模式建立 OrderService"
```

### 工程師技能

- 建立知識庫和向量索引
- 設計分塊策略
- 相關性檢索和重排
- 上下文視窗管理

### 局限性

資訊太多會污染上下文，太少又不够。模型還是需要人類持續輸入指令。

## 第三階段：Harness 工程（2024-2026）

### 背景

AI Agent 能力越來越強，但可靠度沒跟上。需要系統級的控制。

### 核心問題

```
整個 Agent 系統如何安全、靠譜地運行？
```

### 發展

| 年份 | 里程碑 |
|------|--------|
| 2024 | Anthropic 提出 Agent |
| 2025 | OpenAI Codex 百萬行實驗 |
| 2026 | Harness Engineering 概念正式提出 |

### 兩大機制

```
Feedforward（引導）：在行動前預防問題
  - AGENTS.md、Skills、Linter
  
Feedback（感測）：在行動後修正問題
  - 測試、審查、日誌、自動化驗證
```

### 計算型 vs 推理型

| 類型 | 費用 | 速度 | 確定性 |
|------|------|------|--------|
| 計算型 | 低 | 快 | 高 |
| 推理型 | 高 | 慢 | 低 |

### 三種約束

```typescript
// 1. 可維護性約束
  - 程式碼品質、重複、複雜度
  - 測試覆蓋率、風格規範

// 2. 架構適應性約束
  - 分層架構、依賴方向
  - 模組邊界、系統結構

// 3. 行為約束
  - 功能規格、測試驗證
  - 人類審查、手動測試
```

### OpenCode/Harness 範例

```markdown
# AGENTS.md 內容
## 專案
Order API 服務

## 約束
- 禁止直接 import infrastructure/ 到 routes/
- 錯誤使用 Error 類別

## 驗證
npm run build && npm run test
```

### 工程師角色轉變

```
傳統工程師：       寫程式碼 → 執行
Prompt 工程師：    問問題 → 等待答案
Context 工程師：    整理資料 → 等待生成
Harness 工程師：   設計環境 → 讓 Agent 執行
```

從「執行者」變成「掌舵者」。

### OpenAI 實驗結果

```
- 百萬行代碼（0 人類編寫）
- 1500 個 PR
- 每位工程師每天 3.5 個 PR
- 驗證：Prompt → Context → Harness 演进成功
```

## 第四階段：Agent 自主（2026-）

### 願景

Agent 端到端驅動任務，人類只做驗收。

### 層級自主權

```
Level 0: 人類驅動
  human -> prompt -> OpenCode -> PR
  
Level 1: OpenCode 可自審
  human -> prompt -> OpenCode -> review -> PR

Level 2: OpenCode 可處理回饋
  human -> prompt -> OpenCode -> feedback -> fix -> PR

Level 3: OpenCode 端到端驅動
  human -> prompt -> OpenCode (full loop) -> merge
```

### 關鍵能力

```typescript
// Level 3 Agent 能力
const level3Capabilities = {
  validate: "驗證代碼庫現狀",
  reproduce: "重現 bug",
  implement: "實作修復",
  verify: "驗證修復",
  openPR: "打開 PR",
  respondFeedback: "回應回饋",
  autoMerge: "自動合併（需要時人工介入）"
}
```

### OpenCode Agent 實作

```markdown
# 在 OpenCode 中實現 Level 3 自主權
"1. 請檢查現有程式碼結構
2. 找出 bug 原因
3. 實作修復
4. 執行測試驗證
5. 打開 PR 並回應審查意見
6. 自動合併"
```

## 三者的關係

```
┌─────────────────────────────────────────────────────────────┐
│                                                         │
│                    Prompt 工程                             │
│              「問什麼」— 說清楚任務                       │
│                       ↓                                  │
│                    Context 工程                           │
│              「給什麼」— 喂 對資訊                        │
│                       ↓                                  │
│                    Harness 工程                          │
│              「怎麼跑」— 管得住系統                      │
│                       ↓                                  │
│                    Agent 自主                            │
│              「自己做」— 端到端執行                      │
│                                                         │
└────���─��──────────────────────────────────────────────────────┘
```

| 工程 | 焦點 | 產出 |
|------|------|------|
| Prompt | 交互層 | 問題/指令 |
| Context | 資訊層 | 上下文 |
| Harness | 系統層 | 控制環境 |
| Agent | 任務層 | 完整結果 |

## 工程師的技能演進

```
2020 年：
├── 寫 Prompt
├── Few-shot 範例
└── 输出格式控制

2022 年：
├── 建立 RAG
├── 向量索引
├── 對話管理
└── 分塊策略

2024 年：
├── AGENTS.md 設計
├── 自訂 Linter
├── 測試框架
└── 反饋迴圈

2026 年：
├── 環境設計
├── 約束編碼
├── 垃圾分類
└── 自主權遞進
```

## OpenCode/Hermes Agent 發展

### Hermes Agent 簡介

Hermes 是 Anthropic 推出的 autonomous agent，具有長期記憶和任務規劃能力。

### 核心能力

```typescript
// Hermes Agent 能力
interface HermesAgent {
  // 長期記憶
  memory: PersistentMemory
  
  // 任務規劃
  planning: TaskPlanning
  
  // 自主執行
  execution: AutonomousExecution
  
  // 學習適應
  learning: AdaptiveLearning
}
```

### OpenCode 與 Hermes 的比較

| 特性 | OpenCode | Hermes |
|-------|---------|-------|
| 用途 | 程式碼編輯 | 複雜任務 |
| 控制方式 | 互動式 | 自主式 |
| 記憶 | 對話歷史 | 長期記憶 |
| 工具 | 檔案操作 | 多種工具 |

## 未來展望

```
問題：模型已經足夠強了，未來的競爭是什麼？
答案：誰更會設計它的執行環境。

— OpenAI Harness Engineering
```

- 架構約束將成為核心壁壘
- 持續的「垃圾分類」成為必須
- 人類判斷力的編碼方式決定系統智慧

## 相關資源

- 相關概念：[Prompt工程](Prompt工程.md)
- 相關概念：[Context工程](Context工程.md)
- 相關概念：[Harness工程](Harness工程.md)
- 相關概念：[Skill文檔](Skill文檔.md)

## Tags

#Agent發展史 #AI歷史 #Prompt工程 #Context工程 #Harness工程 #OpenCode #Hermes