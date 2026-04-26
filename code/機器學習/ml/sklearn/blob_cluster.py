import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import make_blobs
from sklearn.cluster import KMeans

X, y_true = make_blobs(n_samples=300, centers=4, cluster_std=0.8, random_state=42)

kmeans = KMeans(n_clusters=4, random_state=42)
y_pred = kmeans.fit_predict(X)

plt.figure(figsize=(10, 5))

plt.subplot(1, 2, 1)
colors = plt.cm.tab10(np.linspace(0, 1, 4))
for i in range(4):
    mask = y_true == i
    plt.scatter(X[mask, 0], X[mask, 1], c=[colors[i]], label=f'Cluster {i}', s=40, alpha=0.7, edgecolors='k', linewidths=0.3)
plt.title('True Labels')
plt.legend()
plt.xlabel('Feature 1')
plt.ylabel('Feature 2')

plt.subplot(1, 2, 2)
for i in range(4):
    mask = y_pred == i
    plt.scatter(X[mask, 0], X[mask, 1], c=[colors[i]], label=f'Cluster {i}', s=40, alpha=0.7, edgecolors='k', linewidths=0.3)
plt.scatter(kmeans.cluster_centers_[:, 0], kmeans.cluster_centers_[:, 1], c='red', marker='X', s=150, edgecolors='k', linewidths=1, label='Centroids')
plt.title('KMeans Predicted')
plt.legend()
plt.xlabel('Feature 1')
plt.ylabel('Feature 2')

plt.tight_layout()
plt.show()