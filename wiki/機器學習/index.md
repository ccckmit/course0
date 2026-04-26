# 機器學習

機器學習研究讓電腦從資料中學習的演算法。

## 概念

- [機器學習的歷史](機器學習的歷史.md) - 從符號 AI 到深度學習的演進 | tags: AI, 神經網路
- [監督式學習](監督式學習.md) - 從標記資料學習預測 | tags: 分類, 回歸
- [非監督式學習](非監督式學習.md) - 從無標記資料發現模式 | tags: 聚類, 降維
- [深度學習](深度學習.md) - 多層神經網路的學習方法 | tags: CNN, RNN, Transformer
- [強化學習](強化學習.md) - 透過環境互動學習決策 | tags: Q-learning, Policy_Gradient
- [神經網路](神經網路.md) - 受生物神經元啟發的計算模型 | tags: 啟動函數, 反向傳播
- [梯度下降法](梯度下降法.md) - 優化演算法 | tags: SGD, Adam
- [反傳遞演算法](反傳遞演算法.md) - 神經網路訓練的核心演算法 | tags: 梯度, 鏈規則
- [Adam](Adam.md) - 自適應學習率優化器 | tags: Momentum, RMSprop
- [分類演算法](分類演算法.md) - 離散類別預測 | tags: SVM, 決策樹, 隨機森林
- [K-近鄰](K-近鄰.md) - 基於實例的分類 | tags: KNN, 距離度量, 投票
- [樸素貝葉斯](樸素貝葉斯.md) - 基於貝葉斯定理的分類 | tags: Naive_Bayes, 貝葉斯, 文本
- [支持向量機](支持向量機.md) - 最大間隔分類器 | tags: SVM, kernel, margin
- [決策樹](決策樹.md) - 樹狀結構分類模型 | tags: entropy, information_gain
- [隨機森林](隨機森林.md) - 集成學習分類器 | tags: ensemble, bootstrap
- [聚類演算法](聚類演算法.md) - 將相似資料分組 | tags: K-Means, DBSCAN, 層次
- [K-均值](K-均值.md) - K-Means 聚類演算法 | tags: K-Means, 聚類, 無監督
- [主成分分析](主成分分析.md) - 維度降低技術 | tags: PCA, 特徵值, 特徵向量
- [散射網路](散射網路.md) - 基於擴散過程的生成模型 | tags: DDPM, 去噪, 生成模型
- [EM演算法](EM演算法.md) - 含隱變量模型的迭代優化 | tags: 期望最大化, 潛在變量, M步
- [馬可夫鏈](馬可夫鏈.md) - 離散時間隨機過程 | tags: 轉移矩陣, 平穩分佈, 馬可夫性
- [隱馬可夫模型](隱馬可夫模型.md) - 具有隱藏狀態的統計模型 | tags: HMM, 維特比算法, 前向算法
- [貝氏網路](貝氏網路.md) - 表示條件獨立的概率圖模型 | tags: DAG, 條件獨立, 推論

## 主題

- [模型評估](模型評估.md) - 機器學習效能度量與驗證策略 | tags: AUC, 交叉驗證, 混淆矩陣
- [損失函數](損失函數.md) - 模型訓練的優化目標函數 | tags: Cross-Entropy, MSE, Focal Loss
- [正則化](正則化.md) - 防止過擬合的核心技術 | tags: L1, L2, Dropout, 早停
- [自然語言處理](自然語言處理.md) - 文本理解與生成技術 | tags: Tokenization, Word2Vec, Transformer
- [電腦視覺](電腦視覺.md) - 圖像理解與分析技術 | tags: CNN, 物體偵測, 語義分割
- [異常偵測](異常偵測.md) - 識別偏離正常模式的數據 | tags: One-Class, Isolation Forest, Autoencoder
- [推薦系統](推薦系統.md) - 個性化推薦核心技術 | tags: 協同過濾, Matrix Factorization
- [聯邦學習](聯邦學習.md) - 保護隱私的分散式學習 | tags: FedAvg, 差分隱私
- [線性回歸](線性回歸.md) - 連續值預測的基礎方法 | tags: 正則化, Ridge, Lasso
- [卷積神經網路](卷積神經網路.md) - 處理網格結構資料 | tags: 圖像, 捲積, 池化
- [循環神經網路](循環神經網路.md) - 處理序列資料 | tags: LSTM, GRU, 時序
- [Transformer](Transformer.md) - 基於自注意力的架構 | tags: Attention, BERT, GPT
- [大型語言模型](大型語言模型.md) - 大規模預訓練語言模型 | tags: LLM, ChatGPT
- [生成對抗網路](生成對抗網路.md) - 生成模型的對抗訓練 | tags: GAN, 生成
- [語音處理與識別](語音處理與識別.md) - 語音信號處理與轉文字 | tags: ASR, TTS, MFCC
- [影像處理與識別](影像處理與識別.md) - 圖像分析與理解 | tags: CNN, 物體偵測
- [多模態模型](多模態模型.md) - 同時處理多種資料類型 | tags: Vision_Language, CLIP

## 工具

- [PyTorch](PyTorch.md) - Facebook 開發的深度學習框架 | tags: Tensor, autograd
- [TensorFlow](TensorFlow.md) - Google 開發的深度學習框架 | tags: Keras, TF Lite
- [scikit-learn](scikit-learn.md) - 傳統機器學習工具庫 | tags: ML, 演算法
- [Gym](Gym.md) - 強化學習環境 | tags: 環境, 模擬
- [microgpt](microgpt.md) - 極簡 GPT 實現 | tags: GPT, 教學
- [mini-openclaw](mini-openclaw.md) - 極簡 AI Agent | tags: Agent, LLM

## 人物

- [Geoffrey_Hinton](Geoffrey_Hinton.md) - 深度學習之父、2018圖靈獎 | 日期: 2026-04-08
- [Yann_LeCun](Yann_LeCun.md) - 卷積網路之父、2018圖靈獎 | 日期: 2026-04-08
- [Yoshua_Bengio](Yoshua_Bengio.md) - 深度學習研究者、2018圖靈獎 | 日期: 2026-04-08
- [Andrej_Karpathy](Andrej_Karpathy.md) - Tesla AI 總監、極簡教學專案 | 日期: 2026-04-08

## 日誌

- 參見 [log.md](log.md)