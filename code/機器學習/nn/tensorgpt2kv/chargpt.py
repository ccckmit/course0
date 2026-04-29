import numpy as np
from engine import Tensor
from nn import Adam
# from gpt import GPT

def train_model(model, optimizer, docs, uchars, BOS, block_size, num_steps=1000):
    """
    執行模型訓練
    """
    params = model.parameters()
    
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
    
    print() # 換行避免與進度條重疊
    return model

def generate_samples(model, uchars, BOS, vocab_size, block_size, num_samples=20, temperature=0.5):
    """
    執行推論 (Auto-regressive 生成，搭載 KV Cache 閃電加速)
    """
    print("\n--- inference (new, hallucinated names) ---")
    results = []
    
    for sample_idx in range(num_samples):
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
            
        generated_name = ''.join(sample)
        results.append(generated_name)
        print(f"sample {sample_idx+1:2d}: {generated_name}")
        
    return results
