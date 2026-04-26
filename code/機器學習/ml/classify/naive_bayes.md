# Naive Bayes

## 原理

樸素貝葉斯是一種基於貝葉斯定理的概率分類算法，假設特徵之間相互獨立（條件獨立假設）。雖然這個假設在現實中很少成立，但演算法在許多實際問題中表現良好。

### 算法步驟

1. 計算每個類別的先驗概率 $P(y)$
2. 計算每個特徵在每個類別下的條件概率 $P(x_i | y)$
3. 對於新樣本，計算後驗概率 $P(y | x) \propto P(y) \prod_i P(x_i | y)$
4. 返回最大後驗概率的類別

### 數學公式

**貝葉斯定理**:
$$P(y | x) = \frac{P(x | y) P(y)}{P(x)}$$

**樸素貝葉斯分類**:
$$\hat{y} = \arg\max_y P(y) \prod_{i} P(x_i | y)$$

**高斯分佈**:
$$P(x | \mu, \sigma) = \frac{1}{\sqrt{2\pi\sigma^2}} e^{-\frac{(x-\mu)^2}{2\sigma^2}}$$

## Python 實作對照

### 訓練

```python
class NaiveBayes:
    def fit(self, data):
        labels = [label for _, _, label in data]
        n = len(labels)
        classes = set(labels)

        for c in classes:
            points = [(x, y) for x, y, label in data if label == c]
            self.means[c] = (sum(p[0] for p in points) / len(points), ...)
            self.vars[c] = (sum((p[0] - mean)^2 for p in points) / len(points) + 0.01, ...)
            self.priors[c] = len(points) / n
```

計算：
- 先驗概率：$P(c) = \frac{N_c}{N}$
- 條件均值：$\mu_c = \frac{1}{N_c}\sum x_i$
- 條件方差：$\sigma^2_c = \frac{1}{N_c}\sum (x_i - \mu_c)^2$

### 預測

```python
def predict(self, x, y):
    posteriors = {}
    for c in self.means:
        prob = self.priors[c]
        prob *= self.gaussian(x, self.means[c][0], self.vars[c][0])
        prob *= self.gaussian(y, self.means[c][1], self.vars[c][1])
        posteriors[c] = prob
    return max(posteriors, key=posteriors.get)
```

計算後驗概率並選擇最大值：
$$P(c | x) \propto P(c) \cdot P(x_1 | c) \cdot P(x_2 | c)$$

## 使用場景

- 文本分類（垃圾郵件過濾）
- 多類別問題
- 數據不平衡

## 優缺點

| 優點 | 缺點 |
|------|------|
| 訓練速度快 | 條件獨立假設不成立 |
| 對小規模數據好 | 對概率估計不準確 |
| 可以處理多類別 | 需要大量特徵 |