import os
import random
import numpy as np
from engine import Tensor
from nn import Adam
from gpt import GPT

# 設定隨機亂數種子
random.seed(42)
np.random.seed(42)

# 1. 準備 Dataset
if not os.path.exists('input.txt'):
    import urllib.request
    names_url = 'https://raw.githubusercontent.com/karpathy/makemore/988aa59/names.txt'
    urllib.request.urlretrieve(names_url, 'input.txt')

docs = [line.strip() for line in open('input.txt') if line.strip()]
random.shuffle(docs)
print(f"num docs: {len(docs)}")

# 2. Tokenizer
uchars = sorted(set(''.join(docs)))
BOS = len(uchars)
vocab_size = len(uchars) + 1
print(f"vocab size: {vocab_size}")

# 3. 初始化模型與優化器
block_size = 16
model = GPT(vocab_size=vocab_size, block_size=block_size, n_layer=1, n_embd=16, n_head=4)
params = model.parameters()
print(f"num params: {len(params)}")

optimizer = Adam(params, lr=0.01)

# 4. 訓練迴圈 (完全向量化)
num_steps = 1000

for step in range(num_steps):
    doc = docs[step % len(docs)]
    tokens = [BOS] + [uchars.index(ch) for ch in doc] + [BOS]
    n = min(block_size, len(tokens) - 1)
    
    x = np.array([tokens[:n]], dtype=int)
    y = np.array([tokens[1:n+1]], dtype=int)
    
    optimizer.zero_grad()
    
    # 訓練時不用 KV Cache，只取 logits
    logits, _ = model(x, kv_caches=None)
    
    loss = logits.cross_entropy(y)
    loss.backward()
    
    # --- 優化：加入簡易梯度裁剪 (Gradient Clipping)，防止訓練崩潰 ---
    max_norm = 1.0
    total_norm = np.sqrt(sum(np.sum(p.grad ** 2) for p in params))
    if total_norm > max_norm:
        clip_coef = max_norm / (total_norm + 1e-6)
        for p in params:
            p.grad *= clip_coef
    # ------------------------------------------------------------------
            
    optimizer.step()
    optimizer.lr = 0.01 * (1 - step / num_steps)
    
    print(f"step {step+1:4d} / {num_steps:4d} | loss {loss.data:.4f}", end='\r')

# 5. 推論 (Auto-regressive 生成，搭載 KV Cache 閃電加速)
print("\n--- inference (new, hallucinated names) ---")
temperature = 0.5

for sample_idx in range(20):
    current_token = BOS
    sample = []
    
    # 每個新樣本開始前，清空快取
    kv_caches = None
    
    for pos_id in range(block_size):
        # 【關鍵】有了快取，輸入長度永遠只有 1 (只需要送入上一步產生的那一個 Token)
        x = np.array([[current_token]], dtype=int)
        
        # 傳入目前的輸入與過去的快取，並取得新的 logits 與更新後的快取
        logits, kv_caches = model(x, kv_caches)
        
        # 取出該 Token 的 logits (因為輸入長度為 1，所以 seq_len 維度只有一個元素)
        last_logits = logits.data[0, 0, :] 
        
        # Temperature Scaling & Softmax
        exps = np.exp(last_logits / temperature - np.max(last_logits / temperature))
        probs = exps / np.sum(exps)
        
        # 依機率取樣
        current_token = np.random.choice(range(vocab_size), p=probs)
        if current_token == BOS:
            break
            
        sample.append(uchars[current_token])
        
    print(f"sample {sample_idx+1:2d}: {''.join(sample)}")