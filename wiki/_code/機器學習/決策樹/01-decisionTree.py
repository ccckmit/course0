#!/usr/bin/env python3
"""決策樹 (Decision Tree) 範例"""

import math
import random
from collections import Counter


def entropy(y):
    if not y:
        return 0
    counter = Counter(y)
    probs = [c / len(y) for c in counter.values()]
    return -sum(p * math.log2(p) for p in probs if p > 0)


def information_gain(X, y, feature, threshold):
    left_idx = [i for i, x in enumerate(X) if x[feature] <= threshold]
    right_idx = [i for i, x in enumerate(X) if x[feature] > threshold]
    
    if not left_idx or not right_idx:
        return 0
    
    parent_entropy = entropy(y)
    n = len(y)
    n_left, n_right = len(left_idx), len(right_idx)
    
    e_left = entropy([y[i] for i in left_idx])
    e_right = entropy([y[i] for i in right_idx])
    
    child_entropy = (n_left / n) * e_left + (n_right / n) * e_right
    return parent_entropy - child_entropy


class DecisionTree:
    def __init__(self, max_depth=5):
        self.max_depth = max_depth
        self.tree = None
    
    def fit(self, X, y):
        self.tree = self._build_tree(X, y, 0)
    
    def _build_tree(self, X, y, depth):
        n_samples = len(X)
        n_features = len(X[0])
        
        if depth >= self.max_depth or len(set(y)) == 1:
            return {'label': Counter(y).most_common(1)[0][0]}
        
        best_gain = -1
        best_feature = 0
        best_threshold = 0
        
        for feature in range(n_features):
            thresholds = set(x[feature] for x in X)
            for threshold in thresholds:
                gain = information_gain(X, y, feature, threshold)
                if gain > best_gain:
                    best_gain = gain
                    best_feature = feature
                    best_threshold = threshold
        
        left_idx = [i for i, x in enumerate(X) if x[feature] <= threshold]
        right_idx = [i for i, x in enumerate(X) if x[feature] > threshold]
        
        return {
            'feature': best_feature,
            'threshold': best_threshold,
            'left': self._build_tree([X[i] for i in left_idx], [y[i] for i in left_idx], depth + 1),
            'right': self._build_tree([X[i] for i in right_idx], [y[i] for i in right_idx], depth + 1)
        }
    
    def predict(self, X):
        return [self._predict_single(x, self.tree) for x in X]
    
    def _predict_single(self, x, node):
        if 'label' in node:
            return node['label']
        if x[node['feature']] <= node['threshold']:
            return self._predict_single(x, node['left'])
        return self._predict_single(x, node['right'])


def generate_data():
    random.seed(42)
    X = []
    y = []
    
    for _ in range(30):
        X.append([random.uniform(0, 3), random.uniform(0, 3)])
        y.append('A')
    
    for _ in range(30):
        X.append([random.uniform(4, 7), random.uniform(4, 7)])
        y.append('B')
    
    combined = list(zip(X, y))
    random.shuffle(combined)
    X, y = zip(*combined)
    return list(X), list(y)


if __name__ == "__main__":
    X, y = generate_data()
    
    print("=== 決策樹 ===")
    print("樣本數: " + str(len(X)))
    
    dt = DecisionTree(max_depth=5)
    dt.fit(X, y)
    
    predictions = dt.predict(X)
    accuracy = sum(p == t for p, t in zip(predictions, y)) / len(y)
    print("訓練準確率: " + str(round(accuracy * 100, 2)) + "%")
    
    test_points = [[1.5, 1.5], [5, 5], [3.5, 3.5]]
    print("\n測試點預測:")
    for point in test_points:
        pred = dt.predict([point])[0]
        print("  " + str(point) + " -> " + pred)
