import sys
import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import make_blobs, make_moons, make_circles, make_classification
from sklearn.cluster import KMeans, DBSCAN, AgglomerativeClustering, SpectralClustering, MeanShift, Birch
from sklearn.mixture import GaussianMixture
from sklearn.metrics import silhouette_score, davies_bouldin_score, calinski_harabasz_score

DATASETS = {
    "1": ("make_blobs", lambda: make_blobs(n_samples=300, centers=3, cluster_std=1.0, random_state=42)),
    "2": ("make_moons", lambda: make_moons(n_samples=300, noise=0.1, random_state=42)),
    "3": ("make_circles", lambda: make_circles(n_samples=300, noise=0.05, factor=0.5, random_state=42)),
    "4": ("make_classification", lambda: make_classification(n_samples=300, n_features=2, n_informative=2, n_redundant=0, n_clusters_per_class=1, random_state=42)),
    "5": ("blobs (4 clusters)", lambda: make_blobs(n_samples=400, centers=4, cluster_std=0.8, random_state=42)),
    "6": ("blobs (anisotropic)", lambda: (np.dot(np.random.randn(300, 2), [[1, 0.5], [0.5, 1]]) + [0, 0], np.concatenate([np.zeros(100), np.ones(100), np.full(100, 2)]))),
}

CLUSTERS = {
    "1": ("KMeans", KMeans),
    "2": ("DBSCAN", DBSCAN),
    "3": ("AgglomerativeClustering", AgglomerativeClustering),
    "4": ("SpectralClustering", SpectralClustering),
    "5": ("GaussianMixture", GaussianMixture),
    "6": ("MeanShift", MeanShift),
    "7": ("Birch", Birch),
}

def evaluate(X, labels):
    unique_labels = set(labels)
    if len(unique_labels) < 2:
        print(f"\nClusters found: {len(unique_labels)}")
        return None
    if -1 in labels:
        n_noise = list(labels).count(-1)
        print(f"\nClusters: {len(unique_labels) - 1}, Noise: {n_noise}")
    else:
        print(f"\nClusters: {len(unique_labels)}")
    sil = silhouette_score(X, labels)
    dbi = davies_bouldin_score(X, labels)
    chs = calinski_harabasz_score(X, labels)
    print(f"Silhouette Score: {sil:.3f} (higher is better, -1 to 1)")
    print(f"Davies-Bouldin Index: {dbi:.3f} (lower is better)")
    print(f"Calinski-Harabasz Score: {chs:.3f} (higher is better)")
    return labels

def plot_clusters(X, labels, title):
    plt.figure(figsize=(8, 6))
    unique_labels = set(labels)
    colors = plt.cm.tab10(np.linspace(0, 1, max(len(unique_labels), 10)))
    
    for k, col in zip(sorted(unique_labels), colors):
        if k == -1:
            col = 'gray'
            marker = 'x'
            label = 'Noise'
        else:
            marker = 'o'
            label = f'Cluster {k}'
        
        mask = labels == k
        plt.scatter(X[mask, 0], X[mask, 1], c=[col], marker=marker, 
                   label=label, s=30, alpha=0.7, edgecolors='k', linewidths=0.3)
    
    plt.title(title)
    plt.xlabel('Feature 1')
    plt.ylabel('Feature 2')
    plt.legend()
    plt.tight_layout()
    plt.show()

def main():
    print("可用資料集:")
    for key, (name, _) in DATASETS.items():
        print(f"  {key}. {name}")

    if len(sys.argv) > 1:
        ds_choice = sys.argv[1]
    else:
        ds_choice = input("\n請選擇資料集: ").strip()

    if ds_choice not in DATASETS:
        print("無效的資料集選擇")
        return

    ds_name, ds_func = DATASETS[ds_choice]
    print(f"\n使用資料集: {ds_name}")

    print("\n可用聚類演算法:")
    for key, (name, _) in CLUSTERS.items():
        print(f"  {key}. {name}")

    if len(sys.argv) > 2:
        cl_choice = sys.argv[2]
    else:
        cl_choice = input("\n請選擇聚類演算法: ").strip()

    if cl_choice not in CLUSTERS:
        print("無效的聚類演算法選擇")
        return

    cl_name, cl_class = CLUSTERS[cl_choice]
    print(f"\n使用聚類演算法: {cl_name}")

    X, y_true = ds_func()
    n_clusters = len(set(y_true))

    if cl_choice == "1":
        cl = cl_class(n_clusters=n_clusters, random_state=42)
    elif cl_choice == "2":
        cl = cl_class(eps=0.3, min_samples=5)
    elif cl_choice == "3":
        cl = cl_class(n_clusters=n_clusters)
    elif cl_choice == "4":
        cl = cl_class(n_clusters=n_clusters, random_state=42)
    elif cl_choice == "5":
        cl = cl_class(n_components=n_clusters, random_state=42)
    elif cl_choice == "6":
        cl = cl_class()
    elif cl_choice == "7":
        cl = cl_class(n_clusters=n_clusters)

    labels = cl.fit_predict(X)
    evaluate(X, labels)
    plot_clusters(X, labels, f"{ds_name} + {cl_name}")

if __name__ == "__main__":
    main()