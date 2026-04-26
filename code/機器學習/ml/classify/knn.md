# K-Nearest Neighbors (KNN)

## 原理

KNN 是一種基於距離的分類算法，通過找出訓練集中與測試點最近的 K 個樣本，根據它們的標籤進行投票來決定測試點的類別。

### 算法步驟

1. 計算測試點與所有訓練點的距離（通常使用歐氏距離）
2. 找出最近的 K 個鄰居
3. 統計 K 個鄰居的類別分布
4. 返回多數決的類別作為預測結果

### 數學公式

**歐氏距離**:
$$d(x, y) = \sqrt{\sum_{i=1}^{n}(x_i - y_i)^2}$$

**投票預測**:
$$\hat{y} = \arg\max_{c} \sum_{i \in N_k(x)} I(y_i = c)$$

## Python 實作對照

### 距離計算

```python
def distance(p1, p2):
    return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)
```

對應公式中的歐氏距離計算。

### KNN 預測

```python
def knn(train_data, test_point, k=5):
    dists = []
    for point in train_data:
        d = distance(test_point, (point[0], point[1]))
        dists.append((d, point[2]))
    dists.sort(key=lambda x: x[0])
    neighbors = [label for _, label in dists[:k]]
    return collections.Counter(neighbors).most_common(1)[0][0]
```

對應算法步驟 1-4：
1. 計算所有訓練點的距離
2. 排序取前 K 個
3. 統計各類別數量
4. 返回最多票數的類別

## 使用場景

- 樣本量不大的分類問題
- 非線性邊界
- 需要可解釋性強的模型

## 優缺點

| 優點 | 缺點 |
|------|------|
| 簡單直觀 | 對特徵尺度敏感 |
| 無需訓練 | 計算量大（預測時）|
| 適用非線性 | 需要選擇合適的 K |