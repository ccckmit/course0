import numpy as np
from engine import Tensor, cat  # 引入我們剛剛寫的 cat
from nn import Module, Linear, Embedding, RMSNorm

class CausalSelfAttention(Module):
    def __init__(self, n_embd, n_head):
        self.n_head = n_head
        self.head_dim = n_embd // n_head
        
        self.wq = Linear(n_embd, n_embd)
        self.wk = Linear(n_embd, n_embd)
        self.wv = Linear(n_embd, n_embd)
        self.wo = Linear(n_embd, n_embd)

    def __call__(self, x, kv_cache=None):
        B, T, C = x.data.shape
        
        q = self.wq(x).reshape(B, T, self.n_head, self.head_dim).transpose(1, 2)
        k = self.wk(x).reshape(B, T, self.n_head, self.head_dim).transpose(1, 2)
        v = self.wv(x).reshape(B, T, self.n_head, self.head_dim).transpose(1, 2)

        # 若有過去的快取，將過去的 K, V 與當前的 K, V 拼接 (沿著序列維度 T，即 axis=2)
        if kv_cache is not None:
            past_k, past_v = kv_cache
            k = cat([past_k, k], axis=2)
            v = cat([past_v, v], axis=2)
            
        T_k = k.data.shape[2] # 當前所有的 Key 總長度

        attn_logits = (q @ k.transpose(-2, -1)) * (self.head_dim ** -0.5)
        
        # Causal Mask (只有在訓練時，也就是一次輸入整段序列 T>1 時才需要遮蔽未來)
        # 在 KV Cache 推論時 T=1，新 Token 可以看見過去所有的 Key，不需遮蔽
        if T > 1:
            mask = np.triu(np.ones((T, T_k)), k=1) == 1
            attn_logits = attn_logits.masked_fill(mask, float('-inf'))
        
        attn_weights = attn_logits.softmax(axis=-1)
        out = attn_weights @ v 
        out = out.transpose(1, 2).reshape(B, T, C)
        
        # 回傳輸出，以及更新後的快取
        return self.wo(out), (k, v)

class MLP(Module):
    def __init__(self, n_embd):
        self.fc1 = Linear(n_embd, 4 * n_embd)
        self.fc2 = Linear(4 * n_embd, n_embd)

    def __call__(self, x):
        return self.fc2(self.fc1(x).relu())

class Block(Module):
    def __init__(self, n_embd, n_head):
        self.attn = CausalSelfAttention(n_embd, n_head)
        self.mlp = MLP(n_embd)
        self.ln1 = RMSNorm(n_embd)
        self.ln2 = RMSNorm(n_embd)

    def __call__(self, x, kv_cache=None):
        # 注意力層現在會回傳新的快取
        attn_out, new_cache = self.attn(self.ln1(x), kv_cache)
        x = x + attn_out
        x = x + self.mlp(self.ln2(x))
        return x, new_cache

class GPT(Module):
    def __init__(self, vocab_size, block_size, n_layer=1, n_embd=16, n_head=4):
        self.block_size = block_size
        self.wte = Embedding(vocab_size, n_embd)
        self.wpe = Embedding(block_size, n_embd)
        self.blocks = [Block(n_embd, n_head) for _ in range(n_layer)]
        self.ln_f = RMSNorm(n_embd)
        self.lm_head = Linear(n_embd, vocab_size)

    def __call__(self, idx, kv_caches=None):
        B, T = idx.shape
        
        # 計算位置偏移：如果有快取，代表前面已經算過 past_len 個字了
        past_len = kv_caches[0][0].data.shape[2] if kv_caches is not None else 0
        pos = np.arange(past_len, past_len + T, dtype=int)
        
        tok_emb = self.wte(idx)       
        pos_emb = self.wpe(pos)       
        x = tok_emb + pos_emb
        
        new_caches = []
        for i, block in enumerate(self.blocks):
            # 取出對應層的快取
            layer_cache = kv_caches[i] if kv_caches is not None else None
            x, new_cache = block(x, layer_cache)
            new_caches.append(new_cache)
            
        x = self.ln_f(x)
        logits = self.lm_head(x)
        return logits, new_caches # 多回傳更新後的所有層快取