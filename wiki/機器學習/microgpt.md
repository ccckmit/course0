# microgpt

## 概述

microgpt 是 Andrej Karpathy 創作的極簡 GPT 實現，僅用約 200 行無外部依賴的 Python 程式碼，從頭實現了 GPT 的訓練和推理。這是理解大型語言模型核心原理的經典教材。

## 基本資訊

| 項目 | 內容 |
|------|------|
| 作者 | Andrej Karpathy |
| 類型 | GitHub Gist / 網頁 |
| 原始文章 | https://karpathy.github.io/2026/02/12/microgpt/ |
| 程式碼 | https://gist.github.com/karpathy/8627fe009c40f57531cb18360106ce95 |
| 程式碼行數 | ~200 行 |

## 核心元件

### 1. 資料集

```python
# 使用 32,000 個名字作為訓練資料
if not os.path.exists('input.txt'):
    import urllib.request
    names_url = 'https://raw.githubusercontent.com/karpathy/makemore/refs/heads/master/names.txt'
    urllib.request.urlretrieve(names_url, 'input.txt')

docs = [l.strip() for l in open('input.txt').read().split('\n') if l.strip()]
```

**資料範例：**
```
emma
olivia
ava
isabella
sophia
...
```

### 2. 分詞器 (Tokenizer)

```python
# 極簡字元級分詞器
# 每個字元對應一個 token
uchars = sorted(set(''.join(docs)))  # 唯一字元
BOS = len(uchars)                      # 起始 token
vocab_size = len(uchars) + 1           # 詞彙表大小
```

**設計選擇：**
- 使用字元級而非子詞（subword）分詞
- 每個字元獨立編碼，無語意含義
- BOS (Beginning of Sequence) 標記新名字的開始

### 3. Autograd 引擎

```python
class Value:
    def __init__(self, data, children=(), local_grads=()):
        self.data = data          # 前向傳播值
        self.grad = 0             # 梯度
        self._children = children  # 依賴節點
        self._local_grads = local_grads  # 局部梯度
    
    def __add__(self, other):
        other = other if isinstance(other, Value) else Value(other)
        return Value(self.data + other.data, (self, other), (1, 1))
    
    def __mul__(self, other):
        other = other if isinstance(other, Value) else Value(other)
        return Value(self.data * other.data, (self, other), (other.data, self.data))
    
    def backward(self):
        # 反向傳播：計算所有參數的梯度
        topo = []
        visited = set()
        def build_topo(v):
            if v not in visited:
                visited.add(v)
                for child in v._children:
                    build_topo(child)
                topo.append(v)
        build_topo(self)
        self.grad = 1
        for v in reversed(topo):
            for child, local_grad in zip(v._children, v._local_grads):
                child.grad += local_grad * v.grad
```

**支援的操作：**
| 操作 | 前向 | 梯度 |
|------|------|------|
| a + b | a + b | ∂/∂a = 1, ∂/∂b = 1 |
| a * b | a × b | ∂/∂a = b, ∂/∂b = a |
| a ** n | a^n | ∂/∂a = n·a^(n-1) |
| log(a) | ln(a) | ∂/∂a = 1/a |
| exp(a) | e^a | ∂/∂a = e^a |
| relu(a) | max(0,a) | ∂/∂a = 1 if a > 0 else 0 |

### 4. 模型架構

```python
def gpt(token_id, pos_id, keys, values):
    # 嵌入層
    tok_emb = state_dict['wte'][token_id]
    pos_emb = state_dict['wpe'][pos_id]
    x = [t + p for t, p in zip(tok_emb, pos_emb)]
    x = rmsnorm(x)
    
    # Transformer 層
    for li in range(n_layer):
        # Multi-head Attention
        q = linear(x, state_dict[f'layer{li}.attn_wq'])
        k = linear(x, state_dict[f'layer{li}.attn_wk'])
        v = linear(x, state_dict[f'layer{li}.attn_wv'])
        keys[li].append(k)
        values[li].append(v)
        # ... 注意力計算 ...
        
        # MLP
        x = linear(x, state_dict[f'layer{li}.mlp_fc1'])
        x = [xi.relu() for xi in x]
        x = linear(x, state_dict[f'layer{li}.mlp_fc2'])
    
    # 輸出層
    logits = linear(x, state_dict['lm_head'])
    return logits
```

**GPT 架構特點：**
- Token 嵌入 + 位置嵌入
- RMSNorm（取代 LayerNorm）
- 多頭注意力 (Multi-head Attention)
- MLP 區塊
- 殘差連接

### 5. 訓練迴圈

```python
# Adam 優化器
learning_rate, beta1, beta2 = 0.01, 0.85, 0.99

for step in range(num_steps):
    # 前向傳播
    keys, values = [[] for _ in range(n_layer)], [[] for _ in range(n_layer)]
    losses = []
    for pos_id in range(n):
        logits = gpt(token_id, pos_id, keys, values)
        probs = softmax(logits)
        loss_t = -probs[target_id].log()
        losses.append(loss_t)
    loss = sum(losses) / n
    
    # 反向傳播
    loss.backward()
    
    # Adam 更新
    for i, p in enumerate(params):
        m[i] = beta1 * m[i] + (1 - beta1) * p.grad
        v[i] = beta2 * v[i] + (1 - beta2) * p.grad ** 2
        p.data -= lr_t * m_hat / (v_hat ** 0.5 + eps)
```

### 6. 推理

```python
temperature = 0.5  # 控制隨機性

for pos_id in range(block_size):
    logits = gpt(token_id, pos_id, keys, values)
    probs = softmax([l / temperature for l in logits])
    token_id = random.choices(range(vocab_size), weights=probs)[0]
    if token_id == BOS:
        break
```

## 學習路徑

Karpathy 建議的逐步學習順序：

| 檔案 | 內容 |
|------|------|
| train0.py | Bigram 計數表（無神經網路） |
| train1.py | MLP + 手動梯度 + SGD |
| train2.py | Autograd（取代手動梯度） |
| train3.py | 位置嵌入 + 單頭注意力 + RMSNorm |
| train4.py | 多頭注意力 + 多層（完整 GPT） |
| train5.py | Adam 優化器（最終版本） |

## 與生產環境的差距

| 項目 | microgpt | 生產 LLM |
|------|----------|----------|
| 參數量 | 4,192 | 數十億~數千億 |
| 訓練資料 | 32K 名字 | 數兆 tokens |
| 分詞器 | 字元級 | BPE/子詞 |
| 精度 | 純 Python | GPU/TPU |
| 後訓練 | 無 | SFT + RLHF |

## 執行方式

```bash
# 安裝（無需 pip install）
python microgpt.py
```

**預期輸出：**
```
step    1 / 1000 | loss 3.3660
step    2 / 1000 | loss 3.4243
...
step 1000 / 1000 | loss 2.3700

sample  1: kamon
sample  2: ann
sample  3: karai
sample  4: jaire
...
```

## 核心價值

> "This file contains the full algorithmic content of what is needed... Everything else is just efficiency."

microgpt 揭示了 GPT 的核心本質：
1. **語言建模**：根據上下文預測下一個 token
2. **Transformer 架構**：注意力機制 + MLP
3. **梯度優化**：反向傳播 + Adam

ChatGPT 的對話，對模型來說只是「文件補全」。

## 相關資源

- 原始文章：https://karpathy.github.io/2026/02/12/microgpt/
- GitHub Gist：https://gist.github.com/karpathy/8627fe009c40f57531cb18360106ce95
- Google Colab：https://colab.research.google.com/drive/1vyN5zo6rqUp_dYNb4Yrco66zuWCZKoN
- 相關主題：[大型語言模型](大型語言模型.md)
- 相關主題：[Transformer](Transformer.md)

## Tags

#GPT #LLM #深度學習 #Andrej_Karpathy #極簡主義
