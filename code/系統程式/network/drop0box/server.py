import socket
import threading
import os
import struct

HOST = '0.0.0.0'  # 監聽所有網路介面
PORT = 9999       # 設定通訊埠
SERVER_FOLDER = 'server_folder' # 伺服器儲存檔案的資料夾

# 確保伺服器資料夾存在
if not os.path.exists(SERVER_FOLDER):
    os.makedirs(SERVER_FOLDER)

def handle_client(conn, addr):
    print(f"[+] 來自 {addr} 的連線")
    try:
        # 1. 接收檔名字節長度 (4 bytes)
        raw_name_len = conn.recv(4)
        if not raw_name_len:
            return
        name_len = struct.unpack('<I', raw_name_len)[0]

        # 2. 接收檔名
        filename = conn.recv(name_len).decode('utf-8')
        
        # 3. 接收檔案大小 (8 bytes)
        raw_file_size = conn.recv(8)
        file_size = struct.unpack('<Q', raw_file_size)[0]

        print(f"[*] 準備接收檔案: {filename} (大小: {file_size} bytes)")

        # 4. 接收並寫入檔案資料
        filepath = os.path.join(SERVER_FOLDER, filename)
        with open(filepath, 'wb') as f:
            bytes_received = 0
            while bytes_received < file_size:
                # 每次最多接收 4096 bytes
                chunk = conn.recv(min(file_size - bytes_received, 4096))
                if not chunk:
                    break
                f.write(chunk)
                bytes_received += len(chunk)

        print(f"[✔] 檔案 {filename} 接收完成！")
    except Exception as e:
        print(f"[-] 處理連線時發生錯誤: {e}")
    finally:
        conn.close()

def start_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen(5)
    print(f"[*] 伺服器啟動，正在監聽 {HOST}:{PORT}")
    print(f"[*] 儲存目錄: {SERVER_FOLDER}")

    try:
        while True:
            conn, addr = server.accept()
            # 替每個客戶端連線建立新的執行緒
            client_thread = threading.Thread(target=handle_client, args=(conn, addr))
            client_thread.start()
    except KeyboardInterrupt:
        print("\n[*] 伺服器關閉中...")
        server.close()

if __name__ == "__main__":
    start_server()