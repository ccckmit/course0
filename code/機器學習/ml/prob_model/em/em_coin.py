def em_coin_toss(observations, theta_A_init, theta_B_init, max_iter=10, tol=1e-6):
    """
    雙硬幣問題的 EM 演算法實作
    
    :param observations: 觀測數據，格式為 [(正面次數, 反面次數), ...]
    :param theta_A_init: 硬幣 A 擲出正面的初始機率猜測
    :param theta_B_init: 硬幣 B 擲出正面的初始機率猜測
    :param max_iter: 最大迭代次數
    :param tol: 收斂的容忍度 (當參數變化小於此值時停止)
    """
    
    # 載入初始猜測值
    theta_A = theta_A_init
    theta_B = theta_B_init
    
    print(f"【初始猜測】 Theta A: {theta_A:.4f}, Theta B: {theta_B:.4f}\n")
    print("-" * 50)
    
    for iteration in range(max_iter):
        # --- E 步驟 (Expectation) ---
        # 準備用來收集這一輪「期望」出現的正反面總數
        expected_heads_A, expected_tails_A = 0.0, 0.0
        expected_heads_B, expected_tails_B = 0.0, 0.0
        
        for heads, tails in observations:
            # 1. 計算在當前參數下，產生這組數據的機率 (Likelihood)
            # 備註：嚴格來說前面應該乘上二項式係數 C(10, heads)，
            # 但因為在計算權重時會被分子分母約掉，所以這裡為了簡化直接省略。
            likelihood_A = (theta_A ** heads) * ((1 - theta_A) ** tails)
            likelihood_B = (theta_B ** heads) * ((1 - theta_B) ** tails)
            
            # 2. 計算這組數據來自硬幣 A 和硬幣 B 的「機率權重」 (Soft assignment)
            weight_A = likelihood_A / (likelihood_A + likelihood_B)
            weight_B = likelihood_B / (likelihood_A + likelihood_B)
            
            # 3. 根據權重，把這組數據的正反面次數「分配」給 A 和 B
            expected_heads_A += weight_A * heads
            expected_tails_A += weight_A * tails
            
            expected_heads_B += weight_B * heads
            expected_tails_B += weight_B * tails
            
        # --- M 步驟 (Maximization) ---
        # 紀錄舊的參數用來判斷是否收斂
        old_theta_A, old_theta_B = theta_A, theta_B
        
        # 根據剛剛算出的期望總數，重新更新 Theta A 和 Theta B
        theta_A = expected_heads_A / (expected_heads_A + expected_tails_A)
        theta_B = expected_heads_B / (expected_heads_B + expected_tails_B)
        
        print(f"第 {iteration + 1} 次迭代:")
        print(f"  Theta A 更新為: {theta_A:.4f}")
        print(f"  Theta B 更新為: {theta_B:.4f}")
        
        # --- 檢查是否收斂 ---
        if abs(theta_A - old_theta_A) < tol and abs(theta_B - old_theta_B) < tol:
            print("-" * 50)
            print(f"演算法在第 {iteration + 1} 次迭代達到收斂！")
            break
            
    return theta_A, theta_B

# ==========================================
# 測試數據與執行
# ==========================================
if __name__ == "__main__":
    # 這是經典論文中使用的 5 輪拋擲數據，每輪拋 10 次
    # 格式: (正面次數, 反面次數)
    data = [
        (5, 5),  # 第 1 輪
        (9, 1),  # 第 2 輪
        (8, 2),  # 第 3 輪
        (4, 6),  # 第 4 輪
        (7, 3)   # 第 5 輪
    ]
    
    # 初始猜測：假設硬幣 A 正面機率 0.6，硬幣 B 正面機率 0.5
    final_theta_A, final_theta_B = em_coin_toss(data, theta_A_init=0.6, theta_B_init=0.5)
    
    print("\n【最終結果】")
    print(f"硬幣 A 擲出正面的機率約為: {final_theta_A:.4f}")
    print(f"硬幣 B 擲出正面的機率約為: {final_theta_B:.4f}")