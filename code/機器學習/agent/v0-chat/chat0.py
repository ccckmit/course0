#!/usr/bin/env python3
# chat0.py - AI Chat using Ollama
# Run: python chat0.py

import asyncio
import aiohttp
import os

MODEL = "minimax-m2.5:cloud"

async def chat_ollama(messages: list) -> str:
    """Call Ollama API with chat format"""
    payload = {
        "model": MODEL,
        "messages": messages,
        "stream": False
    }
    
    async with aiohttp.ClientSession() as session:
        async with session.post(
            "http://localhost:11434/api/chat",
            json=payload,
            timeout=aiohttp.ClientTimeout(total=120)
        ) as resp:
            result = await resp.json()
            return result.get("message", {}).get("content", "").strip()

def main():
    print(f"Chat0 - {MODEL}")
    print("Commands: /quit, /clear\n")
    
    messages = []
    
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
        if user_input.lower() == "/clear":
            messages = []
            print("Conversation cleared.\n")
            continue
        
        messages.append({"role": "user", "content": user_input})
        
        response = asyncio.run(chat_ollama(messages))
        messages.append({"role": "assistant", "content": response})
        
        print(f"\n🤖 {response}\n")

if __name__ == "__main__":
    main()
