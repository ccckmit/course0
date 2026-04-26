import sqlite3
import json
import random

def generate_pure_chinese_data():
    # 純中文人名
    usernames = ["陳志明", "林宛妤", "王大衛", "張嘉玲", "李柏翰", "黃郁婷", "謝孟哲", "郭采潔", "吳志強", "楊雅婷"]
    
    # 純中文貼文標題
    titles = [
        "今天的晚餐紀錄", "推薦一部最近看的紀錄片", "關於生活的一點小感觸", "週末的台南小旅行",
        "運動後的心情真的很好", "居家辦公的日常心得", "看完這本書後的啟發", "理財真的是一輩子的事",
        "我家那隻貓又闖禍了", "下雨天的午後散步", "第一次挑戰做提拉米蘇", "今年的目標達成度報告"
    ]
    
    # 純中文貼文內文
    contents = [
        "這家店的湯頭味道真的很紮實，麵條也很有嚼勁，下次一定要帶家人來。",
        "有時候覺得步調太快了，應該停下來聽聽自己內心的聲音。",
        "這本書裡面的觀點讓我重新思考了工作的意義，非常有深度。",
        "大家最近有什麼推薦的私房景點嗎？最近想出去走走，但沒什麼靈感。",
        "清理完房間後，感覺思考也變得清晰許多。斷捨離果然有其魔力。",
        "其實只要肯努力，夢想真的不是遙不可及，共勉之。"
    ]

    # 生成 10 位用戶
    users = [{"username": u, "password": f"123"} for u in usernames]
    
    # 生成 100 筆貼文
    posts = []
    for i in range(1, 101):
        posts.append({
            "title": f"{random.choice(titles)} 第 {i} 篇",
            "content": f"{random.choice(contents)} 這是我的生活隨筆，希望你們也會喜歡。",
            "author_id": random.randint(1, 10)
        })
        
    return {"users": users, "posts": posts}

def run_import():
    data = generate_pure_chinese_data()
    
    # 寫入 blog.json (確保 UTF-8 編碼且不轉義中文)
    with open('blog.json', 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, indent=2)
    print("✓ blog.json 已生成（純中文內容）")

    # 建立並導入 blog.db
    conn = sqlite3.connect('blog.db')
    cursor = conn.cursor()

    # 建立資料表 (根據你的架構需求)
    cursor.executescript('''
    DROP TABLE IF EXISTS users;
    DROP TABLE IF EXISTS posts;
    
    CREATE TABLE users (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      username TEXT UNIQUE NOT NULL,
      password TEXT NOT NULL,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP
    );
    
    CREATE TABLE posts (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      title TEXT NOT NULL,
      content TEXT NOT NULL,
      author_id INTEGER,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
      FOREIGN KEY (author_id) REFERENCES users(id)
    );
    ''')

    # 插入用戶資料
    for u in data['users']:
        cursor.execute("INSERT INTO users (username, password) VALUES (?, ?)", (u['username'], u['password']))
    
    # 插入貼文資料
    for p in data['posts']:
        cursor.execute("INSERT INTO posts (title, content, author_id) VALUES (?, ?, ?)", 
                       (p['title'], p['content'], p['author_id']))

    conn.commit()
    conn.close()
    print("✓ blog.db 已生成，10 位用戶與 100 筆貼文導入成功！")

if __name__ == "__main__":
    run_import()