import os
import random
import numpy as np
from nn import Adam
from chargpt import train_model, generate_samples, GPT

def main():
    # 1. 設定隨機亂數種子
    random.seed(42)
    np.random.seed(42)

    # 2. 準備 Dataset
    if not os.path.exists('input.txt'):
        import urllib.request
        names_url = 'https://raw.githubusercontent.com/karpathy/makemore/988aa59/names.txt'
        urllib.request.urlretrieve(names_url, 'input.txt')

    docs = [line.strip() for line in open('input.txt') if line.strip()]
    random.shuffle(docs)
    print(f"num docs: {len(docs)}")

    # 3. Tokenizer
    uchars = sorted(set(''.join(docs)))
    BOS = len(uchars)
    vocab_size = len(uchars) + 1
    print(f"vocab size: {vocab_size}")

    # 4. 初始化模型與優化器
    block_size = 16
    model = GPT(vocab_size=vocab_size, block_size=block_size, n_layer=1, n_embd=16, n_head=4)
    print(f"num params: {len(model.parameters())}")

    optimizer = Adam(model.parameters(), lr=0.01)

    # 5. 呼叫訓練函數
    train_model(
        model=model, 
        optimizer=optimizer, 
        docs=docs, 
        uchars=uchars, 
        BOS=BOS, 
        block_size=block_size, 
        num_steps=1000
    )

    # 6. 呼叫推論函數
    generate_samples(
        model=model, 
        uchars=uchars, 
        BOS=BOS, 
        vocab_size=vocab_size, 
        block_size=block_size, 
        num_samples=20, 
        temperature=0.5
    )

if __name__ == '__main__':
    main()