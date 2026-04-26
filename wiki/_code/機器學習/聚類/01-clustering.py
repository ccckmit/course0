#!/usr/bin/env python3
"""聚類 - K-Means, DBSCAN 演算法範例"""

import math
import random
from collections import Counter
from typing import List


def euclidean_distance(a, b):
    return math.sqrt(sum((ai - bi) ** 2 for ai, bi in zip(a, b)))


class KMeans:
    def __init__(self, k=3, max_iter=50):
        self.k = k
        self.max_iter = max_iter
        self.centroids = []
        self.labels = []
    
    def fit(self, X):
        indices = random.sample(range(len(X)), self.k)
        self.centroids = [X[i][:] for i in indices]
        
        for _ in range(self.max_iter):
            labels = []
            for x in X:
                distances = [euclidean_distance(x, c) for c in self.centroids]
                labels.append(distances.index(min(distances)))
            
            for i in range(self.k):
                pts = [X[j] for j in range(len(X)) if labels[j] == i]
                if pts:
                    self.centroids[i] = [sum(p[j] for p in pts) / len(pts) for j in range(len(X[0]))]
            
            if labels == self.labels:
                break
            self.labels = labels
    
    def predict(self, X):
        return [min(range(self.k), key=lambda j: euclidean_distance(x, self.centroids[j])) for x in X]


class DBSCAN:
    def __init__(self, eps=1.0, min_samples=2):
        self.eps = eps
        self.min_samples = min_samples
        self.labels = []
    
    def fit(self, X):
        n = len(X)
        self.labels = [0] * n
        cluster_id = 0
        
        def region_query(idx):
            return [i for i in range(n) if i != idx and euclidean_distance(X[idx], X[i]) <= self.eps]
        
        for i in range(n):
            if self.labels[i] != 0:
                continue
            seeds = region_query(i)
            if len(seeds) < self.min_samples:
                self.labels[i] = -1
            else:
                self.labels[i] = cluster_id
                while seeds:
                    new_idx = seeds.pop(0)
                    if self.labels[new_idx] == 0:
                        self.labels[new_idx] = cluster_id
                        new_seeds = region_query(new_idx)
                        if len(new_seeds) >= self.min_samples:
                            seeds.extend(new_seeds)
                    elif self.labels[new_idx] == -1:
                        self.labels[new_idx] = cluster_id
                cluster_id += 1
    
    def predict(self, X):
        return self.labels


if __name__ == "__main__":
    random.seed(42)
    X = []
    centers = [(0, 0), (5, 5), (10, 2)]
    for i, center in enumerate(centers):
        for _ in range(20):
            X.append([center[0] + random.gauss(0, 0.8), center[1] + random.gauss(0, 0.8)])
    
    print("=== K-Means ===")
    kmeans = KMeans(k=3, max_iter=50)
    kmeans.fit(X)
    print("中心點: " + str([[round(c, 2) for c in centroids] for centroids in kmeans.centroids]))
    print("聚類分佈: " + str(Counter(kmeans.labels)))
    print("測試點 [0,0]: 類別 " + str(kmeans.predict([[0, 0]])[0]))
    print("測試點 [5,5]: 類別 " + str(kmeans.predict([[5, 5]])[0]))
    print("測試點 [10,2]: 類別 " + str(kmeans.predict([[10, 2]])[0]))
    
    print("\n=== DBSCAN ===")
    dbscan = DBSCAN(eps=1.2, min_samples=3)
    dbscan.fit(X)
    n_clusters = len(set(dbscan.labels)) - (1 if -1 in dbscan.labels else 0)
    n_noise = list(dbscan.labels).count(-1)
    print("發現的聚類數: " + str(n_clusters))
    print("噪音點數: " + str(n_noise))
