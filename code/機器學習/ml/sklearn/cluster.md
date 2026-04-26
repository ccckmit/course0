# Cluster

## 原理

### 互動式聚類實驗工具

本程式提供互動式的聚類實驗環境，可選擇不同資料集和演算法。

---

## 支援的資料集

| 資料集 | 產生函數 | 特性 |
|--------|----------|------|
| make_blobs | make_blobs | 標準球形分布 |
| make_moons | make_moons | 新月形狀 |
| make_circles | make_circles | 同心圓 |
| make_classification | make_classification | 分離的類別 |
| blobs (4 clusters) | make_blobs | 4 簇 |
| blobs (anisotropic) | 自訂 | 拉伸分布 |

---

## 支援的聚類演算法

### 1. KMeans
- **分割式**聚類
- 需指定 k
- 適合球形集群

### 2. DBSCAN
- **density-based**
- 自動偵測集群數量
- 可識別 outlier
- 對參數敏感

### 3. AgglomerativeClustering
- **階層式**聚類
- 需指定 k
- 可用不同連結法

### 4. SpectralClustering
- **圖論**方法
- 需指定 k
- 適合非凸形狀

### 5. GaussianMixture
- **機率模型**
- 需指定 components
- 回傳機率

### 6. MeanShift
- **密度導向**
- 自動偵測
- 適合任意形狀

### 7. Birch
- **階層式**聚類
- 適合大規模
- 需指定 n_clusters

---

## 評估指標

### Silhouette Score
$$s = \frac{b - a}{max(a, b)}$$

- a: 該點到同集群其他點的平均距離
- b: 該點到最近集群的平均距離
- 範圍: [-1, 1]，越高越好

### Davies-Bouldin Index
$$DB = \frac{1}{k} \sum_{i=1}^{k} \max_{j \neq i} \frac{d_i + d_j}{c_i - j}$$

- d_i: 集群內平均距離
- c_i_j: 兩集群中心距離
- 越低越好

### Calinski-Harabasz Score
$$CH = \frac{BGSS / (k-1)}{WGSS / (n-k)}$$

- BGSS: 集群間變異
- WGSS: 集群內變異
- 越高越好

---

## 執行方式

```bash
python3 cluster.py              # 互動模式
python3 cluster.py 1 1         # iris + KMeans
python3 cluster.py 2 2         # moons + DBSCAN
python3 cluster.py 3 4        # circles + Spectral
```

- 第一個參數：資料集選擇
- 第二個參數：演算法選擇