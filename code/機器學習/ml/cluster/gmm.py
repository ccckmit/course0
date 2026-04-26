import random
import math

def generate_clusters(n_samples, n_clusters, seed=42):
    random.seed(seed)
    centers = [(2, 2), (8, 8), (2, 8), (8, 2)]
    data = []
    for cx, cy in centers[:n_clusters]:
        for _ in range(n_samples // n_clusters):
            x = cx + random.gauss(0, 1)
            y = cy + random.gauss(0, 1)
            data.append((x, y))
    random.shuffle(data)
    return data

class GMM:
    def __init__(self, n_clusters, max_iter=100):
        self.n_clusters = n_clusters
        self.max_iter = max_iter
        self.means = []
        self.covars = []
        self.weights = []

    def init_params(self, data):
        indices = random.sample(range(len(data)), self.n_clusters)
        self.means = [list(data[i]) for i in indices]
        self.covars = [[[1, 0], [0, 1]] for _ in range(self.n_clusters)]
        self.weights = [1.0 / self.n_clusters] * self.n_clusters

    def gaussian(self, x, mean, cov):
        det = cov[0][0] * cov[1][1] - cov[0][1] * cov[1][0]
        inv = [[cov[1][1] / det, -cov[0][1] / det],
               [-cov[1][0] / det, cov[0][0] / det]]
        dx = x[0] - mean[0]
        dy = x[1] - mean[1]
        z = dx * (inv[0][0] * dx + inv[0][1] * dy) + dy * (inv[1][0] * dx + inv[1][1] * dy)
        return math.exp(-0.5 * z) / (2 * math.pi * math.sqrt(det))

    def fit(self, data):
        self.init_params(data)
        n = len(data)

        for _ in range(self.max_iter):
            responsibilities = []
            for x in data:
                probs = [self.weights[k] * self.gaussian(x, self.means[k], self.covars[k])
                           for k in range(self.n_clusters)]
                total = sum(probs)
                responsibilities.append([p / total for p in probs])

            for k in range(self.n_clusters):
                Nk = sum(r[k] for r in responsibilities)
                self.weights[k] = Nk / n

                new_mean = [0, 0]
                for i, x in enumerate(data):
                    new_mean[0] += responsibilities[i][k] * x[0]
                    new_mean[1] += responsibilities[i][k] * x[1]
                self.means[k] = [new_mean[0] / Nk, new_mean[1] / Nk]

                new_cov = [[0, 0], [0, 0]]
                for i, x in enumerate(data):
                    dx = x[0] - self.means[k][0]
                    dy = x[1] - self.means[k][1]
                    new_cov[0][0] += responsibilities[i][k] * dx * dx
                    new_cov[0][1] += responsibilities[i][k] * dx * dy
                    new_cov[1][0] += responsibilities[i][k] * dy * dx
                    new_cov[1][1] += responsibilities[i][k] * dy * dy
                self.covars[k] = [[new_cov[0][0] / Nk, new_cov[0][1] / Nk],
                                 [new_cov[1][0] / Nk, new_cov[1][1] / Nk]]

    def predict(self, x):
        probs = [self.weights[k] * self.gaussian(x, self.means[k], self.covars[k])
                for k in range(self.n_clusters)]
        return probs.index(max(probs))

def main():
    n_samples = 200
    n_clusters = 4
    data = generate_clusters(n_samples, n_clusters)

    gmm = GMM(n_clusters)
    gmm.fit(data)

    labels = [gmm.predict(x) for x in data]

    print(f"GMM (Gaussian Mixture)")
    print(f" Clusters: {n_clusters}")

    import matplotlib.pyplot as plt
    colors = ['red', 'green', 'blue', 'orange', 'purple']

    for i, point in enumerate(data):
        plt.scatter(point[0], point[1], c=colors[labels[i]], alpha=0.5, s=20)

    for k, mean in enumerate(gmm.means):
        plt.scatter(mean[0], mean[1], c='black', marker='x', s=100)

    plt.show()

if __name__ == '__main__':
    main()