# Gradient Boosting Decision Tree (GBDT)

## 原理

GBDT 是一種Boosting 集成算法，透過逐步建立決策樹來減少前一棵樹的殘差。每棵新樹學習前面所有樹的組合預測與真實值之間的差異（殘差），最終結果是所有樹的加權和。

### 算法步驟

1. 初始化基礎預測（如類別平均值）
2. 計算負梯度（殘差）：$r_i = y_i - f(x_i)$
3. 用決策樹擬合殘差
4. 更新模型：$f_{m}(x) = f_{m-1}(x) + \eta \cdot h_m(x)$
5. 重複步驟 2-4

### 數學公式

**負梯度**:
$$r_j = -\frac{\partial L(y, F(x))}{\partial F(x_j)}$$

**MSE 損失的負梯度**:
$$r_j = y_j - F(x_j)$$

**更新規則**:
$$F_m(x) = F_{m-1}(x) + \eta \cdot h_m(x)$$

其中 $\eta$ 是學習率。

## Python 實作對照

### 計算殘差

```python
residuals = [labels[i] - preds[i] for i in range(len(data))]
tree = self.build_tree(data, residuals)
```

對應步驟 2：計算殘差並用決策樹擬合。

### 更新預測

```python
for i, (x, y, label) in enumerate(data):
    pred = self.predict_single(x, tree)
    preds[i] += self.lr * pred
```

對應步驟 4：加入新樹的預測（乘以學習率）。

## 使用場景

- 結構化數據
- Kaggle 競賽
- 需要高精度

## 優缺點

| 優點 | 缺點 |
|------|------|
| 精度極高 | 訓練時間長 |
| 處理各種類型特徵 | 容易過擬合 |
| 特徵重要性 | 超參數多 |