# ==========================================
# 1. 定義 HMM 模型參數 (機率矩陣)
# ==========================================

states = ('PRO', 'V', 'N') # 隱藏狀態：代名詞(Pronoun), 動詞(Verb), 名詞(Noun)

# 初始機率 (Start Probabilities): 句子開頭的第一個詞的詞性機率
start_p = {
    'PRO': 0.6,  # 句子很常以代名詞開頭 (我, 他)
    'N': 0.3,    # 也可能以名詞開頭
    'V': 0.1     # 較少以動詞開頭
}

# 轉移機率 (Transition Probabilities): 從 詞性A 轉移到 詞性B 的機率
trans_p = {
    'PRO': {'PRO': 0.0, 'V': 0.7, 'N': 0.3}, # 代名詞後面常接動詞
    'V':   {'PRO': 0.1, 'V': 0.3, 'N': 0.6}, # 動詞後面常接名詞 (看 書)
    'N':   {'PRO': 0.1, 'V': 0.4, 'N': 0.5}  # 名詞後面可以接動詞，也可以接名詞
}

# 發射機率 (Emission Probabilities): 某個詞性 生成 某個特定單字的機率
# ⚠️ 注意「計畫」這個詞的歧義：它在動詞和名詞中都可能出現！
emit_p = {
    'PRO': {'我': 0.6, '他': 0.4},
    'V':   {'看': 0.5, '計畫': 0.4, '打': 0.1}, 
    'N':   {'書': 0.5, '計畫': 0.3, '球': 0.2}
}

# ==========================================
# 2. 實作 Viterbi 演算法
# ==========================================
def viterbi(obs, states, start_p, trans_p, emit_p):
    """
    obs: 觀測序列 (例如: ['我', '計畫', '看', '書'])
    """
    # Viterbi 矩陣：記錄在第 t 步，狀態為 s 的「最大機率」
    V = [{}]
    # path 字典：記錄走到目前狀態的最佳「歷史路徑」
    path = {}

    # 步驟 1: 初始化 (t = 0，句子的第一個詞)
    for state in states:
        # 機率 = 初始機率 * 發射機率
        # 使用 .get(..., 0) 避免未知的字報錯
        prob = start_p[state] * emit_p[state].get(obs[0], 0)
        V[0][state] = prob
        path[state] = [state]

    # 步驟 2: 遞迴計算 (t > 0，接下來的詞)
    for t in range(1, len(obs)):
        V.append({})
        new_path = {}

        for curr_state in states:
            # 尋找從哪一個「前一步的狀態 (prev_state)」走過來，機率會最大
            # 計算公式: 前一步的累積機率 * 轉移機率 * 目前字詞的發射機率
            max_prob, best_prev_state = max(
                (V[t-1][prev_state] * trans_p[prev_state][curr_state] * emit_p[curr_state].get(obs[t], 0), prev_state)
                for prev_state in states
            )
            
            # 記錄最大機率與最佳路徑
            V[t][curr_state] = max_prob
            new_path[curr_state] = path[best_prev_state] + [curr_state]

        # 更新路徑記錄
        path = new_path

    # 步驟 3: 終止 (找出最後一步機率最大的狀態)
    n = len(obs) - 1
    max_final_prob, final_best_state = max((V[n][state], state) for state in states)

    return max_final_prob, path[final_best_state]

# ==========================================
# 3. 測試與驗證：見證 HMM 的強大
# ==========================================

print("🔍 測試一詞多義的「計畫」：\n")

# 測試句 1
sentence1 = ['我', '計畫', '看', '書']
prob1, tags1 = viterbi(sentence1, states, start_p, trans_p, emit_p)
print(f"句子 1: {' '.join(sentence1)}")
print(f"預測詞性: {tags1} (機率: {prob1:.6f})\n")

# 測試句 2
sentence2 = ['他', '看', '計畫']
prob2, tags2 = viterbi(sentence2, states, start_p, trans_p, emit_p)
print(f"句子 2: {' '.join(sentence2)}")
print(f"預測詞性: {tags2} (機率: {prob2:.6f})\n")