import random
import math
import collections

def generate_class_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples):
        x = random.uniform(0, 10)
        if x < 5:
            label = 0 if x < 2.5 else 1
        else:
            label = 0 if x > 7.5 else 1
        data.append((x, label))
    random.shuffle(data)
    return data

def gini_impurity(labels):
    if not labels:
        return 0
    counts = collections.Counter(labels)
    impurity = 1
    for count in counts.values():
        p = count / len(labels)
        impurity -= p * p
    return impurity

def best_split(data):
    best_gain = -1
    best_feature = None
    best_threshold = None

    current_gini = gini_impurity([label for _, label in data])

    for x, label in data:
        threshold = x
        left = [l for v, l in data if v <= threshold]
        right = [l for v, l in data if v > threshold]

        if not left or not right:
            continue

        n = len(data)
        gain = current_gini - (len(left) / n) * gini_impurity(left) - (len(right) / n) * gini_impurity(right)

        if gain > best_gain:
            best_gain = gain
            best_feature = 0
            best_threshold = threshold

    return best_feature, best_threshold

class Node:
    def __init__(self, threshold=None, left=None, right=None, label=None):
        self.threshold = threshold
        self.left = left
        self.right = right
        self.label = label

def build_tree(data, depth=0, max_depth=5):
    labels = [label for _, label in data]

    if len(set(labels)) == 1 or depth >= max_depth:
        return Node(label=collections.Counter(labels).most_common(1)[0][0])

    feature, threshold = best_split(data)
    if threshold is None:
        return Node(label=collections.Counter(labels).most_common(1)[0][0])

    left_data = [d for d in data if d[0] <= threshold]
    right_data = [d for d in data if d[0] > threshold]

    return Node(
        threshold=threshold,
        left=build_tree(left_data, depth + 1, max_depth),
        right=build_tree(right_data, depth + 1, max_depth),
        label=None
    )

def predict(node, x):
    if node.label is not None:
        return node.label
    if x <= node.threshold:
        return predict(node.left, x)
    return predict(node.right, x)

def draw_tree(node, x, y, dx, ax):
    import matplotlib.pyplot as plt

    if node.label is not None:
        color = 'lightgreen' if node.label == 0 else 'lightblue'
        ax.add_patch(plt.Rectangle((x - dx/2, y - 0.2), dx, 0.4, facecolor=color, edgecolor='black'))
        ax.text(x, y, f'Class {node.label}', ha='center', va='center', fontsize=8)
        return y

    ax.text(x, y + 0.25, f'x <= {node.threshold:.2f}', ha='center', va='bottom', fontsize=8)

    dx_child = dx / 2
    y_left = draw_tree(node.left, x - dx_child, y - 1, dx_child, ax)
    y_right = draw_tree(node.right, x + dx_child, y - 1, dx_child, ax)

    ax.plot([x, x - dx_child], [y, y_left], 'k-', lw=1)
    ax.plot([x, x + dx_child], [y, y_right], 'k-', lw=1)

    return y

def main():
    n_samples = 200
    train_data = generate_class_data(n_samples)
    test_data = generate_class_data(100, seed=99)

    tree = build_tree(train_data, max_depth=5)

    correct = sum(predict(tree, x) == label for x, label in test_data)
    accuracy = correct / len(test_data) * 100
    print(f"Decision Tree")
    print(f" Accuracy: {accuracy:.1f}%")

    import matplotlib.pyplot as plt
    fig, ax = plt.subplots(1, 1, figsize=(12, 6))

    draw_tree(tree, 0, 0, 8, ax)

    ax.set_xlim(-10, 10)
    ax.set_ylim(-6, 2)
    ax.axis('off')
    ax.set_title('Decision Tree')
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    main()