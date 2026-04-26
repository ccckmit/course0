# Iris Classify

## 原理

### K 最近鄰分類器 (K-Nearest Neighbors, KNN)

KNN 是一種**基於距離**的監督式學習演算法，屬於「惰性學習」(lazy learning)，沒有顯式的訓練過程。

---

## KNN 原理

### 核心概念

1. **計算距離**: 找到測試樣本最近的 k 個鄰居
2. **投票決定**: 根據 k 個鄰居的類別投票
3. **輸出預測**: 票數最多的類別為預測結果

### 距離度量

常用**歐氏距離**：
$$d(x, y) = \sqrt{\sum_{i=1}^{n}(x_i - y_i)^2}$$

### 演算法步驟

1. 計算測試點與所有訓練點的距離
2. 選擇最近的 k 個點
3. 統計 k 個點的類別分布
4. 回傳多數類別

---

## 程式碼說明

```python
clf = KNeighborsClassifier(n_neighbors=5)
clf.fit(X_train, y_train)
y_pred = clf.predict(X_test)
```

- `n_neighbors=5`: 選擇 5 個最近鄰居
- `fit`: 儲存訓練資料（無實際訓練）
- `predict`: 找最近鄰居並投票

---

## KNN 優缺點

### 優點
- 簡單直觀，易於理解
- 無需訓練時間
- 適合多類別問題
- 對資料分布無假設

### 缺點
- 預測時間 O(n)，適合小資料集
- 對維度敏感（維度災難）
- 對 outlier 敏感
- 需要標準化特徵

---

## Iris 資料集

經典的機器學習資料集：

- **150 個樣本**: 3 類 × 50 個
- **4 個特徵**: 花萼/花瓣的長/寬度
- **3 個類別**: setosa, versicolor, virginica

---

## 執行

```bash
python3 iris_classify.py
```

輸出：
- Accuracy 分數
- 每個類別的 precision, recall, f1-score
- 整體分類報告
