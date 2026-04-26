# py0

Python 直譯器實驗專案

## 架構

| 元件 | 描述 |
|------|------|
| **py0** | Python 直譯器 (使用 Python stdlib `ast`，自己實作執行引擎) |
| **cpy0/py** | 極簡 Python host，支援 Python 版本自我解譯路徑 |
| **cpy0/c** | C 語言 host/runtime，支援 C 版本自我解譯路徑 |

## 快速開始

### py0 直譯器
```bash
python py0/py0i.py <script.py> [args...]
```

### cpy0 Python host
```bash
python cpy0/py/host.py cpy0/py/cpy0i.py <script.py> [args...]
```

### cpy0 C host
```bash
cd cpy0/c
./py0i ../../tests/hello.py
```

### 測試
```bash
bash py0/../cpy0/py/test.sh
bash py0/../cpy0/c/test.sh
bash py0/test.sh
```

## 文件

- `_doc/BNF.md` - py0 支援的 Python 語法
- `py0/README.md` - py0 架構說明
- `_doc/cpy0i_EBNF.md` - cpy0i 子集語法

## 測試檔案

- `tests/hello.py`, `tests/fact.py`, `tests/json.py` - 共享測試

## py0 語言特色

- 類別與繼承
- 閉包與裝飾器
- 例外處理 (try/catch/finally)
- 列表/字典/集合推導式
- f-string 格式化
