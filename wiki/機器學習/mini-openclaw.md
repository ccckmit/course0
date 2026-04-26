# mini-openclaw

## 概述

mini-openclaw 是一個用約 400 行 Python 實現的極簡 AI Agent，模擬 Claude/OpenClaw 的設計理念。支援多 Agent、工具呼叫、對話記憶和任務排程。

## 基本資訊

| 項目 | 內容 |
|------|------|
| 作者 |@dabit3 |
| GitHub Gist | https://gist.github.com/dabit3/86ee04a1c02c839409a02b20fe99a492 |
| 語言 | Python |
| 星標 | 14 |
| 依賴 | anthropic, schedule |

## 執行方式

```bash
uv run --with anthropic --with schedule python mini-openclaw.py
```

## 核心架構

```
mini-openclaw 架構
┌─────────────────────────────────────────────────────────────┐
│                        Agent 系統                            │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  Agent 定義                                          │    │
│  │  - main: Jarvis (主助理)                             │    │
│  │  - researcher: Scout (研究專家)                      │    │
│  └─────────────────────────────────────────────────────┘    │
│                         ↓                                   │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  工具系統 (Tools)                                   │    │
│  │  - run_command: 執行 shell 命令                      │    │
│  │  - read_file/write_file: 檔案操作                   │    │
│  │  - save_memory/memory_search: 長期記憶               │    │
│  └─────────────────────────────────────────────────────┘    │
│                         ↓                                   │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  會話管理 (Session Management)                       │    │
│  │  - 對話歷史持久化                                   │    │
│  │  - 對話壓縮 (Compaction)                            │    │
│  │  - Token 估算                                       │    │
│  └─────────────────────────────────────────────────────┘    │
│                         ↓                                   │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  排程系統 (Scheduler)                               │    │
│  │  - 定時任務執行                                     │    │
│  │  - 每日心跳                                         │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

## Agent 定義

```python
AGENTS = {
    "main": {
        "name": "Jarvis",
        "model": "claude-sonnet-4-5-20250929",
        "soul": (
            "You are Jarvis, a personal AI assistant.\n"
            "Be genuinely helpful. Skip the pleasantries. Have opinions.\n"
            "You have tools — use them proactively.\n\n"
            "## Memory\n"
            f"Your workspace is {WORKSPACE}.\n"
            "Use save_memory to store important information."
        ),
    },
    "researcher": {
        "name": "Scout",
        "model": "claude-sonnet-4-5-20250929",
        "soul": (
            "You are Scout, a research specialist.\n"
            "Your job: find information and cite sources."
        ),
    },
}
```

## 工具系統

### 可用工具

```python
TOOLS = [
    {
        "name": "run_command",
        "description": "Run a shell command",
        "input_schema": {
            "type": "object",
            "properties": {
                "command": {"type": "string"}
            },
            "required": ["command"]
        }
    },
    {
        "name": "read_file",
        "description": "Read a file from the filesystem"
    },
    {
        "name": "write_file",
        "description": "Write content to a file"
    },
    {
        "name": "save_memory",
        "description": "Save important information to long-term memory"
    },
    {
        "name": "memory_search",
        "description": "Search long-term memory for relevant information"
    },
]
```

### 工具執行

```python
def execute_tool(name, tool_input):
    if name == "run_command":
        cmd = tool_input["command"]
        # 安全檢查
        safety = check_command_safety(cmd)
        if safety == "needs_approval":
            confirm = input(f"Allow '{cmd}'? (y/n): ")
            if confirm != "y":
                return "Permission denied."
        
        # 執行命令
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.stdout + result.stderr
    
    elif name == "read_file":
        with open(tool_input["path"], "r") as f:
            return f.read()[:10000]
    
    elif name == "write_file":
        os.makedirs(os.path.dirname(tool_input["path"]) or ".", exist_ok=True)
        with open(tool_input["path"], "w") as f:
            f.write(tool_input["content"])
        return f"Wrote to {tool_input['path']}"
    
    elif name == "save_memory":
        filepath = os.path.join(MEMORY_DIR, f"{tool_input['key']}.md")
        with open(filepath, "w") as f:
            f.write(tool_input["content"])
        return f"Saved to memory: {tool_input['key']}"
    
    elif name == "memory_search":
        # 搜尋記憶體
        results = []
        for fname in os.listdir(MEMORY_DIR):
            if fname.endswith(".md"):
                with open(os.path.join(MEMORY_DIR, fname)) as f:
                    if query.lower() in f.read().lower():
                        results.append(f.read())
        return "\n\n".join(results)
```

## 安全控制

```python
# 白名單命令（無需審批）
SAFE_COMMANDS = {"ls", "cat", "head", "tail", "wc", "date", 
                 "whoami", "echo", "pwd", "which", "git", 
                 "python", "node", "npm"}

# 許可權持久化
APPROVALS_FILE = "~/.mini-openclaw/exec-approvals.json"

def check_command_safety(command):
    base_cmd = command.strip().split()[0]
    if base_cmd in SAFE_COMMANDS:
        return "safe"
    if command in approvals["allowed"]:
        return "approved"
    return "needs_approval"
```

## 會話管理

### 對話持久化

```python
def get_session_path(session_key):
    safe_key = session_key.replace(":", "_").replace("/", "_")
    return os.path.join(SESSIONS_DIR, f"{safe_key}.jsonl")

def load_session(session_key):
    messages = []
    with open(get_session_path(session_key)) as f:
        for line in f:
            messages.append(json.loads(line))
    return messages

def append_message(session_key, message):
    with open(get_session_path(session_key), "a") as f:
        f.write(json.dumps(message) + "\n")
```

### 對話壓縮

```python
def compact_session(session_key, messages):
    # 超過 100K tokens 時壓縮前半段
    if estimate_tokens(messages) < 100_000:
        return messages
    
    split = len(messages) // 2
    old, recent = messages[:split], messages[split:]
    
    # 使用 LLM 總結歷史
    summary = client.messages.create(
        model="claude-sonnet-4-5-20250929",
        messages=[{
            "role": "user",
            "content": f"Summarize this conversation:\n{json.dumps(old)}"
        }]
    )
    
    return [{"role": "user", "content": f"[Summary]\n{summary}"}, recent]
```

## Agent 執行迴圈

```python
def run_agent_turn(session_key, user_text, agent_config):
    with session_locks[session_key]:
        messages = load_session(session_key)
        messages = compact_session(session_key, messages)
        
        for _ in range(20):  # 最大工具呼叫次數
            response = client.messages.create(
                model=agent_config["model"],
                system=agent_config["soul"],
                tools=TOOLS,
                messages=messages
            )
            
            # 處理回覆
            if response.stop_reason == "end_turn":
                return response.text
            
            if response.stop_reason == "tool_use":
                # 執行工具
                for block in response.content:
                    if block.type == "tool_use":
                        result = execute_tool(block.name, block.input)
                        messages.append({
                            "type": "tool_result",
                            "tool_use_id": block.id,
                            "content": result
                        })
```

## 排程系統

```python
def setup_heartbeats():
    def morning_check():
        result = run_agent_turn(
            "cron:morning-check",
            "Good morning! Check today's date and give me a motivational quote.",
            AGENTS["main"]
        )
        print(f"🤖 {result}")
    
    schedule.every().day.at("07:30").do(morning_check)

def scheduler_loop():
    while True:
        schedule.run_pending()
        time.sleep(60)
```

## REPL 介面

```bash
Mini OpenClaw
Agents: Jarvis, Scout
Workspace: ~/.mini-openclaw
Commands: /new (reset), /research <query>, /quit

You: /research transformer architecture
[Scout] The Transformer architecture was introduced in "Attention Is All You Need"...

You: Create a new file called hello.py
[Jarvis] I'll create that file for you.
 → Wrote to hello.py
[Jarvis] Done! Created hello.py with a basic template.

You: /quit
Goodbye!
```

## 設計模式

### 極簡 Agent 架構
```
使用者輸入 → Agent 路由 → LLM 呼叫 → 工具執行 → 回應輸出
     ↑                                    │
     └─────────── 記憶體搜尋 ←────────────┘
```

### 多 Agent 協作
```
使用者 ──┬── /research → researcher Agent
         └── 一般問題 → main Agent
```

## 與 OpenClaw 的差異

| 功能 | mini-openclaw | OpenClaw |
|------|---------------|----------|
| Agent 數量 | 2 | 可擴展 |
| 工具數量 | 5 | 更多 |
| 記憶體 | 檔案系統 | 向量資料庫 |
| 排程 | 基本 | 完整 |
| 許可權控制 | 基礎 | 進階 |

## 教學價值

mini-openclaw 展示了 AI Agent 的核心組成：
1. **對話管理** - 歷史維護和上下文
2. **工具系統** - 擴展 LLM 能力
3. **記憶系統** - 跨會話知識
4. **安全控制** - 工具執行審批
5. **排程執行** - 自動化任務

## 相關資源

- GitHub Gist：https://gist.github.com/dabit3/86ee04a1c02c839409a02b20fe99a492
- 相關主題：[大型語言模型](大型語言模型.md)

## Tags

#AI_Agent #Claude #LLM #工具學習 #Python
