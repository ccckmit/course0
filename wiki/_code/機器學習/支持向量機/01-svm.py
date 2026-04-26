#!/usr/bin/env python3
"""支持向量機 (SVM) 範例 - 簡化版"""

import math
import random


def dot(a, b):
    return sum(ai * bi for ai, bi in zip(a, b))


class LinearSVM:
    def __init__(self, lr=0.1, epochs=100):
        self.lr = lr
        self.epochs = epochs
        self.weights = []
        self.bias = 0
    
    def fit(self, X, y):
        n_features = len(X[0])
        self.weights = [0.0] * n_features
        self.bias = 0
        
        for _ in range(self.epochs):
            for xi, yi in zip(X, y):
                condition = yi * (dot(xi, self.weights) + self.bias)
                
                if condition >= 1:
                    for i in range(n_features):
                        self.weights[i] -= self.lr * 0.01 * self.weights[i]
                else:
                    for i in range(n_features):
                        self.weights[i] -= self.lr * (0.01 * self.weights[i] - yi * xi[i])
                    self.bias -= self.lr * (-yi)
    
    def predict(self, X):
        return [1 if dot(x, self.weights) + self.bias >= 0 else -1 for x in X]


def generate_data():
    random.seed(42)
    X = []
    y = []
    
    for _ in range(30):
        X.append([random.gauss(1, 0.5), random.gauss(1, 0.5)])
        y.append(1)
    
    for _ in range(30):
        X.append([random.gauss(4, 0.5), random.gauss(4, 0.5)])
        y.append(-1)
    
    combined = list(zip(X, y))
    random.shuffle(combined)
    X, y = zip(*combined)
    return list(X), list(y)


if __name__ == "__main__":
    X, y = generate_data()
    
    print("=== 支持向量機 (SVM) - 線性 ===")
    print("樣本數: " + str(len(X)))
    
    svm = LinearSVM(lr=0.01, epochs=200)
    svm.fit(X, y)
    
    predictions = svm.predict(X)
    accuracy = sum(p == t for p, t in zip(predictions, y)) / len(y)
    print("訓練準確率: " + str(round(accuracy * 100, 2)) + "%")
    
    test_points = [[1.5, 1.5], [3, 3], [4.5, 4.5]]
    print("\n測試點預測:")
    for point in test_points:
        pred = svm.predict([point])[0]
        label = "類別 1" if pred == 1 else "類別 -1"
        print("  " + str(point) + " -> " + label)
    
    print("\n權重: " + str([round(w, 3) for w in svm.weights]))
    print("偏置: " + str(round(svm.bias, 3)))
