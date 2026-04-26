from collections import defaultdict, Counter

class SingleCharKeyboardPredictor:
    def __init__(self):
        # 儲存馬可夫鏈的轉移機率
        self.markov_model = defaultdict(Counter)
        
    def train(self, text):
        """
        依照「換行符號」將文章切分成句子。
        這樣就不會把上一句的結尾，錯誤連到下一句的開頭！
        """
        # 利用換行符號將文本切割成一個一個的句子陣列
        sentences = text.strip().split('\n')
        
        for sentence in sentences:
            # 清除每句可能多餘的空白
            sentence = sentence.replace(" ", "").strip()
            
            # 如果這行只有一個字或空行，就跳過不學習
            if len(sentence) < 2:
                continue
                
            # 在「同一個句子內」學習單字間的轉移關係
            for i in range(len(sentence) - 1):
                current_char = sentence[i]
                next_char = sentence[i + 1]
                
                # 狀態轉移記錄 +1
                self.markov_model[current_char][next_char] += 1
            
    def predict_next_chars(self, current_char, top_n=3):
        """
        根據目前的單字，預測最可能出現的下 n 個單字
        """
        if current_char not in self.markov_model:
            return ["(無資料)"]
        
        possible_next_chars = self.markov_model[current_char].most_common(top_n)
        return [char for char, count in possible_next_chars]

# ==========================================
# 1. 準備我們的「20字世界語料庫」 (一句完就換行)
# 限定使用：我,你,他,想,要,吃,喝,去,看,買,水,茶,飯,書,早,中,晚,好,很,不
# ==========================================
corpus = """
我想吃飯
你不想要喝水
他去買書
我看書
他喝茶
我喝水
你吃早飯
他吃晚飯
我很好
你很好
他不好
早飯好
晚飯好
我想去看他
你想去買茶
他去買水
我不去
你要去
我不吃
他想喝茶
你去看書
我想買書
他很好
你想要去買飯
他好
我不喝水
他不喝茶
我想吃早飯
他想吃中飯
你要喝水
我去看書
你去買茶
他不去看書
我想要買書
他想要買茶
很好
很不好
不買
不看
不吃
不喝
我想去買早飯
我買書
他想去買晚飯
你想要喝茶
我想要喝水
他很好
你不好
我看他
不看他
喝水
我喝茶
你吃中飯
我吃早飯
他去買書
"""

# ==========================================
# 2. 初始化並訓練模型
# ==========================================
keyboard = SingleCharKeyboardPredictor()
keyboard.train(corpus)

# --- 驗證這個世界真的只有 20 個字 ---
unique_chars = set(corpus.replace("\n", "").replace(" ", ""))
print(f"🌍 這個世界共有 {len(unique_chars)} 個字：", " ".join(unique_chars))
print("-" * 40)

# --- 偷看馬可夫鏈內部的統計 ---
print("🔍 鍵盤內部的機率統計 (部分)：")
chars_to_peek = ['想', '要', '不', '早', '很']
for char in chars_to_peek:
    total_next = sum(keyboard.markov_model[char].values())
    if total_next > 0:
        print(f"輸入【 {char} 】之後的統計 (共 {total_next} 次轉移):")
        for next_c, count in keyboard.markov_model[char].most_common(4):
            print(f"  └─ 接「{next_c}」: {count} 次 ({count/total_next*100:.1f}%)")
print("-" * 40)

# ==========================================
# 3. 模擬手機鍵盤預測
# ==========================================
print("📱 模擬手機輸入預測：")
test_chars = ["我", "想", "要", "買", "早", "很", "不"]

for char in test_chars:
    predictions = keyboard.predict_next_chars(char, top_n=3)
    print(f"輸入：【 {char} 】 => 鍵盤選字預測：{predictions}")