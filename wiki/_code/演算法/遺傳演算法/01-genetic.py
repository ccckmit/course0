"""
遺傳演算法範例
展示遺傳演算法的實現和應用
"""

import random
import math
from typing import List, Tuple, Callable


class GeneticAlgorithm:
    """遺傳演算法"""
    
    def __init__(self, pop_size: int = 100, mutation_rate: float = 0.01,
                 crossover_rate: float = 0.8):
        self.pop_size = pop_size
        self.mutation_rate = mutation_rate
        self.crossover_rate = crossover_rate
    
    def create_population(self, gene_length: int) -> List[List[int]]:
        """建立初始族群"""
        return [[random.randint(0, 1) for _ in range(gene_length)] 
                for _ in range(self.pop_size)]
    
    def fitness(self, individual: List[int], target_func: Callable) -> float:
        """適應度函數"""
        x = self._decode(individual)
        return 1 / (1 + abs(target_func(x)))
    
    def _decode(self, individual: List[int]) -> float:
        """解碼基因為數值"""
        binary = ''.join(map(str, individual))
        return int(binary, 2) / (2 ** len(individual) - 1) * 10 - 5
    
    def selection(self, population: List[List[int]], fitnesses: List[float]) -> List[List[int]]:
        """選擇 (輪盤式)"""
        total_fitness = sum(fitnesses)
        probabilities = [f / total_fitness for f in fitnesses]
        
        selected = []
        for _ in range(self.pop_size):
            r = random.random()
            cumulative = 0
            for i, p in enumerate(probabilities):
                cumulative += p
                if cumulative >= r:
                    selected.append(population[i].copy())
                    break
        return selected
    
    def crossover(self, parent1: List[int], parent2: List[int]) -> Tuple[List[int], List[int]]:
        """交配"""
        if random.random() < self.crossover_rate:
            point = random.randint(1, len(parent1) - 1)
            child1 = parent1[:point] + parent2[point:]
            child2 = parent2[:point] + parent1[point:]
            return child1, child2
        return parent1.copy(), parent2.copy()
    
    def mutate(self, individual: List[int]) -> List[int]:
        """突變"""
        mutated = individual.copy()
        
        for i in range(len(mutated)):
            if random.random() < self.mutation_rate:
                mutated[i] = 1 - mutated[i]
        
        return mutated
    
    def evolve(self, target_func: Callable, gene_length: int, 
               generations: int) -> Tuple[float, List[int]]:
        """演化"""
        population = self.create_population(gene_length)
        best_individual = None
        best_fitness = -float('inf')
        
        for gen in range(generations):
            fitnesses = [self.fitness(ind, target_func) for ind in population]
            
            best_idx = fitnesses.index(max(fitnesses))
            if fitnesses[best_idx] > best_fitness:
                best_fitness = fitnesses[best_idx]
                best_individual = population[best_idx]
            
            if best_fitness > 0.9999:
                break
            
            selected = self.selection(population, fitnesses)
            
            new_population = []
            while len(new_population) < self.pop_size:
                p1, p2 = random.sample(selected, 2)
                c1, c2 = self.crossover(p1, p2)
                new_population.append(self.mutate(c1))
                if len(new_population) < self.pop_size:
                    new_population.append(self.mutate(c2))
            
            population = new_population[:self.pop_size]
        
        return best_fitness, best_individual


def target_function(x: float):
    """目標函數: (x-2)²"""
    return (x - 2) ** 2


def demo_basic_ga():
    """基本遺傳演算法"""
    print("=" * 50)
    print("1. 基本遺傳演算法")
    print("=" * 50)
    
    ga = GeneticAlgorithm(pop_size=50, mutation_rate=0.01)
    
    best_fitness, best = ga.evolve(target_function, gene_length=16, generations=100)
    
    x_value = ga._decode(best)
    
    print(f"\n目標: 最小化 (x-2)²")
    print(f"最佳解: x = {x_value:.4f}")
    print(f"最佳適應度: {best_fitness:.4f}")


def demo_selection():
    """選擇機制"""
    print("\n" + "=" * 50)
    print("2. 選擇機制")
    print("=" * 50)
    
    population = [[0, 0], [0, 1], [1, 0], [1, 1]]
    fitnesses = [0.1, 0.3, 0.5, 0.8]
    
    ga = GeneticAlgorithm()
    selected = ga.selection(population, fitnesses)
    
    print(f"\n族群: {population}")
    print(f"適應度: {fitnesses}")
    print(f"選擇結果: {selected}")


def demo_crossover():
    """交配"""
    print("\n" + "=" * 50)
    print("3. 交配")
    print("=" * 50)
    
    ga = GeneticAlgorithm()
    
    p1 = [0, 0, 0, 0, 1, 1, 1, 1]
    p2 = [1, 1, 1, 1, 0, 0, 0, 0]
    
    c1, c2 = ga.crossover(p1, p2)
    
    print(f"\n父母:")
    print(f"  P1: {p1}")
    print(f"  P2: {p2}")
    print(f"子代:")
    print(f"  C1: {c1}")
    print(f"  C2: {c2}")


def demo_mutation():
    """突變"""
    print("\n" + "=" * 50)
    print("4. 突變")
    print("=" * 50)
    
    ga = GeneticAlgorithm(mutation_rate=0.1)
    
    individual = [0, 0, 0, 0, 0, 0, 0, 0]
    
    print(f"\n原始: {individual}")
    
    mutated = []
    for _ in range(10):
        mutated.append(ga.mutate(individual))
    
    print(f"突變後 (10次):")
    for m in mutated:
        print(f"  {m}")


def demo_tsp():
    """旅行商人問題"""
    print("\n" + "=" * 50)
    print("5. 旅行商人問題 (TSP)")
    print("=" * 50)
    
    cities = [(0, 0), (1, 3), (4, 2), (5, 5), (2, 1)]
    
    def distance(path: List[int]) -> float:
        dist = 0
        for i in range(len(path) - 1):
            x1, y1 = cities[path[i]]
            x2, y2 = cities[path[i + 1]]
            dist += math.sqrt((x2-x1)**2 + (y2-y1)**2)
        return dist
    
    def fitness(path: List[int]) -> float:
        return 1 / (1 + distance(path))
    
    pop_size = 100
    n_cities = len(cities)
    population = [random.sample(range(n_cities), n_cities) for _ in range(pop_size)]
    
    for gen in range(50):
        fitnesses = [fitness(p) for p in population]
        
        new_pop = []
        for _ in range(pop_size):
            p1, p2 = random.sample(population, 2)
            child = p1[:n_cities//2]
            for gene in p2:
                if gene not in child:
                    child.append(gene)
            new_pop.append(child)
        
        population = new_pop
    
    best_path = min(population, key=distance)
    best_dist = distance(best_path)
    
    print(f"\n城市: {cities}")
    print(f"最佳路徑: {best_path}")
    print(f"總距離: {best_dist:.2f}")


def demo_convergence():
    """收斂過程"""
    print("\n" + "=" * 50)
    print("6. 收斂過程")
    print("=" * 50)
    
    ga = GeneticAlgorithm(pop_size=30)
    
    best_fitnesses = []
    
    for gen in range(20):
        best_fitness, best = ga.evolve(target_function, gene_length=10, generations=gen+1)
        best_fitnesses.append(best_fitness)
    
    print(f"\n適應度變化:")
    for i, f in enumerate(best_fitnesses):
        print(f"  Generation {i+1}: {f:.4f}")


if __name__ == "__main__":
    random.seed(42)
    
    print("遺傳演算法 Python 範例")
    print("=" * 50)
    
    demo_basic_ga()
    demo_selection()
    demo_crossover()
    demo_mutation()
    demo_tsp()
    demo_convergence()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
