# make0

**make0** 是一個純 Python 實作的 Makefile 建置工具，完全不依賴系統的 `make`，透過直接呼叫 shell 來執行 recipe。

## 特色

- ✅ 解析標準 Makefile 語法
- ✅ 變數展開（`=`, `:=`, `?=`, `+=`, `!=`）
- ✅ 自動變數：`$@`, `$<`, `$^`, `$?`, `$*`, `$+`
- ✅ `.PHONY` 偽目標
- ✅ 檔案時間戳記比較（判斷是否需要重建）
- ✅ Recipe 前綴：`@`（靜音）、`-`（忽略錯誤）、`+`（強制執行）
- ✅ 內建函式：`wildcard`, `shell`, `notdir`, `dir`, `basename`, `suffix`, `subst`, `patsubst`, `filter`, `strip`, `word`, `words`, `firstword`, `lastword`
- ✅ `include` 指令
- ✅ Dry-run 模式 (`-n`)
- ✅ 彩色輸出

## 安裝

```bash
chmod +x make0.py
# 或直接用 python3 執行
python3 make0.py
```

## 用法

```
make0 [選項] [target ...]

選項:
  -f FILE       指定 Makefile（預設: Makefile）
  -n            Dry-run，只印出命令不執行
  -s            靜音模式，不印出 recipe 命令
  -p            印出解析後的變數與規則資料庫
  -l            列出所有 target
  -C DIR        切換至 DIR 目錄後再執行
  -e            環境變數覆蓋 Makefile 變數
  --var VAR=VAL 覆蓋特定變數
```

## 範例

```bash
pip install make0

# 建置預設 target
make0

# 建置多個 targets
make0 all clean

# 使用指定 Makefile
make0 -f Makefile.demo greet

# Dry-run
make0 -n all

# 列出所有 target
make0 -l

# 印出解析的資料庫
make0 -p

# 覆蓋變數
make0 --var CC=clang all
```

## Makefile 語法支援

| 語法 | 說明 |
|------|------|
| `VAR = val` | 遞迴展開（lazy） |
| `VAR := val` | 立即展開 |
| `VAR ?= val` | 僅在未定義時設定 |
| `VAR += val` | 附加 |
| `VAR != cmd` | Shell 命令輸出 |
| `$(VAR)` | 變數引用 |
| `$(shell cmd)` | 執行 shell |
| `$(wildcard *.c)` | 萬用字元 |
| `.PHONY: t` | 宣告偽目標 |
| `$@` | 目標檔案名稱 |
| `$<` | 第一個 prerequisite 檔案 |
| `$^` | 所有 prerequisites（自動去重） |
| `$?` | 所有比目標更新的 prerequisites |
| `@cmd` | 靜音執行 |
| `-cmd` | 忽略錯誤 |
