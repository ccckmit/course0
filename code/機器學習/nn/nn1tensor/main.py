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
    # 取出資料並加上 BOS
    doc = docs[step % len(docs)]
    tokens = [BOS] + [uchars.index(ch) for ch in doc] + [BOS]
    n = min(block_size, len(tokens) - 1)
    
    # 準備張量輸入 (Batch_size = 1)
    x = np.array([tokens[:n]], dtype=int)
    y = np.array([tokens[1:n+1]], dtype=int)
    
    optimizer.zero_grad()
    
    # 前向傳遞 (一次計算整段 Sequence)
    logits = model(x)
    
    # 損失計算 (內含自動微分處理)
    loss = logits.cross_entropy(y)
    
    # 反向傳遞與優化
    loss.backward()
    optimizer.step()
    
    # Linear Learning rate decay
    optimizer.lr = 0.01 * (1 - step / num_steps)
    
    print(f"step {step+1:4d} / {num_steps:4d} | loss {loss.data:.4f}", end='\r')

# 5. 推論 (Auto-regressive 生成)
print("\n--- inference (new, hallucinated names) ---")
temperature = 0.5

for sample_idx in range(20):
    idx = [BOS]
    for pos_id in range(block_size):
        x = np.array([idx], dtype=int)
        
        # 每次都做前向傳遞
        logits = model(x)
        # 取出最後一步的 logits
        last_logits = logits.data[0, -1, :] 
        
        # Temperature Scaling & Softmax
        exps = np.exp(last_logits / temperature - np.max(last_logits / temperature))
        probs = exps / np.sum(exps)
        
        # 依機率取樣
        next_token = np.random.choice(range(vocab_size), p=probs)
        if next_token == BOS:
            break
            
        idx.append(next_token)
        
    sample = "".join([uchars[i] for i in idx[1:]])
    print(f"sample {sample_idx+1:2d}: {sample}")