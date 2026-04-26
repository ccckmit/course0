# Skill 文檔 (Claude Skills)

## 概述

Skill 是 Claude Code 推出的擴展機制，將領域知識和專業能力封裝成可重用的套件。Claude 會根據任務自動發現並載入相關的 Skill，無需手動呼叫。

基於 [Andrej Karpathy](https://github.com/karpathy) 的觀察和 [forrestchang/andrej-karpathy-skills](https://github.com/forrestchang/andrej-karpathy-skills)（43.9k stars）。

```
┌─────────────────────────────────────────────────────────────┐
│                    Claude Code 擴展機制                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  MCP Servers     → 新增工具（需要知道存在）                    │
│     ↓                                                    │
│  Slash Commands → 預定義 Prompt（手動呼叫）              │
│     ↓                                                    │
│  Skills       → 領域知識（自動發現）                        │
│     ↓                                                    │
│  CLAUDE.md    → 基本規則（始終載入）                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## Karpathy 的四原則

[Karpathy](https://github.com/karpathy) 指出 LLM 編碼的常見問題：

> "模型會幫你做錯誤的假設卻不檢查。它們不管理困惑，不尋求澄清，不呈現不一致，不在應該時反對。"
> "它們喜歡過度複雜化代碼和 API，膨脹抽象，不改變清理死代碼..."

### 1. Think Before Coding

**不要假設。不要隱藏困惑。呈現取捨。**

```
- 明確陳述假設 — 不確定時询问而非猜測
- 呈現多種解釋 — 模糊時不默默選擇
- 在應該時反對 — 如果有更簡單的方法，說出來
- 困惑時停止 — 說明什麼不清楚並請求澄清
```

### 2. Simplicity First

**最少代碼解決問題。不做推測。**

```
- 不要功能超出要求
- 單用途代碼不做抽象
- 不要「靈活性」或「可配置性」未被要求
- 不要不可能場景的錯誤處理
- 200 行可變 50 行則重寫
```

自問：「資深工程師會說這過度設計嗎？」如果是，簡化。

### 3. Surgical Changes

**只 touch 必須的。清理自己的爛攤子。**

```
當編輯現有代碼時：
- 不要「改進」相鄰代碼、注釋或格式
- 不要重構沒壞的東西
- 匹配現有風格，即使你會不同
- 注意到無關死代碼，提及但不刪除

當你的更改造成孤兒：
- 刪除你的更改造成的未使用 import/變數/函式
- 不要刪除預先存在的死代碼除非被要求
```

測試：每個更改的行應該可以直接追溯到用戶的請求。

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

強成功標準讓 LLM 可以獨立循環。弱的標準（「让它工作」）需要持續澄清。

---

## Skill 的核心概念

### 定義

Skill 是將領域專業知識和任務特定指令打包的機制：
- 包含詳細指令說明
- 可選的辅助腳本和資源
- Claude 自動發現和載入

```
Skill = 專業知識 + 執行指引 + 輔助工具
```

### 與其他機制的比較

| 機制 | 调用方式 | 用途 |
|------|----------|------|
| MCP Servers | 工具调用 | 新增功能 API |
| Slash Commands | 手動輸入 `/cmd` | 預定義 workflow |
| Skills | 自動發現 | 領域知識注入 |
| CLAUDE.md | 總是載入 | 專案基本資訊 |

## Skill 資料夾結構

```
.claude/skills/
├── pdf/
│   ├── SKILL.md           # 必需：Skill 定義
│   ├── extract_text.ts   # 可選：輔助腳本
│   └── templates/       # 可選：資源
├── csv/
│   └── SKILL.md
└── xlsx/
    ├── SKILL.md
    └── scripts/
        └── analyze.ts
```

## SKILL.md 格式

### YAML Frontmatter

```yaml
---
name: pdf
description: 提取和分析 PDF 文件。當用戶要求處理或閱讀 PDF 時使用。
version: "1.0.0"
allowed-tools: Read,Write,Bash,Glob,Grep,Edit
model: claude-sonnet-4-20250514
---

# PDF Skill
... 詳細指令 ...
```

### 欄位說明

| 欄位 | 必需 | 說明 |
|------|------|------|
| `name` | 是 | Skill 名稱 |
| `description` | 是 | 描述（Claude 據此自動判斷） |
| `version` | 否 | 版本號 |
| `allowed-tools` | 否 | 允許的工具列表 |
| `model` | 否 | 指定使用的模型 |

### 指令主體（融入 Karpathy 原則）

```yaml
---
name: code-reviewer
description: 执行代码审查并提供改进建议
allowed-tools: Read,Grep,Glob,Bash
---

# Code Reviewer Skill

## 審查原則（基於 Karpathy）

### 1. Think Before Coding
- 確認需求，是否有多种解释
- 呈現可能的取捨

### 2. Simplicity First
- 不要過度設計
- 200 行可完成不要用 1000 行

### 3. Surgical Changes
- 只改必要的
- 不要顺便「改進」其他代碼

### 4. Goal-Driven
- 定義明确的成功標準

## 何時使用
當用戶要求：
- 代碼審查
- 檢查程式碼品質
- 找出潛在問題

## 審查標準
- 是否有重複程式碼
- 函式是否過長（>50 行）
- 命名是否清晰
- 安全問題
- 效能問題
```

## 使用位置

### 1. 使用者層級

```
~/.claude/skills/
```

### 2. 專案層級

```
.claude/skills/
```

### 3. Claude Code Plugin（推薦）

```
/plugin marketplace add forrestchang/andrej-karpathy-skills
/plugin install andrej-karpathy-skills@karpathy-skills
```

## 安裝 Karpathy Skills

### Option A: Claude Code Plugin（推薦）

```bash
/plugin marketplace add forrestchang/andrej-karpathy-skills
/plugin install andrej-karpathy-skills@karpathy-skills
```

### Option B: 下載 CLAUDE.md

```bash
# 新專案
curl -o CLAUDE.md https://raw.githubusercontent.com/forrestchang/andrej-karpathy-skills/main/CLAUDE.md

# 現有專案
echo "" >> CLAUDE.md
curl https://raw.githubusercontent.com/forrestchang/andrej-karpathy-skills/main/CLAUDE.md >> CLAUDE.md
```

## 預設 Skills

Claude Code 預設包含一些常用 Skills：

| Skill | 用途 |
|-------|------|
| `pdf` | PDF 處理 |
| `xlsx` | Excel 處理 |
| `pptx` | PowerPoint 處理 |
| `docx` | Word 處理 |
| `csv` | CSV 處理 |

## 建立自訂 Skill

### 步驟 1：建立資料夾

```bash
mkdir -p .claude/skills/my-skill
```

### 步驟 2：建立 SKILL.md（融入 Karpathy 原則）

```yaml
---
name: my-skill
description: 我的自訂技能說明
allowed-tools: Read,Write,Bash,Glob
---

# My Skill

## 何時使用
[描述何時應使用此技能]

## 原則（基於 Karpathy）

### 1. Think Before Coding
- 先確認需求
- 不确定的要問

### 2. Simplicity First
- 最少代碼完成任務

### 3. Surgical Changes
- 只改必要的

### 4. Goal-Driven
- 有明確成功標準

## 指令
[詳細的執行指引]
```

### 步驟 3：加入輔助腳本（可選）

```typescript
// .claude/skills/my-skill/scripts/helper.ts
export function main(): number {
  console.log("Helper script executed")
  return 0
}
```

## Skill 執行流程

```
1. 使用者提出任務
      ↓
2. Claude 檢查可用 Skills
      ↓
3. 根據 description 匹配
      ↓
4. 自動載入 Skill 指令
      ↓
5. 執行任務（遵循 Karpathy 原則）
```

## OpenCode 實作範例

### 使用 Karpathy 原則建立服務

```markdown
"建立一個 UserService：

## 遵循 Karpathy 原則：

### 1. Think Before Coding
- 先確認需要哪些方法
- 有多種實現方式時呈現

### 2. Simplicity First
- 最少方法滿足需求
- 不要過度抽象

### 3. Surgical Changes
- 只建立必要的
- 不做多餘的重構

### 4. Goal-Driven
- 有明確成功標準：可編譯 + 測試通過

---
[實作程式碼]"
```

### 使用 Skill 進行代碼審查

```markdown
"使用 code-reviewer skill 審查這個代碼：

[代碼內容]

遵循原則：
- Think: 先理解需求
- Simple: 不過度複雜
- Surgical: 只標記問題
- Goal: 有具體改進建議"
```

## Best Practices

### 1. Progressive Disclosure

不要在一個 Skill 中塞入所有資訊：

```yaml
# ❌ 不好：太长
description: 可以做很多事情，包括 A、B、C、D...

# ✅ 好：簡潔明確
description: 生成單元測試，使用 vitest 和 Mock
```

### 2. 清晰的觸發條件

```yaml
## 何時使用
當用戶說：
- "生成測試"
- "不夠"
- "coverage"
```

### 3. 具體的輸出格式

```yaml
## 輸出格式

```yaml
file: tests/test_xxx.ts
coverage: 80%
cases:
  - name: 登入成功
  - name: 登入失敗
```
```

### 4. 限制允許的工具

```yaml
# 只允許必要工具
allowed-tools: Read,Write,Glob,Grep

# 避免過度權限
# ❌ allowed-tools: *
```

### 5. 融入 Karpathy 原則

在每個 Skill 中明確說明：
- Think Before Coding：確認什麼需要做
- Simplicity First：最少代碼
- Surgical Changes：只改必要的
- Goal-Driven：有成功標準

## 常見問題

### Q: Skill 沒有被觸發？

檢查：
1. description 是否說明清楚用途
2. 是否在正確位置（.claude/skills/）
3. SKILL.md 格式是否正確

### Q: 如何除錯？

在對話中明確要求：
```
Use the xxx skill to...
```

### Q: 多個 Skill 如何選擇？

Claude 會根據 description 自動選擇最相關的。可以明確指定。

### Q: 如何平衡速度與謹慎？

Karpathy 原則偏向謹慎。對於簡單任務（如修正 typo），使用判斷不需要完整遵循。

## 運作跡象

這些指南在運作時你會看到：
- **diff 中不必要的更改減少** — 只有請求的更改出現
- **因過度設計導致的重寫減少** — 代碼第一次就簡單
- **澄清問題在實作前到來** — 而非錯誤後
- **最小、乾淨的 PR** — 沒有附带重構或「改進」

## 相關資源

- [Karpathy GitHub](https://github.com/karpathy)
- [andrej-karpathy-skills](https://github.com/forrestchang/andrej-karpathy-skills)
- [Claude Code Skills](https://platform.claude.com/docs/en/agents-and-tools/agent-skills/overview)
- [Karpathy 四原則詳解](karpathy.md)

## Tags

#Skill #Claude #Karpathy #四原則 #OpenCode