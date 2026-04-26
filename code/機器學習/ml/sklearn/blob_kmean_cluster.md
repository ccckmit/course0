# Blob KMeans Cluster

## 原理

### KMeans 聚類演算法

KMeans 是一種**分割式 (partition-based)** 聚類演算法，目標是將資料點分組到 k 個集群中。

**核心概念：**
1. 隨機選擇 k 個 centroid（中心點）
2. 將每個資料點分配給最近的 centroid
3. 重新計算每個集群的 centroid
4. 重複步驟 2-3 直到收斂

**目標函數：**
- 最小化**集群內平方誤差 (Within-Cluster Sum of Squares, WCSS)**：
$$WCSS = \sum_{i=1}^{k} \sum_{x \in C_i} \|x - \mu_i\|^2$$

其中 $\mu_i$ 是集群 $C_i$ 的中心點。

**優點：**
- 簡單快速，時間複雜度 O(n × k × t)
- 適合球形、大小相近的集群

**缺點：**
- 需要預先指定 k
- 對初始 centroid 敏感
- 只能找到球形集群
- 對 outlier 敏感

## 程式碼說明

```python
X, y_true = make_blobs(n_samples=300, centers=4, cluster_std=0.8, random_state=42)
kmeans = KMeans(n_clusters=4, random_state=42)
y_pred = kmeans.fit_predict(X)
```

- `make_blobs`: 生成 4 個 blobs 資料集
- `KMeans`: 指定 k=4 個集群
- `fit_predict`: 同時訓練模型並預測標籤
- 顯示 centroids 位置（紅色 X 標記）

## 執行

```bash
python3 blob_kmean_cluster.py
```