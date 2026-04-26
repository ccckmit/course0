# Hierarchical Clustering

## 原理

層次聚類（凝聚式）是一種構建層次結構的聚類算法，從每個點作為一個簇開始，逐步合併最相似的簇，直到只剩一個簇。

### 算法步驟

1. 每個點作為一個簇
2. 計算所有簇對之間的距離
3. 合併最近的兩個簇
4. 重複步驟 2-3 直到只剩 K 個簇

### 數學公式

**Single Linkage**:
$$d(A, B) = \min_{a \in A, b \in B} d(a, b)$$

**Complete Linkage**:
$$d(A, B) = \max_{a \in A, b \in B} d(a, b)$$

**Average Linkage**:
$$d(A, B) = \frac{1}{|A||B|} \sum_{a \in A} \sum_{b \in B} d(a, b)$$

## Python 實作對照

### 計算簇間距離

```python
def dist(c1, c2):
    return min(distance(data[i], data[j]) for i in c1 for j in c2)
```

使用 Single Linkage：取兩簇中最近點對的距離。

### 合併最近簇

```python
while len(clusters) > n_clusters:
    min_dist = float('inf')
    for i in range(len(clusters)):
        for j in range(i + 1, len(clusters)):
            d = dist(clusters[i], clusters[j])
            if d < min_dist:
                min_dist = d
                merge = (i, j)
    clusters[i].extend(clusters[j])
    clusters.pop(j)
```

對應步驟 3：找到最近的兩簇並合併。

## 使用場景

- 需要層次結構
- 發現數據的層次關係
- 不知道最佳簇數量

## 優缺點

| 優點 | 缺點 |
|------|------|
| 產生層次結構 | 時間複雜度 O(n³)|
| 不需預設 K | 貪心算法 |
| 可視化樹狀圖 | 難以修正錯誤 |