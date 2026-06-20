# Agentic Engineering（智慧體工程）

## 概述

Agentic Engineering 是設計和建構自主 AI 智慧體的工程實踐。智慧體能感知環境、規劃行動、使用工具、執行任務，並在過程中自主決策。這是 Prompt → Context → Harness → Loop 之後的進階範式。

```
Agent 自主循環
┌────────────────────────────────────────────────────────────┐
│                                                            │
│  感知 (Perceive) → 規劃 (Plan) → 執行 (Act) → 觀察 (Observe)│
│       ↑                                            │       │
│       └──────────────── 迭代 ──────────────────────┘       │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

## 與傳統工程範式的比較

```
範式             人類角色            AI 角色         輸出
─────           ──────────        ──────────      ──────────
Prompt 工程      提問者            回答者           片段程式碼
Context 工程     背景提供者         理解者           有上下文的回應
Harness 工程     約束設計者         執行者           完整專案貢獻
Loop 工程        迴圈設計者         迭代者           持續改進
Agentic 工程     目標設定者         自主執行者       完整交付
```

## Agent 架構

### 核心組件

```
Agent = 模型 + 規劃器 + 工具 + 記憶 + 安全層
```

| 組件 | 功能 | 範例 |
|------|------|------|
| **模型** | 推理與決策核心 | LLM（GPT-4, Claude） |
| **規劃器** | 分解任務、制定步驟 | ReAct, Plan-and-Execute |
| **工具** | 與外部系統互動 | 程式碼執行器、搜尋、API |
| **記憶** | 儲存上下文和經驗 | 對話記錄、向量資料庫 |
| **安全層** | 約束和驗證 | 權限控制、輸出過濾 |

### Agent 模式

#### 1. ReAct（Reasoning + Acting）

思考→行動→觀察循環：

```
思考：我需要計算 fib(10)
行動：執行 python -c "print(fib(10))"
觀察：55
思考：結果是 55，可以回覆使用者
```

#### 2. Plan-and-Execute

先規劃再執行：

```
規劃：
1. 搜尋 API 文件
2. 產生 API 呼叫程式碼
3. 測試 API 回應
4. 處理錯誤

執行：依序執行步驟，失敗時重新規劃
```

#### 3. Multi-Agent

多個專業 Agent 協作：

```
協調者 Agent → 分配子任務
    ├── 程式碼 Agent → 產生程式碼
    ├── 測試 Agent → 撰寫測試
    └── Review Agent → 程式碼審查
```

## Agentic Engineering 實踐

### AGENTS.md 中的 Agent 定義

```markdown
# AGENTS.md — Agent 操作手冊

## Agent 能力邊界
- 可以修改 code/ 下的任何檔案
- 不可以修改 wiki/ 下的檔案（需人類批准）
- 不可以部署到 production（需人類確認）

## Agent 工作流程
1. 讀取 AGENTS.md 和相關文件
2. 規劃執行步驟
3. 執行並記錄進度
4. 提交 PR 供人類 Review
```

### 安全設計

```
人類批准閘門（必修）：
├── 任何 production 變更
├── 任何 schema 變更
├── 任何權限變更
└── 任何超過 1000 行的 PR

自動執行（無需批准）：
├── Lint 修正
├── 測試補充
├── 文件更新
└── 重構（不改變行為）
```

### 工具定義

Agent 使用的工具需要明確的介面合約：

```python
# tool_contract.py
class ToolContract:
    name: str
    description: str
    input_schema: dict
    output_schema: dict
    error_handling: str
    rate_limit: int
```

## 與其他工程的關係

```
Prompt 工程 → 提供 Agent 與人類溝通的能力
Context 工程 → 提供 Agent 所需的背景資訊
Harness 工程 → 提供 Agent 的約束和驗證
Loop 工程   → 提供 Agent 的反饋迴圈結構
Agentic 工程 → 整合以上所有，實現自主執行
```

## 參考

- [Agent發展史](Agent發展史.md) — AI Agent 的發展脈絡
- [Harness工程](Harness工程.md) — Agent 的約束系統
- [Loop工程](Loop工程.md) — Agent 的反饋迴圈設計
- [Context工程](Context工程.md) — Agent 的上下文管理
- [Skill文檔](Skill文檔.md) — Agent 的領域知識封裝
