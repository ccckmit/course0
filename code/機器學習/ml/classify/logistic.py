import random
import math
import collections

def generate_class_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples // 2):
        data.append((random.uniform(0, 4), random.uniform(0, 4), 0))
        data.append((random.uniform(6, 10), random.uniform(6, 10), 1))
    random.shuffle(data)
    return data

def sigmoid(z):
    return 1 / (1 + math.exp(-z))

def predict_prob(x, y, weights):
    z = weights[0] + weights[1] * x + weights[2] * y
    return sigmoid(z)

def predict(x, y, weights):
    return 1 if predict_prob(x, y, weights) >= 0.5 else 0

def train_logistic(data, lr=0.1, epochs=1000):
    weights = [0, 0, 0]
    n = len(data)

    for _ in range(epochs):
        for x, y, label in data:
            prob = predict_prob(x, y, weights)
            error = label - prob
            weights[0] += lr * error
            weights[1] += lr * error * x
            weights[2] += lr * error * y

    return weights

def main():
    n_samples = 200
    train_data = generate_class_data(n_samples)
    test_data = generate_class_data(100, seed=99)

    weights = train_logistic(train_data)
    print(f"Logistic Regression")
    print(f" Weights: {weights}")

    correct = sum(predict(x, y, w) == label for x, y, label, w in 
                [(d[0], d[1], d[2], weights) for d in test_data])
    accuracy = correct / len(test_data) * 100
    print(f" Accuracy: {accuracy:.1f}%")

    import matplotlib.pyplot as plt
    colors = ['red', 'blue']

    for x, y, label in train_data:
        plt.scatter(x, y, c=colors[label], alpha=0.5, s=20)

    x_range = [0, 10]
    y1 = - (weights[0] + weights[1] * x_range[0]) / weights[2]
    y2 = - (weights[0] + weights[1] * x_range[1]) / weights[2]
    plt.plot(x_range, [y1, y2], 'g-', linewidth=2)

    plt.show()

if __name__ == '__main__':
    main()