import random
import math
import collections

def generate_class_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples // 3):
        data.append((random.uniform(0, 3), random.uniform(0, 3), 0))
        data.append((random.uniform(4, 7), random.uniform(4, 7), 1))
        data.append((random.uniform(7, 10), random.uniform(7, 10), 2))
    random.shuffle(data)
    return data

def distance(p1, p2):
    return math.sqrt((p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2)

def knn(train_data, test_point, k=5):
    dists = []
    for point in train_data:
        d = distance(test_point, (point[0], point[1]))
        dists.append((d, point[2]))
    dists.sort(key=lambda x: x[0])
    neighbors = [label for _, label in dists[:k]]
    return collections.Counter(neighbors).most_common(1)[0][0]

def split_data(data, test_ratio=0.2):
    random.shuffle(data)
    n_test = int(len(data) * test_ratio)
    return data[n_test:], data[:n_test]

def main():
    n_samples = 300
    train_data = generate_class_data(n_samples)
    train_set, test_set = split_data(train_data, 0.2)

    print(f"Training set: {len(train_set)} samples")
    print(f"Test set: {len(test_set)} samples")

    correct = 0
    for x, y, label in test_set:
        pred = knn(train_set, (x, y), k=5)
        if pred == label:
            correct += 1

    accuracy = correct / len(test_set) * 100
    print(f"\nK=5 accuracy: {accuracy:.1f}%")

    import matplotlib.pyplot as plt
    colors = ['red', 'green', 'blue']
    markers = ['o', 's', '^']

    for x, y, label in train_set:
        plt.scatter(x, y, c=colors[label], marker=markers[label], alpha=0.5, s=30)

    for x, y, label in test_set:
        pred = knn(train_set, (x, y), k=5)
        plt.scatter(x, y, c=colors[pred], marker='x', s=80, linewidths=2)

    plt.show()

if __name__ == '__main__':
    main()