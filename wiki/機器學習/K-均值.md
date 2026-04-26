# K-均值 (K-Means)

## 原理

K-均值是最廣泛使用的聚類演算法之一，目標是將 n 個資料點劃分為 K 個簇，使得每個簇內的資料點相似度高，簇間相似度低。演算法通過迭代優化目標函數（簇內平方和，Within-Cluster Sum of Squares，WCSS）直到收斂。

$$WCSS = \sum_{k=1}^{K} \sum_{x_i \in C_k} \|x_i - \mu_k\|^2$$

其中 $\mu_k$ 是第 k 個簇的中心點。

## 演算法流程

1. 隨機選擇 K 個初始中心點
2. 將每個點分配到最近的中心點所屬的簇
3. 更新每個簇的中心點為該簇所有點的均值
4. 重複步驟 2-3 直到收斂（中心點不再改變或達到最大迭代次數）

## 實現

```python
import numpy as np

class KMeans:
    """K-均值聚類"""
    
    def __init__(self, n_clusters=3, max_iter=100, tol=1e-4, random_state=None):
        self.n_clusters = n_clusters
        self.max_iter = max_iter
        self.tol = tol
        self.random_state = random_state
        self.centers = None
        self.labels = None
    
    def fit(self, X):
        np.random.seed(self.random_state)
        n_samples = X.shape[0]
        
        # 隨機選擇初始中心點
        indices = np.random.choice(n_samples, self.n_clusters, replace=False)
        self.centers = X[indices].copy()
        
        for _ in range(self.max_iter):
            # 分配每個點到最近的中心
            distances = np.zeros((n_samples, self.n_clusters))
            for k in range(self.n_clusters):
                distances[:, k] = np.linalg.norm(X - self.centers[k], axis=1)
            
            new_labels = np.argmin(distances, axis=1)
            
            # 檢查是否收斂
            if np.array_equal(new_labels, self.labels):
                break
            
            self.labels = new_labels
            
            # 更新中心點
            for k in range(self.n_clusters):
                if np.sum(self.labels == k) > 0:
                    self.centers[k] = X[self.labels == k].mean(axis=0)
        
        return self
    
    def predict(self, X):
        distances = np.zeros((X.shape[0], self.n_clusters))
        for k in range(self.n_clusters):
            distances[:, k] = np.linalg.norm(X - self.centers[k], axis=1)
        return np.argmin(distances, axis=1)
    
    def fit_predict(self, X):
        self.fit(X)
        return self.predict(X)
```

## 使用 sklearn

```python
from sklearn.cluster import KMeans

kmeans = KMeans(n_clusters=3, init='k-means++', n_init=10, max_iter=300, random_state=42)
labels = kmeans.fit_predict(X)

# 中心點
centers = kmeans.cluster_centers_

# 評估（簇內平方和）
inertia = kmeans.inertia_
```

### init 參數

- `'random'`：隨機選擇初始中心
- `'k-means++'`：智慧選擇初始中心，收斂更快（預設）

## K 值選擇

### 肘部法則 (Elbow Method)

```python
import matplotlib.pyplot as plt

wcss = []
K_range = range(1, 11)

for k in K_range:
    kmeans = KMeans(n_clusters=k, n_init=10, random_state=42)
    kmeans.fit(X)
    wcss.append(kmeans.inertia_)

plt.plot(K_range, wcss, 'bo-')
plt.xlabel('K')
plt.ylabel('WCSS')
plt.title('Elbow Method')
plt.show()
```

### 輪廓係數 (Silhouette Score)

```python
from sklearn.metrics import silhouette_score

for k in range(2, 11):
    kmeans = KMeans(n_clusters=k, random_state=42)
    labels = kmeans.fit_predict(X)
    score = silhouette_score(X, labels)
    print(f"K={k}: Silhouette Score = {score:.4f}")
```

## 特徵標準化

K-Means ��特徵尺度敏感，必須標準化：

```python
from sklearn.preprocessing import StandardScaler

scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)
```

## 限制

- 必須預先指定 K 值
- 假設簇為球形、大小相近
- 對初始中心點敏感
- 對噪音和離群值敏感

## 演算法比較

| 演算法 | 優點 | 缺點 | 適用場景 |
|--------|------|------|----------|
| K-Means | 簡單、快速 | 需預設 K、假設球形 | 大型資料、球形簇 |
| DBSCAN | 無需預設 K、可發現任意形狀 | 參數敏感 | 有噪音資料、任意形狀 |
| 層次聚類 | 無需預設 K、可視化樹狀 | 計算量大 | 小型資料層次結構 |

## 相關概念

- [聚類演算法](./聚類演算法.md) - 聚類演算法概述
- [非監督式學習](./非監督式學習.md) - 非監督式學習框架

## Tags

#K-Means #聚類演算法 #非監督式學習