# Prompt Engineering (提示詞工程)

## 概述

提示詞工程是設計和最佳化輸入提示詞（Prompt）的實踐，以獲得更好的 AI 模型輸出。是與大型語言模型 (LLM) 有效互動的關鍵技能。

根據 [Prompt Engineering Guide](https://www.promptingguide.ai/)，提示詞工程涵蓋多種技巧，用於與 LLM 互動、開發和研究。

## 提示詞基本結構

```
┌─────────────────────────────────────────────────────────────┐
│                    Prompt 結構                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  [System]     角色定義、能力邊界                            │
│      ↓                                                   │
│  [Context]    背景資訊、上下文                              │
│      ↓                                                   │
│  [Examples]   範例（Few-shot）                              │
│      ↓                                                   │
│  [Task]       具體任務                                      │
│      ↓                                                   │
│  [Format]     輸出格式                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 核心技巧

### 1. Zero-shot Prompting (零樣本提示)

不提供任何範例，直接要求模型執行任務。

```markdown
"將以下文字分類為正面或負面：
這產品很棒！
"
```

**適用場景**：簡單任務、模型擅長的任務

### 2. Few-shot Prompting (少樣本提示)

提供 1-多個範例，讓模型學習模式。

```markdown
"將以下文字分類為正面或負面：

範例 1:
這太棒了！ → 正面
這很差 → 負面

範例 2:
超級喜歡這個！ → 正面
不推薦 → 負面

現在分類：
我覺得還可以接受 →"
```

**關鍵發現** [Min et al., 2022]：
- 標籤空間和輸入分布都很重要
- 格式比標籤正確與否更重要
- 隨機標籤仍比沒有標籤好

```markdown
# 格式範例（即使標籤錯誤也能工作）
This is awesome! // Negative
This is bad! // Positive
Wow that movie was rad! // Positive
What a horrible show! //
```

### 3. Chain-of-Thought (思維鏈)

要求模型逐步思考，展示推理過程。

```markdown
"""
問題：小明有 15 顆蘋果，給了朋友 7 顆，又買了 12 顆。他現在有幾顆蘋果？

讓我們逐步思考：
1. 初始數量：15 顆
2. 送出後：15 - 7 = 8 顆
3. 買進後：8 + 12 = 20 顆

答案：20 顆
"""
```

**在 OpenCode 中使用**：

```markdown
"請逐步思考這個問題：
1. 先分析需求
2. 列出可能的解決方案
3. 選擇最佳方案
4. 實作

[你的問題]"
```

### 4. Self-Consistency (自我一致性)

多次生成，選擇最一致的答案。

```markdown
"回答這個問題，嘗試 3 次：
[問題]

如果答案不同，請說明每個答案的理由和 confidence level。"
```

### 5. Generate Knowledge Prompting (生成知識提示)

���模型先產生相關知識，再回答問題。

```markdown
"""
首先，列出與這個問題相關的關鍵概念：
[問題]

然後，基於上述概念回答問題：
"""
```

### 6. Prompt Chaining (提示鏈)

將複雜任務拆分為多個較短的提示。

```markdown
"**步驟 1**: 這個程式碼在做什麼？
[程式碼]

**步驟 2**: 找出可能的效能問題
[根據步驟 1 的分析]

**步驟 3**: 提供優化建議
[根據步驟 2 的分析]
"
```

### 7. Tree of Thoughts (思考樹)

探索多種思考路徑。

```markdown
"分析這個問題，探索 3 種不同的思考路徑：

路徑 A:[想法]
  - 優點：
  - 缺點：

路徑 B:[想法]
  - 優點：
  - 缺點：

路徑 C:[想法]
  - 優點：
  - 缺點：

請選擇最佳路徑並說明理由。"
```

### 8. ReAct (Reasoning + Acting)

結合推理和行動。

```markdown
"使用 ReAct 推理這個問題：

思考 1: [推理]
行動 1: [搜索相關資料]
觀察 1: [找到的結果]

思考 2: [基於觀察的推理]
行動 2: [進一步搜索]
觀察 2: [結果]

最終答案：[綜合所有信息]"
```

---

## 進階技巧

### 1. Role Prompting (角色提示)

指定模型扮演的角色。

```markdown
"你是一位資深架構師，專精：
- 雲端原生設計
- 微服務架構
- 事件驅動系統

請評估這個設計並提供建議：
[設計描述]
"
```

### 2. Meta Prompting (元提示)

讓模型自己改進提示。

```markdown
"這個提示可能有哪些問題？
[你的原始 prompt]

請改進這個提示，使其更清楚、更有效。"
```

### 3. Program-Aided Language Models (PAL)

讓模型產生程式碼來解決問題。

```markdown
"用 Python 程式碼解決這個問題，然後執行：
[數學問題]

請產生程式碼並解釋結果。"
```

### 4. Reflexion (反思)

讓模型反思自己的輸出。

```markdown
"回答這個問題：
[問題]

然後反思：
- 我的答案正確嗎？
- 有什麼遺漏的點？
- 如何改進？"
```

### 5. Active-Prompt (主動提示)

讓模型主動詢問 clarifications。

```markdown
"我需要你完成這個任務：
[任務]

在開始之前，請列出任何需要澄清的問題。"
```

### 6. Directional Stimulus Prompting (方向性刺激提示)

提供明確的方向或線索。

```markdown
"回答時請注意：
- 使用最新的研究
- 給出具體的數據
- 附上來源

[問題]"
```

---

## OpenCode 實作範例

### Few-shot with OpenCode

```markdown
"建立這個功能的三個範例：

範例 1 (建立 Service):
建立 UserService，返回用戶資料

範例 2 (建立 Handler):
建立 /users/:id GET endpoint

範例 3 (建立 Middleware):
建立 auth middleware
---
現在請建立：[你的需求]
"
```

### Chain-of-Thought with OpenCode

```markdown
"請逐步建立這個功能：

步驟 1: 理解需求
- 這是一個什麼功能？
- 輸入輸出是什麼？

步驟 2: 設計結構
- 需要哪些類型？
- 服務如何組織？

步驟 3: 實作
- [請開始實作]

步驟 4: 測試
- [請寫測試]
"
```

### ReAct with OpenCode

```markdown
"用 ReAct 方式解決這個問題：

Thought: 我需要先了解現有的程式碼結構
Action: 查看 src/services/ 目錄
Observation: 找到 UserService, ProductService

Thought: 需要建立類似的 OrderService
Action: 參考 UserService 的模式建立 OrderService
Observation: 完成建立

Final Answer: OrderService 已建立完成
"
```

---

## 輸出格式控制

### 1. JSON 格式

```markdown
"以 JSON 格式回傳：
{
  "name": "用��名",
  "email": "email",
  "role": "角色"
}
只回傳 JSON。"
```

### 2. Markdown 表格

```markdown
"以表格格式呈現：

| 功能 | 描述 | 優先順序 |
|------|------|--------|
"
```

### 3. YAML 格式

```markdown
"以 YAML 格式回傳：
```yaml
service:
  name: 
  methods:
    - 
```
"
```

---

## 常見陷阱

| 陷阱 | 說明 | 解決方法 |
|------|------|----------|
| 模糊指令 | 指令不夠具體 | 使用明確的動作詞 |
| 資訊過載 | 一次給太多資訊 | 分步驟處理 |
| 缺乏上下文 | 模型不知道背景 | 提供相關上下文 |
| 忽略限制 | 沒說不要做什麼 | 明確列舉限制 |
| 過度依賴 Few-shot |複雜推理任務效果差 | 使用 Chain-of-Thought |

---

## 進階參考

### Chain-of-Thought 的限制

標準的 Few-shot 對複雜推理任務效果有限：

```markdown
# 這個例子失敗了
"奇數相加是否為偶數？15, 32, 5, 13, 82, 7, 1"
# Model 回答: Yes (錯誤)
```

**解決方案**：使用 Chain-of-Thought 或更複雜的提示技巧。

### 選擇正確的技巧

| 任務類型 | 推薦技巧 |
|----------|---------|
| 簡單分類 | Zero-shot, Few-shot |
| 數學推理 | Chain-of-Thought, PAL |
| 複雜決策 | Tree of Thoughts, ReAct |
| 程式碼生成 | Few-shot + Chain-of-Thought |
| 創意寫作 | Role Prompting |
| 自我改進 | Meta Prompting |

---

## 相關資源

- [Prompt Engineering Guide](https://www.promptingguide.ai/)
- [Chain-of-Thought Prompting](https://arxiv.org/abs/2201.11903)
- [Few-shot Prompting](https://arxiv.org/abs/2005.14165)
- [ReAct Prompting](https://arxiv.org/abs/2210.03629)
- 相關概念：[Context工程](Context工程.md)
- 相關概念：[Harness工程](Harness工程.md)

## Tags

#Prompt #提示詞工程 #LLM #AI工程 #FewShot #ChainOfThought #ReAct