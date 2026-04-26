import random
import math
import sys

def generate_blobs(n_samples, n_clusters, seed=42):
    random.seed(seed)
    centers = [(random.uniform(0, 10), random.uniform(0, 10)) for _ in range(n_clusters)]
    blobs = []
    for cx, cy in centers:
        for _ in range(n_samples // n_clusters):
            x = cx + random.gauss(0, 1.5)
            y = cy + random.gauss(0, 1.5)
            blobs.append((x, y))
    random.shuffle(blobs)
    return blobs, centers

def distance(p1, p2):
    return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

def kmeans(data, k, max_iters=100):
    centroids = random.sample(data, k)
    assignments = [-1] * len(data)

    for _ in range(max_iters):
        new_centroids = [(0, 0)] * k
        counts = [0] * k

        for i, point in enumerate(data):
            dists = [distance(point, c) for c in centroids]
            assignments[i] = dists.index(min(dists))

        for i, point in enumerate(data):
            c = assignments[i]
            new_centroids = list(new_centroids)
            new_centroids[c] = (new_centroids[c][0] + point[0], new_centroids[c][1] + point[1])
            counts[c] += 1

        for i in range(k):
            if counts[i] > 0:
                new_centroids[i] = (new_centroids[i][0] / counts[i], new_centroids[i][1] / counts[i])
            else:
                new_centroids[i] = centroids[i]

        if new_centroids == centroids:
            break
        centroids = new_centroids

    return centroids, assignments

def main():
    n_samples = 300
    n_clusters = 4
    data, true_centers = generate_blobs(n_samples, n_clusters)

    print(f"Generated {len(data)} blobs with {n_clusters} clusters")
    print(f"True centers: {true_centers}")

    centroids, assignments = kmeans(data, n_clusters)

    print(f"\nK-means converged:")
    print(f"Cluster centers: {centroids}")

    for i, (cx, cy) in enumerate(centroids):
        count = assignments.count(i)
        print(f"  Cluster {i}: {count} points")

    import matplotlib.pyplot as plt

    colors = ['red', 'green', 'blue', 'orange', 'purple', 'cyan']
    for i, point in enumerate(data):
        plt.scatter(point[0], point[1], c=colors[assignments[i]], alpha=0.5, s=10)

    for i, (cx, cy) in enumerate(centroids):
        plt.scatter(cx, cy, c='black', marker='x', s=100, linewidths=2)

    plt.show()

if __name__ == '__main__':
    main()