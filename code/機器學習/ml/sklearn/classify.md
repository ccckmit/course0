# Classify

## 原理

### 互動式分類實驗工具

本程式提供互動式的監督式學習分類實驗環境。

---

## 支援的資料集

| 編號 | 資料集 | 樣本數 | 特徵數 | 類別數 |
|------|--------|--------|--------|--------|
| 1 | iris | 150 | 4 | 3 |
| 2 | breast_cancer | 569 | 30 | 2 |
| 3 | wine | 178 | 13 | 3 |
| 4 | digits | 1797 | 64 | 10 |
| 5 | covtype | 581012 | 54 | 7 |

---

## 支援的分類器

### 1. RandomForestClassifier
**隨機森林**
- 集成多棵決策樹
- 適合高維度資料
- 不易過擬合

### 2. GradientBoostingClassifier
**梯度提升**
- 逐步修正錯誤
- 高準確率
- 可能過擬合

### 3. AdaBoostClassifier
**AdaBoost**
- 聚焦錯誤樣本
- 適合二元分類

### 4. LogisticRegression
**邏輯迴歸**
- 線性分類
- 可解釋性高
- 輸出機率

### 5. SVC
**支援向量機**
- 最大邊界分類
- 適合高維度
- 需要標準化

### 6. KNeighborsClassifier
**K 最近鄰**
- 基於距離
- 簡單直觀
- 對維度敏感

### 7. DecisionTreeClassifier
**決策樹**
- 規則易解釋
- 容易視覺化
- 容易過擬合

### 8. GaussianNB
**高斯朴素貝葉斯**
- 基於貝葉斯定理
- 假設特徵獨立
- 快速簡單

---

## 評估指標

### Accuracy
$$\text{Accuracy} = \frac{TP + TN}{TP + TN + FP + FN}$$

### Confusion Matrix


|  | 預測正 | 預測負 |
|--|--------|--------|
| 實際正 | TP | FN |
| 實際負 | FP | TN |

### Precision, Recall, F1
- Precision = TP / (TP + FP)
- Recall = TP / (TP + FN)
- F1 = 2 × P × R / (P + R)

---

## 執行方式

```bash
python3 classify.py              # 互動模式
python3 classify.py 1 1         # iris + RandomForest
python3 classify.py 2 5         # breast_cancer + SVC
```