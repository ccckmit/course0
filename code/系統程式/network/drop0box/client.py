import socket
import os
import struct
import time
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

SERVER_HOST = '127.0.0.1' # 伺服器 IP (本機測試用 127.0.0.1)
SERVER_PORT = 9999        # 伺服器 Port
CLIENT_FOLDER = 'client_folder' # 客戶端要同步的資料夾

# 確保客戶端資料夾存在
if not os.path.exists(CLIENT_FOLDER):
    os.makedirs(CLIENT_FOLDER)

def send_file(filepath):
    # 給予短暫延遲，避免檔案還沒寫入完成就被讀取 (例如複製大檔案時)
    time.sleep(0.5) 
    
    try:
        filename = os.path.basename(filepath)
        file_size = os.path.getsize(filepath)

        # 建立 Socket 連線
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((SERVER_HOST, SERVER_PORT))

        # 1. 傳送檔名長度 (打包為 4 bytes unsigned int)
        filename_bytes = filename.encode('utf-8')
        client.sendall(struct.pack('<I', len(filename_bytes)))
        
        # 2. 傳送檔名
        client.sendall(filename_bytes)
        
        # 3. 傳送檔案大小 (打包為 8 bytes unsigned long long)
        client.sendall(struct.pack('<Q', file_size))

        # 4. 傳送檔案內容
        with open(filepath, 'rb') as f:
            while True:
                chunk = f.read(4096)
                if not chunk:
                    break
                client.sendall(chunk)
                
        print(f"[✔] 成功同步檔案: {filename}")
        client.close()
    except Exception as e:
        print(f"[-] 同步檔案 {filepath} 失敗: {e}")

class SyncHandler(FileSystemEventHandler):
    """監聽檔案系統變動的事件處理器"""
    def on_created(self, event):
        if not event.is_directory:
            print(f"[*] 偵測到新檔案: {event.src_path}")
            send_file(event.src_path)

    def on_modified(self, event):
        if not event.is_directory:
            print(f"[*] 偵測到檔案修改: {event.src_path}")
            send_file(event.src_path)

def start_client():
    print(f"[*] 客戶端啟動，正在監控資料夾: {CLIENT_FOLDER}")
    print("[*] 隨時可以將檔案拖入該資料夾進行同步...")
    
    event_handler = SyncHandler()
    observer = Observer()
    observer.schedule(event_handler, CLIENT_FOLDER, recursive=False)
    observer.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\n[*] 客戶端關閉中...")
        observer.stop()
    observer.join()

if __name__ == "__main__":
    start_client()