import numpy as np
from scipy.stats import norm
import matplotlib.pyplot as plt

def gmm_1d_em(X, mu_init, sigma_init, pi_init, max_iter=50, tol=1e-4):
    """
    1D 高斯混合模型的 EM 演算法實作
    
    :param X: 觀測數據 (1D numpy array)
    :param mu_init: 初始平均值猜測 (list)
    :param sigma_init: 初始標準差猜測 (list)
    :param pi_init: 初始群體比例猜測 (list)
    """
    mu = np.array(mu_init, dtype=float)
    sigma = np.array(sigma_init, dtype=float)
    pi = np.array(pi_init, dtype=float)
    N = len(X)
    
    print(f"【初始猜測】")
    print(f"群體 1 (女生) -> 平均: {mu[0]:.1f}, 標準差: {sigma[0]:.1f}, 比例: {pi[0]:.2f}")
    print(f"群體 2 (男生) -> 平均: {mu[1]:.1f}, 標準差: {sigma[1]:.1f}, 比例: {pi[1]:.2f}")
    print("-" * 60)

    log_likelihoods = []

    for iteration in range(max_iter):
        # ==========================================
        # E 步驟 (Expectation)
        # ==========================================
        # 1. 根據目前的 mu 和 sigma，算出每個數據點在常態分佈下的機率密度 (PDF)
        pdf_1 = norm.pdf(X, loc=mu[0], scale=sigma[0])
        pdf_2 = norm.pdf(X, loc=mu[1], scale=sigma[1])
        
        # 2. 乘上群體本身的比例 (這點來自哪個群體的初步機率)
        p_1 = pi[0] * pdf_1
        p_2 = pi[1] * pdf_2
        p_total = p_1 + p_2  # 總機率 (用於歸一化)
        
        # 3. 計算責任值 (Responsibilities, 也就是「機率權重」)
        # r1[i] 代表第 i 個人有多大的機率是女生
        r1 = p_1 / p_total
        r2 = p_2 / p_total
        
        # 紀錄目前的 Log-Likelihood (對數概似度，用來判斷模型有多貼合數據)
        log_likelihood = np.sum(np.log(p_total))
        log_likelihoods.append(log_likelihood)

        # ==========================================
        # M 步驟 (Maximization)
        # ==========================================
        # 1. 計算每個群體的「有效總人數」 (把機率相加)
        N1 = np.sum(r1)
        N2 = np.sum(r2)
        
        # 2. 重新更新平均值 mu (加權平均)
        new_mu_1 = np.sum(r1 * X) / N1
        new_mu_2 = np.sum(r2 * X) / N2
        
        # 3. 重新更新標準差 sigma (加權標準差)
        new_sigma_1 = np.sqrt(np.sum(r1 * (X - new_mu_1)**2) / N1)
        new_sigma_2 = np.sqrt(np.sum(r2 * (X - new_mu_2)**2) / N2)
        
        # 4. 重新更新群體比例 pi
        new_pi_1 = N1 / N
        new_pi_2 = N2 / N
        
        # 更新參數
        mu = np.array([new_mu_1, new_mu_2])
        sigma = np.array([new_sigma_1, new_sigma_2])
        pi = np.array([new_pi_1, new_pi_2])
        
        # ==========================================
        # 檢查收斂 (如果 Log-Likelihood 變化極小，則停止)
        # ==========================================
        if iteration > 0 and abs(log_likelihoods[-1] - log_likelihoods[-2]) < tol:
            print(f"\n演算法在第 {iteration + 1} 次迭代達到收斂！")
            break
            
        if iteration % 5 == 0:
            print(f"第 {iteration + 1:>2} 次迭代 -> Log-Likelihood: {log_likelihood:.2f}")

    print("-" * 60)
    print("【最終預測結果】")
    print(f"群體 1 (女生) -> 平均: {mu[0]:.1f}cm, 標準差: {sigma[0]:.2f}, 比例: {pi[0]:.2%}")
    print(f"群體 2 (男生) -> 平均: {mu[1]:.1f}cm, 標準差: {sigma[1]:.2f}, 比例: {pi[1]:.2%}")
    
    return mu, sigma, pi

# ==========================================
# 1. 產生模擬數據 (上帝視角：我們知道真實答案)
# ==========================================
np.random.seed(42)
# 假設真實女生：300人，平均 162cm，標準差 5cm
female_heights = np.random.normal(loc=162, scale=5, size=300)
# 假設真實男生：700人，平均 176cm，標準差 6cm
male_heights = np.random.normal(loc=176, scale=6, size=700)

# 將男女身高混合，並打亂順序 (剝奪標籤，變成未標記數據)
X_data = np.concatenate([female_heights, male_heights])
np.random.shuffle(X_data)

# ==========================================
# 2. 執行 EM 演算法
# ==========================================
# 我們故意給一個不太準的初始猜測
initial_mu = [155.0, 185.0]     # 猜測平均身高為 155 和 185
initial_sigma = [10.0, 10.0]    # 猜測標準差都很寬 (10)
initial_pi = [0.5, 0.5]         # 猜測男女各半

final_mu, final_sigma, final_pi = gmm_1d_em(
    X_data, 
    mu_init=initial_mu, 
    sigma_init=initial_sigma, 
    pi_init=initial_pi
)

# ==========================================
# 3. 視覺化結果 (畫圖)
# ==========================================
plt.figure(figsize=(10, 6))

# 畫出原始數據的直方圖 (背景)
plt.hist(X_data, bins=30, density=True, alpha=0.5, color='gray', label='Mixed Data Histogram')

# 產生 X 軸的連續點用來畫常態分佈曲線
x_axis = np.linspace(X_data.min(), X_data.max(), 1000)

# 畫出預測出的兩個常態分佈曲線 (乘以各自的比例 pi)
curve_1 = final_pi[0] * norm.pdf(x_axis, final_mu[0], final_sigma[0])
curve_2 = final_pi[1] * norm.pdf(x_axis, final_mu[1], final_sigma[1])

plt.plot(x_axis, curve_1, color='red', linewidth=2, label=f'Predicted Female (mu={final_mu[0]:.1f})')
plt.plot(x_axis, curve_2, color='blue', linewidth=2, label=f'Predicted Male (mu={final_mu[1]:.1f})')

# 畫出混合後的總曲線
plt.plot(x_axis, curve_1 + curve_2, color='black', linestyle='--', linewidth=2, label='Combined GMM Curve')

plt.title('Gaussian Mixture Model (EM Algorithm)')
plt.xlabel('Height (cm)')
plt.ylabel('Density')
plt.legend()
plt.grid(True, alpha=0.3)
plt.show()