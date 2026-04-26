#!/usr/bin/env python3
"""
回溯法 (Backtracking) -經典問題範例
展示深度優先搜索與剪枝策略
"""

def n_queens(n: int) -> list:
    """
    N 皇后問題
    
    找出所有可行的擺放方式
    """
    solutions = []
    
    def solve(row: int, cols: set, diag1: set, diag2: set, board: list):
        if row == n:
            solutions.append([''.join(row) for row in board])
            return
        
        for col in range(n):
            if col in cols or (row - col) in diag1 or (row + col) in diag2:
                continue
            
            board[row][col] = 'Q'
            solve(row + 1, 
                  cols | {col}, 
                  diag1 | {row - col}, 
                  diag2 | {row + col}, 
                  board)
            board[row][col] = '.'
    
    board = [['.' for _ in range(n)] for _ in range(n)]
    solve(0, set(), set(), set(), board)
    return solutions


def sudoku_solve(board: list) -> bool:
    """
    數獨求解
    
    使用回溯法求解數獨
    """
    for i in range(9):
        for j in range(9):
            if board[i][j] == 0:
                for num in range(1, 10):
                    if is_valid(board, i, j, num):
                        board[i][j] = num
                        if sudoku_solve(board):
                            return True
                        board[i][j] = 0
                return False
    return True


def is_valid(board: list, row: int, col: int, num: int) -> bool:
    """檢查數獨放置是否有效"""
    for i in range(9):
        if board[row][i] == num or board[i][col] == num:
            return False
    
    box_row, box_col = 3 * (row // 3), 3 * (col // 3)
    for i in range(box_row, box_row + 3):
        for j in range(box_col, box_col + 3):
            if board[i][j] == num:
                return False
    
    return True


def subset_sum_backtrack(nums: list, target: int) -> list:
    """
    子集和回溯版本
    
    找出所有和為 target 的子集
    """
    result = []
    
    def backtrack(start: int, current: list, remaining: int):
        if remaining == 0:
            result.append(current[:])
            return
        
        for i in range(start, len(nums)):
            if nums[i] <= remaining:
                current.append(nums[i])
                backtrack(i + 1, current, remaining - nums[i])
                current.pop()
    
    nums.sort()
    backtrack(0, [], target)
    return result


def permutation_backtrack(nums: list) -> list:
    """
    全排列回溯版本
    
    時間複雜度: O(n! * n)
    """
    result = []
    used = [False] * len(nums)
    
    def backtrack(current: list):
        if len(current) == len(nums):
            result.append(current[:])
            return
        
        for i in range(len(nums)):
            if not used[i]:
                used[i] = True
                current.append(nums[i])
                backtrack(current)
                current.pop()
                used[i] = False
    
    backtrack([])
    return result


def combination_sum(candidates: list, target: int) -> list:
    """
    組合總和
    
    找出所有和為 target 的組合（元素可重複使用）
    """
    result = []
    candidates.sort()
    
    def backtrack(start: int, current: list, remaining: int):
        if remaining == 0:
            result.append(current[:])
            return
        
        for i in range(start, len(candidates)):
            if candidates[i] > remaining:
                break
            current.append(candidates[i])
            backtrack(i, current, remaining - candidates[i])
            current.pop()
    
    backtrack(0, [], target)
    return result


def graph_coloring(graph: dict, m: int) -> dict:
    """
    圖著色問題
    
    使用 m 種顏色著色
    """
    colors = {}
    vertices = list(graph.keys())
    
    def is_safe(v: int, color: int) -> bool:
        for neighbor in graph.get(v, []):
            if colors.get(neighbor) == color:
                return False
        return True
    
    def backtrack(v: int) -> bool:
        if v == len(vertices):
            return True
        
        for color in range(m):
            if is_safe(vertices[v], color):
                colors[vertices[v]] = color
                if backtrack(v + 1):
                    return True
                del colors[vertices[v]]
        return False
    
    backtrack(0)
    return colors


def word_search(board: list, word: str) -> bool:
    """
    单词搜索
    
    在二維網格中找單詞路徑
    """
    if not board or not board[0]:
        return False
    
    rows, cols = len(board), len(board[0])
    
    def dfs(r: int, c: int, index: int) -> bool:
        if index == len(word):
            return True
        
        if (r < 0 or r >= rows or c < 0 or c >= cols or 
            board[r][c] != word[index]):
            return False
        
        temp = board[r][c]
        board[r][c] = '#'
        
        found = (dfs(r+1, c, index+1) or 
                 dfs(r-1, c, index+1) or 
                 dfs(r, c+1, index+1) or 
                 dfs(r, c-1, index+1))
        
        board[r][c] = temp
        return found
    
    for i in range(rows):
        for j in range(cols):
            if dfs(i, j, 0):
                return True
    
    return False


def solve_maze(maze: list, start: tuple, end: tuple) -> list:
    """
    迷宮求解
    
    找出從起點到終點的路徑
    """
    rows, cols = len(maze), len(maze[0])
    path = []
    
    def is_valid(r: int, c: int) -> bool:
        return (0 <= r < rows and 0 <= c < cols and 
                maze[r][c] != 1)
    
    def dfs(r: int, c: int) -> bool:
        if (r, c) == end:
            path.append((r, c))
            return True
        
        maze[r][c] = 1  # 標記已訪問
        path.append((r, c))
        
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]
        for dr, dc in directions:
            nr, nc = r + dr, c + dc
            if is_valid(nr, nc):
                if dfs(nr, nc):
                    return True
        
        path.pop()
        return False
    
    if dfs(*start):
        return path
    return []


def generate_parentheses(n: int) -> list:
    """
    括號生成
    
    產生 n 對括號的所有有效組合
    """
    result = []
    
    def backtrack(current: list, open: int, close: int):
        if len(current) == 2 * n:
            result.append(''.join(current))
            return
        
        if open < n:
            current.append('(')
            backtrack(current, open + 1, close)
            current.pop()
        
        if close < open:
            current.append(')')
            backtrack(current, open, close + 1)
            current.pop()
    
    backtrack([], 0, 0)
    return result


if __name__ == "__main__":
    # 測試 N 皇后
    solutions = n_queens(4)
    print(f"4皇后解數: {len(solutions)}")
    
    # 測試數獨
    board = [
        [5, 3, 0, 0, 7, 0, 0, 0, 0],
        [6, 0, 0, 1, 9, 5, 0, 0, 0],
        [0, 9, 8, 0, 0, 0, 0, 6, 0],
        [8, 0, 0, 0, 6, 0, 0, 0, 3],
        [4, 0, 0, 8, 0, 3, 0, 0, 1],
        [7, 0, 0, 0, 2, 0, 0, 0, 6],
        [0, 6, 0, 0, 0, 0, 2, 8, 0],
        [0, 0, 0, 4, 1, 9, 0, 0, 5],
        [0, 0, 0, 0, 8, 0, 0, 7, 9]
    ]
    sudoku_solve(board)
    print("數獨解答:")
    for row in board:
        print(row)
    
    # 測試括號生成
    print(f"3對括號: {generate_parentheses(3)}")
