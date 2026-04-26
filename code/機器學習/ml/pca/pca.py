import random
import math

def generate_2d_data(n_samples, seed=42):
    random.seed(seed)
    data = []
    angle = math.radians(30)
    cos_a, sin_a = math.cos(angle), math.sin(angle)
    for _ in range(n_samples):
        x1 = random.gauss(0, 3)
        x2 = random.gauss(0, 1)
        x = x1 * cos_a - x2 * sin_a + 5
        y = x1 * sin_a + x2 * cos_a + 3
        data.append((x, y))
    return data

def mean(values):
    return sum(values) / len(values)

def covariance_matrix(data):
    n = len(data)
    xs = [p[0] for p in data]
    ys = [p[1] for p in data]
    x_mean = mean(xs)
    y_mean = mean(ys)

    var_xx = sum((x - x_mean) ** 2 for x in xs) / (n - 1)
    var_yy = sum((y - y_mean) ** 2 for y in ys) / (n - 1)
    var_xy = sum((xs[i] - x_mean) * (ys[i] - y_mean) for i in range(n)) / (n - 1)

    return [[var_xx, var_xy], [var_xy, var_yy]]

def eigenvalue(matrix):
    a, b, c, d = matrix[0][0], matrix[0][1], matrix[1][0], matrix[1][1]
    trace = a + d
    det = a * d - b * c
    l1 = trace / 2 + math.sqrt(trace * trace / 4 - det)
    l2 = trace / 2 - math.sqrt(trace / 2 * trace / 2 - det)
    return l1, l2

def eigenvector(matrix, eigenvalue):
    a, b = matrix[0][0], matrix[0][1]
    c, d = matrix[1][0], matrix[1][1]
    if abs(b) > 1e-10:
        v = (eigenvalue - d) / c
        norm = math.sqrt(v * v + 1)
        return (v / norm, 1 / norm)
    elif abs(c) > 1e-10:
        v = (eigenvalue - a) / c
        norm = math.sqrt(v * v + 1)
        return (v / norm, 1 / norm)
    else:
        return (1, 0) if eigenvalue > a else (0, 1)

def project(data, component, mean_x, mean_y):
    px, py = component
    result = []
    for x, y in data:
        proj = (x - mean_x) * px + (y - mean_y) * py
        result.append(proj)
    return result

def main():
    n_samples = 200
    data = generate_2d_data(n_samples)

    cov = covariance_matrix(data)
    print(f"Covariance matrix:")
    print(f"  [{cov[0][0]:.2f}, {cov[0][1]:.2f}]")
    print(f"  [{cov[1][0]:.2f}, {cov[1][1]:.2f}]")

    ev1, ev2 = eigenvalue(cov)
    print(f"\nEigenvalues: {ev1:.2f}, {ev2:.2f}")

    vec1 = eigenvector(cov, ev1)
    print(f"First principal component: ({vec1[0]:.3f}, {vec1[1]:.3f})")

    variance_explained = ev1 / (ev1 + ev2) * 100
    print(f"Variance explained: {variance_explained:.1f}%")

    xs = [p[0] for p in data]
    ys = [p[1] for p in data]
    x_mean = mean(xs)
    y_mean = mean(ys)

    import matplotlib.pyplot as plt

    plt.scatter(xs, ys, alpha=0.5, s=20)

    scale = 3
    plt.arrow(x_mean, y_mean, vec1[0] * scale * math.sqrt(ev1), vec1[1] * scale * math.sqrt(ev1),
            head_width=0.2, head_length=0.1, fc='red', ec='red', linewidth=2)

    plt.scatter([x_mean], [y_mean], c='black', s=50, marker='+')

    plt.axis('equal')
    plt.show()

if __name__ == '__main__':
    main()