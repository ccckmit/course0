# Harness Engineering (馭繮工程)

## 概述

Harness Engineering（馭繮工程）是 OpenAI 在 2026 年提出的工程範式：工程師不再寫代碼，而是設計環境、明確意圖、建構反饋迴圈，讓 AI 智慧體可靠地完成工作。

根據 [deusyu/harness-engineering](https://github.com/deusyu/harness-engineering) 學習指南和 [OpenAI 原文](https://openai.com/index/harness-engineering/)。

## 核心轉變

```
傳統工程：人類寫代碼 → 機器執行代碼
Harness Engineering：人類設計約束 → 智慧體寫代碼 → 機器執行代碼
```

**工程師的產出從程式碼變成了約束系統** — AGENTS.md、架構規則、自訂 linter、反饋迴圈。

人類的角色變成「掌舵者」，而非「執行者」：
```
人類：描述任務、打開 PR、驗證結果
智慧體：寫代碼、跑測試、回應回饋
```

## 關鍵數據

| 指標 | 數據 |
|------|------|
| 團隊規模 | 3 人 → 7 人 |
| 時間跨度 | 5 個月 |
| 代碼量 | ~100 萬行 |
| PR 數量 | ~1,500 個 |
| 人均日 PR | 3.5 個 |
| 單次运行时长 | 6+ 小時 |
| 效率估算 | 手工編寫的 ~1/10 時間 |

---

## 六大核心概念

### 1. 倉庫即記錄系統

**不在倉庫裡的東西，對智慧體不存在。**

```
位置              對人類    對智慧體
────────────────────────────────
Google Docs        ✅       ❌
Slack 討論       ✅       ❌
團隊成員腦中      ✅       ❌
倉庫內 Markdown   ✅       ✅
代碼 + 註釋     ✅       ✅
Lint 規則       間接 ✅    ✅ (強制)
```

**實踐**：
1. AGENTS.md 是目錄，不是百科 — ~100行，只指路
2. 專職 linter + CI 驗證 — 知識庫是否更新、是否交叉連結
3. doc-gardening 智慧體 — 定期掃描過時文檔，自動發起修復 PR
4. 執行計劃是一等工件 — 提交到倉庫，版本控制

```markdown
# 文档结构
AGENTS.md              ← 入口目錄 (~100行)
ARCHITECTURE.md         ← 域和包分層的頂層地圖
docs/
├── design-docs/       ← 設計決策，帶驗證狀態
├── exec-plans/        ← 執行計劃，帶進度和決策日誌
├── product-specs/     ← 產品規格
├── references/       ← 外部參考
├── generated/        ← 自動生成（DB schema 等）
└── QUALITY_SCORE.md  ← 每個領域的質量評分
```

### 2. 地圖而非手冊

**AGENTS.md 是目錄頁，不是百科全書。**

巨型指令檔案的三個死因：
- 擠占上下文
- 無法維護
- 無法機械驗證

**漸進式披露**：智慧體從小入口點開始，被指導下一步該看什麼。

```markdown
# AGENTS.md (~100行)

## 專案
[一行描述]

## 技術棧
- [技術 1]
- [技術 2]

## 架構
[ARCHITECTURE.md 的摘要]

## 約束
- [指向詳細規則]

## 驗證
[如何跑測試]

## 深入資料
[List of docs/ files]
```

### 3. 機械化執行

**文檔會腐爛，lint 規則不會。**

- 自訂 linter + 結構測試 = 不變量的守護者
- lint 錯誤資訊裡內嵌修復指令，智慧體可以自我糾正
- 在中央層面強制執行邊界，在本地層面允許自主權

```typescript
// lint-arch.ts：依賴方向檢查
export function checkLayerImports(): Violation[] {
  const violations: Violation[] = []
  
  for (const file of allSourceFiles) {
    const imports = extractImports(file)
    const layer = getLayer(file)
    
    for (const imp of imports) {
      const impLayer = getLayer(imp)
      if (!canImport(layer, impLayer)) {
        violations.push({
          file,
          import: imp,
          message: `${layer} cant import ${impLayer}`
        })
      }
    }
  }
  
  return violations
}
```

### 4. 智慧體可讀性

**優先為智慧體的推理能力優化。**

- 選「無聊」技術（API 穩定、訓練集覆蓋好）
- 有時重新 implement 子集比包裝不透明的上游行為更划算
- 讓應用可以按 git worktree 啟動

```markdown
# 選擇技術的原則
1. API 穩定
2. 訓練集覆蓋好
3. 組合性高
4. 可被完全內化和推理
```

### 5. 吞吐量改變合併理念

**糾錯成本低，等待成本高。**

- PR 生命週期很短
- 測試偶發失敗通過後續重跑解決
- 在智慧體吞吐量遠超人類注意力的系統中，這通常是正確的選擇

```markdown
# 合併哲學
- 不阻塞：快速疊代優先
- 短生命：PR 盡快合併
- 重試：測試失敗用重跑修復
- 持續：等待是最貴的
```

### 6. 熵管理 = 垃圾回收

**智慧體會複製倉庫中已有的模式——包括壞模式。**

將「黃金規則」編碼進倉庫，定期後台任務：
- 掃描偏差
- 更新品質評分
- 發起重構 PR

**技術債是高息借貸 — 持續償還優於一次還清。**

```markdown
# 黃金原則清單
1. 偏好共享工具包 over 手寫輔助函式
2. 不做「YOLO-style」探索資料
3. 結構化日誌是必須的
4. Schema 和類型有命名規範
5. 檔案大小有限制
```

---

## Ralph 六條信條

[Ralph](https://github.com/snarktank/ralph) 是 Harness Engineering 的核心實現框架。

| 信條 | Harness Engineering 對應概念 |
|------|--------------------------|
| Fresh Context Is Reliability | 智慧體可讀性 — 每次疊代重新讀取 |
| Backpressure Over Prescription | 機械化執行 — 不規定怎麼做，但門控拒絕壞結果 |
| The Plan Is Disposable | 熵管理 — 重新生成的成本只是一次 planning loop |
| Disk Is State, Git Is Memory | 倉庫即記錄系統 — 檔案是交接機制 |
| Steer With Signals, Not Scripts | 人類掌舵 — 加路標，不加腳本 |
| Let Ralph Ralph | 智慧體執行 — 坐在迴圈上，不坐在迴圈裡 |

---

## 控制論視角

根據 [Martin Fowler](https://martinfowler.com/articles/harness-engineering.html)，Harness 是控制論的調節器：

### 兩大控制機制

```
Feedforward（引導）：在行動前預防問題
  - AGENTS.md、Skills、Linter

Feedback（感測）：在行動後修正問題
  - 測試、審查、日誌、自動化驗證
```

### 三種約束類別

| 類別 | 說明 |
|------|------|
| 可維護性 | 程式碼品質、重複、複雜度 |
| 架構適應性 | 分層架構、依賴方向 |
| 行為 | 功能規格、測試驗證 |

### 計算型 vs 推理型

| 類型 | 費用 | 速度 | 確定性 |
|------|------|------|--------|
| 計算型 | 低 | 快 | 高 |
| 推理型 | 高 | 慢 | 低 |

---

## OpenCode 實作

### AGENTS.md

```markdown
# AGENTS.md

## 專案
這是一個電子商務 API 服務

## 技術棧
- Framework: Hono
- Language: TypeScript
- Database: PostgreSQL + Prisma

## 架構
分層架構：
- src/routes/: API 路由
- src/services/: 業務邏輯
- src/repositories/: 資料存取

## 約束
- 禁止直接 import infrastructure/ 到 routes/
- 新服務放在 src/services/
- 遵循 src/CONVENTIONS.md

## 驗證
npm run build   # 建置
npm run lint   # Lint
npm run test   # 測試
```

### 自訂 Linter

```typescript
// lint-arch.ts
export function checkNoInfrastructureInRoutes(): Violation[] {
  return allFiles
    .filter(f => f.path.startsWith('src/routes/'))
    .filter(f => f.imports.some(i => i.path.includes('infrastructure/')))
    .map(f => ({
      file: f.path,
      message: 'routes/ cannot import infrastructure/'
    }))
}
```

### 品質檢查

```markdown
"請建立品質檢查指令：
1. npm run lint
2. npm run type-check
3. 執行測試
4. 檢查複雜度
5. 輸出整體評分"
```

---

## 與其他工程的關係

```
Prompt 工程     → 問什麼（說清楚）
Context 工程   → 給什麼（喂對）
Harness 工程  → 整個系統怎麼跑（管得住）
```

Harness 是 Prompt 和 Context 工程的具體形式，專注於讓智慧體可靠工作。

---

## 相關資源

- [OpenAI: Harness Engineering](https://openai.com/index/harness-engineering/)
- [deusyu/harness-engineering](https://github.com/deusyu/harness-engineering)
- [Ralph Framework](https://github.com/snarktank/ralph)
- [Martin Fowler: Harness Engineering](https://martinfowler.com/articles/harness-engineering.html)
- 相關概念：[Prompt工程](Prompt工程.md)
- 相關概念：[Context工程](Context工程.md)
- 相關概念：[Skill文檔](Skill文檔.md)

## Tags

#Harness #馭繮工程 #AI工程 #智慧體 #OpenCode #Ralph