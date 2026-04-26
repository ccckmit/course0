# Compare Cluster

## 原理

### 比較多種聚類演算法

本程式比較 11 種聚類演算法在 6 種不同資料分布上的表現。

---

## 1. MiniBatch KMeans

KMeans 的變體，使用小批量隨機樣本加速。

- 時間複雜度 O(t × k × m)
- 適合大規模資料

---

## 2. Affinity Propagation

**訊息傳遞演算法**，根據點之間的「訊息交換」決定典範點。

- 不需要預設 k
- 自動決定集群數量
- 時間複雜度 O(n²)

---

## 3. Mean Shift

**密度導向**演算法，向密度最高處移動。

- 自動發現集群數量
- 適合任意形狀
- 對 bandwidth 參數敏感

---

## 4. Spectral Clustering

**圖論**方法，用拉普拉斯矩陣做維度降低。

- 適合非凸形狀
- 需要預設 k
- 時間複雜度 O(n³)

---

## 5. Ward (Hierarchical)

**階層式聚類**，最小化合併時的變異增加。

- 適合不同大小/形狀
- 需要預設 k

---

## 6. Agglomerative (Hierarchical)

**凝聚式階層聚類**，逐步合併最相似的集群。

- 可用不同連結方法：single, complete, average
- 適合多樣結構

---

## 7. DBSCAN

**密度-based**，根據密度連通性分組。

- 自動發現集群數量
- 可以偵測 outlier（標記為 -1）
- 對 eps, min_samples 敏感
- 適合任意形狀

---

## 8. HDBSCAN

DBSCAN 的改良版：

- 自動選擇最佳 eps
- 更穩定的集群偵測
- 適合多變密度的資料

---

## 9. OPTICS

DBSCAN 的拡張：

- 建立密度排序
- 可產生層次式結果
- 對 xi, min_samples 敏感

---

## 10. BIRCH

**記憶效率**高的階層式聚類：

- 適合大規模資料
- 無法處理非凸形狀

---

## 11. Gaussian Mixture Model (GMM)

**機率模型**，假設資料來自多個高斯分布。

- 軟性分派（回傳機率）
- 適合橢圓形集群
- 需要預設 components

---

## 資料集類型

| 資料集 | 特性 |
|--------|------|
| Noisy Circles | 兩個同心圓 + 雜訊 |
| Noisy Moons | 兩個月亮形 + 雜訊 |
| Varied Blobs | 不同方差的 blobs |
| Anisotropic | 拉伸變形的 blobs |
| Blobs | 標準球形 |
| No Structure | 隨機分布 |

## 執行

```bash
python3 compare_cluster.py
```