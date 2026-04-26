import random
import math

def generate_polynomial_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples):
        x = random.uniform(-3, 3)
        y = 0.5 * x ** 3 - 2 * x ** 2 + x + 1 + random.gauss(0, 2)
        data.append((x, y))
    return data

def poly_features(x, degree):
    return [x ** i for i in range(degree + 1)]

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

def polynomial_regression(data, degree):
    X = [poly_features(x, degree) for x, y in data]
    Y = [[y] for x, y in data]

    Xt = transpose(X)
    XtX = mat_mul(Xt, X)
    XtX_inv = mat_inv(XtX)
    XtY = mat_vec_mul(Xt, [y for x, y in data])
    weights = mat_vec_mul(XtX_inv, XtY)

    return weights

def predict(x, weights):
    return sum(w * x ** i for i, w in enumerate(weights))

def mse(data, weights):
    return sum((y - predict(x, weights)) ** 2 for x, y in data) / len(data)

def main():
    n_samples = 200
    degree = 3
    data = generate_polynomial_data(n_samples)

    weights = polynomial_regression(data, degree)

    print(f"Polynomial Regression (degree={degree})")
    print(f" Weights: {[f'{w:.2f}' for w in weights]}")
    print(f" MSE: {mse(data, weights):.2f}")

    import matplotlib.pyplot as plt

    xs = [x for x, y in data]
    ys = [y for x, y in data]
    plt.scatter(xs, ys, alpha=0.5, s=20)

    xs = sorted(xs)
    ys = [predict(x, weights) for x in xs]
    plt.plot(xs, ys, 'r-', linewidth=2)

    plt.show()

if __name__ == '__main__':
    main()