import random
import math

def generate_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples):
        x = random.uniform(0, 10)
        y = 2 * x + 3 + random.gauss(0, 1)
        data.append((x, y))
    return data

def lasso_fit(data, lambda_reg, max_iter=500):
    w0, w1 = 0.0, 0.0
    n = len(data)

    for _ in range(max_iter):
        r0 = sum(w1 * x for x, y in data)
        w0 = (sum(y for x, y in data) - r0) / n

        rho = sum(x * (y - w0) for x, y in data)
        if rho > lambda_reg:
            w1 = (rho - lambda_reg) / sum(x * x for x, y in data)
        elif rho < -lambda_reg:
            w1 = (rho + lambda_reg) / sum(x * x for x, y in data)
        else:
            w1 = 0

    return [w0, w1]

def predict(x, weights):
    return weights[0] + weights[1] * x

def mse(data, weights):
    return sum((y - predict(x, weights)) ** 2 for x, y in data) / len(data)

def main():
    n_samples = 200
    data = generate_data(n_samples)

    for lambda_reg in [0, 0.1, 1, 5]:
        weights = lasso_fit(data, lambda_reg)
        m = mse(data, weights)
        print(f"Lasso (lambda={lambda_reg}): weights={[f'{w:.2f}' for w in weights]}, MSE={m:.2f}")

    import matplotlib.pyplot as plt

    xs = [x for x, y in data]
    ys = [y for x, y in data]
    plt.scatter(xs, ys, alpha=0.5, s=20)

    weights = lasso_fit(data, 1)
    xs_line = [0, 10]
    ys_line = [predict(x, weights) for x in xs_line]
    plt.plot(xs_line, ys_line, 'r-', linewidth=2)

    plt.show()

if __name__ == '__main__':
    main()