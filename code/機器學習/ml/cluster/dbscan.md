# DBSCAN

## 原理

DBSCAN (Density-Based Spatial Clustering of Applications with Noise) 是一種基於密度的聚類算法，可以發現任意形狀的簇並自動識別離群點（噪聲）。它不需要預先指定簇數量。

### 算法步驟

1. 找到核心點：半徑 eps 內至少有 min_pts 個點
2. 從核心點出發，密度可達的點歸入同一簇
3. 邊界點分配給相應核心點的簇
4. 非核心點且非邊界點標記為噪聲

### 數學公式

**核心點條件**:
$$|N_{eps}(x)| \geq min\_pts$$

其中 $N_{eps}(x)$ 是在 eps 範圍內的鄰居集。

**密度可達**:
點 p 從點 q 密度可達，如果存在一條點序列 $p = p_0, p_1, ..., p_n = q$，每個 $p_i$ 是 $p_{i-1}$ 的核心點。

## Python 實作對照

### 找鄰居

```python
def regions(data, point_idx, eps, visited):
    neighbors = []
    for j, point in enumerate(data):
        if j != point_idx and j not in visited:
            if distance(data[point_idx], point) <= eps:
                neighbors.append(j)
    return neighbors
```

計算 eps 半徑內的所有鄰居。

### DBSCAN 主循環

```python
def dbscan(data, eps=1.0, min_pts=3):
    labels = [-1] * len(data)
    for i in range(len(data)):
        if labels[i] != -1:
            continue
        neighbors = regions(data, i, eps, visited)
        if len(neighbors) >= min_pts:
            labels[i] = cluster_id
            while neighbors:
                j = neighbors.pop()
                if labels[j] == -1:
                    labels[j] = cluster_id
                    new_neighbors = regions(data, j, eps, visited)
                    if len(new_neighbors) >= min_pts:
                        neighbors.extend(new_neighbors)
            cluster_id += 1
    return labels
```

對應算法步驟 1-4：從核心點擴展簇。

## 使用場景

- 任意形狀的簇
- 有離群點的數據
- 不知道簇數量

## 優缺點

| 優點 | 缺點 |
|------|------|
| 發現任意形狀 | 對參數敏感 |
| 自動識別噪聲 | 高維效果差 |
| 不需指定 K | 密度不均勻時效果差 |