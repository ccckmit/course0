# Random Forest

## 原理

隨機森林是一種集成學習方法，透過建立多棵決策樹並結合它們的預測結果來提高準確性和穩定性。每棵樹使用隨機抽樣的數據和特徵進行訓練，最終結果由所有樹投票決定。

### 算法步驟

1. Bootstrap 抽樣：從訓練集中有放回地隨機抽取樣本
2. 特徵隨機選擇：每次分割時只考慮隨機選擇的特徵
3. 建立決策樹
4. 投票：所有樹的預測結果投票

### 數學公式

**投票預測**:
$$\hat{y} = \arg\max_c \sum_{t=1}^{T} I(h_t(x) = c)$$

**Bagging**:
$$f(x) = \frac{1}{T} \sum_{t=1}^{T} h_t(x)$$

## Python 實作對照

### Bootstrap

```python
def bootstrap(data):
    return [random.choice(data) for _ in data]
```

有放回抽樣，用於訓練每棵樹。

### 隨機森林預測

```python
class RandomForest:
    def fit(self, data):
        for _ in range(self.n_trees):
            tree = DecisionTree(self.max_depth)
            tree.fit(bootstrap(data))
            self.trees.append(tree)

    def predict(self, x):
        votes = [tree.predict(x) for tree in self.trees]
        return collections.Counter(votes).most_common(1)[0][0]
```

建立多棵樹，投票決定最終類別。

## 使用場景

- 分類和回歸
- 特徵重要性分析
- 數據不平衡

## 優缺點

| 優點 | 缺點 |
|------|------|
| 準確性高 | 訓練時間長 |
| 抗過擬合 | 可解釋性較差 |
| 處理缺失值 | 佔用資源多 |