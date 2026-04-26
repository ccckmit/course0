# GPT (Generative Pre-trained Transformer)

## 概述

GPT 是由 OpenAI 開發的大型語言模型系列，使用 Transformer 架構的解碼器，透過預訓練和微調的方式實現通用語言理解和生成能力。

## GPT 發展歷程

| 版本 | 發布 | 參數 | 重要特性 |
|------|------|------|----------|
| GPT-1 | 2018 | 1.17 億 | 首個 GPT 模型 |
| GPT-2 | 2019 | 15 億 | 網頁級文本生成 |
| GPT-3 | 2020 | 1750 億 | Few-shot Learning |
| GPT-3.5 | 2022 | - | ChatGPT 基礎 |
| GPT-4 | 2023 | 未公開 | 多模態、長上下文 |
| GPT-4o | 2024 | 未公開 | 原生音訊/視訊支援 |

## GPT 架構

### Transformer 解碼器

```
輸入 Token
    ↓
┌─────────────────┐
│  Token Embedding│
│  + Pos Embedding│
└────────┬────────┘
         ↓
┌─────────────────────────────────────────────────┐
│              Transformer Block (×N)              │
│  ┌─────────────────────────────────────────┐   │
│  │           Masked Self-Attention         │   │
│  │  Q, K, V → Attention(Q,K,V)            │   │
│  │  其中 Future Tokens 被 Mask              │   │
│  └─────────────────────────────────────────┘   │
│                      ↓                          │
│  ┌─────────────────────────────────────────┐   │
│  │              Feed Forward                │   │
│  │  Linear → ReLU → Linear                │   │
│  └─────────────────────────────────────────┘   │
│                      ↓                          │
│              Residual + LayerNorm               │
└─────────────────────────────────────────────────┘
         ↓
┌─────────────────┐
│    Language     │
│     Model      │ → 預測下一個 Token
│     Head       │
└─────────────────┘
```

### GPT-3 架構參數

| 參數 | 值 |
|------|-----|
| 層數 (N) | 96 |
| 維度 (d_model) | 12288 |
| 頭數 (h) | 96 |
| 頭維度 | 128 |
| 上下文長度 | 2048 |
| 總參數 | ~1750 億 |

## 預訓練 (Pre-training)

### 下一個 Token 預測

```python
def pretraining_loss(model, tokens, positions):
    """
    輸入: tokens (batch, seq_len)
    目標: 預測下一個 token
    """
    # 前饋傳播
    logits = model(tokens)
    
    # 計算交叉熵損失
    # 預測位置 i 的 token，給定位置 < i 的 tokens
    shift_logits = logits[..., :-1, :].contiguous()
    shift_labels = tokens[..., 1:].contiguous()
    
    loss = cross_entropy(shift_logits, shift_labels)
    return loss
```

### 訓練資料

GPT-3 的訓練資料混合：
- Common Crawl (60%)
- WebText2 (22%)
- Books (8%)
- Wikipedia (3%)
- 其他 (7%)

## 微調 (Fine-tuning)

### Supervised Fine-Tuning (SFT)

```python
def sft_training(model, prompt_response_pairs):
    """
    使用人類提供的示範進行微調
    """
    losses = []
    for prompt, response in prompt_response_pairs:
        # 組合 prompt 和 response
        input_text = prompt + response
        
        # 前饋和損失計算
        logits = model(input_text)
        
        # 只在 response 部分計算損失
        loss = cross_entropy(logits[len(prompt):], response)
        losses.append(loss)
    
    return mean(losses)
```

### RLHF (Reinforcement Learning from Human Feedback)

```python
# 三階段訓練流程

# Stage 1: SFT
model_sft = supervised_finetuning(base_model, human_data)

# Stage 2: 訓練獎勵模型
def train_reward_model(prompt, chosen_response, rejected_response):
    chosen_reward = reward_model(prompt, chosen_response)
    rejected_reward = reward_model(prompt, rejected_response)
    loss = -log(sigmoid(chosen_reward - rejected_reward))
    return loss

# Stage 3: PPO 強化學習
def rl_training(policy, reward_model, prompt):
    # 生成回覆
    response = policy.generate(prompt)
    
    # 計算獎勵
    reward = reward_model(prompt, response)
    
    # PPO 更新
    ...
```

## Prompt Engineering

### Few-shot Learning

```python
# GPT-3 的 Few-shot 能力
prompt = """
翻譯以下句子為法語:

範例:
英文: Hello
法語: Bonjour

英文: Thank you
法語: Merci

英文: Good morning
法語:"""

response = gpt3.complete(prompt)
# 輸出: "Bonjour"
```

### Chain-of-Thought

```python
prompt = """
問題：小明有 5 個蘋果，他給了朋友 2 個，又買了 3 個，他有多少蘋果？

讓我們逐步思考:
1. 初始：5 個蘋果
2. 給出：5 - 2 = 3 個
3. 買入：3 + 3 = 6 個

答案：6 個"""
```

### 常用技巧

| 技巧 | 說明 |
|------|------|
| Zero-shot | 只給予任務描述 |
| One-shot | 提供一個範例 |
| Few-shot | 提供多個範例 |
| Chain-of-Thought | 要求逐步推理 |
| Tree-of-Thought | 探索多條推理路徑 |

## GPT 的能力

### 語言生成
- 回答問題
- 寫文章、詩歌、程式碼
- 翻譯語言
- 總結文本

### 推理能力
- 數學推理
- 邏輯推論
- 程式設計

### 知識應用
- 回答開放式問題
- 解釋概念
- 提供建議

## GPT 的限制

| 限制 | 說明 |
|------|------|
| 幻覺 (Hallucination) | 生成看似合理但錯誤的內容 |
| 知識截止 | 訓練資料有時間限制 |
| 推理深度 | 複雜推理可能出錯 |
| 事實準確性 | 可能混淆相似事實 |
| 計算資源 | 訓練和推論成本高 |

## API 使用

```python
import openai

# 基本呼叫
response = openai.ChatCompletion.create(
    model="gpt-4",
    messages=[
        {"role": "system", "content": "你是個有用的助理"},
        {"role": "user", "content": "解釋量子糾纏"}
    ],
    temperature=0.7,
    max_tokens=500
)

print(response.choices[0].message.content)
```

## OpenAI API 參數

| 參數 | 說明 | 常用值 |
|------|------|--------|
| temperature | 隨機性 | 0.0-2.0 |
| top_p | 核採樣 | 0.0-1.0 |
| max_tokens | 最大輸出長度 | 1-4096 |
| n | 生成數量 | 1-10 |
| stop | 停止序列 | 自定義 |

## 與 BERT 的比較

| 特性 | GPT | BERT |
|------|-----|------|
| 架構 | Decoder-only | Encoder-only |
| 注意力 | Masked (單向) | Bidirectional (雙向) |
| 任務 | 生成 | 分類/填空 |
| 預訓練 | Next token prediction | Masked LM |

## 相關資源

- 相關概念：[Transformer](Transformer.md)
- 相關概念：[大型語言模型](大型語言模型.md)
- 相關工具：[microgpt](microgpt.md) - 極簡 GPT 實現

## Tags

#GPT #LLM #OpenAI #Transformer #語言模型
