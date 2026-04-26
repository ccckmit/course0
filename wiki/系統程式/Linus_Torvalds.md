# Linus Torvalds

Linus Torvalds（1969 年 12 月 28 日－），芬蘭裔美國軟體工程師，Linux 核心與 Git 版本控制系統的原創作者，被譽為「Linux 之父」，是現代開源軟體運動的精神領袖。他的貢獻重新定義了作業系統與軟體開發的協作模式，影響了數十億使用者。

## 早年生活

### 赫爾辛基時期

Linus Torvalds 出生於芬蘭赫爾辛基，父親 Nils Torvalds 是芬蘭記者、詩人與政治家，母親 Anna Torvalds 是統計學家。從小在高智商家庭環境中成長，父親鼓勵他探索資訊科技。

### 程式設計的開始

```
早期經歷：
- 11 歲時在祖父的電腦上學習 BASIC
- 16 歲時開始使用 IBM PC 和 MS-DOS
- 1988 年進入赫爾辛基大學學習電腦科學
```

## Linux 核心

### 創作動機

1991 年，21 歲的 Linus 作為赫爾辛基大學學生，對當時的 MINIX 作業系統（Andrew Tanenbaum 教學用作業系統）不滿，決定編寫自己的作業系統核心：

```
時間線：
- 1991.08.25: 在 comp.os.minix 發布 Linux 0.01
- 1991.10.05: 發布 Linux 0.02
- 1991.12.20: 發布 Linux 0.11
- 1992.02: 改用 GPL 授權
- 1994.03: Linux 1.0.0 發布
```

### 技術特點

```c
// 原始 Linux核心特點
// - 簡潔設計
// - 預設排程
// - POSIX 相容
// - x86 架構

// 核心子系統
// - 程序管理
// - 記憶體管理
// - 檔案系統
// - 網路 Stack
// - 裝置驅動
```

### 發展模式

Linus 採用獨特的開發模式：

1. **日常dictatorship**：Linus 對核心重大決定有最終裁決權
2. **可信維護者**：信任核心貢獻者管理特定子系統
3. **快速發布**：每 6-10 週發布新版本
4. **公開開發**：所有開發在郵件列表上公開討論

```
開發社羣：
- 核心維護者 (Maintainer)
- 子系統維護者 (Subsystem Maintainer)
- 開發者 (Developer)
- 測試者 (Tester)
- 使用者 (User)
```

### 成就與影響

```
統計數據（截至 Linux 6.x）：
- 代碼行數：3000萬+ 行
- 貢獻者：15000+
- 支援架構：30+
- 支援硬體：數千種

生態系統：
- 發行版：Ubuntu, Debian, Fedora, Arch, etc.
- 嵌入式：Android, OpenWrt, etc.
- 雲端：AWS, GCP, Azure
- 超級電腦：Top500 的 99%
```

## Git 版本控制

### 創作背景

2005 年，由於 Linux 核心開發使用的 BitKeeper 撤銷免費授權，Linus 決定自己編寫版本控制系統：

```
目標：
- 速度
- 簡潔設計
- 對非線性開發的強大支援
- 分佈式
- 能有效處理大型專案
```

### 技術創新

```python
# Git 設計核心概念
# 
# 1. 快照 vs 差異
#    每個 commit 儲存完整快照
#    而非儲存差異
#
# 2. 分佈式
#    每個 clone 都是完整 repository
#    無需中央伺服器
#
# 3. 分支輕量
#    分支只是指向 commit 的指標
#    切換極快
#
# 4. 完整性
#    SHA-1 確保完整性
#    不可變更歷史
```

### Git 命令設計哲學

```bash
# Git 命令結構
# git <verb> <options>

# 核心命令
git init          # 初始化
git clone        # 複製
git add          # 暫存
git commit       # 提交
git push         # 推送
git pull         # 拉取
git branch       # 分支
git checkout     # 切換
git merge       # 合併
git rebase      # 變基
git status      # 狀態
git log         # 日誌

# 設計原則
# - 子命令模式
# - 快照語意
# - 可組合性
# - 離線優先
```

### 成就

```
現況：
- 1000萬+ Git 使用者
- 數百萬 repositories
- GitHub, GitLab, Bitbucket
- 標準版本控制
```

## 哲學與原則

### 開放原始碼

```c
// 開放原始碼哲學
// "Software is like a Cathedral..."
// - 功能比美學重要
// - 但兩者兼顧最好
// - 原始碼應該公開
// - 錯誤應盡快修復
```

### 實用主義

```
Linus 的原則：
- "Talk is cheap. Show me the code."
- 解決問題優先於理論
- 簡潔的解決方案
- 不要過度設計
- "If it works, don't fix it"
```

### 社羣管理

```c
// 開發原則
// 1. 完全透明
//    所有討論公開
// 2. 貢獻感謝
//    尊重每個貢獻者
// 3. 質量控制
//    嚴格的 code review
// 4. 快速回應
//    及時回覆 patch
```

## 個人特質

### 公開形象

```
言論風格：
- 直接、有時爭議性
- 對技術問題立場強硬
- 對政治正確批評
- 自稱內向

興趣愛好：
- 歷史
- 射擊
- 收集硬幣
- 科幻
```

### 工作風格

```c
// 工作特點
// - 早晨工作
// - 注重コード質量
// - 快速決策
// - 親自律
```

## 獲獎與榮譽

| 年份 | 獎項 |
|------|------|
| 2012 | Internet Hall of Fame |
| 2012 | Software Hall of Fame |
| 2014 | IEEE Computer Society Computer Pioneer Award |
| 2018 | The Mårbacka Prize |

## 相關概念

- [ Linux系統呼叫](Linux系統呼叫.md) - Linux 系統呼叫
- [ 作業系統](作業系統.md) - 作業系統基礎
- [ Git](../軟體工程/Git.md) - 版本控制
- [ Git](Git.md) - Git 系統程式
- [ 排程](排程.md) - 程序排程