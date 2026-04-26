#!/usr/bin/env python3
# https://gist.github.com/dabit3/86ee04a1c02c839409a02b20fe99a492
# mini-openclaw.py - A minimal OpenClaw clone
# Run: uv run --with anthropic --with schedule python mini-openclaw.py

import anthropic
import subprocess
import json
import os
import re
import threading
import time
import schedule
from collections import defaultdict
from datetime import datetime

client = anthropic.Anthropic()

# ─── Configuration ───

WORKSPACE = os.path.expanduser("~/.mini-openclaw")
SESSIONS_DIR = os.path.join(WORKSPACE, "sessions")
MEMORY_DIR = os.path.join(WORKSPACE, "memory")
APPROVALS_FILE = os.path.join(WORKSPACE, "exec-approvals.json")

# ─── Agents ───

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
            "Use save_memory to store important information across sessions.\n"
            "Use memory_search at the start of conversations to recall context."
        ),
        "session_prefix": "agent:main",
    },
    "researcher": {
        "name": "Scout",
        "model": "claude-sonnet-4-5-20250929",
        "soul": (
            "You are Scout, a research specialist.\n"
            "Your job: find information and cite sources. Every claim needs evidence.\n"
            "Use tools to gather data. Be thorough but concise.\n"
            "Save important findings with save_memory for other agents to reference."
        ),
        "session_prefix": "agent:researcher",
    },
}

# ─── Tools ───

TOOLS = [
    {
        "name": "run_command",
        "description": "Run a shell command",
        "input_schema": {
            "type": "object",
            "properties": {
                "command": {"type": "string", "description": "The command to run"}
            },
            "required": ["command"]
        }
    },
    {
        "name": "read_file",
        "description": "Read a file from the filesystem",
        "input_schema": {
            "type": "object",
            "properties": {
                "path": {"type": "string", "description": "Path to the file"}
            },
            "required": ["path"]
        }
    },
    {
        "name": "write_file",
        "description": "Write content to a file (creates directories if needed)",
        "input_schema": {
            "type": "object",
            "properties": {
                "path": {"type": "string", "description": "Path to the file"},
                "content": {"type": "string", "description": "Content to write"}
            },
            "required": ["path", "content"]
        }
    },
    {
        "name": "save_memory",
        "description": "Save important information to long-term memory",
        "input_schema": {
            "type": "object",
            "properties": {
                "key": {"type": "string", "description": "Short label (e.g. 'user-preferences')"},
                "content": {"type": "string", "description": "The information to remember"}
            },
            "required": ["key", "content"]
        }
    },
    {
        "name": "memory_search",
        "description": "Search long-term memory for relevant information",
        "input_schema": {
            "type": "object",
            "properties": {
                "query": {"type": "string", "description": "What to search for"}
            },
            "required": ["query"]
        }
    },
]

# ─── Permission Controls ───

SAFE_COMMANDS = {"ls", "cat", "head", "tail", "wc", "date", "whoami",
                 "echo", "pwd", "which", "git", "python", "node", "npm"}

def load_approvals():
    if os.path.exists(APPROVALS_FILE):
        with open(APPROVALS_FILE) as f:
            return json.load(f)
    return {"allowed": [], "denied": []}

def save_approval(command, approved):
    approvals = load_approvals()
    key = "allowed" if approved else "denied"
    if command not in approvals[key]:
        approvals[key].append(command)
    with open(APPROVALS_FILE, "w") as f:
        json.dump(approvals, f, indent=2)

def check_command_safety(command):
    base_cmd = command.strip().split()[0] if command.strip() else ""
    if base_cmd in SAFE_COMMANDS:
        return "safe"
    approvals = load_approvals()
    if command in approvals["allowed"]:
        return "approved"
    return "needs_approval"

# ─── Tool Execution ───

def execute_tool(name, tool_input):
    if name == "run_command":
        cmd = tool_input["command"]
        safety = check_command_safety(cmd)
        if safety == "needs_approval":
            print(f"\n  ⚠️  Command: {cmd}")
            confirm = input("  Allow? (y/n): ").strip().lower()
            if confirm != "y":
                save_approval(cmd, False)
                return "Permission denied by user."
            save_approval(cmd, True)
        try:
            result = subprocess.run(
                cmd, shell=True, capture_output=True, text=True, timeout=30
            )
            output = result.stdout + result.stderr
            return output if output else "(no output)"
        except subprocess.TimeoutExpired:
            return "Command timed out after 30 seconds"
        except Exception as e:
            return f"Error: {e}"

    elif name == "read_file":
        try:
            with open(tool_input["path"], "r") as f:
                return f.read()[:10000]
        except Exception as e:
            return f"Error: {e}"

    elif name == "write_file":
        try:
            os.makedirs(os.path.dirname(tool_input["path"]) or ".", exist_ok=True)
            with open(tool_input["path"], "w") as f:
                f.write(tool_input["content"])
            return f"Wrote to {tool_input['path']}"
        except Exception as e:
            return f"Error: {e}"

    elif name == "save_memory":
        os.makedirs(MEMORY_DIR, exist_ok=True)
        filepath = os.path.join(MEMORY_DIR, f"{tool_input['key']}.md")
        with open(filepath, "w") as f:
            f.write(tool_input["content"])
        return f"Saved to memory: {tool_input['key']}"

    elif name == "memory_search":
        query = tool_input["query"].lower()
        results = []
        if os.path.exists(MEMORY_DIR):
            for fname in os.listdir(MEMORY_DIR):
                if fname.endswith(".md"):
                    with open(os.path.join(MEMORY_DIR, fname), "r") as f:
                        content = f.read()
                    if any(w in content.lower() for w in query.split()):
                        results.append(f"--- {fname} ---\n{content}")
        return "\n\n".join(results) if results else "No matching memories found."

    return f"Unknown tool: {name}"

# ─── Session Management ───

def get_session_path(session_key):
    os.makedirs(SESSIONS_DIR, exist_ok=True)
    safe_key = session_key.replace(":", "_").replace("/", "_")
    return os.path.join(SESSIONS_DIR, f"{safe_key}.jsonl")

def load_session(session_key):
    path = get_session_path(session_key)
    messages = []
    if os.path.exists(path):
        with open(path, "r") as f:
            for line in f:
                if line.strip():
                    try:
                        messages.append(json.loads(line))
                    except json.JSONDecodeError:
                        continue
    return messages

def append_message(session_key, message):
    with open(get_session_path(session_key), "a") as f:
        f.write(json.dumps(message) + "\n")

def save_session(session_key, messages):
    with open(get_session_path(session_key), "w") as f:
        for msg in messages:
            f.write(json.dumps(msg) + "\n")

# ─── Compaction ───

def estimate_tokens(messages):
    return sum(len(json.dumps(m)) for m in messages) // 4

def compact_session(session_key, messages):
    if estimate_tokens(messages) < 100_000:
        return messages
    split = len(messages) // 2
    old, recent = messages[:split], messages[split:]
    print("\n  📦 Compacting session history...")
    summary = client.messages.create(
        model="claude-sonnet-4-5-20250929",
        max_tokens=2000,
        messages=[{
            "role": "user",
            "content": (
                "Summarize this conversation concisely. Preserve key facts, "
                "decisions, and open tasks:\n\n"
                f"{json.dumps(old, indent=2)}"
            )
        }]
    )
    compacted = [{
        "role": "user",
        "content": f"[Conversation summary]\n{summary.content[0].text}"
    }] + recent
    save_session(session_key, compacted)
    return compacted

# ─── Command Queue ───

session_locks = defaultdict(threading.Lock)

# ─── Agent Loop ───

def serialize_content(content):
    serialized = []
    for block in content:
        if hasattr(block, "text"):
            serialized.append({"type": "text", "text": block.text})
        elif block.type == "tool_use":
            serialized.append({
                "type": "tool_use", "id": block.id,
                "name": block.name, "input": block.input
            })
    return serialized

def run_agent_turn(session_key, user_text, agent_config):
    """Run a full agent turn: load session, call LLM in a loop, save."""
    with session_locks[session_key]:
        messages = load_session(session_key)
        messages = compact_session(session_key, messages)

        user_msg = {"role": "user", "content": user_text}
        messages.append(user_msg)
        append_message(session_key, user_msg)

        for _ in range(20):  # max tool-use turns
            response = client.messages.create(
                model=agent_config["model"],
                max_tokens=4096,
                system=agent_config["soul"],
                tools=TOOLS,
                messages=messages
            )

            content = serialize_content(response.content)
            assistant_msg = {"role": "assistant", "content": content}
            messages.append(assistant_msg)
            append_message(session_key, assistant_msg)

            if response.stop_reason == "end_turn":
                return "".join(
                    b.text for b in response.content if hasattr(b, "text")
                )

            if response.stop_reason == "tool_use":
                tool_results = []
                for block in response.content:
                    if block.type == "tool_use":
                        print(f"  🔧 {block.name}: {json.dumps(block.input)[:100]}")
                        result = execute_tool(block.name, block.input)
                        display = str(result)[:150]
                        print(f"     → {display}")
                        tool_results.append({
                            "type": "tool_result",
                            "tool_use_id": block.id,
                            "content": str(result)
                        })

                results_msg = {"role": "user", "content": tool_results}
                messages.append(results_msg)
                append_message(session_key, results_msg)

        return "(max turns reached)"

# ─── Multi-Agent Routing ───

def resolve_agent(message_text):
    """Route messages to the right agent based on prefix commands."""
    if message_text.startswith("/research "):
        return "researcher", message_text[len("/research "):]
    return "main", message_text

# ─── Cron / Heartbeats ───

def setup_heartbeats():
    def morning_check():
        print("\n⏰ Heartbeat: morning check")
        result = run_agent_turn(
            "cron:morning-check",
            "Good morning! Check today's date and give me a motivational quote.",
            AGENTS["main"]
        )
        print(f"🤖 {result}\n")

    schedule.every().day.at("07:30").do(morning_check)

    def scheduler_loop():
        while True:
            schedule.run_pending()
            time.sleep(60)

    threading.Thread(target=scheduler_loop, daemon=True).start()

# ─── REPL ───

def main():
    for d in [WORKSPACE, SESSIONS_DIR, MEMORY_DIR]:
        os.makedirs(d, exist_ok=True)

    setup_heartbeats()

    session_key = "agent:main:repl"

    print("Mini OpenClaw")
    print(f"  Agents: {', '.join(a['name'] for a in AGENTS.values())}")
    print(f"  Workspace: {WORKSPACE}")
    print("  Commands: /new (reset), /research <query>, /quit\n")

    while True:
        try:
            user_input = input("You: ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\nGoodbye!")
            break

        if not user_input:
            continue
        if user_input.lower() in ["/quit", "/exit", "/q"]:
            print("Goodbye!")
            break
        if user_input.lower() == "/new":
            session_key = f"agent:main:repl:{datetime.now().strftime('%Y%m%d%H%M%S')}"
            print("  Session reset.\n")
            continue

        agent_id, message_text = resolve_agent(user_input)
        agent_config = AGENTS[agent_id]
        sk = (
            f"{agent_config['session_prefix']}:repl"
            if agent_id != "main" else session_key
        )

        response = run_agent_turn(sk, message_text, agent_config)
        print(f"\n🤖 [{agent_config['name']}] {response}\n")

if __name__ == "__main__":
    main()