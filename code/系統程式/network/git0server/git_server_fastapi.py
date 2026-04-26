import os
import re
import subprocess
from fastapi import FastAPI, Request, Response, HTTPException
import uvicorn

app = FastAPI(title="FastAPI Git Server")

# 設定存放 Git 儲存庫的根目錄
REPOS_DIR = os.path.join(os.path.dirname(os.path.abspath(__name__)), 'repos')

def secure_filename(filename: str) -> str:
    """簡單過濾檔名，防止路徑穿越 (Path Traversal) 攻擊"""
    return re.sub(r'[^a-zA-Z0-9_-]', '', filename)

def init_repo_if_not_exists(repo_name: str) -> str:
    """確保裸儲存庫 (Bare Repository) 存在，若無則自動建立"""
    repo_path = os.path.join(REPOS_DIR, repo_name)
    if not os.path.exists(repo_path):
        os.makedirs(repo_path)
        # 初始化 bare repo
        subprocess.run(["git", "init", "--bare"], cwd=repo_path, check=True)
        # 允許透過 HTTP 進行 push 操作 (非常重要)
        subprocess.run(["git", "config", "http.receivepack", "true"], cwd=repo_path, check=True)
    return repo_path

def get_pkt_line(data: str) -> bytes:
    """Git 協定需要的封包長度標頭格式 (4位十六進位長度 + 內容)"""
    length = len(data) + 4
    return f"{length:04x}{data}".encode('utf-8')

@app.get("/{repo_name}.git/info/refs")
async def info_refs(repo_name: str, service: str = None):
    """階段一：協商與發現 (Discovery)"""
    if service not in ['git-upload-pack', 'git-receive-pack']:
        raise HTTPException(status_code=400, detail="Unsupported service or service missing")

    safe_repo_name = secure_filename(repo_name)
    repo_path = init_repo_if_not_exists(safe_repo_name)
    
    # 執行 git 指令獲取 refs
    git_cmd = service.replace("git-", "")
    cmd = ["git", git_cmd, "--stateless-rpc", "--advertise-refs", repo_path]
    
    try:
        output = subprocess.check_output(cmd)
    except subprocess.CalledProcessError:
        raise HTTPException(status_code=500, detail="Git command failed")

    # 構造 Git Smart HTTP 回應
    # 格式: pkt-line(service_name) + 0000 + git_output
    service_line = get_pkt_line(f"# service={service}\n")
    flush_packet = b"0000"
    
    res_body = service_line + flush_packet + output
    
    return Response(
        content=res_body,
        media_type=f"application/x-{service}-advertisement"
    )

@app.post("/{repo_name}.git/{service}")
async def git_rpc(repo_name: str, service: str, request: Request):
    """階段二：資料傳輸 (Push / Pull)"""
    if service not in ['git-upload-pack', 'git-receive-pack']:
        raise HTTPException(status_code=400, detail="Unsupported service")

    safe_repo_name = secure_filename(repo_name)
    repo_path = init_repo_if_not_exists(safe_repo_name)
    
    git_cmd = service.replace("git-", "")
    cmd = ["git", git_cmd, "--stateless-rpc", repo_path]
    
    # 非同步讀取 Client 傳送過來的二進位資料 (Packfile)
    client_data = await request.body()
    
    # 將資料透過 stdin 餵給 Git，並讀取 stdout
    process = subprocess.Popen(
        cmd,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    
    # communicate 會將 client_data 送進 stdin 並等待執行完成
    stdout_data, stderr_data = process.communicate(input=client_data)
    
    if process.returncode != 0:
        raise HTTPException(status_code=500, detail="Git RPC failed")

    return Response(
        content=stdout_data,
        media_type=f"application/x-{service}-result"
    )

if __name__ == '__main__':
    # 確保儲存庫資料夾存在
    if not os.path.exists(REPOS_DIR):
        os.makedirs(REPOS_DIR)
        
    print(f"FastAPI Git Server 啟動中... 儲存庫目錄: {REPOS_DIR}")
    print("你可以使用以下指令測試：")
    print("git clone http://localhost:8000/fastapi_test.git")
    
    # 使用 Uvicorn 啟動 FastAPI 應用程式
    uvicorn.run(app, host='0.0.0.0', port=8000)