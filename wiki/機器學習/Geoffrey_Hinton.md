# Geoffrey Hinton

Geoffrey Everest Hinton（1947 年 12 月 6 日－），加拿大電腦科學家，被譽為「深度學習之父」（Godfather of Deep Learning）。他是反向傳播算法（Backpropagation）的發明者之一，深度學習技術的先驅，2018 年圖靈獎得主（Turing Award）。他的研究徹底改變了人工智慧的發展軌跡，影響了電腦視覺、自然語言處理等領域。

## 早年生活

### 求學時期

Hinton 出生於英國 Wimbledon，父親是電子工程師，祖父是邏輯學家 George Hinton。他從小對心智與思維感興趣：

```
求學經歷：
- 1960s 劍橋大學實驗心理學學士
- 1970 劍橋大學人工智慧博士
- 研究領域：心智計算表達
```

### 家庭背景

```python
# 家族傳統：
# - 曾曾祖父：George Boole (布爾代數)
# - 姨媽：Joan Hinton (電腦科學家)
# - 堂兄：Machine Learning 先驅
```

### 學術生涯

```
職業歷程：
- 1970s 萊斯大學 研究人員
- 1980s 卡內基梅隆大學 副教授
- 1990s 多倫多大學 教授
- 2012-現在 Vector Institute 創始科學家
```

## 反向傳播算法

### 發明歷程

1986 年，Hinton 與 David Rumelhart、Ronald Williams 共同發表了反向傳播經典論文：

```
時間線：
- 1974 Werbos 提出概念
- 1986 Hinton 等人發表論文
- Learning representations by back-propagating errors
```

### 核心概念

```python
# 反向傳播核心
#
# 前向傳播：
# input -> hidden -> output
# 計算預測與目標的誤差
#
# 反向傳播：
# output -> hidden -> input
# 從輸出層向輸入層傳播誤差
# 計算梯度並更新權重

# 簡化流程：
# 1. 前向傳播計算輸出
# 2. 計算損失函數
# 3. 反向傳播計算梯度
# 4. 更新權重
```

### 數學推導

```python
# 損失函數
# L = (1/2) * ||y - a||^2
#
# 輸出層梯度
# δL = (a - y) * f'(z)
#
# 隱藏層梯度
# δh = (W.T @ δ) * f'(zh)
#
# 權重更新
# W = W - learning_rate * (δ @ a)
```

## 限制波茲曼機

### 發明與發展

Hinton 是限制波茲曼機（Restricted Boltzmann Machine, RBM）的關鍵開發者：

```
時間線：
- 1986 RBM 概念 提出
- 2006 Deep Belief Networks 發明
- 2006 A fast learning algorithm for deep belief nets
```

### 核心概念

```python
# RBM 結構
#
# 可見層 (v) - 輸入
# 隱藏層 (h) - 特徵
#
# 能量函數
# E(v, h) = -b*v - c*h - v*W*h
#
# 機率
# P(h=1|v) = σ(c + W*v)
# P(v=1|h) = σ(b + W.T*h)
```

### 深度信念網路

```python
# Deep Belief Network (DBN)
#
# 多層 RBM 堆疊
# +------------------+
# | Output Layer     |
# +------------------+
# | RBM 3         |
# +------------------+
# | RBM 2         |
# +------------------+
# | RBM 1         |
# +------------------+
# | Input Layer    |
# +------------------+

# 訓練方式
# 貪婪逐層訓練
# 每層獨立訓練
# 然後Fine-tune
```

### 貢獻

```
RBM 的意義：
- 首次訓練深層網路
- 解決梯度消失問題
- 開啟深度學習時代
- 生成模型基礎
```

## 膠囊網路

### 發明背景

2017 年，Hinton 發表了膠囊網路（Capsule Networks）論文：

```
膠囊網路動機：
- CNN 缺乏空間理解
- 最大池化丟失位置資訊
- 需要更強的表達能力
```

### 核心概念

```python
# 膠囊網路結構
#
# 膠囊 (Capsule)：
# - 向量輸出
# - 方向表示姿態
# - 長度表示存在機率
#
# 動態路由：
# 低層膠囊預測高層
# 協議調整
# 迭代更新

# 損失函數
# Lc = Tc * max(0, m+ - ||vc||)^2
#       + (1-Tc) * max(0, ||vc|| - m-)^2
```

### 與 CNN 比較

| 特性 | CNN | CapsuleNet |
|------|-----|-----------|
| 表達方式 | 純量 | 向量 |
| 池化 | Max Pooling | 協議 |
| 位置 | 丟失 | 保留 |
| 數據需求 | 大 | 小 |

## 深度學習先驅

### 貢獻時間線

```
Hinton 的重要年份：
- 1986 Backpropagation
- 1995 Capsule Networks 原型
- 2006 深層信念網路
- 2012 ImageNet 冠軍 (AlexNet)
- 2017 膠囊網路正式發表
- 2018 圖靈獎
- 2023 Transformer 改進
```

### 重要論文

| 年份 | 論文 | 引用 |
|------|------|------|
| 1986 | Learning representations by back-propagating errors | 25000+ |
| 2006 | A fast learning algorithm for deep belief nets | 12000+ |
| 2006 | Reducing the dimensionality of data with neural networks | 8000+ |
| 2012 | ImageNet classification with deep CNN | 30000+ |
| 2017 | Dynamic Routing Between Capsules | 5000+ |

## 與業界合作

### Google Brain

2013 年，Hinton 的公司 DNNresearch 被 Google 收購，他加入 Google Brain：

```
貢獻：
- 醫療影像AI
- 自然語言處理
- AI 研究指導
```

### Vector Institute

2017 年，Hinton 共同創立 Vector Institute：

```
目標：
- 推動 AI 研究
- 培訓 AI 人才
-產業合作
```

## 獲獎與榮譽

| 年份 | 獎項 |
|------|------|
| 1990 | Fellow of the Royal Society |
| 1991 | Fellow of the Royal Academy of Engineering |
| 2001 | IJCAI Award for Excellence |
| 2005 | Fellow of the Cognitive Science Society |
| 2011 | Killam Prize |
| 2018 | ACM 圖靈獎 (Yann LeCun, Yoshua Bengio) |
| 2021 | 牛津大學榮譽博士 |

```
圖靈獎頒發理由：
"for conceptual and engineering breakthroughs
that have made deep neural networks
a critical component of computing"
```

## 與 Deep Learning 三劍客

### 三人合作

2018 年，Hinton、LeCun、Bengio 共同獲得圖靈獎：

```
圖靈獎三劍客：
- Geoffrey Hinton - 反向傳播, RBM, CapsuleNets
- Yann LeCun - CNN, 深度學習實踐
- Yoshua Bengio - 自然語言處理, 深度學習理論
```

## 研究風格

### 科研特點

```
Hinton 的研究風格：
- 直覺驅動
- 工程導向
- 大膽假設
- 持續創新
```

### 著名觀點

```
語錄：
- "Unsupervised learning is the future"
- "Neural networks will revolution"
- "AI 需要理解物理"
- "膠囊網路纔是正確方向"
```

## 個人特質

### 興趣愛好

```
興趣：
- 划船
- 園藝
- 歷史
- 木工
```

### 家庭

```
家庭：
- 妻子：Mollie
- 子女：Charlotte, Alex
- 家族：多位科學家
```

## 遺產與影響

### 深層學習革命

```
Hinton 的貢獻：
- 復興神經網路
- 推動深度學習
- 改變AI格局
- 影響數十億人
```

### 技術遺產

```python
# Hinton 的核心貢獻：
# 1. 反向傳播 - 訓練基礎
# 2. RBM - 深度網路
# 3. CapsuleNets - 新範式
# 4. Dropout - 正規化
# 5. 深度信念網路 - 生成模型
```

## 相關概念

- [ 深度學習](深度學習.md) - 深度學習基礎
- [ 卷積神經網路](卷積神經網路.md) - CNN
- [ 循環神經網路](循環神經網路.md) - RNN
- [ 大型語言模型](大型語言模型.md) - Transformer/LLM
- [ Yann_LeCun](Yann_LeCun.md) - CNN 發明者
- [ Yoshua_Bengio](Yoshua_Bengio.md) - 深度學習先驅