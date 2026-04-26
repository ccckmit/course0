#!/usr/bin/env python3
"""隨機森林 (Random Forest) 範例"""

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
    parent = entropy(y)
    n = len(y)
    child = (len(left_idx) / n) * entropy([y[i] for i in left_idx]) + \
            (len(right_idx) / n) * entropy([y[i] for i in right_idx])
    return parent - child


class DecisionTree:
    def __init__(self, max_depth=5):
        self.max_depth = max_depth
        self.tree = None
    
    def fit(self, X, y):
        self.tree = self._build(X, y, 0)
    
    def _build(self, X, y, depth):
        if depth >= self.max_depth or len(set(y)) == 1:
            return {'label': Counter(y).most_common(1)[0][0]}
        
        best_gain, best_feature, best_threshold = -1, 0, 0
        for feature in range(len(X[0])):
            for threshold in set(x[feature] for x in X):
                gain = information_gain(X, y, feature, threshold)
                if gain > best_gain:
                    best_gain, best_feature, best_threshold = gain, feature, threshold
        
        left_idx = [i for i, x in enumerate(X) if x[best_feature] <= best_threshold]
        right_idx = [i for i, x in enumerate(X) if x[best_feature] > best_threshold]
        
        return {
            'feature': best_feature,
            'threshold': best_threshold,
            'left': self._build([X[i] for i in left_idx], [y[i] for i in left_idx], depth + 1),
            'right': self._build([X[i] for i in right_idx], [y[i] for i in right_idx], depth + 1)
        }
    
    def predict(self, X):
        return [self._predict(x, self.tree) for x in X]
    
    def _predict(self, x, node):
        if 'label' in node:
            return node['label']
        if x[node['feature']] <= node['threshold']:
            return self._predict(x, node['left'])
        return self._predict(x, node['right'])


class RandomForest:
    def __init__(self, n_trees=10, max_depth=5):
        self.n_trees = n_trees
        self.max_depth = max_depth
        self.trees = []
    
    def fit(self, X, y):
        self.trees = []
        n = len(X)
        
        for _ in range(self.n_trees):
            indices = [random.randint(0, n - 1) for _ in range(n)]
            X_bootstrap = [X[i] for i in indices]
            y_bootstrap = [y[i] for i in indices]
            
            tree = DecisionTree(self.max_depth)
            tree.fit(X_bootstrap, y_bootstrap)
            self.trees.append(tree)
    
    def predict(self, X):
        all_predictions = [tree.predict(X) for tree in self.trees]
        result = []
        for i in range(len(X)):
            votes = [pred[i] for pred in all_predictions]
            result.append(Counter(votes).most_common(1)[0][0])
        return result


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
    
    print("=== 隨機森林 ===")
    print("樣本數: " + str(len(X)))
    print("樹的數量: 10")
    
    rf = RandomForest(n_trees=10, max_depth=5)
    rf.fit(X, y)
    
    predictions = rf.predict(X)
    accuracy = sum(p == t for p, t in zip(predictions, y)) / len(y)
    print("訓練準確率: " + str(round(accuracy * 100, 2)) + "%")
    
    test_points = [[1.5, 1.5], [5, 5], [3.5, 3.5]]
    print("\n測試點預測:")
    for point in test_points:
        pred = rf.predict([point])[0]
        print("  " + str(point) + " -> " + pred)
