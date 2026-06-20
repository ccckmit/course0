import subprocess
import time
import requests
from playwright.sync_api import sync_playwright


def test_frontend():
    api_proc = subprocess.Popen(
        ["uvicorn", "todo.api:app", "--port", "8001"],
    )
    frontend_proc = subprocess.Popen(
        ["npx", "vite", "--port", "5173"],
        cwd="../frontend"
    )
    time.sleep(3)

    try:
        requests.post("http://localhost:8001/api/todos", json={"text": "test from playwright"})

        with sync_playwright() as p:
            browser = p.chromium.launch()
            page = browser.new_page()
            page.goto("http://localhost:5173")
            assert "test from playwright" in page.text_content("body")
            browser.close()
    finally:
        api_proc.terminate()
        frontend_proc.terminate()
