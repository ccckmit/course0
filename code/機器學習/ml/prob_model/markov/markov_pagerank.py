import numpy as np

class PageRank:
    def __init__(self, pages, links):
        self.pages = pages
        self.N = len(pages)
        self.links = links
        self.M = self._build_transition_matrix()
        
    def _build_transition_matrix(self):
        """
        建立馬可夫鏈的「轉移矩陣 (Transition Matrix)」。
        把超連結轉化為機率。
        """
        M = np.zeros((self.N, self.N))
        
        for j, page in enumerate(self.pages):
            outbound_links = self.links[page]
            num_links = len(outbound_links)
            
            # 如果這個網頁有對外連結，平均分配點擊機率
            if num_links > 0:
                for target in outbound_links:
                    i = self.pages.index(target)
                    M[i, j] = 1.0 / num_links
            else:
                # 如果是沒有對外連結的「死胡同 (Dead End)」，
                # 假設使用者會隨機跳到任何一個網頁
                M[:, j] = 1.0 / self.N
                
        return M

    def calculate(self, damping_factor=0.85, iterations=20):
        """
        利用馬可夫鏈的狀態轉移，計算最終的 PageRank。
        """
        # 初始狀態：假設隨機衝浪者一開始出現在每個網頁的機率是一樣的 (1/4 = 0.25)
        state_vector = np.ones(self.N) / self.N
        
        print("🚀 初始狀態 (第 0 步):")
        self._print_rank(state_vector)
        print("-" * 40)
        
        # 開始馬可夫鏈的轉移迭代
        for step in range(1, iterations + 1):
            # PageRank 公式：
            # 下一步的機率 = (隨機跳轉機率) + (點擊連結走過來的機率)
            random_jump = (1 - damping_factor) / self.N
            link_click = damping_factor * np.dot(self.M, state_vector)
            
            # 更新狀態
            state_vector = random_jump + link_click
            
            # 印出前 3 步與最後一步，觀察馬可夫鏈如何收斂
            if step <= 3 or step == iterations:
                print(f"👣 第 {step} 步狀態轉移後:")
                self._print_rank(state_vector)
                if step == 3 and iterations > 3:
                    print("...\n(馬可夫鏈持續運作中，直到狀態穩定)\n...")
                
        return state_vector

    def _print_rank(self, vector):
        for i, page in enumerate(self.pages):
            print(f"  網頁 {page}: {vector[i]:.4f} ({vector[i]*100:.1f}%)")


# ==========================================
# 1. 定義我們的迷你網際網路 (Graph)
# ==========================================
pages = ['A', 'B', 'C', 'D']

# 字典的 Key 是「來源網頁」，Value 是「它連向的網頁」
links = {
    'A': ['B', 'C', 'D'],
    'B': ['C', 'D'],
    'C': ['A'],
    'D': ['C']
}

# ==========================================
# 2. 執行 PageRank
# ==========================================
print(f"🌍 建立 {len(pages)} 個網頁的網路結構...")
pr_model = PageRank(pages, links)

print("\n🔍 檢視馬可夫鏈轉移矩陣 (直行代表從哪來，橫列代表往哪去):")
print(np.round(pr_model.M, 2))
print("-" * 40)

# 開始計算！d=0.85 是 Google 經典的阻尼係數
final_scores = pr_model.calculate(damping_factor=0.85, iterations=20)

print("-" * 40)
print("🏆 最終 PageRank 排名結果:")
# 將結果與網頁配對並排序
ranked_pages = sorted(zip(pages, final_scores), key=lambda x: x[1], reverse=True)
for rank, (page, score) in enumerate(ranked_pages, 1):
    print(f" 第 {rank} 名: 網頁 {page} (權重: {score:.4f})")