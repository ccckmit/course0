import random
import math

def generate_linear_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples):
        x = random.uniform(0, 10)
        y = 2 * x + 3 + random.gauss(0, 1)
        data.append((x, y))
    return data

def mean(values):
    return sum(values) / len(values)

def transpose(matrix):
    return list(map(list, zip(*matrix)))

def mat_mul(A, B):
    return [[sum(a * b for a, b in zip(row, col)) for col in transpose(B)] for row in A]

def mat_vec_mul(A, v):
    return [sum(a * b for a, b in zip(row, v)) for row in A]

def mat_inv(A):
    n = len(A)
    M = [row[:] + [1 if i == j else 0 for j in range(n)] for i, row in enumerate(A)]

    for i in range(n):
        pivot = M[i][i]
        if abs(pivot) < 1e-10:
            continue
        for j in range(2 * n):
            M[i][j] /= pivot
        for k in range(n):
            if k != i:
                factor = M[k][i]
                for j in range(2 * n):
                    M[k][j] -= factor * M[i][j]

    return [row[n:] for row in M]

def ridge_regression(data, lambda_reg):
    X = [[1, x] for x, y in data]
    Y = [y for x, y in data]

    Xt = transpose(X)
    XtX = mat_mul(Xt, X)

    for i in range(len(XtX)):
        XtX[i][i] += lambda_reg

    XtX_inv = mat_inv(XtX)
    XtY = mat_vec_mul(Xt, Y)
    weights = mat_vec_mul(XtX_inv, XtY)

    return weights

def predict(x, weights):
    return weights[0] + weights[1] * x

def mse(data, weights):
    return sum((y - predict(x, weights)) ** 2 for x, y in data) / len(data)

def main():
    n_samples = 200
    data = generate_linear_data(n_samples)

    for lambda_reg in [0, 0.1, 1, 10]:
        weights = ridge_regression(data, lambda_reg)
        m = mse(data, weights)
        print(f"Ridge (lambda={lambda_reg}): weights={[f'{w:.2f}' for w in weights]}, MSE={m:.2f}")

    import matplotlib.pyplot as plt

    xs = [x for x, y in data]
    ys = [y for x, y in data]
    plt.scatter(xs, ys, alpha=0.5, s=20)

    weights = ridge_regression(data, 1)
    xs_line = [0, 10]
    ys_line = [predict(x, weights) for x in xs_line]
    plt.plot(xs_line, ys_line, 'r-', linewidth=2)

    plt.show()

if __name__ == '__main__':
    main()