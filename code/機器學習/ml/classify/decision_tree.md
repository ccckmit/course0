# Decision Tree

## 原理

決策樹是一種樹狀結構的監督學習算法，通過對特徵進行遞迴分割來進行分類或回歸。每個內部節點代表一個特徵的判斷條件，每個葉節點代表一個類別。

### 算法步驟

1. 選擇最佳分割特徵和閾值
2. 按照條件分割數據
3. 遞迴處理子集
4. 達到停止條件時建立葉節點

### 數學公式

**Gini 不純度**:
$$Gini = 1 - \sum_{c} p_c^2$$

**信息增益**:
$$Gain = Gini_{parent} - \frac{N_L}{N} Gini_{left} - \frac{N_R}{N} Gini_{right}$$

## Python 實作對照

### Gini 計算

```python
def gini_impurity(labels):
    if not labels:
        return 0
    counts = collections.Counter(labels)
    impurity = 1
    for count in counts.values():
        p = count / len(labels)
        impurity -= p * p
    return impurity
```

計算公式：$1 - \sum p_c^2$

### 最佳分割

```python
def best_split(data):
    best_gain = -1
    for x, label in data:
        threshold = x
        left = [l for v, l in data if v <= threshold]
        right = [l for v, l in data if v > threshold]
        n = len(data)
        gain = current_gini - (len(left) / n) * gini_impurity(left) - (len(right) / n) * gini_impurity(right)
        if gain > best_gain:
            best_gain = gain
    return best_threshold
```

對應信息增益計算，選擇最大增益的分割點。

### 樹的建立

```python
def build_tree(data, depth=0, max_depth=5):
    labels = [label for _, label in data]
    if len(set(labels)) == 1 or depth >= max_depth:
        return Node(label=collections.Counter(labels).most_common(1)[0][0])
    threshold = best_split(data)
    left_data = [d for d in data if d[0] <= threshold]
    right_data = [d for d in data if d[0] > threshold]
    return Node(threshold=threshold, left=build_tree(left_data, depth+1), right=build_tree(right_data, depth+1))
```

遞迴建立決策樹，達到最大深度或純淨時停止。

## 使用場景

- 需要可解釋性
- 特徵重要性分析
- 非線性關係

## 優缺點

| 優點 | 缺點 |
|------|------|
| 可解釋性強 | 容易過擬合 |
| 處理非數值特徵 | 對數據變化敏感 |
| 自動特徵選擇 | 可能創建不平衡樹 |