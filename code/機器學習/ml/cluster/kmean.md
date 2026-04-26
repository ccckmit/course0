# K-Means Clustering

## 原理

K-Means 是一種劃分式聚類算法，透過迭代方式將數據劃分為 K 個簇，使得簇內數據點的平方誤差最小。核心思想是找到 K 個 centroids，使得每個点到其所属 centroid 的距離平方和最小。

### 算法步驟

1. 隨機選擇 K 個初始 centroids
2. 分配每個點到最近的 centroid
3. 更新每個簇的 centroid 為該簇所有點的平均值
4. 重複步驟 2-3 直到收斂

### 數學公式

**目標函數**:
$$J = \sum_{k=1}^{K} \sum_{x_i \in C_k} ||x_i - \mu_k||^2$$

**Centroid 更新**:
$$\mu_k = \frac{1}{|C_k|} \sum_{x_i \in C_k} x_i$$

## Python 實作對照

### 初始 centroids

```python
centroids = random.sample(data, k)
```

隨機選擇 K 個點作為初始 centroids（對應步驟 1）。

### 分配點到最近的 centroid

```python
for i, point in enumerate(data):
    dists = [distance(point, c) for c in centroids]
    assignments[i] = dists.index(min(dists))
```

對應步驟 2：計算每點到各 centroid 的距離，選擇最近的。

### 更新 centroid

```python
for i, point in enumerate(data):
    c = assignments[i]
    new_centroids[c] = (new_centroids[c][0] + point[0], new_centroids[c][1] + point[1])
    counts[c] += 1

for i in range(k):
    new_centroids[i] = (new_centroids[i][0] / counts[i], new_centroids[i][1] / counts[i])
```

對應步驟 3：計算每簇的平均值作為新的 centroid。

## 使用場景

- 球形簇
- 簇大小相近
- 預先知道簇數量

## 優缺點

| 優點 | 缺點 |
|------|------|
| 簡單快速 | 需要指定 K |
| 對球形簇效果好 | 對初始值敏感 |
| 可解釋性強 | 對噪聲和離群點敏感 |