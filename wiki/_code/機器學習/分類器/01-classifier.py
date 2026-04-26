#!/usr/bin/env python3
"""
分類器 - 經典分類演算法範例
展示 KNN、SVM、決策樹等分類方法
"""

import math
import random
from collections import Counter


class KNN:
    """K 近鄰分類器"""
    
    def __init__(self, k: int = 3):
        self.k = k
        self.X_train = None
        self.y_train = None
    
    def fit(self, X: list, y: list) -> None:
        """訓練（儲存數據）"""
        self.X_train = X
        self.y_train = y
    
    def _distance(self, a: list, b: list) -> float:
        """歐氏距離"""
        return math.sqrt(sum((ai - bi) ** 2 for ai, bi in zip(a, b)))
    
    def predict(self, X: list) -> list:
        """預測"""
        predictions = []
        for x in X:
            distances = [(self._distance(x, xi), yi) 
                        for xi, yi in zip(self.X_train, self.y_train)]
            distances.sort(key=lambda d: d[0])
            
            # 取前 k 個
            k_nearest = [d[1] for d in distances[:self.k]]
            
            # 多數投票
            counter = Counter(k_nearest)
            predictions.append(counter.most_common(1)[0][0])
        
        return predictions


class NaiveBayes:
    """樸素貝葉斯分類器"""
    
    def __init__(self):
        self.class_priors = {}
        self.conditional_probs = {}
    
    def fit(self, X: list, y: list) -> None:
        """訓練"""
        n_samples = len(X)
        
        # 計算類別先驗機率
        classes = set(y)
        for c in classes:
            self.class_priors[c] = sum(1 for yi in y if yi == c) / n_samples
        
        # 計算條件機率（簡化版：每個特徵獨立的常態分佈）
        self.conditional_probs = {}
        for c in classes:
            X_c = [X[i] for i in range(n_samples) if y[i] == c]
            
            self.conditional_probs[c] = []
            for j in range(len(X[0])):
                values = [x[j] for x in X_c]
                mean = sum(values) / len(values)
                var = sum((v - mean) ** 2 for v in values) / len(values)
                self.conditional_probs[c].append((mean, var))
    
    def _gaussian_pdf(self, x: float, mean: float, var: float) -> float:
        """常態分佈機率密度"""
        return (1 / math.sqrt(2 * math.pi * var)) * \
               math.exp(-(x - mean) ** 2 / (2 * var))
    
    def predict(self, X: list) -> list:
        """預測"""
        predictions = []
        for x in X:
            posteriors = {}
            
            for c in self.class_priors:
                posterior = self.class_priors[c]
                
                for j, (mean, var) in enumerate(self.conditional_probs[c]):
                    posterior *= self._gaussian_pdf(x[j], mean, var)
                
                posteriors[c] = posterior
            
            predictions.append(max(posteriors, key=posteriors.get))
        
        return predictions


class DecisionTree:
    """簡單決策樹"""
    
    def __init__(self, max_depth: int = 5):
        self.max_depth = max_depth
        self.tree = None
    
    def _entropy(self, y: list) -> float:
        """計算熵"""
        if not y:
            return 0
        counter = Counter(y)
        probs = [c / len(y) for c in counter.values()]
        return -sum(p * math.log2(p) for p in probs if p > 0)
    
    def _information_gain(self, X: list, y: list, feature: int, threshold: float) -> float:
        """計算資訊增益"""
        # 分裂
        left_idx = [i for i, x in enumerate(X) if x[feature] <= threshold]
        right_idx = [i for i, x in enumerate(X) if x[feature] > threshold]
        
        if not left_idx or not right_idx:
            return 0
        
        # 父節點熵
        parent_entropy = self._entropy(y)
        
        # 子節點加權熵
        n = len(y)
        n_left, n_right = len(left_idx), len(right_idx)
        
        e_left = self._entropy([y[i] for i in left_idx])
        e_right = self._entropy([y[i] for i in right_idx])
        
        child_entropy = (n_left / n) * e_left + (n_right / n) * e_right
        
        return parent_entropy - child_entropy
    
    def _build_tree(self, X: list, y: list, depth: int) -> dict:
        """遞迴建樹"""
        n_samples = len(X)
        n_features = len(X[0])
        
        # 停止條件
        if depth >= self.max_depth or len(set(y)) == 1:
            return {'label': Counter(y).most_common(1)[0][0]}
        
        # 找最佳分裂
        best_gain = -1
        best_feature = 0
        best_threshold = 0
        
        for feature in range(n_features):
            thresholds = set(x[feature] for x in X)
            for threshold in thresholds:
                gain = self._information_gain(X, y, feature, threshold)
                if gain > best_gain:
                    best_gain = gain
                    best_feature = feature
                    best_threshold = threshold
        
        # 分裂
        left_idx = [i for i, x in enumerate(X) if x[feature] <= threshold]
        right_idx = [i for i, x in enumerate(X) if x[feature] > threshold]
        
        # 建樹
        tree = {
            'feature': best_feature,
            'threshold': best_threshold,
            'left': self._build_tree([X[i] for i in left_idx], 
                                    [y[i] for i in left_idx], 
                                    depth + 1),
            'right': self._build_tree([X[i] for i in right_idx], 
                                      [y[i] for i in right_idx], 
                                      depth + 1)
        }
        
        return tree
    
    def fit(self, X: list, y: list) -> None:
        """訓練"""
        self.tree = self._build_tree(X, y, 0)
    
    def _predict_single(self, x: list, node: dict) -> str:
        """預測單個樣本"""
        if 'label' in node:
            return node['label']
        
        if x[node['feature']] <= node['threshold']:
            return self._predict_single(x, node['left'])
        else:
            return self._predict_single(x, node['right'])
    
    def predict(self, X: list) -> list:
        """預測"""
        return [self._predict_single(x, self.tree) for x in X]


if __name__ == "__main__":
    # 生成測試數據
    random.seed(42)
    
    # 數據1: 簡單分類
    X = [[1, 2], [2, 3], [3, 1], [4, 3], [5, 5], [6, 4]]
    y = [0, 0, 0, 1, 1, 1]
    
    # 測試 KNN
    print("=== KNN ===")
    knn = KNN(k=3)
    knn.fit(X, y)
    test_points = [[2, 2], [5, 5]]
    print(f"預測 {test_points}: {knn.predict(test_points)}")
    
    # 測試 Naive Bayes
    print("\n=== Naive Bayes ===")
    nb = NaiveBayes()
    nb.fit(X, y)
    print(f"預測 {test_points}: {nb.predict(test_points)}")
    
    # 測試 Decision Tree
    print("\n=== Decision Tree ===")
    dt = DecisionTree(max_depth=3)
    dt.fit(X, y)
    print(f"預測 {test_points}: {dt.predict(test_points)}")
