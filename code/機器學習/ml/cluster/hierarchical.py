import random
import math

def generate_clusters(n_samples, n_clusters, seed=42):
    random.seed(seed)
    centers = [(2, 2), (8, 8), (2, 8), (8, 2)]
    data = []
    for cx, cy in centers[:n_clusters]:
        for _ in range(n_samples // n_clusters):
            x = cx + random.gauss(0, 0.8)
            y = cy + random.gauss(0, 0.8)
            data.append((x, y))
    random.shuffle(data)
    return data

def distance(p1, p2):
    return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

def linkage(data, n_clusters):
    clusters = [[i] for i in range(len(data))]
    distances = {}

    def dist(c1, c2):
        key = (tuple(sorted(c1)), tuple(sorted(c2)))
        if key not in distances:
            d = min(distance(data[i], data[j]) for i in c1 for j in c2)
            distances[key] = d
        return distances[key]

    while len(clusters) > n_clusters:
        min_dist = float('inf')
        merge = None

        for i in range(len(clusters)):
            for j in range(i + 1, len(clusters)):
                d = dist(clusters[i], clusters[j])
                if d < min_dist:
                    min_dist = d
                    merge = (i, j)

        i, j = merge
        clusters[i].extend(clusters[j])
        clusters.pop(j)

    labels = [-1] * len(data)
    for label, cluster in enumerate(clusters):
        for idx in cluster:
            labels[idx] = label

    return labels

def main():
    n_samples = 200
    n_clusters = 4
    data = generate_clusters(n_samples, n_clusters)

    labels = linkage(data, n_clusters)

    print(f"Hierarchical Clustering")
    print(f" Clusters: {n_clusters}")

    import matplotlib.pyplot as plt
    colors = ['red', 'green', 'blue', 'orange', 'purple']

    for i, point in enumerate(data):
        plt.scatter(point[0], point[1], c=colors[labels[i]], alpha=0.5, s=20)

    plt.show()

if __name__ == '__main__':
    main()