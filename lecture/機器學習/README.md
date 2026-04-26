# 機器學習

本課程介紹機器學習的核心概念與實作，涵蓋傳統 ML 到深度學習。

## 內容架構

### 1. 監督式學習
* 線性迴歸
    * [code/機器學習/ml/regression/](../code/機器學習/ml/regression/) - 迴歸實作
* 羅吉斯迴歸
    * [code/機器學習/ml/classify/logistic.py](../code/機器學習/ml/classify/logistic.py) - 實作
* KNN、決策樹、SVM、朴素貝葉斯
    * [code/機器學習/ml/classify/](../code/機器學習/ml/classify/) - 分類器實作
* 隨機森林、梯度提升
    * [code/機器學習/ml/classify/random_forest.py](../code/機器學習/ml/classify/random_forest.py) - 隨機森林
    * [code/機器學習/ml/classify/gbdt.md](../code/機器學習/ml/classify/gbdt.md) - GBDT

### 2. 非監督式學習
* K-Means 聚類
    * [code/機器學習/ml/cluster/kmean.py](../code/機器學習/ml/cluster/kmean.py) - 實作
* 層次聚類
    * [code/機器學習/ml/cluster/hierarchical.py](../code/機器學習/ml/cluster/hierarchical.py) - 實作
* DBSCAN、GMM
    * [code/機器學習/ml/cluster/](../code/機器學習/ml/cluster/) - 聚類實作
* PCA 維度降低
    * [code/機器學習/ml/pca/](../code/機器學習/ml/pca/) - PCA 實作

### 3. 機率模型
* 貝葉斯網路
    * [code/機器學習/ml/prob_model/bayesnet/](../code/機器學習/ml/prob_model/bayesnet/) - 實作
* 馬可夫模型
    * [code/機器學習/ml/prob_model/markov/](../code/機器學習/ml/prob_model/markov/) - 實作
* 隱藏馬可夫模型 (HMM)
    * [code/機器學習/ml/prob_model/hmm/](../code/機器學習/ml/prob_model/hmm/) - 實作
* EM 演算法
    * [code/機器學習/ml/prob_model/em/](../code/機器學習/ml/prob_model/em/) - 實作

### 4. 深度學習
* 神經網路基礎
* CNN 卷積神經網路
    * [code/機器學習/image/](../code/機器學習/image/) - 影像辨識
* RNN 循環神經網路
* Transformer

### 5. 強化學習
* Q-Learning
* Policy Gradient
    * [code/機器學習/](../code/機器學習/) - RL 專案

### 6. scikit-learn 實務
* 鳶尾花分類
    * [code/機器學習/ml/sklearn/](../code/機器學習/ml/sklearn/) - sklearn 範例

## 使用技術
- Python + scikit-learn + PyTorch + OpenAI Gym

## 相關資源
- Wiki: [_wiki/機器學習/](../_wiki/) - 理論補充