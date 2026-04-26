#!/usr/bin/env python3
import agent0
import asyncio

import sys
import os

async def run():
    agent = agent0.Agent0()
    agent.workspace = os.path.expanduser("~/.agent0")
    os.makedirs(agent.workspace, exist_ok=True)
    
    print(f"Agent0 - {agent.model}")
    print("指令：/quit、/memory\n")
    
    while True:
        user_input = input("你：").strip()
        
        if not user_input:
            continue
        if user_input.lower() in ["/quit", "/exit", "/q"]:
            print("再見！")
            break
        if user_input.lower() == "/memory":
            print(f"關鍵資訊：{agent.key_info}")
            continue
        
        response, tool_result = await agent.run(user_input)
        
        print(f"\n🤖 {response}\n")

if __name__ == "__main__":
    asyncio.run(run())