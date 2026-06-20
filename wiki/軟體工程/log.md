# 日誌

## [2026-06-20] add | Rust 軟體工程 + Rust 測試 + E2E 測試 wiki

- 新增 Rust軟體工程.md — Rust 在軟體工程的應用、Axum/rusqlite/sled、效能對比
- 新增 Rust測試.md — cargo test、整合測試、CLI 子行程測試模式
- 新增 E2E測試.md — Playwright 端到端測試詳細說明、Todo 專案 E2E 流程
- 更新 docs/html/softwareEngineering/index.html: 三處加入 wiki 鏈接
- 更新 測試.md: 導航列指向 E2E測試.md + Rust測試.md
- 更新 index.md: 新增 4 條條目

## [2026-06-20] add | 金流整合範例 + Pay4 自建金流系統 wiki

- 新增 金流整合範例.md — payment/ 目錄的三閘道整合 (Stripe/ECPay/Pay4)
- 新增 Pay4自建金流系統.md — pay4/ 引擎完整說明
- 更新 index.md: 新增 2 條條目
- 更新 docs/html/softwareEngineering/index.html: 新增「金流整合範例」章節，含三欄閘道比較 + Pay4 核心模組 + 雙式記帳範例

## [2026-06-20] add | 營運與商業主題（金流/公司登記/上線）

- 新增 金流整合.md — 第三方支付、信用卡、訂閱制金流與對帳
- 新增 公司登記與開業.md — 台灣軟體公司設立流程與法規
- 新增 上線營運.md — 上線前檢查清單、營運週期、常見問題
- 更新 index.md: 新增 3 條條目
- 更新 docs/html/softwareEngineering/index.html: 新增「營運與商業」章節，含金流整合/公司登記與開業/上線營運三卡片

## [2026-06-20] expand | 國際軟體公司營運（Stripe + 美國公司登記）

- 擴充 金流整合.md: 新增 Stripe 國際收款章節 + Stripe Atlas + 多貨幣注意事項
- 擴充 公司登記與開業.md: 新增美國 Delaware C-Corp 章節、Stripe Atlas、銀行帳戶、台美比較表
- 擴充 上線營運.md: 新增國際市場營運章節（跨境稅務/GDPR/多語言）
- 更新 docs/html/softwareEngineering/index.html: 三卡片加入國際面向內容
- 更新 index.md: 3 條條目摘要與標籤擴充

## [2026-06-20] restructure | todo/ 目錄重組

- 將三個後端實作集中到 `todo/` 下：`todo-py/`, `todo-rust/`, `todo-rust-fast/`
- `frontend/` 保持原位
- 更新 README.md、AGENTS.md、BENCHMARKS.md、CI、wiki 等跨參照
- 新增 `todo/README.md` — 總覽所有四子專案

## [2026-06-20] add | Loop工程、Agentic工程、API設計、人機協作模式

- 新增 Loop工程.md — 多層反饋迴圈的設計與管理
- 新增 Agentic工程.md — 自主 AI 智慧體的設計與建構
- 新增 API設計.md — CLI → REST → React 設計模式
- 新增 人機協作模式.md — 人類與 AI 的分工協作
- 新增 todo/ 範例專案 — FastAPI + SQLite + React 完整展示
- index.md: 新增 4 條條目

## [2026-04-15] add | 軟體工程進階主題

- 新增 需求工程.md - 需求獲取、用戶故事、接受標準
- 新增 領域驅動設計.md - DDD、限界上下文、聚合、領域服務
- 新增 微服務架構.md - 服務拆分、Saga 模式、API 設計
- 新增 雲端架構.md - AWS/GCP/Azure、Serverless、雲原生
- 新增 訊息佇列.md - Kafka、RabbitMQ、事件驅動
- 新增 災難復原.md - DR、RPO/RTO、備份策略
- 更新 index.md

## [2026-04-15] add | 軟體工程的歷史

- 新增 軟體工程的歷史.md - 從瀑布模型到 AI 輔助開發
- 涵蓋：軟體危機、瀑布模型、物件導向、敏捷革命、開源運動、DevOps
- 約 333 行

## [2026-04-16] rewrite | AI 軟體工程系列

- 重寫 Prompt工程.md：改用 OpenCode 範例
- 重寫 Context工程.md：改用 OpenCode 上下文管理
- 重寫 Harness工程.md：改用 OpenCode/Claude Code 範例
- 重寫 Skill文檔.md：改用 TypeScript 範例
- 重寫 Agent發展史.md：加入 OpenCode/Hermes Agent

## [2026-04-16] add | Skill文檔

- 新增 Skill文檔.md
- 介紹 Claude Code 的 Skill 擴展機制
- 包含 SKILL.md 格式、建立範例
- 更新 index.md

## [2026-04-16] add | Agent發展史

- 新增 Agent發展史.md
- 組織 Prompt/Context/Harness/Agent 發展脈絡
- 以 Agent 視角呈現演進關係
- 更新 index.md

## [2026-04-16] rewrite | Harness工程

- 基於 YouTube 影片和網路資源重寫 Harness工程.md
- 更新為馭繮工程取向（而非測試架構）
- 新增六大核心元件、四 大核心原則
- 更新 index.md

## [2026-04-07] add | AI 軟體工程

- 新增 Prompt工程 - 提示詞設計、Few-shot、Chain-of-Thought
- 新增 Context工程 - RAG、對話管理、分塊策略
- 新增 Harness工程 - AI 測試架構、功能/語意/對抗測試
- 更新 index.md 和 log.md

## [2026-04-07] expand | 軟體工程 Wiki 擴充

- 新增概念：軟體度量、程式碼品質、技術債
- 新增主題：雲端原生、安全開發、效能優化、監控與日誌、容器編排
- 新增工具：Jenkins
- 更新 index.md 和 log.md

## [2026-04-07] init | 軟體工程 Wiki

- 初始化了軟體工程 Wiki 結構
- 建立了概念、主題、工具分類