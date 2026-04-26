# VSCode

## 概述

Visual Studio Code（簡稱 VSCode）是由 Microsoft 開發的免費開源程式碼編輯器，於 2015 年首次發布。VSCode 以其輕量、快速的特性，豐富的擴展生態系統，以及對多種程式語言的原生支援，成為全球最受歡迎的開發環境之一。

VSCode 基於 Electron 框架構建，結合了編輯器的速度和灵活性與 IDE 的功能齊全。它支援 Windows、macOS 和 Linux 三大平台，擁有活躍的社區支持和持續的功能更新。

## 核心功能

### IntelliSense

VSCode 的 IntelliSense 提供智能程式碼完成功能，包括語法高亮、程式碼片段、參數提示等。

```typescript
// 智慧型自動完成
const user = {
    name: "John",
    age: 30
};

// 輸入 user. 時會自動提示 name, age
console.log(user.name);
```

### 偵錯功能

VSCode 內建強大的偵錯支援，支援多種語言和執行環境。

```json
// launch.json 偵錯配置
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "node",
            "request": "launch",
            "name": "Launch Program",
            "program": "${workspaceFolder}/app.js"
        },
        {
            "type": "python",
            "request": "launch",
            "name": "Python: Current File",
            "pythonPath": "${config:python.pythonPath}"
        }
    ]
}
```

### Git 整合

VSCode 內建 Git 版本控制支援，讓開發者可以直接在編輯器中管理程式碼版本。

```bash
# VSCode Git 面板功能
- 提交更改 (Commit)
- 推送 (Push)
- 拉取 (Pull)
- 分支管理 (Branch)
- 衝突解決 (Merge Conflict)
- 歷史記錄 (History)
```

### 終端整合

VSCode 內建整合式終端，無需切換視窗即可執行命令。

```bash
# 在 VSCode 終端中
$ npm run dev
$ git commit -m "feat: add new feature"
$ docker-compose up
```

## 常用快捷鍵

### 導航類

| 快捷鍵 | 功能 | macOS |
|--------|------|-------|
| Cmd+P | 快速開啟檔案 | ⌘P |
| Cmd+Shift+P | 命令面板 | ⌘⇧P |
| Cmd+Shift+O | 跳轉到符號 | ⌘⇧O |
| Cmd+G | 跳轉到行 | ⌘G |
| Cmd+Tab | 切換標籤頁 | ⌘Tab |

### 編輯類

| 快捷鍵 | 功能 | macOS |
|--------|------|-------|
| Cmd+D | 選取下一個相同文字 | ⌘D |
| Cmd+Shift+L | 選取所有相同文字 | ⌘⇧L |
| Cmd+/ | 註解切換 | ⌘/ |
| Alt+Up/Down | 移動行 | ⌥↑/↓ |
| Cmd+Alt+F | 格式化選取 | ⌘⌥F |
| Cmd+K Cmd+S | 快捷鍵設定 | ⌘K⌘S |

### 搜尋類

| 快捷鍵 | 功能 |
|--------|------|
| Cmd+F | 檔案內搜尋 |
| Cmd+Shift+F | 全域搜尋 |
| Cmd+H | 取代 |

### 多游標

| 快捷鍵 | 功能 |
|--------|------|
| Alt+Click | 新增游標 |
| Cmd+Alt+Down | 向下新增游標 |
| Cmd+Shift+L | 選取所有匹配 |

## 擴充推薦

### 必備擴充

```bash
# 程式碼格式化
Prettier - Code formatter

# 程式碼檢查
ESLint
Pylint (Python)

# 主題
One Dark Pro
Dracula Theme

# 圖示
Material Icon Theme

# 遠端開發
Remote - SSH
Remote - Containers
```

### 前端開發

```bash
# Vue
Vetur
Vue 3 Snippets

# React
ES7+ React snippets
Prettier - Code formatter

# Tailwind CSS
Tailwind CSS IntelliSense
```

### 後端開發

```bash
# Python
Python
Jupyter

# Node.js
npm Intellisense
Node.js Modules Intellisense

# Docker
Docker
Docker Explorer
```

### 工具類

```bash
# Git
GitLens — Git supercharged
GitHub Pull Requests and Issues

# AI 輔助
GitHub Copilot
Copilot Chat

# REST 客戶端
Thunder Client
REST Client
```

## 設定檔案

### settings.json

```json
{
    // 編輯器
    "editor.formatOnSave": true,
    "editor.tabSize": 2,
    "editor.wordWrap": "on",
    "editor.minimap.enabled": true,
    "editor.fontSize": 14,
    "editor.fontFamily": "Fira Code, Consolas, monospace",
    "editor.fontLigatures": true,
    
    // 檔案
    "files.autoSave": "onFocusChange",
    "files.exclude": {
        "**/node_modules": true,
        "**/.git": true
    },
    
    // 終端
    "terminal.integrated.fontSize": 14,
    "terminal.integrated.cursorStyle": "line",
    
    // 工作區
    "workbench.colorTheme": "One Dark Pro",
    "workbench.iconTheme": "material-icon-theme",
    
    // TypeScript
    "typescript.format.enable": true,
    "javascript.format.enable": true,
    
    // Python
    "python.linting.enabled": true,
    "python.formatting.provider": "black"
}
```

### keybindings.json

```json
[
    {
        "key": "cmd+k cmd+s",
        "command": "workbench.action.openGlobalKeybindings"
    },
    {
        "key": "cmd+b",
        "command": "toggleSidebarVisibility"
    },
    {
        "key": "cmd+j",
        "command": "togglePanel"
    }
]
```

## 遠端開發

### Remote - SSH

透過 SSH 連接到遠端伺服器進行開發。

```bash
# 1. 安裝 Remote - SSH 擴充
# 2. Cmd+Shift+P -> Remote-SSH: Connect to Host
# 3. 輸入伺服器 SSH 地址
# 4. 輸入密碼/金鑰
```

### Dev Containers

使用 Docker 容器進行隔離開發。

```dockerfile
# .devcontainer/Dockerfile
FROM mcr.microsoft.com/vscode/devcontainers/javascript-node:18

# 安裝額外工具
RUN npm install -g typescript eslint prettier

# 設定工作目錄
WORKDIR /workspace
```

```json
// .devcontainer/devcontainer.json
{
    "build": {
        "dockerfile": "Dockerfile"
    },
    "customizations": {
        "vscode": {
            "extensions": [
                "dbaeumer.vscode-eslint",
                "esbenp.prettier-vscode"
            ]
        }
    }
}
```

## 工作區

### 多資料夾工作區

```json
// .code-workspace
{
    "folders": [
        {
            "path": "."
        },
        {
            "path": "../shared-lib"
        }
    ],
    "settings": {
        "typescript.preferences.importModuleSpecifier": "relative"
    }
}
```

### 偵錯配置

```json
// .vscode/launch.json
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "node",
            "request": "launch",
            "name": "Debug Server",
            "skipFiles": ["<node_internals>/**"],
            "program": "${workspaceFolder}/src/index.js",
            "env": {
                "NODE_ENV": "development"
            }
        }
    ]
}
```

## 整合終端

### Zsh + Oh My Zsh

```bash
# 安裝 Oh My Zsh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"

# 在 VSCode 終端中使用
# 設定 → terminal.integrated.defaultProfile.osx: zsh
```

### Task 自動化

```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build",
            "type": "shell",
            "command": "npm run build",
            "problemMatcher": [],
            "group": "build"
        },
        {
            "label": "Test",
            "type": "shell", 
            "command": "npm test",
            "problemMatcher": ["$jest"],
            "group": "test"
        }
    ]
}
```

## 效能優化

### 停用不需要的功能

```json
{
    "editor.minimap.enabled": false,
    "editor.cursorBlinking": "solid",
    "editor.selectionHighlight": false,
    "editor bracketPairColorization.enabled": false
}
```

### 排除大型資料夾

```json
{
    "files.exclude": {
        "**/node_modules": true,
        "**/dist": true,
        "**/build": true,
        "**/.git": true
    }
}
```

## 相關概念

- [Git](../軟體工程/Git.md) - 版本控制
- [JavaScript](JavaScript.md) - JavaScript 程式語言
- [Node.js](Node.js.md) - JavaScript 執行環境

## Tags

#VSCode #IDE #編輯器 #開發工具 #Microsoft