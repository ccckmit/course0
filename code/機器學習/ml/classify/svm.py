import random
import math

def generate_class_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples // 2):
        data.append((random.uniform(0, 4), random.uniform(0, 4), 1))
        data.append((random.uniform(6, 10), random.uniform(6, 10), -1))
    random.shuffle(data)
    return data

def predict_svm(x, y, w, b):
    return w[0] * x + w[1] * y + b

def train_svm(data, lr=0.01, epochs=1000, C=1.0):
    w = [0, 0]
    b = 0

    for _ in range(epochs):
        for x, y, label in data:
            val = label * (predict_svm(x, y, w, b))
            if val >= 1:
                w[0] -= lr * w[0]
                w[1] -= lr * w[1]
            else:
                w[0] -= lr * (w[0] - C * label * x)
                w[1] -= lr * (w[1] - C * label * y)
                b -= lr * (-C * label)

    return w, b

def predict(x, y, w, b):
    return 1 if predict_svm(x, y, w, b) >= 0 else -1

def main():
    n_samples = 200
    train_data = generate_class_data(n_samples)
    test_data = generate_class_data(100, seed=99)

    w, b = train_svm(train_data)

    print(f"SVM (Linear)")
    print(f" Weights: {w}, b: {b}")

    correct = sum(predict(x, y, w, b) == label for x, y, label in test_data)
    accuracy = correct / len(test_data) * 100
    print(f" Accuracy: {accuracy:.1f}%")

    import matplotlib.pyplot as plt
    colors = {1: 'red', -1: 'blue'}

    for x, y, label in train_data:
        plt.scatter(x, y, c=colors[label], alpha=0.5, s=20)

    x_range = [0, 10]
    y1 = - (w[0] * x_range[0] + b) / w[1]
    y2 = - (w[0] * x_range[1] + b) / w[1]
    plt.plot(x_range, [y1, y2], 'g-', linewidth=2)

    margin = 1 / math.sqrt(w[0] ** 2 + w[1] ** 2)
    plt.plot(x_range, [y1 + margin, y2 + margin], 'g--', alpha=0.5)
    plt.plot(x_range, [y1 - margin, y2 - margin], 'g--', alpha=0.5)

    plt.show()

if __name__ == '__main__':
    main()