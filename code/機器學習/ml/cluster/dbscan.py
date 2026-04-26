import random
import math

def generate_clusters(n_samples, n_clusters, seed=42):
    random.seed(seed)
    centers = [(2, 2), (8, 8), (2, 8), (8, 2), (5, 5)]
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

def regions(data, point_idx, eps, visited):
    neighbors = []
    for j, point in enumerate(data):
        if j != point_idx and j not in visited:
            if distance(data[point_idx], point) <= eps:
                neighbors.append(j)
    return neighbors

def dbscan(data, eps=1.0, min_pts=3):
    labels = [-1] * len(data)
    cluster_id = 0

    for i in range(len(data)):
        if labels[i] != -1:
            continue

        visited = set()
        neighbors = regions(data, i, eps, visited)

        if len(neighbors) >= min_pts:
            labels[i] = cluster_id
            while neighbors:
                j = neighbors.pop()
                if labels[j] == -1:
                    labels[j] = cluster_id
                    new_neighbors = regions(data, j, eps, visited)
                    if len(new_neighbors) >= min_pts:
                        neighbors.extend(new_neighbors)
            cluster_id += 1

    return labels

def main():
    n_samples = 300
    n_clusters = 4
    data = generate_clusters(n_samples, n_clusters)

    labels = dbscan(data, eps=1.0, min_pts=3)

    n_clusters_found = len(set(labels)) - (1 if -1 in labels else 0)
    noise_points = labels.count(-1)
    print(f"DBSCAN")
    print(f" Clusters found: {n_clusters_found}")
    print(f" Noise points: {noise_points}")

    import matplotlib.pyplot as plt
    colors = ['gray', 'red', 'green', 'blue', 'orange', 'purple']

    for i, point in enumerate(data):
        color = colors[labels[i] + 1] if labels[i] >= 0 else 'gray'
        marker = 'x' if labels[i] == -1 else 'o'
        plt.scatter(point[0], point[1], c=color, marker=marker, alpha=0.5, s=20)

    plt.show()

if __name__ == '__main__':
    main()