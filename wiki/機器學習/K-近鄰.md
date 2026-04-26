# K-近鄰 (KNN)

## 原理

K-近鄰（K-Nearest Neighbors）是一種基於實例的學習，屬於「惰性學習」（Lazy Learning），沒有明顯的訓練階段。它通過計算輸入樣本與訓練樣本之間的距離，找出最近的 K 個鄰居，根據這些鄰居的類別進行投票來決定輸入樣本的類別。KNN 的核心假設是「相似的事物彼此相互關聯」，因此同類別的樣本在特徵空間中距離較近。

$$P(y=c|x) = \frac{1}{K} \sum_{i \in N_K(x)} I(y_i = c)$$

其中 $N_K(x)$ 是與 $x$ 距離最近的 K 個樣本。

## 距離度量

```python
import numpy as np

class KNN:
    """K-近鄰分類器"""
    
    def __init__(self, k=3, metric='euclidean'):
        self.k = k
        self.metric = metric
        self.X_train = None
        self.y_train = None
    
    def distance(self, x1, x2):
        """計算距離"""
        if self.metric == 'euclidean':
            return np.sqrt(np.sum((x1 - x2) ** 2))
        elif self.metric == 'manhattan':
            return np.sum(np.abs(x1 - x2))
        elif self.metric == 'cosine':
            return 1 - np.dot(x1, x2) / (np.linalg.norm(x1) * np.linalg.norm(x2))
    
    def fit(self, X, y):
        """訓練（實際只是儲存資料）"""
        self.X_train = X
        self.y_train = y
        return self
    
    def predict_single(self, x):
        """單個樣本預測"""
        distances = np.array([self.distance(x, x_train) for x_train in self.X_train])
        k_indices = np.argsort(distances)[:self.k]
        k_labels = self.y_train[k_indices]
        counts = np.bincount(k_labels)
        return np.argmax(counts)
    
    def predict(self, X):
        return np.array([self.predict_single(x) for x in X])
```

## 使用 sklearn

```python
from sklearn.neighbors import KNeighborsClassifier

knn = KNeighborsClassifier(n_neighbors=5, metric='euclidean', weights='distance')
knn.fit(X_train, y_train)
predictions = knn.predict(X_test)
```

- `weights='distance'`：距離加權投票，距離越近的權重越大
- `weights='uniform'`：均勻投票（預設）

## K 值選擇

- K 值太小：容易受到噪音影響
- K 值太大：容易被遠離目標的點影響
- 通常使用奇數避免平票
- 使用交叉驗證選擇最佳 K 值

```python
from sklearn.model_selection import cross_val_score

for k in [1, 3, 5, 7, 9]:
    knn = KNeighborsClassifier(n_neighbors=k)
    scores = cross_val_score(knn, X, y, cv=5, scoring='accuracy')
    print(f"K={k}: {scores.mean():.4f} (+/- {scores.std()*2:.4f})")
```

## 特徵標準化

KNN 對特徵尺度敏感，必須標準化：

```python
from sklearn.preprocessing import StandardScaler

scaler = StandardScaler()
X_train_scaled = scaler.fit_transform(X_train)
X_test_scaled = scaler.transform(X_test)
```

## 演算法比較

| 演算法 | 優點 | 缺點 | 適用場景 |
|--------|------|------|----------|
| KNN | 簡單、無訓練階段、非線性 | 預測慢、高維度失效 | 小型資料、簡單分類 |
| 邏輯回歸 | 快速、可解釋 | 只能處理線性 | 基線模型 |
| SVM | 高維度表現好 | 訓練慢 | 高維度資料 |
| 決策樹 | 可解釋 | 容易過擬合 | 特徵分析 |

## 相關概念

- [分類演算法](./分類演算法.md) - 分類演算法概述
- [監督式學習](./監督式學習.md) - 監督式學習框架

## Tags

#KNN #分類演算法 #監督式學習