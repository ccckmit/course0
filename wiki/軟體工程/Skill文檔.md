# Skill 文檔 (Claude Skills)

## 概述

Skill 是 Claude Code 推出的擴展機制，將領域知識和專業能力封裝成可重用的套件。Claude 會根據任務自動發現並載入相關的 Skill，無需手動呼叫。

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

### 與 KarpathySkill 的關係

- [KarpathySkill](KarpathySkill.md) - 基於 Karpathy 四原則的編碼指南
- Skill 文檔 - Claude Code 的 Skill 擴展機制說明

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

### 指令主體範例

```yaml
---
name: code-reviewer
description: 执行代码审查并提供改进建议
allowed-tools: Read,Grep,Glob,Bash
---

# Code Reviewer Skill

## 審查標準

### 1. 程式碼品質
- 是否有重複程式碼
- 函式是否過長（>50 行）
- 命名是否清晰

### 2. 安全問題
- SQL 注入風險
- 敏感資料處理
- 權限控制

### 3. 效能
- 是否有 N+1 查詢
- 是否有不必要的循環

## 輸出格式

```markdown
## 審查結果

### 問題清單
1. [檔案:行號] 問題描述

### 建議
- 改進建議 1
- 改進建議 2

### 整體評分
/10
```
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

### 3. Claude Code Plugin

```
/plugin marketplace add <owner>/<repo>
/plugin install <repo>@<skill>
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

### 步驟 2：建立 SKILL.md

```yaml
---
name: my-skill
description: 我的自訂技能說明
allowed-tools: Read,Write,Bash,Glob
---

# My Skill

## 何時使用
[描述何時應使用此技能]

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
5. 執行任務
```

### 關鍵機制

Claude 不使用演算法路由來決定載入哪個 Skill。而是將所有可用 Skills 格式化後嵌入工具定義中，讓模型自行判斷。

```typescript
const availableSkills: Skill[] = []
for (const skillFolder of skillDirectories) {
  const skillMd = readFile(`${skillFolder}/SKILL.md`)
  availableSkills.push({
    name: skillMd.frontmatter.name,
    description: skillMd.frontmatter.description,
    content: skillMd.body
  })
}
```

## OpenCode 實作範例

### 建立安全審計 Skill

```yaml
---
name: security-auditor
description: 執行安全審計，檢查漏洞和風險
allowed-tools: Read,Grep,Glob,Bash
---

# Security Auditor

## 何時使用
- 要求安全審計
- 檢查 Authentication/Authorization
- 密碼處理檢查

## 檢查項目

### 1. 身份驗證
- 密碼是否雜湊存儲
- Session 管理
- JWT 實現

### 2. 授權
- 權限檢查點
- 角色驗證

### 3. 輸入驗證
- SQL 注入防護
- XSS 防護
```

### 建立測試生成 Skill

```yaml
---
name: test-generator
description: 為代碼生成測試案例
allowed-tools: Read,Write,Bash,Glob
---

# Test Generator

## 何時使用
- 要求生成測試
- 缺少測試覆蓋率
- TDD 工作流

## 測試策略

### 單元測試
- 測試每個公開函式
- Mock 外部依賴

### 整合測試
- API endpoint 測試
- 資料庫互動
```

## Best Practices

### 1. Progressive Disclosure

```yaml
# ❌ 不好：太长
description: 可以做很多事情...

# ✅ 好：簡潔明確
description: 生成單元測試，使用 pytest 和 Mock
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
```
```

### 4. 限制允許的工具

```yaml
# 只允許必要工具
allowed-tools: Read,Write,Glob,Grep

# 避免過度權限
# ❌ allowed-tools: *
```

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

Claude 會根據 description 自動選擇最相關的。

## 相關資源

- [Claude Code Skills](https://platform.claude.com/docs/en/agents-and-tools/agent-skills/overview)
- [KarpathySkill](KarpathySkill.md) - Karpathy 四原則

## Tags

#Skill #Claude #擴展 #領域知識