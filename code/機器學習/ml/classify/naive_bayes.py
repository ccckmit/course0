import random
import math
import collections

def generate_class_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples // 2):
        data.append((random.gauss(3, 1), random.gauss(3, 1), 0))
        data.append((random.gauss(7, 1), random.gauss(7, 1), 1))
    random.shuffle(data)
    return data

class NaiveBayes:
    def __init__(self):
        self.means = {}
        self.vars = {}
        self.priors = {}

    def fit(self, data):
        labels = [label for _, _, label in data]
        n = len(labels)
        classes = set(labels)

        for c in classes:
            points = [(x, y) for x, y, label in data if label == c]
            self.means[c] = (sum(p[0] for p in points) / len(points),
                          sum(p[1] for p in points) / len(points))
            self.vars[c] = (sum((p[0] - self.means[c][0]) ** 2 for p in points) / len(points) + 0.01,
                        sum((p[1] - self.means[c][1]) ** 2 for p in points) / len(points) + 0.01)
            self.priors[c] = len(points) / n

    def gaussian(self, x, mean, var):
        return math.exp(-(x - mean) ** 2 / (2 * var)) / math.sqrt(2 * math.pi * var)

    def predict(self, x, y):
        posteriors = {}
        for c in self.means:
            prob = self.priors[c]
            prob *= self.gaussian(x, self.means[c][0], self.vars[c][0])
            prob *= self.gaussian(y, self.means[c][1], self.vars[c][1])
            posteriors[c] = prob
        return max(posteriors, key=posteriors.get)

def main():
    n_samples = 200
    train_data = generate_class_data(n_samples)
    test_data = generate_class_data(100, seed=99)

    model = NaiveBayes()
    model.fit(train_data)

    print(f"Naive Bayes (Gaussian)")
    print(f" Class 0: mean={model.means[0]}, var={model.vars[0]}")
    print(f" Class 1: mean={model.means[1]}, var={model.vars[1]}")

    correct = sum(model.predict(x, y) == label for x, y, label in test_data)
    accuracy = correct / len(test_data) * 100
    print(f" Accuracy: {accuracy:.1f}%")

    import matplotlib.pyplot as plt
    colors = ['red', 'blue']

    for x, y, label in train_data:
        plt.scatter(x, y, c=colors[label], alpha=0.5, s=20)

    for c, (mx, my) in model.means.items():
        plt.scatter(mx, my, c='black', marker='x', s=100)

    plt.show()

if __name__ == '__main__':
    main()