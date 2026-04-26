# Transformer

## 概述

Transformer 是 2017 年由 Google Brain、Google Research 和 University of Toronto 共同發表的論文「Attention Is All You Need」中提出的深度學習架構。這篇論文徹底改變了自然語言處理（NLP）領域，並在此後幾年內擴展到電腦視覺、音訊處理等領域，成為深度學習歷史上最具影響力的架構之一。

Transformer 的核心創新是完全拋棄了傳統的循環和卷積結構，僅依賴注意力機制（Attention Mechanism）來處理序列資料。這種設計使得模型可以並行處理序列中的所有位置，解決了 RNN 的長期依賴問題和訓練速度限制。Transformer 的出現催生了 BERT、GPT、T5 等一系列大型語言模型，奠定了現代 AI 的基礎。

## 架構總覽

### 整體結構

```
┌─────────────────────────────────────────────────────────────────┐
│                        Transformer 架構                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  輸入嵌入                                                       │
│       ↓                                                        │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │                    編碼器 (Encoder)                      │  │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐        │  │
│  │  │ Layer 1│ │ Layer 2│ │ Layer N│ │ Layer N│        │  │
│  │  │ └─ Multi-Head Self-Attention                        │  │
│  │  │ └─ Feed Forward Network                            │  │
│  │  └─────────┘ ┌─────────┘ ┌─────────┘ ┌─────────┘        │  │
│  └─────────────────────────────────────────────────────────┘  │
│       ↓                                                        │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │                    解碼器 (Decoder)                      │  │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐        │  │
│  │  │ Layer 1│ │ Layer 2│ │ Layer N│ │ Layer N│        │  │
│  │  │ └─ Masked Multi-Head Self-Attention                 │  │
│  │  │ └─ Cross-Attention (關注 Encoder 輸出)              │  │
│  │  │ └─ Feed Forward Network                            │  │
│  │  └─────────┘ ┌─────────┘ ┌─────────┘ ┌─────────┘        │  │
│  └─────────────────────────────────────────────────────────┘  │
│       ↓                                                        │
│  輸出概率                                                       │
└─────────────────────────────────────────────────────────────────┘
```

### 編碼器-解碼器架構

Transformer 最初作為 Sequence-to-Sequence 模型設計，包含編碼器和解碼器兩部分：

- **編碼器**：將輸入序列編碼為連續表示
- **解碼器**：根據編碼器輸出和已生成的 tokens 逐步生成輸出序列

## 自注意力機制

### 原理

自注意力（Self-Attention）允許序列中的每個位置關注序列中的所有其他位置，計算它們之間的相關性。這種機制使得模型可以捕捉長距離依賴關係。

```python
import torch
import torch.nn as nn
import math

class SelfAttention(nn.Module):
    """縮放點積注意力 (Scaled Dot-Product Attention)"""
    
    def __init__(self, d_model, num_heads):
        super().__init__()
        self.d_model = d_model
        self.num_heads = num_heads
        self.d_k = d_model // num_heads
        
        # 線性投影
        self.W_q = nn.Linear(d_model, d_model)
        self.W_k = nn.Linear(d_model, d_model)
        self.W_v = nn.Linear(d_model, d_model)
        
        self.W_o = nn.Linear(d_model, d_model)
    
    def split_heads(self, x, batch_size):
        """分割多個注意力頭"""
        x = x.view(batch_size, -1, self.num_heads, self.d_k)
        return x.permute(0, 2, 1, 3)  # (batch, heads, seq, d_k)
    
    def forward(self, query, key, value, mask=None):
        batch_size = query.size(0)
        
        # 線性投影
        Q = self.W_q(query)
        K = self.W_k(key)
        V = self.W_v(value)
        
        # 分割頭
        Q = self.split_heads(Q, batch_size)
        K = self.split_heads(K, batch_size)
        V = self.split_heads(V, batch_size)
        
        # 縮放點積注意力
        # Attention(Q, K, V) = softmax(QK^T / sqrt(d_k))V
        scores = torch.matmul(Q, K.transpose(-2, -1)) / math.sqrt(self.d_k)
        
        if mask is not None:
            scores = scores.masked_fill(mask == 0, -1e9)
        
        attention_weights = torch.softmax(scores, dim=-1)
        attention_output = torch.matmul(attention_weights, V)
        
        # 合併頭
        attention_output = attention_output.permute(0, 2, 1, 3).contiguous()
        attention_output = attention_output.view(batch_size, -1, self.d_model)
        
        # 輸出投影
        output = self.W_o(attention_output)
        
        return output, attention_weights
```

### 多頭注意力

多頭注意力（Multi-Head Attention）並行運行多個注意力函數，增強模型捕捉不同類型關係的能力：

```python
class MultiHeadAttention(nn.Module):
    """多頭注意力"""
    
    def __init__(self, d_model, num_heads, dropout=0.1):
        super().__init__()
        assert d_model % num_heads == 0
        
        self.d_model = d_model
        self.num_heads = num_heads
        self.d_k = d_model // num_heads
        
        # 四個線性層
        self.W_q = nn.Linear(d_model, d_model)
        self.W_k = nn.Linear(d_model, d_model)
        self.W_v = nn.Linear(d_model, d_model)
        self.W_o = nn.Linear(d_model, d_model)
        
        self.dropout = nn.Dropout(dropout)
        self.layer_norm = nn.LayerNorm(d_model)
    
    def forward(self, x, mask=None):
        batch_size = x.size(0)
        
        # 線性投影 + 分割頭
        Q = self.split_heads(self.W_q(x), batch_size)
        K = self.split_heads(self.W_k(x), batch_size)
        V = self.split_heads(self.W_v(x), batch_size)
        
        # 注意力計算
        scores = torch.matmul(Q, K.transpose(-2, -1)) / math.sqrt(self.d_k)
        
        if mask is not None:
            scores = scores.masked_fill(mask == 0, -1e9)
        
        attention_weights = torch.softmax(scores, dim=-1)
        attention_weights = self.dropout(attention_weights)
        
        # 加權求和
        attention_output = torch.matmul(attention_weights, V)
        
        # 合併頭
        attention_output = self.merge_heads(attention_output, batch_size)
        
        # 輸出投影 + 殘差連接 + LayerNorm
        output = self.W_o(attention_output)
        output = self.layer_norm(x + output)
        
        return output
    
    def split_heads(self, x, batch_size):
        return x.view(batch_size, -1, self.num_heads, self.d_k).permute(0, 2, 1, 3)
    
    def merge_heads(self, x, batch_size):
        return x.permute(0, 2, 1, 3).contiguous().view(batch_size, -1, self.d_model)
```

## 位置編碼

Transformer 本身不具備順序感知能力，因為自注意力是對稱的。位置編碼（Positional Encoding）通過向輸入嵌入添加位置資訊來解決這個問題：

```python
class PositionalEncoding(nn.Module):
    """正弦位置編碼"""
    
    def __init__(self, d_model, max_len=5000, dropout=0.1):
        super().__init__()
        self.dropout = nn.Dropout(p=dropout)
        
        # 創建位置編碼矩陣
        pe = torch.zeros(max_len, d_model)
        position = torch.arange(0, max_len).unsqueeze(1).float()
        div_term = torch.exp(torch.arange(0, d_model, 2).float() * 
                            (-math.log(10000.0) / d_model))
        
        pe[:, 0::2] = torch.sin(position * div_term)
        pe[:, 1::2] = torch.cos(position * div_term)
        
        pe = pe.unsqueeze(0)  # (1, max_len, d_model)
        self.register_buffer('pe', pe)
    
    def forward(self, x):
        x = x + self.pe[:, :x.size(1)]
        return self.dropout(x)


class LearnedPositionalEncoding(nn.Module):
    """可學習的位置編碼"""
    
    def __init__(self, d_model, max_len=5000):
        super().__init__()
        self.position_embeddings = nn.Embedding(max_len, d_model)
    
    def forward(self, x):
        batch_size, seq_len = x.size(0), x.size(1)
        positions = torch.arange(seq_len, device=x.device).unsqueeze(0)
        position_embeddings = self.position_embeddings(positions)
        return x + position_embeddings
```

### 旋轉位置編碼 (RoPE)

較新的模型如 LLaMA 使用旋轉位置編碼：

```python
class RotaryPositionalEmbedding(nn.Module):
    """旋轉位置編碼 (RoPE)"""
    
    def __init__(self, d_model, base=10000):
        super().__init__()
        self.d_model = d_model
        self.base = base
        
        # 預計算旋轉角度
        inv_freq = 1.0 / (base ** (torch.arange(0, d_model, 2).float() / d_model))
        self.register_buffer('inv_freq', inv_freq)
    
    def forward(self, seq_len, device):
        # 創建位置
        t = torch.arange(seq_len, device=device).type_as(self.inv_freq)
        
        # 計算旋轉角度
        freqs = torch.einsum('i,j->ij', t, self.inv_freq)
        emb = torch.cat([freqs, freqs], dim=-1)
        
        return emb.cos(), emb.sin()
    
    def rotate_half(x):
        """旋轉一半"""
        x1, x2 = x[..., :x.shape[-1] // 2], x[..., x.shape[-1] // 2:]
        return torch.cat([-x2, x1], dim=-1)
    
    def apply_rotary_pos_emb(q, k, cos, sin):
        """應用 RoPE"""
        q = (q * cos) + (self.rotate_half(q) * sin)
        k = (k * cos) + (self.rotate_half(k) * sin)
        return q, k
```

## 前饋網路

每個 Transformer 層還包含一個前饋網路（Feed Forward Network，FFN），通常由兩個線性變換和中間的 ReLU 激活組成：

```python
class FeedForward(nn.Module):
    """位置前饋網路"""
    
    def __init__(self, d_model, d_ff, dropout=0.1):
        super().__init__()
        self.linear1 = nn.Linear(d_model, d_ff)
        self.linear2 = nn.Linear(d_ff, d_model)
        self.dropout = nn.Dropout(dropout)
        self.layer_norm = nn.LayerNorm(d_model)
    
    def forward(self, x):
        # FFN: ReLU(W1 * x + b1) * W2 + b2
        residual = x
        x = torch.relu(self.linear1(x))
        x = self.dropout(x)
        x = self.linear2(x)
        x = self.dropout(x)
        
        # 殘差連接 + LayerNorm
        return self.layer_norm(residual + x)
```

## Transformer 層

將所有組件組合成完整的 Transformer 層：

```python
class TransformerLayer(nn.Module):
    """Transformer 編碼器層"""
    
    def __init__(self, d_model, num_heads, d_ff, dropout=0.1):
        super().__init__()
        
        self.attention = MultiHeadAttention(d_model, num_heads, dropout)
        self.feed_forward = FeedForward(d_model, d_ff, dropout)
    
    def forward(self, x, mask=None):
        # 自注意力
        x = self.attention(x, mask)
        
        # 前饋網路
        x = self.feed_forward(x)
        
        return x


class TransformerEncoder(nn.Module):
    """Transformer 編碼器"""
    
    def __init__(self, num_layers, d_model, num_heads, d_ff, dropout=0.1):
        super().__init__()
        
        self.layers = nn.ModuleList([
            TransformerLayer(d_model, num_heads, d_ff, dropout)
            for _ in range(num_layers)
        ])
        self.layer_norm = nn.LayerNorm(d_model)
    
    def forward(self, x, mask=None):
        for layer in self.layers:
            x = layer(x, mask)
        
        return self.layer_norm(x)
```

## 解碼器與掩碼

解碼器在訓練時需要防止「未來資訊洩露」：

```python
class DecoderLayer(nn.Module):
    """Transformer 解碼器層"""
    
    def __init__(self, d_model, num_heads, d_ff, dropout=0.1):
        super().__init__()
        
        self.self_attention = MultiHeadAttention(d_model, num_heads, dropout)
        self.cross_attention = MultiHeadAttention(d_model, num_heads, dropout)
        self.feed_forward = FeedForward(d_model, d_ff, dropout)
        
        self.layer_norm1 = nn.LayerNorm(d_model)
        self.layer_norm2 = nn.LayerNorm(d_model)
        self.layer_norm3 = nn.LayerNorm(d_model)
    
    def forward(self, x, encoder_output, src_mask=None, tgt_mask=None):
        # Masked Self-Attention
        x = self.self_attention(x, tgt_mask)
        x = self.layer_norm1(x)
        
        # Cross-Attention
        x = self.cross_attention(x, encoder_output, src_mask)
        x = self.layer_norm2(x)
        
        # Feed Forward
        x = self.feed_forward(x)
        x = self.layer_norm3(x)
        
        return x


def create_causal_mask(seq_len, device):
    """創建因果掩碼 - 防止看到未來"""
    mask = torch.triu(torch.ones(seq_len, seq_len, device=device), diagonal=1)
    return mask.masked_fill(mask == 1, float('-inf'))
```

## 經典模型

### BERT

BERT（Bidirectional Encoder Representations from Transformers）是雙向編碼器，預訓練使用 Masked Language Model (MLM) 和 Next Sentence Prediction (NSP)：

```python
class BERT(nn.Module):
    """BERT 模型"""
    
    def __init__(self, vocab_size, d_model, num_layers, num_heads, d_ff, max_len):
        super().__init__()
        
        self.embedding = nn.Embedding(vocab_size, d_model)
        self.positional_encoding = LearnedPositionalEncoding(d_model, max_len)
        
        self.encoder = TransformerEncoder(num_layers, d_model, num_heads, d_ff)
        
        # MLM 頭
        self.mlm_head = nn.Linear(d_model, vocab_size)
        
        # NSP 頭
        self.nsp_head = nn.Linear(d_model, 2)
    
    def forward(self, input_ids, segment_ids=None, masked_positions=None):
        # 嵌入
        x = self.embedding(input_ids)
        x = self.positional_encoding(x)
        
        # 編碼
        x = self.encoder(x)
        
        # MLM 預測
        mlm_logits = self.mlm_head(x)
        
        # NSP 分類
        pooled = x[:, 0]  # [CLS] token
        nsp_logits = self.nsp_head(pooled)
        
        return mlm_logits, nsp_logits
```

### GPT

GPT（Generative Pre-trained Transformer）是單向解碼器，使用自回歸生成：

```python
class GPT(nn.Module):
    """GPT 模型"""
    
    def __init__(self, vocab_size, d_model, num_layers, num_heads, d_ff, max_len):
        super().__init__()
        
        self.embedding = nn.Embedding(vocab_size, d_model)
        self.positional_encoding = PositionalEncoding(d_model, max_len)
        
        self.decoder = nn.ModuleList([
            DecoderLayer(d_model, num_heads, d_ff)
            for _ in range(num_layers)
        ])
        
        self.lm_head = nn.Linear(d_model, vocab_size, bias=False)
        
        # 權重共享
        self.lm_head.weight = self.embedding.weight
    
    def forward(self, input_ids):
        x = self.embedding(input_ids)
        x = self.positional_encoding(x)
        
        # 創建因果掩碼
        seq_len = input_ids.size(1)
        causal_mask = create_causal_mask(seq_len, input_ids.device)
        
        # 逐層解碼
        for layer in self.decoder:
            x = layer(x, encoder_output=None, tgt_mask=causal_mask)
        
        # 語言模型頭
        logits = self.lm_head(x)
        
        return logits
    
    @torch.no_grad()
    def generate(self, input_ids, max_new_tokens, temperature=1.0):
        """自回歸生成"""
        for _ in range(max_new_tokens):
            # 截取最後的位置
            input_ids_cond = input_ids if input_ids.size(1) <= self.max_len else input_ids[:, -self.max_len:]
            
            # 前向
            logits = self(input_ids_cond)
            logits = logits[:, -1, :] / temperature
            
            # 採樣
            probs = torch.softmax(logits, dim=-1)
            next_token = torch.multinomial(probs, num_samples=1)
            
            input_ids = torch.cat([input_ids, next_token], dim=1)
        
        return input_ids
```

## Transformer 的變體

### Vision Transformer (ViT)

將 Transformer 應用於圖像：

```python
class ViT(nn.Module):
    """Vision Transformer"""
    
    def __init__(self, image_size=224, patch_size=16, num_classes=1000, 
                 d_model=768, num_layers=12, num_heads=12):
        super().__init__()
        
        self.num_patches = (image_size // patch_size) ** 2
        
        # 線性投影
        self.patch_embed = nn.Conv2d(3, d_model, patch_size, patch_size)
        
        # 類別 token 和位置編碼
        self.cls_token = nn.Parameter(torch.zeros(1, 1, d_model))
        self.pos_embedding = nn.Parameter(torch.zeros(1, self.num_patches + 1, d_model))
        
        # Transformer 編碼器
        self.transformer = TransformerEncoder(num_layers, d_model, num_heads, d_model * 4)
        
        # 分類頭
        self.head = nn.Linear(d_model, num_classes)
    
    def forward(self, x):
        # 分割 patch
        x = self.patch_embed(x)  # (B, d_model, H/P, W/P)
        x = x.flatten(2).transpose(1, 2)  # (B, num_patches, d_model)
        
        # 添加 CLS token
        cls_tokens = self.cls_token.expand(x.size(0), -1, -1)
        x = torch.cat([cls_tokens, x], dim=1)
        
        # 添加位置編碼
        x = x + self.pos_embedding
        
        # Transformer
        x = self.transformer(x)
        
        # 分類
        return self.head(x[:, 0])
```

### Swin Transformer

使用階層式結構和滑動窗口注意力：

```python
class SwinTransformer(nn.Module):
    """Swin Transformer - 階層式 Vision Transformer"""
    
    def __init__(self, image_size=224, patch_size=4, num_classes=1000,
                 d_model=96, num_layers=[2, 2, 6, 2], num_heads=[3, 6, 12, 24]):
        super().__init__()
        
        # Patch 嵌入
        self.patch_embed = nn.Conv2d(3, d_model, patch_size, patch_size)
        
        # 多層 Swin Transformer Blocks
        self.stages = nn.ModuleList()
        for i, (n_layers, n_heads) in enumerate(zip(num_layers, num_heads)):
            stage = SwinStage(
                d_model=d_model * (2 ** i),
                num_layers=n_layers,
                num_heads=n_heads,
                downsample=(i > 0)
            )
            self.stages.append(stage)
        
        # 分類頭
        self.head = nn.Linear(d_model * 16, num_classes)
```

## 應用領域

| 領域 | 應用 |
|------|------|
| 自然語言處理 | 機器翻譯、文本生成、問答 |
| 電腦視覺 | 圖像分類、物體偵測、分割 |
| 語音處理 | 語音識別、語音合成 |
| 多模態 | 圖文生成、視覺問答 |
| 生物資訊 | 蛋白質結構預測 |

## 訓練技巧

```python
# 學習率排程 (Warmup)
def get_lr(optimizer, step, d_model, warmup_steps):
    step = max(1, step)
    return d_model ** (-0.5) * min(step ** (-0.5), step * warmup_steps ** (-1.5))

# 標籤平滑
class LabelSmoothingLoss(nn.Module):
    def __init__(self, vocab_size, smoothing=0.1):
        super().__init__()
        self.smoothing = smoothing
        self.vocab_size = vocab_size
    
    def forward(self, pred, target):
        log_probs = torch.log_softmax(pred, dim=-1)
        nll_loss = -log_probs.gather(dim=-1, index=target.unsqueeze(1))
        smooth_loss = -log_probs.mean(dim=-1)
        
        loss = (1 - self.smoothing) * nll_loss + self.smoothing * smooth_loss
        return loss.mean()

# 梯度裁剪
torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=1.0)
```

## 相關概念

- [循環神經網路](循環神經網路.md) - RNN 架構
- [大型語言模型](大型語言模型.md) - LLM 基礎
- [注意力機制](注意力機制.md) - 注意力基礎
- [BERT](BERT.md) - BERT 模型
- [GPT](GPT.md) - GPT 模型

## Tags

#Transformer #Attention #NLP #ViT #BERT #GPT #深度學習