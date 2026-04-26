# scikit-learn

scikit-learn 是 Python 生態系統中最廣泛使用的機器學習經典庫，提供了從資料預處理到模型訓練、評估、調參的完整工具鏈。於 2007 年發布，採用 NumPy/SciPy 陣列運算框架，設計簡潔優雅，API 統一一致，適合處理傳統機器學習任務。

## 安裝與環境

```bash
pip install scikit-learn numpy scipy
conda install scikit-learn
```

## 基本工作流程

```python
from sklearn import datasets
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score

# 載入資料
iris = datasets.load_iris()
X, y = iris.data, iris.target

# 分割資料
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# 標準化
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)

# 訓練
model = LogisticRegression()
model.fit(X_train, y_train)

# 預測
pred = model.predict(X_test)
print(accuracy_score(y_test, pred))
```

## 監督式學習

監督式學習是最常見的機器學習範式，訓練資料包含輸入特徵和對應的標籤。

### 分類 (Classification)

#### 羅吉斯回歸 (Logistic Regression)

```python
from sklearn.linear_model import LogisticRegression

model = LogisticRegression(
    penalty='l2',      # 正則化 ('l1', 'l2', 'elasticnet', None)
    C=1.0,              # 正則化強度倒數
    solver='lbfgs',     # 優化演算法
    max_iter=1000,      # 最大迭代次數
    multi_class='auto'  # 多分類策略
)
model.fit(X_train, y_train)
pred = model.predict(X_train)

# 機率預測
proba = model.predict_proba(X_test)
```

#### 支援向量機 (Support Vector Machine)

```python
from sklearn.svm import SVC, SVR

# 分類
svc = SVC(
    kernel='rbf',       # 核心函數 ('linear', 'poly', 'rbf', 'sigmoid')
    C=1.0,              # 正則化參數
    gamma='scale',      # 核心係數
    probability=True   # 啟用機率估計
)
svc.fit(X_train, y_train)

# 回歸
svr = SVR(kernel='rbf', C=100, gamma=0.1)
svr.fit(X_train, y_train)
```

#### 決策樹 (Decision Tree)

```python
from sklearn.tree import DecisionTreeClassifier, plot_tree
import matplotlib.pyplot as plt

dt = DecisionTreeClassifier(
    criterion='gini',    # 分裂標準 ('gini', 'entropy')
    max_depth=None,       # 樹的最大深度
    min_samples_split=2, # 分裂所需最小樣本數
    min_samples_leaf=1    # 葉節點最小樣本數
)
dt.fit(X_train, y_train)

# 視覺化
plt.figure(figsize=(20, 10))
plot_tree(dt, filled=True, rounded=True)
plt.savefig('tree.png')
```

#### 隨機森林 (Random Forest)

```python
from sklearn.ensemble import RandomForestClassifier

rf = RandomForestClassifier(
    n_estimators=100,    # 樹的數量
    max_depth=None,       # 樹的最大深度
    min_samples_split=2,
    min_samples_leaf=1,
    max_features='sqrt', # 特徵選擇數
    bootstrap=True,      # 抽樣策略
    n_jobs=-1            # 並行處理
)
rf.fit(X_train, y_train)

# 特徵重要性
feature_importance = rf.feature_importances_
```

#### 梯度提升 (Gradient Boosting)

```python
from sklearn.ensemble import GradientBoostingClassifier

gb = GradientBoostingClassifier(
    n_estimators=100,    # 弱學習器數量
    learning_rate=0.1,    # 學習率
    max_depth=3,         # 樹的最大深度
    subsample=1.0,       # 抽樣比例
    random_state=42
)
gb.fit(X_train, y_train)
```

### 回歸 (Regression)

#### 線性回歸

```python
from sklearn.linear_model import LinearRegression, Ridge, Lasso, ElasticNet

# 簡單線性回歸
lr = LinearRegression()
lr.fit(X_train, y_train)

# 嶺回歸 (L2 正則化)
ridge = Ridge(alpha=1.0)
ridge.fit(X_train, y_train)

# Lasso 回歸 (L1 正則化)
lasso = Lasso(alpha=1.0)
lasso.fit(X_train, y_train)

# ElasticNet (L1 + L2)
elastic = ElasticNet(alpha=1.0, l1_ratio=0.5)
elastic.fit(X_train, y_train)
```

#### 支持向量回歸

```python
from sklearn.svm import SVR

svr = SVR(
    kernel='rbf',
    C=1.0,
    epsilon=0.1,
    gamma='scale'
)
svr.fit(X_train, y_train)
pred = svr.predict(X_test)
```

## 非監督式學習

非監督式學習處理沒有標籤的資料，目標是發現資料的內在結構。

### 聚類 (Clustering)

#### K-Means

```python
from sklearn.cluster import KMeans

kmeans = KMeans(
    n_clusters=3,       # 叢集數量
    init='k-means++',   # 初始化方法
    n_init=10,         # 初始化次數
    max_iter=300,      # 最大迭代次數
    random_state=42
)
kmeans.fit(X)

# 預測叢集標籤
labels = kmeans.labels_

# 找最佳叢集數 (肘部法)
inertias = []
K_range = range(1, 11)
for k in K_range:
    km = KMeans(n_clusters=k, random_state=42)
    km.fit(X)
    inertias.append(km.inertia_)
```

#### DBSCAN

```python
from sklearn.cluster import DBSCAN

dbscan = DBSCAN(
    eps=0.5,            # 鄰域半徑
    min_samples=5,     # 核心點最小鄰居數
    metric='euclidean'
)
labels = dbscan.fit_predict(X)
```

#### 層次聚類

```python
from sklearn.cluster import AgglomerativeClustering

agg = AgglomerativeClustering(
    n_clusters=3,
    linkage='ward'       # ('ward', 'complete', 'average')
)
labels = agg.fit_predict(X)
```

### 降維 (Dimensionality Reduction)

#### PCA (主成份分析)

```python
from sklearn.decomposition import PCA

pca = PCA(n_components=2)
X_pca = pca.fit_transform(X)

# 解釋變異量
print(f"解釋變異量: {pca.explained_variance_ratio_}")
print(f"累積解釋變異量: {sum(pca.explained_variance_ratio_)}")
```

#### t-SNE

```python
from sklearn.manifold import TSNE

tsne = TSNE(
    n_components=2,
    perplexity=30,
    learning_rate='auto',
    random_state=42
)
X_tsne = tsne.fit_transform(X)
```

#### UMAP (需安裝 umap-learn)

```python
import umap.umap_ as umap

reducer = umap.UMAP(
    n_components=2,
    n_neighbors=15,
    min_dist=0.1,
    random_state=42
)
X_umap = reducer.fit_transform(X)
```

## 模型評估

### 分類指標

```python
from sklearn.metrics import (
    accuracy_score,
    precision_score,
    recall_score,
    f1_score,
    confusion_matrix,
    classification_report,
    roc_auc_score,
    roc_curve
)

# 準確率
accuracy = accuracy_score(y_test, y_pred)

# 精確率、召回率、F1
precision = precision_score(y_test, y_pred, average='macro')
recall = recall_score(y_test, y_pred, average='macro')
f1 = f1_score(y_test, y_pred, average='macro')

# 混淆矩陣
cm = confusion_matrix(y_test, y_pred)
print(cm)

# 完整報告
print(classification_report(y_test, y_pred))

# ROC-AUC (二分類)
y_prob = model.predict_proba(X_test)[:, 1]
roc_auc = roc_auc_score(y_test, y_prob)
fpr, tpr, thresholds = roc_curve(y_test, y_prob)
```

### 回歸指標

```python
from sklearn.metrics import (
    mean_squared_error,
    mean_absolute_error,
    r2_score,
    median_absolute_error
)

mse = mean_squared_error(y_test, y_pred)
rmse = np.sqrt(mse)
mae = mean_absolute_error(y_test, y_pred)
r2 = r2_score(y_test, y_pred)
```

### 交叉驗證

```python
from sklearn.model_selection import (
    cross_val_score,
    KFold,
    StratifiedKFold,
    LeaveOneOut,
    ShuffleSplit
)

# K-Fold 交叉驗證
kf = KFold(n_splits=5, shuffle=True, random_state=42)
scores = cross_val_score(model, X, y, cv=kf, scoring='accuracy')

# 分層 K-Fold (保持類別比例)
skf = StratifiedKFold(n_splits=5, shuffle=True, random_state=42)
scores = cross_val_score(model, X, y, cv=skf, scoring='accuracy')
```

### 超參數調參

```python
from sklearn.model_selection import (
    GridSearchCV,
    RandomizedSearchCV
)

# 網格搜尋
param_grid = {
    'n_estimators': [100, 200, 300],
    'max_depth': [3, 5, 7],
    'learning_rate': [0.01, 0.1, 0.2]
}

grid_search = GridSearchCV(
    estimator=GradientBoostingClassifier(),
    param_grid=param_grid,
    cv=5,
    scoring='accuracy',
    n_jobs=-1
)
grid_search.fit(X_train, y_train)

print(f"最佳參數: {grid_search.best_params_}")
print(f"最佳分數: {grid_search.best_score_}")

# 隨機搜尋 (適合大參數空間)
from scipy.stats import randint, uniform
param_dist = {
    'n_estimators': randint(100, 500),
    'max_depth': randint(3, 10),
    'learning_rate': uniform(0.01, 0.3)
}

random_search = RandomizedSearchCV(
    estimator=GradientBoostingClassifier(),
    param_distributions=param_dist,
    n_iter=50,
    cv=5,
    random_state=42,
    n_jobs=-1
)
random_search.fit(X_train, y_train)
```

## 資料預處理

```python
from sklearn.preprocessing import (
    StandardScaler,      # 標準化
    MinMaxScaler,     # 最小最大縮放
    RobustScaler,     # 魯棒縮放
    MaxAbsScaler,     # 最大絕對值縮放
    LabelEncoder,     # 標籤編碼
    OneHotEncoder,    # 獨熱編碼
    PolynomialFeatures # 多項式特徵
)

# 標準化 (均值 0, 標準差 1)
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# 最小最大縮放 [0, 1]
minmax = MinMaxScaler()
X_scaled = minmax.fit_transform(X)

# 獨熱編碼
from sklearn.compose import ColumnTransformer
ct = ColumnTransformer(
    transformers=[('encoder', OneHotEncoder(), [0])],
    remainder='passthrough'
)
X_encoded = ct.fit_transform(X)
```

## Pipelines

```python
from sklearn.pipeline import Pipeline, make_pipeline

# 方法 1
pipe = Pipeline([
    ('scaler', StandardScaler()),
    ('classifier', LogisticRegression())
])
pipe.fit(X_train, y_train)
pred = pipe.predict(X_test)

# 方法 2 (簡化語法)
pipe = make_pipeline(StandardScaler(), LogisticRegression())
pipe.fit(X_train, y_train)
```

## 常見模組總覽

| 模組 | 功能 |
|------|------|
| datasets | 載入範例資料、自訂資料生成 |
| model_selection | 交叉驗證、網格搜尋、隨機搜尋 |
| preprocessing | 標準化、編碼、特徵工程 |
| linear_model | 線性回歸、羅吉斯回歸、嶺回歸 |
| tree | 決策樹 |
| ensemble | 隨機森林、梯度提升、AdaBoost |
| svm | 支援向量機 |
| cluster | K-means、DBSCAN、層次聚類 |
| decomposition | PCA、ICA |
| manifold | t-SNE、UMAP |
| metrics | 評估指標 |
| feature_selection | 特徵選擇 |

## 資料集

```python
from sklearn.datasets import (
    load_iris,           # 鳶尾花資料
    load_digits,        # 手寫數字
    load_diabetes,       # 糖尿病
    load_boston,        # 波士頓房價 (已棄用)
    load_breast_cancer,  # 乳癌
    make_classification,
    make_regression,
    make_blobs,
    make_moons,
    make_circles
)

# 內建資料集
iris = load_iris()
digits = load_digits()

# 生成資料
X, y = make_classification(
    n_samples=1000,
    n_features=20,
    n_informative=15,
    n_redundant=5,
    n_classes=2,
    random_state=42
)
```

## 相關概念

- [監督式學習](監督式學習.md)
- [非監督式學習](非監督式學習.md)
- [深度學習](深度學習.md) - 適合大規模資料
- [TensorFlow](TensorFlow.md) - 深度���習���架