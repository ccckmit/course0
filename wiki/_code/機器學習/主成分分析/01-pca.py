#!/usr/bin/env python3
"""主成分分析 (PCA) 範例"""

import math
import random


def mean(data):
    return sum(data) / len(data)


def variance(data):
    m = mean(data)
    return sum((x - m) ** 2 for x in data) / len(data)


def covariance(x, y):
    mx, my = mean(x), mean(y)
    return sum((x[i] - mx) * (y[i] - my) for i in range(len(x))) / len(x)


def standard_deviation(data):
    return math.sqrt(variance(data))


class PCA:
    def __init__(self, n_components=2):
        self.n_components = n_components
        self.mean = []
        self.std = []
        self.components = []
        self.explained_variance = []
    
    def _standardize(self, X):
        n_features = len(X[0])
        self.mean = [mean([X[i][j] for i in range(len(X))]) for j in range(n_features)]
        self.std = [standard_deviation([X[i][j] for i in range(len(X))]) for j in range(n_features)]
        
        X_std = []
        for row in X:
            X_std.append([(row[j] - self.mean[j]) / self.std[j] if self.std[j] > 0 else 0 
                         for j in range(n_features)])
        return X_std
    
    def _compute_covariance_matrix(self, X):
        n = len(X[0])
        cov = [[0] * n for _ in range(n)]
        for i in range(n):
            for j in range(n):
                if i == j:
                    cov[i][j] = variance([X[k][i] for k in range(len(X))])
                elif i < j:
                    cov[i][j] = covariance([X[k][i] for k in range(len(X))], 
                                          [X[k][j] for k in range(len(X))])
                    cov[j][i] = cov[i][j]
        return cov
    
    def _power_iteration(self, matrix, n_iter=100):
        n = len(matrix)
        vector = [1.0 / math.sqrt(n)] * n
        
        for _ in range(n_iter):
            new_vector = [0] * n
            for i in range(n):
                for j in range(n):
                    new_vector[i] += matrix[i][j] * vector[j]
            
            norm = math.sqrt(sum(v ** 2 for v in new_vector))
            if norm > 0:
                vector = [v / norm for v in new_vector]
        
        eigenvalue = sum(vector[i] * sum(matrix[i][j] * vector[j] for j in range(n)) 
                      for i in range(n))
        
        return eigenvalue, vector
    
    def fit(self, X):
        X_std = self._standardize(X)
        cov = self._compute_covariance_matrix(X_std)
        
        n = len(X[0])
        self.components = []
        self.explained_variance = []
        
        matrix = [row[:] for row in cov]
        
        for _ in range(min(self.n_components, n)):
            eigenvalue, eigenvector = self._power_iteration(matrix)
            self.components.append(eigenvector)
            self.explained_variance.append(eigenvalue)
            
            for i in range(n):
                for j in range(n):
                    matrix[i][j] -= eigenvalue * eigenvector[i] * eigenvector[j]
        
        total_var = sum(self.explained_variance)
        self.explained_variance_ratio = [v / total_var for v in self.explained_variance]
    
    def transform(self, X):
        X_std = [[(X[i][j] - self.mean[j]) / self.std[j] if self.std[j] > 0 else 0 
                 for j in range(len(X[0]))] for i in range(len(X))]
        
        result = []
        for row in X_std:
            transformed = []
            for component in self.components:
                transformed.append(sum(row[j] * component[j] for j in range(len(row))))
            result.append(transformed)
        return result
    
    def fit_transform(self, X):
        self.fit(X)
        return self.transform(X)


def generate_test_data():
    random.seed(42)
    X = []
    for i in range(100):
        t = i / 100 * 4 * math.pi
        x = 3 * math.cos(t) + random.gauss(0, 0.3)
        y = 2 * math.sin(t) + random.gauss(0, 0.3)
        z = x + y + random.gauss(0, 0.1)
        X.append([x, y, z])
    return X


if __name__ == "__main__":
    X = generate_test_data()
    
    print("=== 主成分分析 (PCA) ===")
    print("原始資料維度: " + str(len(X[0])) + "D")
    print("樣本數: " + str(len(X)))
    
    pca = PCA(n_components=2)
    X_pca = pca.fit_transform(X)
    
    print("\n轉換後維度: " + str(len(X_pca[0])) + "D")
    print("解釋變異量比:")
    for i, ratio in enumerate(pca.explained_variance_ratio):
        print("  PC" + str(i+1) + ": " + str(round(ratio * 100, 2)) + "%")
    
    print("\n前5個轉換後的點:")
    for i in range(5):
        print("  " + str([round(x, 2) for x in X_pca[i]]))
