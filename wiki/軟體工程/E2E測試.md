---
tags: [E2E, 端到端測試, Playwright, 自動化測試, 瀏覽器測試]
date: 2026-06-20
---

# 端到端測試（E2E Testing）

端到端測試從使用者角度模擬真實操作，驗證整個系統從前端到後端的正確協作。E2E 測試位於測試金字塔的頂端，數量最少但最接近真實使用情境。

## 為什麼需要 E2E 測試

- **真實情境驗證** — 模擬使用者點擊、輸入、瀏覽的完整流程
- **跨元件協作** — 確保前端、API、資料庫、第三方服務整合無誤
- **回歸防護** — 重大重構後確保核心流程仍正常
- **驗收標準** — 可作為功能完成的判斷依據

## Playwright 介紹

Playwright 是 Microsoft 開發的現代瀏覽器自動化框架，支援 Chromium、Firefox、WebKit。

### 優勢

| 特性 | 說明 |
|------|------|
| 多瀏覽器 | Chromium + Firefox + WebKit 單一 API |
| 自動等待 | 元素出現前自動等待，不需手動 sleep |
| 網路攔截 | Mock API 回應，測試邊界情況 |
| 行動模擬 | 模擬手機/平板瀏覽器 |
| 追蹤錄製 | 錄製測試執行過程供除錯 |
| 程式碼生成 | 操作瀏覽器時自動產生測試程式碼 |

### 安裝

```bash
pip install playwright
playwright install chromium         # 安裝 Chromium 瀏覽器
playwright install firefox webkit   # 或其他瀏覽器
```

## Todo 專案的 E2E 測試

`code/軟體工程/todo/todo-py/tests/test_e2e.py` 示範完整的 E2E 測試模式：

```python
import subprocess
import time
import requests
from playwright.sync_api import sync_playwright


def test_frontend():
    # 1. 啟動後端 API
    api_proc = subprocess.Popen(
        ["uvicorn", "todo.api:app", "--port", "8001"],
    )
    # 2. 啟動前端 Dev Server
    frontend_proc = subprocess.Popen(
        ["npx", "vite", "--port", "5173"],
        cwd="../frontend"
    )
    time.sleep(3)  # 等待服務啟動

    try:
        # 3. 透過 API 預先建立測試資料
        requests.post(
            "http://localhost:8001/api/todos",
            json={"text": "test from playwright"}
        )

        # 4. 使用 Playwright 模擬瀏覽器操作
        with sync_playwright() as p:
            browser = p.chromium.launch()
            page = browser.new_page()
            page.goto("http://localhost:5173")
            # 5. 驗證前端正確顯示後端資料
            assert "test from playwright" in page.text_content("body")
            browser.close()
    finally:
        # 6. 清理：終止所有子行程
        api_proc.terminate()
        frontend_proc.terminate()
```

### 測試流程解析

```
┌─────────────────────────────────────────────────────┐
│                  E2E 測試流程                         │
├─────────────────────────────────────────────────────┤
│                                                      │
│  1. 啟動後端          uvicorn todo.api:app            │
│       │                                              │
│       ▼                                              │
│  2. 啟動前端          npx vite                         │
│       │                                              │
│       ▼                                              │
│  3. 準備測試資料      POST /api/todos {text}          │
│       │                                              │
│       ▼                                              │
│  4. 打開瀏覽器        playwright.chromium.launch()     │
│       │                                              │
│       ▼                                              │
│  5. 操作並驗證        page.goto() + assert            │
│       │                                              │
│       ▼                                              │
│  6. 清理              terminate() 子行程              │
│                                                      │
└─────────────────────────────────────────────────────┘
```

## Playwright 核心 API

### 瀏覽器啟動

```python
from playwright.sync_api import sync_playwright

# 同步 API
with sync_playwright() as p:
    browser = p.chromium.launch(headless=True)  # headless=False 可觀察操作
    page = browser.new_page()
    page.goto("https://example.com")
    # ... 測試操作 ...
    browser.close()
```

```python
import asyncio
from playwright.async_api import async_playwright

# 非同步 API
async def test():
    async with async_playwright() as p:
        browser = await p.chromium.launch()
        page = await browser.new_page()
        await page.goto("https://example.com")
        await browser.close()
```

### 元素定位 (Locators)

Playwright 推薦使用 Locator API（而非舊式的 `page.click()` 等）：

```python
page = browser.new_page()
page.goto("http://localhost:5173")

# 依文字內容
page.get_by_text("Add Todo")
page.get_by_role("button", name="Submit")

# 依 data-testid（推薦）
page.get_by_test_id("todo-input")
page.get_by_test_id("add-button")

# 依 CSS 選擇器
page.locator(".todo-item")
page.locator("#app")

# 依 placeholder
page.get_by_placeholder("Enter todo text")
```

### 常見操作

```python
# 輸入文字
page.get_by_test_id("todo-input").fill("Buy milk")

# 點擊
page.get_by_test_id("add-button").click()

# 勾選
page.get_by_role("checkbox").check()

# 選擇下拉選單
page.get_by_label("Status").select_option("completed")

# 鍵盤操作
page.get_by_test_id("todo-input").press("Enter")

# 等待元素出現
page.get_by_text("Buy milk").wait_for()

# 截圖（用於除錯）
page.screenshot(path="debug.png")
```

### 斷言 (Assertions)

```python
from playwright.sync_api import expect

# 元素可見
expect(page.get_by_text("Buy milk")).to_be_visible()

# 元素包含文字
expect(page.locator(".todo-count")).to_contain_text("3 items left")

# 元素有特定屬性
expect(page.get_by_test_id("todo-item")).to_have_attribute("data-id", "1")

# 輸入框有特定值
expect(page.get_by_test_id("todo-input")).to_have_value("")

# 元素計數
expect(page.locator(".todo-item")).to_have_count(3)

# 頁面網址
expect(page).to_have_url("http://localhost:5173/todos/1")
```

## E2E 測試模式

### 模式 1: 先啟動服務再測試

如 Todo 專案所示：在 pytest 中啟動 subprocess，測試完成後清理。

```python
@pytest.fixture(scope="module")
def services():
    api = subprocess.Popen(["uvicorn", "todo.api:app", "--port", "8001"])
    frontend = subprocess.Popen(["npx", "vite", "--port", "5173"], cwd="../frontend")
    time.sleep(3)
    yield
    api.terminate()
    frontend.terminate()


def test_todo_list(services):
    with sync_playwright() as p:
        browser = p.chromium.launch()
        page = browser.new_page()
        page.goto("http://localhost:5173")
        expect(page.locator("body")).to_contain_text("todos")
        browser.close()
```

### 模式 2: 使用 Playwright Test Runner

Playwright 也提供自己的測試執行器（pytest 外掛）：

```python
# conftest.py
import pytest
from playwright.sync_api import Page, sync_playwright


@pytest.fixture(scope="session")
def browser():
    with sync_playwright() as p:
        browser = p.chromium.launch()
        yield browser
        browser.close()


@pytest.fixture
def page(browser):
    page = browser.new_page()
    page.goto("http://localhost:5173")
    yield page
    page.close()
```

```python
# test_todo_e2e.py
def test_add_todo(page: Page):
    page.get_by_test_id("todo-input").fill("Buy milk")
    page.get_by_test_id("add-button").click()
    expect(page.get_by_test_id("todo-list")).to_contain_text("Buy milk")


def test_complete_todo(page: Page):
    page.get_by_test_id("checkbox-1").check()
    expect(page.get_by_test_id("todo-1")).to_have_class("completed")
```

### 模式 3: CI 整合

GitHub Actions 中的 Playwright 配置：

```yaml
name: E2E Tests
on: [push]
jobs:
  e2e:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with:
          python-version: '3.12'
      - run: pip install -r requirements.txt
      - run: playwright install chromium
      - run: |
          cd frontend
          npm install
          npx vite --port 5173 &
      - run: |
          uvicorn todo.api:app --port 8001 &
          sleep 3
          pytest tests/test_e2e.py -v
```

## 與其他測試層的關係

```
測試金字塔（本課程實作）:

                    ┌─────────────┐
                    │  E2E 測試   │  ← Playwright (test_e2e.py)
                   /│   (1 個)   │\
                  / └─────────────┘ \
                 /   ┌─────────────┐   \
                /    │  整合測試    │    \
               /     │  (7 個)     │     \   ← pytest + httpx (test_api.py)
              /      └─────────────┘      \
             /       ┌─────────────┐       \
            /        │  單元測試    │        \
           /         │  (5 個)     │         \  ← pytest (test_cli.py)
          /          └─────────────┘          \
         /           ┌─────────────┐           \
        /            │  Rust 測試  │            \
       /             │  (7 個)     │             \  ← cargo test (test_cli.rs)
      /              └─────────────┘              \
```

## Playwright vs 其他 E2E 工具

| 面向 | Playwright | Cypress | Selenium |
|------|-----------|---------|----------|
| 瀏覽器支援 | Chromium/Firefox/WebKit | Chrome 為主 | 所有瀏覽器 |
| 語言 | Python/JS/Java/.NET | JavaScript | 多語言 |
| 自動等待 | 內建 | 內建 | 需手動 |
| 網路攔截 | 完整支援 | 有限 | 需 Proxy |
| 行動模擬 | 支援 | 不支援 | 有限 |
| 執行速度 | 快 | 中等 | 慢 |
| CI 整合 | 簡單 | 中等 | 簡單 |
| 除錯工具 | Trace Viewer | Dashboard | DevTools |

## E2E 測試最佳實踐

### 1. 使用 data-testid

避免依賴 CSS class 或文字內容（易變更）：

```html
<!-- 前端元件 -->
<input data-testid="todo-input" />
<button data-testid="add-button">Add</button>
<ul data-testid="todo-list">
  <li data-testid="todo-1">Buy milk</li>
</ul>
```

```python
# Playwright 定位
page.get_by_test_id("todo-input").fill("Buy milk")
page.get_by_test_id("add-button").click()
```

### 2. 測試獨立性

每個測試案例應可獨立執行：

```python
def test_empty_list(page):
    page.goto("http://localhost:5173")
    expect(page.get_by_text("No todos")).to_be_visible()

def test_add_todo(page):
    page.goto("http://localhost:5173")
    page.get_by_test_id("todo-input").fill("Buy milk")
    page.get_by_test_id("add-button").click()
    expect(page.get_by_text("Buy milk")).to_be_visible()
```

### 3. 避免過度等待

不要使用 `time.sleep()`（Playwright 自動等待）：

```python
# ❌ 不好
time.sleep(3)
page.click(".submit")

# ✅ 好
page.get_by_test_id("add-button").click()
page.get_by_text("Success").wait_for()
```

### 4. 關鍵流程優先

專注於最重要的使用者流程：

1. **註冊/登入** — 使用者身份驗證
2. **CRUD 操作** — 新增/讀取/更新/刪除
3. **錯誤處理** — 無效輸入、網路錯誤
4. **邊界案例** — 空列表、大量資料

### 5. 截圖除錯

測試失敗時自動截圖：

```python
def test_todo_flow(page):
    try:
        page.goto("http://localhost:5173")
        page.get_by_test_id("add-button").click()
        expect(page.get_by_text("Todo added")).to_be_visible()
    except Exception:
        page.screenshot(path="failure.png")
        raise
```

## 常見問題

### 瀏覽器未安裝

```bash
playwright install chromium
```

### 字體問題 (CI)

```bash
# Ubuntu CI
sudo apt-get install -y libnss3 libnspr4 libatk1.0-0 libatk-bridge2.0-0
```

### WebSocket 連線問題

確保前端的 API 代理設定正確（`vite.config.js`）：

```javascript
export default defineConfig({
  server: {
    proxy: {
      '/api': 'http://localhost:8001'
    }
  }
})
```

## 相關主題

- [測試](測試.md) — 測試策略總覽
- [持續整合](持續整合.md) — CI pipeline 中的 E2E 測試
- [DevOps](DevOps.md) — 自動化測試在 DevOps 的角色
- [Rust測試](Rust測試.md) — Rust 端測試模式
- [API設計](API設計.md) — API 設計與測試的關係
