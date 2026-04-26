# Git

Git 是目前最流行的分散式版本控制系統，由 Linux 核心作者 Linus Torvalds 於 2005 年開發。Git 的設計目標是處理大型專案的高速需求、支援分散式開發模式、確保資料完整性。與早期的集中式版本控制系統（如 SVN、CVS）不同，Git 是分散式的，每個開發者都有完整的專案歷史副本，這使得離線工作、分支合併變得更加靈活高效。

## Git 的核心概念

### 版本控制系統類型

```
┌─────────────────────────────────────────────────────────────┐
│               版本控制系統類型                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   集中式 (CVCS)          分散式 (DVCS)                      │
│   ┌─────────┐            ┌─────────┐                       │
│   │  SVN   │            │   Git   │                       │
│   │  CVS   │            │ Mercurial│                      │
│   └────┬────┘            └────┬────┘                       │
│        │                      │                             │
│        ▼                      ▼                             │
│   ┌─────────┐            ┌─────────┐                       │
│   │ Server  │            │  Server │                       │
│   │   ○    │            │    ○    │                       │
│   └────┬────┘            └────┬────┘                       │
│        │                      │                             │
│        ▼                      ▼                             │
│   ┌─────────┐            ┌─────────┐                       │
│   │ Client │            │ Client │                       │
│   │   ○    │            │   ○  ○  │                       │
│   └─────────┘            └─────────┘                       │
│                           每個客戶端都有完整副本              │
└─────────────────────────────────────────────────────────────┘
```

### 核心術語

**Repository（倉庫）**
- 存放專案和其所有歷史的地方
- 包含所有版本的資料和元資料
- 分為本地倉庫和遠端倉庫

**Commit（提交）**
- 對專案某一時刻的「快照」
- 包含作者的資訊、時間、提交訊息
- 每個 commit 有一個唯一的 SHA-1 雜湊值

**Branch（分支）**
- 獨立開發的程式碼線
- 允許同時進行多個功能的開發
- 預設分支通常是 main 或 master

**Merge（合併）**
- 將一個分支的變更合併到另一個分支
- 可能需要處理衝突

**HEAD**
- 指向當前checkout的 commit
- 可以移動到不同的分支或 commit

## Git 的資料模型

```
┌─────────────────────────────────────────────────────────────┐
│                   Git 資料模型                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   提交 (Commit)                                             │
│       │                                                     │
│       ├── tree (目錄結構)                                   │
│       │       │                                             │
│       │       ├── file A (blobs)                            │
│       │       └── subdir/ (tree)                            │
│       │             └── file B (blobs)                      │
│       │                                                     │
│       ├── parent (上一個 commit)                            │
│       │                                                     │
│       ├── author (作者)                                     │
│       │                                                     │
│       ├── committer (提交者)                                │
│       │                                                     │
│       └── message (提交訊息)                                 │
│                                                             │
│   物件類型：                                                 │
│   - blob: 檔案內容                                          │
│   - tree: 目錄結構                                          │
│   - commit: 提交                                           │
│   - tag: 標籤                                               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 檔案狀態

```
┌─────────────────────────────────────────────────────────────┐
│                   Git 檔案狀態                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   工作目錄                    暫存區 (Stage)                 │
│   ┌────────────┐           ┌────────────┐                 │
│   │ Modified   │ ───add──→ │  Staged    │ ──commit──→ Git │
│   └────────────┘           └────────────┘    倉庫         │
│        ↑                                                     │
│        │ checkout/ restore                                   │
│        └───────────────────────────────────                  │
│   Untracked (未追蹤): 新檔案，未被 Git 管理                  │
│   Modified (已修改): 已追蹤檔案被修改                        │
│   Staged (已暫存): 準備提交                                 │
│   Committed (已提交): 已安全儲存                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## 常用 Git 命令詳解

### 初始設定

```bash
# 設定使用者資訊（必須）
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# 設定預設分支名稱
git config --global init.defaultBranch main

# 設定 editor
git config --global core.editor vim

# 啟用彩色輸出
git config --global color.ui auto

# 設定 alias
git config --global alias.st status
git config --global alias.co checkout
git config --global alias.br branch
git config --global alias.cm commit
git config --global alias.lg "log --graph --oneline --decorate"

# 查看所有設定
git config --list
git config --list --show-origin
```

### 初始化與複製

```bash
# 初始化新倉庫
git init

# 複製現有倉庫
git clone https://github.com/user/repo.git
git clone https://github.com/user/repo.git my-folder
git clone --depth 1 https://github.com/user/repo.git  # 淺複製
```

### 基本操作

```bash
# 查看狀態
git status
git status -s  # 簡短輸出

# 查看差異
git diff                 # 工作目錄 vs 暫存區
git diff --staged        # 暫存區 vs 最新提交
git diff HEAD            # 工作目錄 vs 最新提交
git diff commit1 commit2 # 兩個 commit 之間的差異
git diff --stat          # 顯示統計資訊

# 暫存檔案
git add filename         # 暫存特定檔案
git add .                # 暫存所有修改
git add -p              # 互動式暫存（選擇部分變更）
git add -u              # 僅暫存已追蹤的檔案

# 提交
git commit -m "Message"           # 提交訊息
git commit -am "Message"           # 自動暫存並提交（僅限已追蹤檔案）
git commit --amend                 # 修改最後一次提交
git commit --amend --no-edit       # 修改最後一次提交但不改訊息
git commit --allow-empty -m "Empty" # 允許空提交

# 移除檔案
git rm filename           # 移除檔案並暫存
git rm --cached filename # 取消追蹤但保留檔案
git rm -r directory/     # 遞迴移除
```

### 分支操作

```bash
# 列出分支
git branch               # 本地分支
git branch -r            # 遠端分支
git branch -a            # 所有分支

# 創建分支
git branch feature-name
git checkout -b feature-name  # 創建並切換

# 切換分支
git checkout main
git switch main          # 更直觀的切換
git switch -c feature    # 創建並切換

# 刪除分支
git branch -d feature    # 合併後可安全刪除
git branch -D feature    # 強制刪除
git push origin --delete feature  # 刪除遠端分支

# 重命名分支
git branch -m old-name new-name

# 查看分支追蹤關係
git branch -vv
```

### 合併與 Rebase

```bash
# 合併分支
git merge feature        # 合併 feature 到當前分支
git merge --no-ff feature # 禁止 Fast-forward，保留分支歷史

# 解決衝突
git status              # 查看衝突檔案
git diff               # 查看衝突內容
# 手動編輯解決衝突
git add resolved-file
git commit            # 完成合併

# Rebase
git rebase main       # 將當前分支重新基於 main
git rebase -i HEAD~3  # 互動式 rebase 修改最近3個提交

# 放棄 rebase
git rebase --abort

# Cherry-pick
git cherry-pick commit-hash  # 撿取特定提交
```

### 遠端操作

```bash
# 設定遠端
git remote add origin https://github.com/user/repo.git
git remote -v              # 查看遠端
git remote show origin    # 查看遠端詳細資訊
git remote rename origin upstream
git remote remove origin

# 取得資料
git fetch origin          # 取得遠端更新（不合併）
git fetch --all           # 取得所有遠端
git pull origin main      # 取得並合併
git pull --rebase origin main  # 使用 rebase 合併

# 推送資料
git push origin main
git push -u origin main  # 設定上游分支
git push --force         # 強制推送（小心！）
git push --force-with-lease  # 安全的強制推送
```

### 查看歷史

```bash
# 檢視提交歷史
git log
git log --oneline        # 簡潔格式
git log --graph          # 圖形化
git log -n               # 最近 n 個
git log --author="Name"  # 特定作者
git log --since="2024-01-01"  # 時間範圍
git log --grep="keyword" # 提交訊息關鍵字
git log -p filename      # 檔案的變更歷史
git log --stat           # 顯示統計資訊

# 查看特定提交
git show commit-hash
git show HEAD~3          # 第3個祖先提交
git show HEAD~3:file     # 特定提交中的檔案

# Blame
git blame filename       # 查看檔案每行的最後修改
```

### stash（儲藏）

```bash
# 儲藏工作
git stash                # 儲藏當前修改
git stash save "message" # 帶訊息儲藏
git stash push -m "msg"  # 另一種語法

# 列出儲藏
git stash list

# 恢復儲藏
git stash pop           # 恢復並刪除
git stash apply         # 恢復但保留
git stash apply stash@{0} # 恢復特定儲藏

# 刪除儲藏
git stash drop stash@{0}
git stash clear         # 清除所有儲藏
```

## Git Flow 工作流程

```
┌─────────────────────────────────────────────────────────────┐
│                     Git Flow 模型                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   main ────────────●────────────────────●──────────────   │
│                         ↑                  ↑                 │
│   release/1.0 ───────────────────●──────↑                   │
│                                ↑  │                         │
│   develop ────●────●────●────●───●───┘                     │
│                    ↑         ↑                              │
│   feature/abc ──────────────●                              │
│                                                             │
│   hotfix/bug ────●───────↑                                  │
│                        │                                    │
│                                                             │
│   main: 生產環境代碼                                        │
│   develop: 開發主分支                                       │
│   feature: 新功能                                          │
│   release: 發布準備                                         │
│   hotfix: 緊急修復                                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Git Flow 命令

```bash
# 初始化 Git Flow
git flow init

# 功能分支
git flow feature start my-feature
git flow feature finish my-feature

# 發布分支
git flow release start 1.0.0
git flow release finish 1.0.0

# 修復分支
git flow hotfix start bugfix
git flow hotfix finish bugfix
```

## 進階技巧

### 修改歷史

```bash
# 修改最後一次提交
git commit --amend

# 互動式 rebase 修改多個提交
git rebase -i HEAD~5
# 在編輯器中將 pick 改為 edit 或 squash

# 還原提交
git revert HEAD           # 建立新提交還原
git reset --soft HEAD~1   # 撤銷提交但保留修改
git reset --hard HEAD~1  # 撤銷提交並丟棄修改（小心！）
```

### 子模組

```bash
# 添加子模組
git submodule add https://github.com/user/repo.git path/to/submodule

# 初始化子模組
git submodule update --init

# 遞迴初始化
git submodule update --init --recursive
```

### 子樹

```bash
# 添加子樹
git subtree add --prefix=dir https://github.com/user/repo.git main --squash

# 更新子樹
git subtree pull --prefix=dir https://github.com/user/repo.git main --squash
```

### Bisect（二分搜尋）

```bash
# 開始 bisect
git bisect start
git bisect bad              # 標記當前為 bad
git bisect good v1.0.0      # 標記已知正常的版本

# Git 會自動checkout 中間的commit
# 測試後標記
git bisect good  # 或 git bisect bad

# 找到後
git bisect reset  # 結束 bisect
```

## 遠端託管平臺

### GitHub

```bash
# GitHub CLI
gh repo create my-repo --public
gh issue create --title "Bug" --body "Description"
gh pr create --title "Feature" --body "Details"
gh pr merge
```

### GitLab

```bash
# GitLab CI 變數
CI_COMMIT_SHA
CI_PIPELINE_URL
CI_JOB_TOKEN
```

## 最佳實踐

### 提交訊息規範

```
<type>(<scope>): <subject>

<body>

<footer>

類型：
- feat: 新功能
- fix: 修復 bug
- docs: 文件
- style: 格式（不影響功能）
- refactor: 重構
- test: 測試
- chore: 維護

範例：
feat(auth): add login page

Add user login page with form validation.
- Email and password fields
- Client-side validation
- Error message display

Closes #123
```

### 常用 Alias

```bash
# ~/.gitconfig
[alias]
    # 狀態
    s = status -sb
    st = status
    
    # 日誌
    lg = log --oneline --graph --decorate
    hist = log --pretty=format:'%h %ad | %s%d [%an]' --graph --date=short
    
    # 差異
    d = diff
    dc = diff --cached
    
    # 提交
    cm = commit -m
    ca = commit -am
    amend = commit --amend --no-edit
    
    # 分支
    co = checkout
    cob = checkout -b
    br = branch
    brd = branch -d
    brD = branch -D
    
    # 其他
    undo = reset --soft HEAD~1
    save = stash push -m
    pop = stash pop
```

## 常見問題處理

### 恢復誤刪

```bash
# 恢復分支
git reflog              # 查看 HEAD 歷史
git checkout branch-name

# 恢復已刪除的檔案
git checkout HEAD~1 -- file.txt
```

### 處理大檔案

```bash
# 使用 Git LFS
git lfs install
git lfs track "*.psd"
git add .gitattributes

# 使用 git filter-repo
git filter-repo --path-folder path/ --invert-paths
```

## 相關主題

- [版本控制](版本控制.md) - 版本控制概念
- [持續整合](../軟體工程/持續整合.md) - CI/CD 實踐
- [Linus Torvalds](Linus_Torvalds.md) - Git 創始人