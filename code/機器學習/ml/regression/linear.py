import random
import math

def generate_linear_data(n_samples, slope=2.5, intercept=1.0, noise=0.5, seed=42):
    random.seed(seed)
    data = []
    for _ in range(n_samples):
        x = random.uniform(0, 10)
        y = slope * x + intercept + random.gauss(0, noise)
        data.append((x, y))
    return data

def mean(values):
    return sum(values) / len(values)

def linear_regression(data):
    xs = [p[0] for p in data]
    ys = [p[1] for p in data]
    n = len(data)

    x_mean = mean(xs)
    y_mean = mean(ys)

    numerator = sum((xs[i] - x_mean) * (ys[i] - y_mean) for i in range(n))
    denominator = sum((xs[i] - x_mean) ** 2 for i in range(n))

    slope = numerator / denominator
    intercept = y_mean - slope * x_mean

    return slope, intercept

def predict(x, slope, intercept):
    return slope * x + intercept

def mse(data, slope, intercept):
    errors = []
    for x, y in data:
        pred = predict(x, slope, intercept)
        errors.append((y - pred) ** 2)
    return mean(errors)

def r2_score(data, slope, intercept):
    ys = [p[1] for p in data]
    y_mean = mean(ys)
    ss_tot = sum((y - y_mean) ** 2 for y in ys)

    ss_res = sum((y - predict(x, slope, intercept)) ** 2 for x, y in data)
    return 1 - (ss_res / ss_tot)

def main():
    n_samples = 200
    true_slope = 2.5
    true_intercept = 1.0
    data = generate_linear_data(n_samples, true_slope, true_intercept)

    slope, intercept = linear_regression(data)

    print(f"True: y = {true_slope}x + {true_intercept}")
    print(f"Fit:  y = {slope:.3f}x + {intercept:.3f}")
    print(f"R² = {r2_score(data, slope, intercept):.3f}")
    print(f"MSE = {mse(data, slope, intercept):.3f}")

    import matplotlib.pyplot as plt

    xs = [p[0] for p in data]
    ys = [p[1] for p in data]
    plt.scatter(xs, ys, alpha=0.5, s=20)

    line_x = [0, 10]
    line_y = [predict(x, slope, intercept) for x in line_x]
    plt.plot(line_x, line_y, 'r-', linewidth=2)

    plt.show()

if __name__ == '__main__':
    main()