var WIKI = 'https://github.com/ccckmit/course0/blob/main/wiki/%E6%A9%9F%E5%99%A8%E5%AD%B8%E7%BF%92';
var CODE = 'https://github.com/ccckmit/course0/blob/main/code/%E6%A9%9F%E5%99%A8%E5%AD%B8%E7%BF%92';

var TOPICS = {
  '機器學習基礎': [
    {name:'機器學習的歷史', file:'機器學習的歷史.md', desc:'從符號推理到深度學習的演進歷程', tags:['History','Overview']},
    {name:'監督式學習', file:'監督式學習.md', desc:'使用標籤資料進行學習 — 分類與回歸', tags:['Supervised','Classification','Regression']},
    {name:'非監督式學習', file:'非監督式學習.md', desc:'從未標記資料中發現模式 — 聚類與降維', tags:['Unsupervised','Clustering','Dimensionality Reduction']},
    {name:'強化學習', file:'強化學習.md', desc:'Agent 透過與環境互動學習最佳決策策略', tags:['RL','Agent','Reward']},
    {name:'生成式AI', file:'生成式AI.md', desc:'能夠創造文字、圖像、音訊等新內容的 AI 技術', tags:['Generative','AI','Content Creation']},
    {name:'模型評估', file:'模型評估.md', desc:'衡量模型在未見資料上表現的指標與方法', tags:['Evaluation','Metrics','Cross-validation']},
    {name:'損失函數', file:'損失函數.md', desc:'衡量預測與真實值差距的函數，訓練最小化的目標', tags:['Loss','Optimization','Objective']},
  ],
  '監督式學習演算法': [
    {name:'分類演算法', file:'分類演算法.md', desc:'將資料分配至預定義類別的監督式方法', tags:['Classification','Supervised']},
    {name:'線性回歸', file:'線性回歸.md', desc:'建立輸入與連續輸出間的線性關係模型', tags:['Regression','Linear','Supervised']},
    {name:'K-近鄰', file:'K-近鄰.md', desc:'基於最近訓練樣本投票的惰性學習演算法', tags:['KNN','Classification','Lazy Learning']},
    {name:'決策樹', file:'決策樹.md', desc:'透過樹狀分裂進行決策的可解釋模型', tags:['Decision Tree','Classification','Interpretable']},
    {name:'隨機森林', file:'隨機森林.md', desc:'多棵決策樹整合的強大集成學習方法', tags:['Ensemble','Random Forest','Bagging']},
    {name:'支持向量機', file:'支持向量機.md', desc:'最大化分類間隔的經典監督式分類器', tags:['SVM','Classification','Kernel']},
    {name:'樸素貝葉斯', file:'樸素貝葉斯.md', desc:'基於貝氏定理與特徵獨立假設的分類器', tags:['Naive Bayes','Bayesian','Classification']},
    {name:'正則化', file:'正則化.md', desc:'透過懲罰項防止過擬合的核心技術', tags:['Regularization','Overfitting','L1','L2']},
    {name:'異常偵測', file:'異常偵測.md', desc:'識別偏離正常模式的少數資料點', tags:['Anomaly','Outlier','Detection']},
  ],
  '非監督式學習演算法': [
    {name:'聚類演算法', file:'聚類演算法.md', desc:'將相似資料分組以發現內在結構', tags:['Clustering','Unsupervised']},
    {name:'K-均值', file:'K-均值.md', desc:'最廣泛使用的劃分式聚類演算法', tags:['K-Means','Clustering','Centroid']},
    {name:'主成分分析', file:'主成分分析.md', desc:'最大變異方向的線性降維技術', tags:['PCA','Dimensionality Reduction','Unsupervised']},
  ],
  '深度學習核心': [
    {name:'深度學習', file:'深度學習.md', desc:'多層神經網路學習資料的階層式表示', tags:['Deep Learning','Neural Network','Representation']},
    {name:'神經網路', file:'神經網路.md', desc:'受生物神經元啟發的計算模型', tags:['Neural Network','Perceptron','Activation']},
    {name:'反傳遞演算法', file:'反傳遞演算法.md', desc:'利用鏈式法則計算梯度的核心訓練演算法', tags:['Backpropagation','Gradient','Chain Rule']},
    {name:'梯度下降法', file:'梯度下降法.md', desc:'沿梯度反方向迭代更新參數的最佳化方法', tags:['Gradient Descent','Optimization','SGD']},
    {name:'Adam', file:'Adam.md', desc:'結合動量與 RMSProp 的自適應學習率最佳化器', tags:['Adam','Optimizer','Adaptive']},
    {name:'卷積神經網路', file:'卷積神經網路.md', desc:'專為網格結構資料設計的空間特徵提取網路', tags:['CNN','Convolution','Image']},
    {name:'循環神經網路', file:'循環神經網路.md', desc:'具有內部狀態的序列建模神經網路架構', tags:['RNN','Sequence','LSTM','GRU']},
    {name:'Transformer', file:'Transformer.md', desc:'完全基於注意力機制的革命性序列模型架構', tags:['Transformer','Attention','Self-attention']},
    {name:'世界模型', file:'世界模型.md', desc:'學習環境動態規律以進行預測與規劃的模型', tags:['World Model','Prediction','Planning']},
  ],
  '語言與生成模型': [
    {name:'GPT', file:'GPT.md', desc:'生成式預訓練 Transformer — 大型語言模型始祖', tags:['GPT','LLM','Decoder-only']},
    {name:'大型語言模型', file:'大型語言模型.md', desc:'海量資料預訓練的通用語言理解與生成模型', tags:['LLM','Pretraining','Scaling']},
    {name:'多模態模型', file:'多模態模型.md', desc:'同時處理文字、圖像、音訊等多種資訊的模型', tags:['Multimodal','Vision-Language','CLIP']},
    {name:'散射網路', file:'散射網路.md', desc:'逐步去噪的機率式生成模型 (Diffusion Model)', tags:['Diffusion','Denoising','Generative']},
    {name:'生成對抗網路', file:'生成對抗網路.md', desc:'生成器與鑑別器相互對抗的生成模型架構', tags:['GAN','Adversarial','Generator']},
    {name:'自然語言處理', file:'自然語言處理.md', desc:'讓電腦理解、生成人類語言的技術', tags:['NLP','Text','Language']},
    {name:'語音處理與識別', file:'語音處理與識別.md', desc:'將語音訊號轉換為文字或理解的技術', tags:['Speech','ASR','TTS']},
    {name:'推薦系統', file:'推薦系統.md', desc:'根據用戶行為提供個人化內容推薦的技術', tags:['Recommendation','Collaborative Filtering','Personalization']},
  ],
  '機率模型': [
    {name:'貝氏網路', file:'貝氏網路.md', desc:'用有向無環圖表示變數間條件獨立關係的模型', tags:['Bayesian Network','DAG','Inference']},
    {name:'馬可夫鏈', file:'馬可夫鏈.md', desc:'無記憶性的離散時間隨機過程', tags:['Markov Chain','Stochastic','Transition']},
    {name:'隱馬可夫模型', file:'隱馬可夫模型.md', desc:'含有隱藏狀態的馬可夫鏈，用於序列標註', tags:['HMM','Viterbi','Sequence Labeling']},
    {name:'EM演算法', file:'EM演算法.md', desc:'處理含有隱藏變數的機率模型參數估計演算法', tags:['EM','Latent Variable','Expectation-Maximization']},
    {name:'聯邦學習', file:'聯邦學習.md', desc:'保護隱私的分散式機器學習範式', tags:['Federated Learning','Privacy','Distributed']},
  ],
  '電腦視覺與影像': [
    {name:'電腦視覺', file:'電腦視覺.md', desc:'讓電腦從圖像和影片中理解視覺世界的技術', tags:['Computer Vision','Image','Object Detection']},
    {name:'影像處理與識別', file:'影像處理與識別.md', desc:'圖像增強、特徵提取與物體辨識的綜合技術', tags:['Image Processing','Recognition','CNN']},
    {name:'電腦下棋', file:'電腦下棋.md', desc:'從深藍到 AlphaGo 的 AI 遊戲策略發展史', tags:['Game AI','AlphaGo','MCTS']},
  ],
  '最佳化與搜尋': [
    {name:'爬山演算法', file:'爬山演算法.md', desc:'向目標值增加方向移動的局部搜尋最佳化方法', tags:['Hill Climbing','Local Search','Optimization']},
    {name:'搜尋法', file:'搜尋法.md', desc:'在問題空間中搜尋目標解的暴力與啟發式方法', tags:['Search','BFS','DFS','Heuristic']},
  ],
  '工具與框架': [
    {name:'PyTorch', file:'PyTorch.md', desc:'Meta 開發的動態計算圖深度學習框架', tags:['PyTorch','Framework','Deep Learning']},
    {name:'TensorFlow', file:'TensorFlow.md', desc:'Google 開發的靜態/動態圖深度學習框架', tags:['TensorFlow','Framework','Keras']},
    {name:'scikit-learn', file:'scikit-learn.md', desc:'Python 最廣泛使用的經典機器學習庫', tags:['scikit-learn','Library','Python']},
    {name:'Gym', file:'Gym.md', desc:'OpenAI 開發的標準化強化學習環境工具包', tags:['Gym','RL','Environment']},
    {name:'microgpt', file:'microgpt.md', desc:'Andrej Karpathy 的極簡 GPT 實作 (約 200 行)', tags:['microgpt','GPT','Minimal']},
    {name:'mini-openclaw', file:'mini-openclaw.md', desc:'約 400 行 Python 實現的極簡 AI Agent', tags:['Agent','Tool Use','Multi-agent']},
  ],
  '人物': [
    {name:'Geoffrey Hinton', file:'Geoffrey_Hinton.md', desc:'「深度學習之父」— 反傳遞演算法、2018 圖靈獎', tags:['Hinton','Deep Learning','Backpropagation']},
    {name:'Yann LeCun', file:'Yann_LeCun.md', desc:'「卷積神經網路之父」— CNN、2018 圖靈獎', tags:['LeCun','CNN','Computer Vision']},
    {name:'Yoshua Bengio', file:'Yoshua_Bengio.md', desc:'深度學習三巨頭之一 — 序列模型、2018 圖靈獎', tags:['Bengio','Deep Learning','Sequence']},
    {name:'Andrej Karpathy', file:'Andrej_Karpathy.md', desc:'AI 教育先驅 — Tesla Autopilot、OpenAI 共同創辦人', tags:['Karpathy','Education','Autopilot']},
  ],
};

var CATEGORY_TAGS = {};
(function(){
  var keys = Object.keys(TOPICS);
  for (var k = 0; k < keys.length; k++) {
    for (var i = 0; i < TOPICS[keys[k]].length; i++) {
      var t = TOPICS[keys[k]][i];
      CATEGORY_TAGS[t.name] = t.tags || [];
    }
  }
})();

var CODES = [
  // --- ml/regression/ ---
  {name:'線性回歸', path:'ml/regression', files:['linear.py'], desc:'最小平方法線性回歸'},
  {name:'多項式回歸', path:'ml/regression', files:['polynomial.py'], desc:'多項式特徵回歸'},
  {name:'嶺回歸 (Ridge)', path:'ml/regression', files:['ridge.py'], desc:'L2 正則化線性回歸'},
  {name:'Lasso 回歸', path:'ml/regression', files:['lasso.py'], desc:'L1 正則化線性回歸'},
  // --- ml/classify/ ---
  {name:'K-近鄰分類', path:'ml/classify', files:['knn.py'], desc:'K-近鄰演算法實作'},
  {name:'邏輯斯諦回歸', path:'ml/classify', files:['logistic.py'], desc:'邏輯斯諦回歸分類'},
  {name:'SVM', path:'ml/classify', files:['svm.py'], desc:'支持向量機分類器'},
  {name:'決策樹', path:'ml/classify', files:['decision_tree.py'], desc:'決策樹分類演算法'},
  {name:'樸素貝葉斯', path:'ml/classify', files:['naive_bayes.py'], desc:'基於貝氏定理的分類器'},
  // --- ml/cluster/ ---
  {name:'K-均值聚類', path:'ml/cluster', files:['kmean.py'], desc:'K-means 聚類演算法'},
  {name:'DBSCAN 聚類', path:'ml/cluster', files:['dbscan.py'], desc:'密度式聚類演算法'},
  {name:'階層式聚類', path:'ml/cluster', files:['hierarchical.py'], desc:'凝聚式階層聚類'},
  {name:'GMM 聚類', path:'ml/cluster', files:['gmm.py'], desc:'高斯混合模型聚類'},
  // --- ml/pca/ ---
  {name:'PCA 主成分分析', path:'ml/pca', files:['pca.py'], desc:'主成分分析降維'},
  // --- ml/sklearn/ ---
  {name:'sklearn 分類', path:'ml/sklearn', files:['classify.py'], desc:'scikit-learn 分類範例'},
  {name:'sklearn 聚類', path:'ml/sklearn', files:['cluster.py'], desc:'scikit-learn 聚類範例'},
  {name:'Iris 分類', path:'ml/sklearn', files:['iris_classify.py'], desc:'Iris 資料集分類示範'},
  // --- nn/gd/ ---
  {name:'梯度下降', path:'nn/gd', files:['gd.py'], desc:'基本梯度下降演算法'},
  {name:'梯度下降回歸', path:'nn/gd', files:['gdRegression.py'], desc:'梯度下降做線性回歸'},
  // --- nn/nn0/ ---
  {name:'nn0 自動微分引擎 (Python)', path:'nn/nn0/nn0py', files:['nn0.py'], desc:'micrograd 風格的自動微分框架'},
  {name:'nn0 GPT-0 (Python)', path:'nn/nn0/nn0py', files:['gpt0.py'], desc:'字元級 GPT 語言模型'},
  {name:'nn0 CNN-0 (Python)', path:'nn/nn0/nn0py', files:['cnn0.py'], desc:'卷積神經網路實作'},
  {name:'nn0 (JavaScript)', path:'nn/nn0/nn0js', files:['nn0.js'], desc:'JavaScript 版神經網路'},
  {name:'nn0 (C)', path:'nn/nn0/nn0c', files:['nn0.c','nn0.h'], desc:'C 語言版神經網路'},
  {name:'nn0 GPT-0 (C)', path:'nn/nn0/nn0c', files:['gpt0.c','gpt0.h'], desc:'C 語言版 GPT 語言模型'},
  {name:'nn0 (Rust)', path:'nn/nn0/nn0rs/src', files:['main.rs','nn0.rs','gpt0.rs'], desc:'Rust 版神經網路 + GPT'},
  // --- nn/nn1tensor/ ---
  {name:'nn1 Tensor + Autograd', path:'nn/nn1tensor', files:['tensor.py'], desc:'Tensor 類別與自動微分'},
  {name:'nn1 GPT 訓練', path:'nn/nn1tensor', files:['main.py','nn.py','gpt.py'], desc:'第一版 GPT 模型訓練'},
  // --- nn/nn2kv/ ---
  {name:'nn2 CharGPT (KV Cache)', path:'nn/nn2kv', files:['chargpt.py','gpt.py','nn.py','tensor.py'], desc:'第二版 GPT + KV Cache 加速推理'},
  // --- nn/microgpt/ ---
  {name:'microgpt', path:'nn/microgpt', files:['microgpt.py','input.txt'], desc:'Karpathy 風格微型 Transformer'},
  // --- mini-llm/ ---
  {name:'mini-llm 預訓練', path:'mini-llm/v1-pretrain', files:['mini-llm.py'], desc:'單檔字元級 LLM 預訓練'},
  {name:'mini-llm v2 模型', path:'mini-llm/v2-finetune', files:['model.py'], desc:'RoPE/RMSNorm/SwiGLU 現代 LLM 模型'},
  {name:'mini-llm v2 預訓練', path:'mini-llm/v2-finetune', files:['pretrain.py'], desc:'現代 LLM 預訓練腳本'},
  {name:'mini-llm v2 微調', path:'mini-llm/v2-finetune', files:['finetune.py'], desc:'LLM 微調腳本'},
  {name:'mini-llm 武俠資料生成', path:'mini-llm/v2-finetune', files:['gen_data_wuxia.py'], desc:'武俠人物對話資料生成'},
  {name:'mini-llm v3 蒸餾資料', path:'mini-llm/v3-distill', files:['gen_data_distill.py'], desc:'大模型蒸餾資料生成'},
  // --- image/ ---
  {name:'CNN MNIST 訓練', path:'image/mnist_recognize', files:['train.py'], desc:'CNN 手寫數字辨識訓練'},
  {name:'GAN MNIST 生成', path:'image/mnist_gan_gen', files:['train.py','generate.py'], desc:'生成對抗網路生成手寫數字'},
  {name:'DDPM MNIST 散射生成', path:'image/mnist_diff_gen', files:['train.py','generate.py'], desc:'去噪擴散機率模型生成 MNIST'},
  // --- game/ ---
  {name:'Breakout DQN', path:'game/breakout', files:['train.py'], desc:'Atari Breakout 深度 Q 網路'},
  {name:'AlphaZero 五子棋', path:'game/gomoku/alphazero', files:['train.py','play.py'], desc:'MCTS + 神經網路自我對弈'},
  {name:'經典五子棋', path:'game/gomoku/classical', files:['gomoku.py'], desc:'Minimax/Alpha-Beta 剪枝五子棋'},
  {name:'AlphaZero TicTacToe', path:'game/tictactoe/alphazero', files:['train.py','play.py'], desc:'AlphaZero 圈圈叉叉'},
  {name:'Q-Learning TicTacToe', path:'game/tictactoe/qlearning', files:['train.py','play.py'], desc:'DQN 圈圈叉叉'},
  // --- reinforce/ ---
  {name:'CartPole 六方法', path:'reinforce/cartpole/cartpoleAllMethod', files:['cartpole.py','DQNAgent.py','ActorCriticAgent.py'], desc:'DQN/SARSA/VPG/Actor-Critic 比較'},
  {name:'Walker2d PPO', path:'reinforce/walker2d', files:['train.py','model.py'], desc:'MuJoCo Walker2d 近端策略最佳化'},
  {name:'BipedalWalker SAC', path:'reinforce/bipedalwalker', files:['train.py'], desc:'BipedalWalker Soft Actor-Critic'},
  {name:'FrozenLake Q-Table', path:'reinforce/qtable', files:['frozenlake.py'], desc:'FrozenLake 表格 Q-Learning'},
  // --- agent/ ---
  {name:'Agent v0 對話', path:'agent/v0-chat', files:['chat0.py'], desc:'基本 LLM 聊天 Agent'},
  {name:'Agent v2 XML', path:'agent/v2-agent-xml', files:['agent0.py'], desc:'XML 格式工具呼叫 Agent'},
  {name:'Agent v5 團隊', path:'agent/v5-agent-team', files:['agent0team.py','agent0.py'], desc:'多 Agent 團隊協作'},
  {name:'Agent A1 Docker', path:'agent/A1-agent_in_docker', files:['agent0.py'], desc:'Docker 容器化安全 Agent'},
  // --- voice/ ---
  {name:'TTS 語音合成', path:'voice/supertonic', files:['tts.py'], desc:'HuggingFace 模型語音合成'},
  // --- music/ ---
  {name:'馬可夫音樂生成', path:'music', files:['musicgen.py'], desc:'馬可夫鏈 MIDI 音樂生成'},
  // --- prob_model/ ---
  {name:'貝氏網路', path:'prob_model/bayesnet', files:['bayesnet.py'], desc:'貝氏網路推論引擎'},
  {name:'EM 高斯混合', path:'prob_model/em', files:['em_gmm.py'], desc:'EM 演算法估計 GMM 參數'},
  {name:'HMM 詞性標註', path:'prob_model/hmm', files:['viterbi_pos_tagging.py'], desc:'維特比演算法做詞性標註'},
  {name:'PageRank', path:'prob_model/markov', files:['markov_pagerank.py'], desc:'馬可夫鏈 PageRank 演算法'},
  {name:'RBM/DBN MNIST', path:'prob_model/rbm_dbn', files:['rbm_dbn_mnist.py'], desc:'受限波茲曼機 + 深度信念網路'},
];

var TOPIC_CONTENT = {};

TOPIC_CONTENT['機器學習的歷史'] = {
  en: 'History of Machine Learning',
  sections: [
    {title:'起源期 (1940s–1960s)', content:'1943 年 McCulloch-Pitts 提出 MCP 神經元模型。1950 年 Turing 發表〈Computing Machinery and Intelligence〉。1957 年 Rosenblatt 發明感知機 (Perceptron)。1958 年 Widrow 提出 Adaline。1969 年 Minsky 與 Papert 出版《Perceptrons》，指出 XOR 問題導致第一次 AI 寒冬。'},
    {title:'知識驅動期 (1970s–1980s)', content:'專家系統興起，如 MYCIN 用於醫學診斷。1986 年 Rumelhart、Hinton、Williams 重新發現反傳遞演算法，使多層感知機得以訓練。1989 年 LeCun 提出卷積神經網路 LeNet 用於手寫郵遞區號辨識。'},
    {title:'統計學習期 (1990s–2000s)', content:'Vapnik 提出支持向量機（SVM），具備堅實的統計學習理論基礎。Freund 與 Schapire 提出 AdaBoost 集成方法。隨機森林、貝氏網路等機率圖模型成為主流。1997 年 IBM 深藍擊敗西洋棋世界冠軍 Kasparov。'},
    {title:'深度學習期 (2010s–至今)', content:'2012 年 AlexNet 在 ImageNet 比賽大幅超越傳統方法，開啟深度學習革命。2014 年 GAN 由 Goodfellow 提出。2017 年 Transformer 架構問世。GPT 系列、BERT 等大型語言模型持續突破。2022 年 ChatGPT 引發全球 AI 熱潮。'},
  ],
};

TOPIC_CONTENT['監督式學習'] = {
  en: 'Supervised Learning',
  sections: [
    {title:'定義', content:'監督式學習使用帶有標籤（Label）的訓練資料，學習從輸入特徵 X 到輸出標籤 Y 的映射函數 f:X→Y。常見任務包含分類（離散輸出）和回歸（連續輸出）。'},
    {title:'分類演算法', content:'決策樹、隨機森林、SVM、KNN、邏輯斯諦回歸、樸素貝葉斯等都是經典分類器。深度學習則用 CNN 處理影像、RNN/Transformer 處理序列。評估指標包含準確率、精確率、召回率、F1-score。'},
    {title:'回歸演算法', content:'線性回歸假設輸出是輸入的線性組合。多項式回歸引入非線性特徵。Ridge/Lasso 透過 L2/L1 正則化防止過擬合。評估指標包含 MSE、MAE、R² 分數。'},
    {title:'過擬合與欠擬合', content:'過擬合（Overfitting）指模型記住訓練資料雜訊，在新資料上表現差；欠擬合（Underfitting）指模型未捕捉到資料中的模式。透過正則化、交叉驗證、更多資料、適當模型複雜度來平衡兩者。'},
  ],
};

TOPIC_CONTENT['非監督式學習'] = {
  en: 'Unsupervised Learning',
  sections: [
    {title:'定義', content:'非監督式學習不使用標籤資料，演算法必須自行發現資料中的隱藏結構、模式和規律。常見任務包括聚類、降維、密度估計、異常偵測。'},
    {title:'聚類', content:'聚類將相似資料點分組，使組內相似度高、組間相似度低。K-Means 使用距離中心點劃分；DBSCAN 基於密度可發現任意形狀簇；階層式聚類產生樹狀結構；GMM 以機率方式分配簇歸屬。'},
    {title:'降維', content:'降維將高維資料映射到低維空間，便於視覺化和去噪。PCA 找最大變異方向進行線性投影。t-SNE 和 UMAP 保留局部鄰域結構，適合高維資料視覺化。'},
    {title:'應用場景', content:'客戶分群（市場行銷）、圖像壓縮（K-Means 量化）、異常偵測（低密度區域）、基因表達分析（發現亞型）、推薦系統（協同過濾中的潛在因素）。'},
  ],
};

TOPIC_CONTENT['強化學習'] = {
  en: 'Reinforcement Learning',
  sections: [
    {title:'核心概念', content:'強化學習中，Agent 在環境中採取動作，環境回饋獎賞（Reward）和下一個狀態。目標是最大化累積獎賞。關鍵元素：策略 π(a|s)、價值函數 V(s)、Q 函數 Q(s,a)、模型（環境轉換）。'},
    {title:'馬可夫決策過程 (MDP)', content:'MDP 由狀態 S、動作 A、轉移機率 P、獎賞 R、折扣因子 γ 構成。策略 π 定義在每個狀態下該採取什麼動作。求解 MDP 的方法包含動態規劃（價值迭代、策略迭代）。'},
    {title:'無模型方法', content:'Q-Learning 是一種 off-policy 的無模型方法，透過 Q 表或深度 Q 網路（DQN）近似最優 Q 函數。SARSA 則是 on-policy 方法。Policy Gradient（如 REINFORCE）直接最佳化策略參數。Actor-Critic 結合了兩者優點。'},
    {title:'進階方法', content:'PPO（近端策略最佳化）透過信賴區域約束實現穩定訓練。SAC（Soft Actor-Critic）加入熵正則化，鼓勵探索。AlphaGo/AlphaZero 結合 MCTS 與神經網路自我對弈，達到超人類水準。應用包含機器人控制、遊戲 AI、自動駕駛。'},
  ],
};

TOPIC_CONTENT['生成式AI'] = {
  en: 'Generative AI',
  sections: [
    {title:'定義', content:'生成式 AI 是能夠創造新內容的人工智慧技術，涵蓋文字（GPT、Claude）、圖像（DALL-E、Stable Diffusion）、音訊、影片、程式碼等多種模態。基於深度學習模型從大規模訓練資料中學習資料的分布。'},
    {title:'技術演進', content:'早期生成模型包含 GAN（生成對抗網路）和 VAE（變分自編碼器）。2017 年 Transformer 架構奠定了現代生成 AI 的基礎。2020 年 GPT-3 展示了大型語言模型的驚人能力。散射模型（Diffusion Models）在圖像生成上取得突破。'},
    {title:'主流模型', content:'大型語言模型（GPT-4、Claude、LLaMA）擅長文字生成與理解。多模態模型（GPT-4V、Gemini）同時處理文字和圖像。圖像生成方面 DALL-E 3、Midjourney、Stable Diffusion 各具特色。影片生成方面 Sora 能生成逼真的影片。'},
    {title:'挑戰與展望', content:'生成式 AI 面臨幻覺（Hallucination）、偏見、著作權、安全性等挑戰。未來方向包含更強的多模態理解、推理能力提升、Agent 自主執行任務、可解釋性增強、以及與人類價值對齊（Alignment）。'},
  ],
};

TOPIC_CONTENT['模型評估'] = {
  en: 'Model Evaluation',
  sections: [
    {title:'泛化能力評估', content:'模型在未見資料上的表現稱為泛化能力。將資料集劃分為訓練集、驗證集、測試集。K 折交叉驗證將資料分為 K 份，輪流用 K-1 份訓練、1 份驗證，減少評估偏差。留出法、自助法（Bootstrap）也是常見策略。'},
    {title:'分類評估指標', content:'準確率（Accuracy）適用平衡資料。精確率（Precision）和召回率（Recall）分別關注預測正確的比例和真實正類被找出的比例。F1-score 是兩者的調和平均。ROC 曲線與 AUC 值衡量模型在不同閾值下的區分能力。混淆矩陣提供了全面的分類結果。'},
    {title:'回歸評估指標', content:'MSE（均方誤差）對大誤差給予較大懲罰。MAE（平均絕對誤差）對異常值較穩健。R² 決定係數衡量模型解釋的變異比例。MAPE（平均絕對百分比誤差）用於衡量相對誤差。調整 R² 對加入無用特徵進行懲罰。'},
    {title:'偏差與方差權衡', content:'高偏差（Bias）導致欠擬合，模型假設過於簡單。高方差（Variance）導致過擬合，模型對訓練資料的雜訊過度敏感。最佳模型位於偏差和方差的平衡點，可透過正則化、集成方法、調整模型複雜度來控制。'},
  ],
};

TOPIC_CONTENT['損失函數'] = {
  en: 'Loss Functions',
  sections: [
    {title:'回歸損失函數', content:'均方誤差（MSE）最常用於回歸問題，具有平滑梯度便於最佳化。平均絕對誤差（MAE）對異常值較不敏感。Huber Loss 結合 MSE 和 MAE 的優點，在誤差小時用 MSE、大時用 MAE。分位數損失用于區間預測。'},
    {title:'分類損失函數', content:'交叉熵損失（Cross-Entropy Loss）是最常用的分類損失函數，配合 Softmax 輸出層使用。二分類使用二元交叉熵（BCE）。合頁損失（Hinge Loss）用於 SVM。Focal Loss 調整難易樣本權重，解決類別不平衡問題。'},
    {title:'排序與其他損失', content:'Triplet Loss 和 Contrastive Loss 用於度量學習和 Siamese 網路，使相似樣本靠近、不同樣本遠離。CTC Loss 用于序列標註（語音辨識）。KL 散度衡量兩個機率分布的差異，用於蒸餾學習和變分推論。'},
  ],
};

TOPIC_CONTENT['分類演算法'] = {
  en: 'Classification Algorithms',
  sections: [
    {title:'任務定義', content:'分類是將輸入樣本分配到預定義類別的監督式學習任務。二元分類區分兩個類別，多元分類處理三個以上類別。多標籤分類允許每個樣本屬於多個類別。應用包含垃圾郵件偵測、疾病診斷、圖像分類、情感分析。'},
    {title:'常見演算法', content:'邏輯斯諦回歸透過 Sigmoid 函數輸出類別機率。決策樹以樹狀結構進行可解釋的分類。隨機森林整合多棵決策樹提高準確率。SVM 尋找最大間隔分類超平面。KNN 基於最近鄰居投票。樸素貝葉斯基於機率模型做出分類。'},
    {title:'深度學習分類', content:'深度神經網路（DNN）能夠學習層次化的特徵表示。CNN 在影像分類中表現優異。RNN 和 Transformer 在序列分類任務中領先。轉移學習（Transfer Learning）使用預訓練模型進行微調，大幅減少所需標籤資料。'},
  ],
};

TOPIC_CONTENT['線性回歸'] = {
  en: 'Linear Regression',
  sections: [
    {title:'模型定義', content:'線性回歸假設目標變數 y 是輸入特徵 x₁, x₂, ..., xₚ 的線性組合加上誤差項：y = β₀ + β₁x₁ + ... + βₚxₚ + ε。β 是回歸係數，ε 是隨機誤差，通常假設服從常態分布。普通最小平方法（OLS）最小化殘差平方和。'},
    {title:'參數估計', content:'正規方程式 (X^T X)^{-1} X^T y 直接求解最佳係數。梯度下降法迭代更新係數，適合大規模資料。最大似然估計（MLE）在誤差常態假設下等價於 OLS。正則化方法（Ridge/Lasso）在估計中加入懲罰項。'},
    {title:'模型診斷', content:'殘差分析檢查模型假設：殘差應隨機分布、無明顯模式。Q-Q 圖檢查殘差常態性。Durbin-Watson 統計量檢測殘差自我相關。VIF（變異膨脹因子）檢測多重共線性。透過轉換或加入交互作用項改進模型。'},
  ],
};

TOPIC_CONTENT['K-近鄰'] = {
  en: 'K-Nearest Neighbors',
  sections: [
    {title:'演算法原理', content:'K-近鄰（KNN）是一種惰性學習演算法，沒有明顯的訓練階段。預測時計算輸入樣本與所有訓練樣本之間的距離，選擇最近的 K 個鄰居。分類問題用投票決定類別，回歸問題用平均值。K 值越小決策邊界越複雜。'},
    {title:'距離度量', content:'歐幾里得距離（L2）最常用。曼哈頓距離（L1）在高維空間更穩定。餘弦相似度用於文本向量。閔可夫斯基距離是 L1 和 L2 的推廣。加權 KNN 根據距離遠近賦予鄰居不同的投票權重。'},
    {title:'優缺點與應用', content:'優點：簡單直覺、無訓練階段、適合多分類問題。缺點：計算成本隨樣本數線性增長（惰性學習的代價）、對特徵尺度敏感（需要標準化）、維度詛咒（高維時距離失去區分力）。應用包含推薦系統、模式識別、異常偵測。'},
  ],
};

TOPIC_CONTENT['決策樹'] = {
  en: 'Decision Tree',
  sections: [
    {title:'樹結構', content:'決策樹由根節點（Root）、內部節點（Internal Node）和葉節點（Leaf）組成。每個內部節點對一個特徵進行測試，根據結果分支到子節點。葉節點代表最終決策輸出。從根到葉的路徑對應一條決策規則。'},
    {title:'分裂準則', content:'ID3 使用資訊增益（Information Gain）選擇分裂特徵，偏好取值多的特徵。C4.5 使用增益率（Gain Ratio）克服 ID3 的偏見。CART 使用基尼不純度（Gini Impurity）或 MSE（回歸樹）。最佳分裂使子節點不純度最小化。'},
    {title:'剪枝', content:'預剪枝：在分裂前檢查是否提升驗證集表現，提前停止樹生長。後剪枝：先完全生長，再從下而上合併不重要的分支。最小成本複雜度剪枝權衡樹大小與誤差。剪枝防止過擬合，提升泛化能力。'},
    {title:'優缺點', content:'優點：可解釋性強（白箱模型）、不需要特徵縮放、可處理混合類型資料。缺點：容易過擬合、對資料微小變化敏感（高方差）、貪婪演算法不一定找到全域最優樹。集成方法（隨機森林、GBDT）解決這些問題。'},
  ],
};

TOPIC_CONTENT['隨機森林'] = {
  en: 'Random Forest',
  sections: [
    {title:'集成學習', content:'隨機森林是 Bagging 的代表方法，建構多棵決策樹並整合它們的預測。分類用投票、回歸用平均。集成降低方差而不增加偏差，使模型更穩定準確。每棵樹的訓練資料透過 Bootstrap 採樣從原始資料中隨機抽取。'},
    {title:'隨機性來源', content:'雙重隨機性：1）資料隨機性 — 每棵樹使用 Bootstrap 樣本訓練（約 63.2% 原始資料）。2）特徵隨機性 — 每個分裂點只考慮隨機子集的特徵（分類通常用 sqrt(p)，回歸用 p/3）。這確保了樹之間的差異性。'},
    {title:'超參數', content:'n_estimators（樹的數量）越多越好，但報酬遞減。max_depth（最大深度）限制樹生長。min_samples_split 和 min_samples_leaf 控制節點最小樣本數。max_features 控制分裂時考慮的特徵比例。OOB（袋外）誤差可作為內建驗證。'},
    {title:'優缺點', content:'優點：高準確率、抗過擬合、可處理高維資料、提供特徵重要性、不需太多調參。缺點：模型體積大、預測速度慢、可解釋性低於單棵樹、在極稀疏資料上表現不如線性模型。'},
  ],
};

TOPIC_CONTENT['支持向量機'] = {
  en: 'Support Vector Machine',
  sections: [
    {title:'最大間隔分類器', content:'SVM 的核心思想是找到一個超平面將不同類別的樣本分開，同時最大化兩類別之間的間隔（Margin）。支援向量是距離超平面最近的訓練樣本點，只有它們決定了超平面的位置。最大化間隔提高了模型的泛化能力。'},
    {title:'軟間隔 SVM', content:'現實資料通常不是線性可分的。軟間隔 SVM 引入了鬆弛變數（Slack Variable），允許部分樣本落在間隔內或被錯誤分類。懲罰參數 C 控制對錯誤的容忍度：C 越大越嚴格，可能過擬合；C 越小間隔越寬，可能欠擬合。'},
    {title:'核技巧', content:'核函數（Kernel Function）將資料映射到更高維的特徵空間，使原本線性不可分的資料在高維空間線性可分。多項式核、高斯徑向基函數（RBF）核、Sigmoid 核是常見選擇。RBF 核的 γ 參數控制高斯函數的寬度。'},
    {title:'優缺點', content:'優點：理論基礎堅實、在高維空間表現優秀、記憶體效率高（僅使用支援向量）、核技巧靈活強大。缺點：對大規模資料訓練慢、不直接提供機率估計、核選擇和超參數調校困難、對特徵縮放敏感。'},
  ],
};

TOPIC_CONTENT['樸素貝葉斯'] = {
  en: 'Naive Bayes',
  sections: [
    {title:'貝氏定理', content:'樸素貝葉斯基於貝氏定理 P(Y|X) = P(X|Y)P(Y)/P(X)，計算後驗機率。給定輸入特徵 X = (x₁, ..., xₙ)，選擇後驗機率最大的類別作為預測結果。先驗機率 P(Y) 從訓練資料中估計，通常使用類別頻率。'},
    {title:'樸素假設', content:'「樸素」的來源是假設所有特徵在給定類別條件下相互獨立。這在現實中幾乎不可能成立，但驚人的是樸素貝葉斯在許多應用中表現很好。特徵獨立假設大大降低了參數估計的複雜度。'},
    {title:'常見變體', content:'高斯樸素貝葉斯：假設連續特徵服從常態分布。多項式樸素貝葉斯：用於離散計數特徵（如文字分類中的詞頻）。伯努利樸素貝葉斯：用於二元特徵（如文件中是否出現某詞）。Complement Naive Bayes 處理不平衡資料。'},
    {title:'應用', content:'文本分類（垃圾郵件過濾、情感分析、新聞分類）是樸素貝葉斯最成功的應用領域。即時分類、推薦系統、醫療診斷中也有廣泛應用。即使有更好的分類器，樸素貝葉斯仍因其速度和簡單性而作為基線模型使用。'},
  ],
};

TOPIC_CONTENT['正則化'] = {
  en: 'Regularization',
  sections: [
    {title:'L1 正則化 (Lasso)', content:'Lasso 回歸在損失函數中加入權重絕對值之和（L1 懲罰）。λ||w||₁。L1 正則化會將不重要特徵的權重壓縮為零，具有特徵選擇的內建能力。適合處理高維稀疏資料。缺點是當特徵數大於樣本數時最多選 n 個特徵。'},
    {title:'L2 正則化 (Ridge)', content:'Ridge 回歸加入權重平方和（L2 懲罰）。λ||w||₂²。L2 正則化將權重等比例縮小但不為零，適合處理多重共線性。所有特徵都保留但影響被縮小。計算上有封閉解，比 L1 更穩定。彈性網路（Elastic Net）結合 L1 和 L2。'},
    {title:'深度學習中的正則化', content:'Dropout 在訓練時隨機丟棄部分神經元，強迫網路學習冗餘表示。Batch Normalization 透過歸一化層輸入加速訓練並有輕微正則化效果。資料擴增（Data Augmentation）增加訓練樣本多樣性。早停法（Early Stopping）在驗證誤差不再下降時停止訓練。'},
  ],
};

TOPIC_CONTENT['異常偵測'] = {
  en: 'Anomaly Detection',
  sections: [
    {title:'定義與類型', content:'異常偵測識別偏離正常行為模式的資料點。點異常（Point Anomaly）：單個資料點異常（如信用卡盜刷）。情境異常（Contextual Anomaly）：在特定情境下的異常（如夏季的雪天）。集體異常（Collective Anomaly）：一組資料點的聯合行為異常（如 DDoS 攻擊）。'},
    {title:'方法', content:'統計方法：假設正常資料符合某種分布，低機率值視為異常（如 Z-score、Grubbs 檢驗）。距離方法：孤立森林（Isolation Forest）隨機切割特徵空間，異常點更容易被孤立。LOF（Local Outlier Factor）比較局部密度。Autoencoder：重建誤差大的樣本為異常。'},
    {title:'應用', content:'金融：信用卡詐欺偵測、洗錢防制。網路安全：入侵偵測系統、DDoS 攻擊識別。製造業：產品品質檢測、設備故障預測。醫療：異常生理訊號監測、疾病篩檢。運維：伺服器日誌異常檢測、資源使用監控。'},
  ],
};

TOPIC_CONTENT['聚類演算法'] = {
  en: 'Clustering Algorithms',
  sections: [
    {title:'任務定義', content:'聚類是將資料集中的樣本劃分為多個簇（Cluster），使得同一簇內的樣本相似度高，不同簇間的樣本相似度低。與分類不同，聚類不使用預定義標籤，屬於非監督式學習。聚類結果可以用於探索性數據分析、預處理或特徵工程。'},
    {title:'聚類類型', content:'劃分式聚類：直接將資料劃分為 K 個不相交子集（如 K-Means、K-Medoids）。層次式聚類：凝聚式（由下而上合併）或分裂式（由上而下分裂）。密度式聚類：基於密度連通性（如 DBSCAN、OPTICS）。模型式聚類：假設資料由機率模型生成（如 GMM）。'},
    {title:'評估', content:'內部指標：輪廓係數（Silhouette Score）衡量簇內緊湊度和簇間分離度。Davies-Bouldin 指數計算簇間相似度。Calinski-Harabasz 指數基於組內和組間離差。外部指標（有真實標籤時）：調整 Rand Index（ARI）、正規化互資訊（NMI）。'},
  ],
};

TOPIC_CONTENT['K-均值'] = {
  en: 'K-Means',
  sections: [
    {title:'演算法步驟', content:'K-Means 是使用最廣泛的劃分式聚類演算法。步驟：1）隨機初始化 K 個簇中心。2）將每個樣本分配到距離最近的簇中心。3）重新計算每個簇的中心（該簇所有樣本的平均值）。4）重複步驟 2-3 直到收斂或達到最大迭代次數。'},
    {title:'K 值的選擇', content:'肘部法則（Elbow Method）：對不同 K 值計算 WCSS（組內平方和），找到轉折點。輪廓係數：選擇使平均輪廓係數最大的 K。Gap 統計量：比較資料的 WCSS 與隨機資料的 WCSS。領域知識和實際應用需求也應納入考慮。'},
    {title:'限制與改進', content:'限制：假設簇為球形且大小相近、對初始中心敏感、需要預設 K 值、對異常值敏感。改進：K-Means++ 改善初始點選擇（使初始中心盡量分散）。Mini-Batch K-Means 加速大規模聚類。ISODATA 演算法可動態調整 K 值。'},
  ],
};

TOPIC_CONTENT['主成分分析'] = {
  en: 'Principal Component Analysis',
  sections: [
    {title:'數學原理', content:'PCA 找到資料中變異最大的方向（主成分），將高維資料投影到低維空間。第一主成分是最大化資料方差的方向，第二主成分與第一正交且最大化剩餘方差。透過特徵值分解協方差矩陣或 SVD（奇異值分解）求解。'},
    {title:'降維與重建', content:'選擇前 k 個主成分保留最大變異量。累積解釋變異比例（Cumulative Explained Variance）指導 k 的選擇（通常保留 95%）。將資料投影到前 k 個主成分後，可以近似重建原始資料。重建誤差衡量資訊損失。'},
    {title:'應用', content:'資料視覺化（將高維資料投影到 2D 或 3D）。去噪（丟棄小變異的主成分相當於濾除雜訊）。特徵壓縮（減少儲存空間和計算成本）。多重共線性處理（主成分彼此正交）。人臉辨識（特徵臉 Eigenface）。異常偵測（重建誤差大的樣本）。'},
  ],
};

TOPIC_CONTENT['深度學習'] = {
  en: 'Deep Learning',
  sections: [
    {title:'階層式學習', content:'深度學習使用多層神經網路學習資料的階層式表示。低層網路學習簡單特徵（邊緣、紋理），中層學習部件特徵（眼睛、車輪），高層學習抽象概念（人臉、車輛）。端到端學習使整個系統可以聯合最佳化，無需手動設計特徵。'},
    {title:'突破與里程碑', content:'2012 年 AlexNet 在 ImageNet 以巨大優勢勝出開啟了深度學習時代。2014 年 GAN 引入對抗生成。2017 年 Transformer 徹底改變了序列建模。2020 年 GPT-3 展示了規模法則（Scaling Laws）。2022 年擴散模型在圖像生成上超越 GAN。'},
    {title:'現代深度學習', content:'硬體：GPU（NVIDIA CUDA）、TPU、NPU 為深度學習提供算力基礎。框架：PyTorch、TensorFlow、JAX 提供自動微分和 GPU 加速。架構：CNN（視覺）、RNN/Transformer（序列）、GNN（圖資料）、Diffusion（生成）。'},
  ],
};

TOPIC_CONTENT['神經網路'] = {
  en: 'Neural Network',
  sections: [
    {title:'神經元模型', content:'神經元接收多個輸入信號 xᵢ，每個信號有對應權重 wᵢ，加總後通過激活函數產生輸出。y = σ(∑wᵢxᵢ + b)。1943 年 McCulloch-Pitts 神經元使用階躍函數。1958 年 Rosenblatt 感知機引入學習規則。現代神經元使用 Sigmoid、ReLU、Tanh 等平滑的激活函數。'},
    {title:'網路架構', content:'前饋神經網路（FFNN）：資訊單向流動，從輸入層經隱藏層到輸出層。全連接層是基本構建模組。多層感知機（MLP）疊加多個全連接層和非線性激活函數。通用近似定理證明：足夠大的單隱藏層網路可以逼近任意連續函數。'},
    {title:'激活函數', content:'Sigmoid：輸出範圍 (0,1)，適合機率輸出，但梯度飽和。Tanh：輸出範圍 (-1,1)，零中心化。ReLU：max(0,x)，計算簡單、緩解梯度消失，但可能造成神經元死亡（Dead Neurons）。Leaky ReLU、ELU、Swish、GELU 是改進版本。'},
  ],
};

TOPIC_CONTENT['反傳遞演算法'] = {
  en: 'Backpropagation',
  sections: [
    {title:'鏈式法則', content:'反傳遞演算法基於微積分的鏈式法則計算損失函數對每個權重的梯度。從輸出層開始，逐層向輸入層反向傳播誤差訊號。∂L/∂w = ∂L/∂y · ∂y/∂z · ∂z/∂w，其中 z 是加權輸入，y 是激活輸出。這使得多層網路的高效訓練成為可能。'},
    {title:'訓練過程', content:'前向傳播：輸入資料由輸入層經各隱藏層計算到輸出層。計算損失。反向傳播：從輸出層向輸入層計算梯度。參數更新：使用梯度下降法或其他最佳化器更新權重。重複前向/反向/更新直到收斂。一個完整遍歷（Epoch）包含所有訓練樣本的上述過程。'},
    {title:'梯度問題', content:'梯度消失（Vanishing Gradient）：在深層網路中，梯度經多層相乘後指數級縮小，使淺層網路無法有效學習。Sigmoid 和 Tanh 在飽和區的梯度接近零。梯度爆炸（Exploding Gradient）：梯度指數級增長，導致參數更新幅度過大。Batch Normalization、殘差連接（ResNet）、ReLU 激活函數是常見解決方案。'},
  ],
};

TOPIC_CONTENT['梯度下降法'] = {
  en: 'Gradient Descent',
  sections: [
    {title:'基本原理', content:'梯度下降是最佳化的核心演算法。梯度指向函數值增加最快的方向，因此沿梯度反方向更新參數可使函數值下降。θ := θ - η∇J(θ)，其中 η 為學習率（Learning Rate），J(θ) 為目標函數。學習率過大不收斂，過小收斂慢。'},
    {title:'變體', content:'批次梯度下降（BGD）：使用全部資料計算梯度，準確但計算成本高。隨機梯度下降（SGD）：每次只用一個樣本更新，隨機性有助逃脫局部極小值。小批次梯度下降（Mini-Batch GD）：折衷方案，每次使用一個小批次（通常 32–256 個樣本）。'},
    {title:'動量與自適應方法', content:'動量（Momentum）：累積過去梯度的指數衰減平均，加速收斂並減少震盪。NAG（Nesterov Accelerated Gradient）：在動量方向上預先更新後計算梯度。AdaGrad：對稀疏特徵給予較大學習率。RMSProp：使用梯度平方的指數移動平均調整學習率。Adam：結合動量與 RMSProp，是最常用的最佳化器。'},
  ],
};

TOPIC_CONTENT['Adam'] = {
  en: 'Adam (Adaptive Moment Estimation)',
  sections: [
    {title:'演算法原理', content:'Adam 結合了 Momentum 和 RMSProp 的優點。維護一階動量（梯度均值）和二階動量（梯度平方均值）的指數移動平均。m_t = β₁m_{t-1} + (1-β₁)g_t，v_t = β₂v_{t-1} + (1-β₂)g_t²。偏差校正解決初始估計偏零的問題：m̂_t = m_t/(1-β₁ᵗ)，v̂_t = v_t/(1-β₂ᵗ)。'},
    {title:'超參數', content:'學習率 η：預設 0.001。β₁：一階動量衰減率，預設 0.9。β₂：二階動量衰減率，預設 0.999。ε：數值穩定常數，預設 10⁻⁸。Adam 對超參數相對不敏感，是許多深度學習專案的預設選擇。AdamW 將權重衰減（Weight Decay）從梯度更新中分離，提升正則化效果。'},
    {title:'優缺點', content:'優點：自適應學習率適用於不同參數、收斂速度快、對梯度縮放不敏感、適合大規模資料和稀疏梯度。缺點：泛化性能在某些情況下不如 SGD + Momentum + 適當學習率排程、記憶體需求較高（儲存一階和二階動量）、Adam 的權重衰減實作存在問題（AdamW 修正）。'},
  ],
};

TOPIC_CONTENT['卷積神經網路'] = {
  en: 'Convolutional Neural Network',
  sections: [
    {title:'卷積運算', content:'卷積神經網路的核心是卷積層，使用濾波器（Kernel）在輸入上滑動進行點積運算。濾波器共享權重，大幅減少參數量。卷積操作保持空間局部連接性，捕捉局部特徵模式。填充（Padding）控制輸出大小，步長（Stride）控制滑動間隔。'},
    {title:'池化層', content:'池化層對特徵圖進行下採樣，降低空間維度並提供平移不變性。最大池化（Max Pooling）取區域最大值，保留最顯著特徵。平均池化（Average Pooling）取區域平均值。全域平均池化（Global Average Pooling）將每個通道壓縮為單一數值，常用於分類前的最後一層。'},
    {title:'經典架構', content:'LeNet-5 (1998)：第一個成功應用的 CNN。AlexNet (2012)：ImageNet 突破，使用 ReLU 和 Dropout。VGGNet (2014)：簡單統一的 3×3 卷積堆疊。GoogLeNet/Inception (2014)：多尺度卷積並行。ResNet (2015)：殘差連接解決梯度消失，可訓練極深網路（152 層）。MobileNet：深度可分離卷積，適合行動裝置。'},
  ],
};

TOPIC_CONTENT['循環神經網路'] = {
  en: 'Recurrent Neural Network',
  sections: [
    {title:'RNN 原理', content:'循環神經網路在每個時間步同時接收當前輸入 x_t 和上一步的隱藏狀態 h_{t-1}，輸出當前隱藏狀態 h_t。h_t = tanh(W_{xh}x_t + W_{hh}h_{t-1} + b_h)。這種循環結構使 RNN 能夠保持對過去輸入的記憶，適合處理序列資料。由於梯度消失問題，簡單 RNN 難以捕捉長期依賴。'},
    {title:'LSTM', content:'長短期記憶網路（LSTM）引入遺忘閘（Forget Gate）、輸入閘（Input Gate）、輸出閘（Output Gate）和細胞狀態（Cell State）來控制資訊流。遺忘閘決定捨棄多少舊資訊，輸入閘決定存入多少新資訊，輸出閘控制輸出內容。LSTM 有效解決了長期依賴問題。'},
    {title:'GRU', content:'閘控循環單元（GRU）是 LSTM 的簡化版本，合併了遺忘閘和輸入閘為更新閘（Update Gate），並移除細胞狀態。重置閘（Reset Gate）控制忽略過去資訊的程度。GRU 參數更少、計算更快，在許多任務上表現與 LSTM 相當。Bi-RNN 雙向處理序列，捕捉前後上下文資訊。'},
  ],
};

TOPIC_CONTENT['Transformer'] = {
  en: 'Transformer',
  sections: [
    {title:'注意力機制', content:'Transformer 完全基於注意力（Attention）機制，沒有循環或卷積。縮放點積注意力（Scaled Dot-Product Attention）：Attention(Q,K,V) = softmax(QK^T/√d)V。Q、K、V 分別是查詢、鍵、值矩陣。除以 √d 防止內積過大使 softmax 梯度消失。'},
    {title:'多頭注意力', content:'多頭注意力（Multi-Head Attention）將 Q、K、V 線性投影到 h 個不同的子空間，分別計算注意力後拼接。讓模型同時關注不同位置的不同表示子空間。每個頭可以學習不同的注意力模式（如語法關係、語義相似度）。'},
    {title:'架構組件', content:'編碼器-解碼器架構。編碼器由多層自注意力（Self-Attention）和前饋網路組成，每層後接殘差連接和層歸一化。解碼器增加遮蔽自注意力（防止看到未來資訊）和編碼器-解碼器注意力。位置編碼（Positional Encoding）用正弦函數為模型提供位置資訊，因為自注意力本身不具備序列順序。BERT 使用編碼器，GPT 使用解碼器。'},
  ],
};

TOPIC_CONTENT['GPT'] = {
  en: 'GPT (Generative Pre-trained Transformer)',
  sections: [
    {title:'架構', content:'GPT 使用 Transformer 解碼器架構，透過遮蔽自注意力（Masked Self-Attention）實現自回歸生成。每個時間步只能看到當前位置之前的文本。多層解碼器堆疊形成深度網路。GPT-1 (2018) 有 12 層、1.17 億參數。GPT-3 (2020) 擴展到 96 層、1750 億參數。'},
    {title:'預訓練與微調', content:'預訓練階段使用大量未標記文字進行語言建模任務（預測下一個詞）。GPT-1 使用 BooksCorpus。GPT-3 使用 Common Crawl、WebText2、Books 等 5 個資料集。微調階段使用少量標記資料適應特定任務。GPT-3 展示了 Zero-shot 和 Few-shot 學習能力，不需微調即能完成多種任務。'},
    {title:'In-Context Learning', content:'GPT-3 及後續模型展現了上下文學習（In-Context Learning）能力。透過在提示（Prompt）中提供少數範例，模型即可理解並執行新任務，無需更新參數。思維鏈（Chain-of-Thought）提示進一步提升了推理能力。指令微調（Instruction Tuning）和 RLHF（基於人類反饋的強化學習）使模型更好地遵循人類指令。'},
  ],
};

TOPIC_CONTENT['大型語言模型'] = {
  en: 'Large Language Models',
  sections: [
    {title:'規模法則 (Scaling Laws)', content:'Kaplan 等人發現模型性能與參數量、資料量、計算量呈冪律關係。隨著模型規模增大，在足夠資料下性能持續提升，未見飽和。資料量同樣重要 — Chinchilla (2022) 提出在大模型上應使用更多訓練資料。規模法則推動了模型不斷擴大。'},
    {title:'訓練技術', content:'預訓練在大規模語料上進行無監督學習。繼續預訓練（Continued Pretraining）在特定領域資料上進一步訓練。指令微調（Instruction Tuning）使用（指令，回覆）對訓練，提升任務理解和遵循指令的能力。RLHF 透過人類偏好訓練獎賞模型，再用 PPO 最佳化語言模型。DPO 直接偏好最佳化簡化了 RLHF 流程。'},
    {title:'主要模型', content:'GPT 系列（OpenAI）：GPT-3、GPT-4、GPT-4o。Llama 系列（Meta）：Llama 2、Llama 3。Claude 系列（Anthropic）：Claude 3、Claude 4。Gemini（Google）：Gemini Pro、Gemini Ultra。Mistral（法國）：Mistral 7B、Mixtral 8x7B（MoE）。開源社群：DeepSeek、Qwen、Yi、Falcon。'},
    {title:'應用', content:'文字生成（寫作、翻譯、摘要）、程式碼生成（GitHub Copilot）、問答系統（ChatGPT、Kimi）、推理與分析（Chain-of-Thought）、Agent 系統（工具使用、多步規劃）、多模態理解（圖像+文字+音訊）。RAG（檢索增強生成）結合外部知識庫提升準確性。'},
  ],
};

TOPIC_CONTENT['多模態模型'] = {
  en: 'Multimodal Models',
  sections: [
    {title:'跨模態學習', content:'多模態模型同時處理和理解多種資訊類型。CLIP (2021) 透過對比學習將圖像和文字嵌入到共同空間，實現零樣本分類和圖文匹配。ViT（Vision Transformer）將 Transformer 應用在圖像區塊上，成為視覺 Backbone 的標準選擇。ALIGN 使用噪聲網路資料進行大規模多模態訓練。'},
    {title:'視覺語言模型', content:'GPT-4V/GPT-4o：能夠「看見」圖像並進行對話。Gemini：原生多模態（文字、圖像、音訊、影片）。LLaVA：將視覺編碼器（CLIP）與語言模型（Vicuna/Llama）通過投影層連接。Flamingo：凍結預訓練視覺和語言模型，僅訓練少量可學習的「感知器重取樣器」和「閘控交叉注意力」層。'},
    {title:'多模態生成', content:'DALL-E 3：文字到圖像生成（擴散模型 + LLM Captioner）。Midjourney：高品質圖像生成。Sora：文字到影片生成（擴散 Transformer / DiT）。Meta Movie Gen：文字到影片 + 音訊同步生成。Stable Audio：文字到音訊生成。未來的多模態模型將更緊密地整合感知、理解和生成能力。'},
  ],
};

TOPIC_CONTENT['散射網路'] = {
  en: 'Diffusion Model',
  sections: [
    {title:'前向過程', content:'散射模型的前向過程（擴散過程）逐步向資料添加高斯雜訊，經過 T 步將原始資料變成純噪聲。x_t = √(1-β_t)x_{t-1} + √β_t ε。β_t 是雜訊排程（Noise Schedule），控制每步添加的雜訊量。常見排程包含線性、餘弦、sigmoid。當 T 足夠大時，x_T ∼ N(0, I)。'},
    {title:'反向過程', content:'反向過程（去噪過程）學習從噪聲恢復資料。模型 ε_θ(x_t, t) 預測添加的噪聲，然後逐步去除。x_{t-1} = 1/√α_t (x_t - β_t/√(1-ᾱ_t) ε_θ(x_t, t)) + σ_t z。DDIM 將去噪過程改為確定性，大幅減少採樣步數（從 1000 降到 50 步）。'},
    {title:'應用', content:'圖像生成：Stable Diffusion、DALL-E 3、Midjourney 都基於擴散模型。文字到圖像生成使用文字編碼器（CLIP）作為條件。圖像到圖像（img2img）用於編輯、修復、超解析度。影片生成：Sora 使用擴散 Transformer。音訊生成：AudioLDM 用擴散模型生成音樂和音效。蛋白質生成：RF Diffusion 設計新型蛋白質結構。'},
  ],
};

TOPIC_CONTENT['生成對抗網路'] = {
  en: 'Generative Adversarial Network',
  sections: [
    {title:'對抗架構', content:'GAN 由生成器（Generator）和鑑別器（Discriminator）兩個神經網路組成。生成器從隨機噪聲 z 生成偽造資料 G(z)。鑑別器區分真實資料和生成資料。訓練目標是使生成器能欺騙鑑別器，鑑別器能正確區分兩者。這是一個零和博弈（Zero-Sum Game），理論上達到 Nash 均衡時生成器完美擬合真實資料分布。'},
    {title:'訓練挑戰', content:'模式崩潰（Mode Collapse）：生成器只學會生成少數幾種樣本。訓練不穩定：兩個網路同時訓練，損失函數的動態平衡難以維持。梯度消失：當鑑別器太強時，生成器無法獲得有效梯度。對超參數敏感。改進方案包含 WGAN（Wasserstein GAN 使用 Earth-Mover 距離）、LSGAN（最小二乘 GAN）、Spectral Normalization（譜歸一化穩定訓練）。'},
    {title:'經典變體', content:'DCGAN：使用卷積層的 GAN，奠定了穩定訓練的經驗法則。CGAN（條件 GAN）：為生成器和鑑別器都加上條件資訊（如類別標籤）。Pix2Pix：成對圖像翻譯（邊緣圖→照片）。CycleGAN：無需成對資料的圖像風格轉換（照片→梵谷風格）。StyleGAN：控制人臉圖像的風格和細節層次。BigGAN：大規模訓練生成高解析度圖像。'},
  ],
};

TOPIC_CONTENT['自然語言處理'] = {
  en: 'Natural Language Processing',
  sections: [
    {title:'核心任務', content:'文本分類：情感分析、垃圾郵件分類、新聞主題分類。命名實體識別（NER）：識別文本中的人名、地名、組織等。關聯抽取：識別實體之間的語義關係。詞性標註（POS Tagging）：為每個詞標註語法類別。句法分析：分析句子的語法結構。語意角色標註：識別謂詞的論元。'},
    {title:'表示學習', content:'Word2Vec 和 GloVe 學習靜態詞向量，每個詞只有一個固定表示。ELMo 使用雙向 LSTM 產生上下文相關的詞表示。BERT 使用 Transformer 編碼器雙向建模上下文，在 GLUE 基準上大幅超越先前方法。Sentence-BERT 產生意義連貫的句子嵌入，用於語義相似度計算。'},
    {title:'現代 NLP', content:'預訓練語言模型（PLM）：BERT、RoBERTa、ELECTRA、T5 為各種 NLP 任務提供了強大的基礎。LLM 主導：GPT-4、Claude、Llama 等大型語言模型可以透過提示直接完成多種 NLP 任務，無需任務特定的微調。RAG（檢索增強生成）結合知識庫解決 LLM 的知識時效性和幻覺問題。'},
  ],
};

TOPIC_CONTENT['電腦視覺'] = {
  en: 'Computer Vision',
  sections: [
    {title:'基礎任務', content:'圖像分類：將整張圖像分配到預定義類別，ImageNet 是標準基準資料集。物體偵測：在圖像中定位並分類多個物體，YOLO、Faster R-CNN 是代表性模型。語義分割：對圖像的每個像素進行分類，U-Net、DeepLab 是常見架構。實例分割：區分同一類別的不同物體實例，Mask R-CNN 是經典方法。'},
    {title:'視覺 Transformer (ViT)', content:'ViT 將圖像分割為固定大小的區塊（Patch），將每個區塊線性投影為嵌入向量，加上位置編碼後送入標準 Transformer 編碼器。ViT 在大規模資料（ImageNet-21k、JFT-300M）上訓練超越 CNN。Swin Transformer 引入移位視窗注意力，在密集預測任務上表現出色。DETR 使用 Transformer 端到端物體偵測。'},
    {title:'應用領域', content:'自動駕駛：車道偵測、行人識別、交通號誌辨識。醫療影像：X 光片分析、CT/MRI 病灶偵測、病理切片診斷。工業檢測：產品瑕疵檢測、裝配線監控。人臉識別：身份驗證、門禁系統。擴增實境：場景理解、物體追蹤。農業：作物健康監測、產量預估。'},
  ],
};

TOPIC_CONTENT['影像處理與識別'] = {
  en: 'Image Processing and Recognition',
  sections: [
    {title:'預處理技術', content:'濾波：高斯濾波去噪、中值濾波去椒鹽噪聲、雙邊濾波保留邊緣。直方圖均衡化增強對比度。形態學操作（膨脹、侵蝕、開運算、閉運算）用於二值圖像處理。邊緣檢測：Sobel、Canny、Laplacian 算子。特徵提取：SIFT、SURF、ORB 等局部特徵用於圖像匹配。'},
    {title:'深度學習方法', content:'CNN 端到端學習特徵提取和分類。資料擴增（Data Augmentation）：翻轉、旋轉、縮放、顏色抖動等增加訓練資料多樣性。轉移學習：使用 ImageNet 預訓練權重進行微調，大幅減少所需資料。注意力機制讓模型關注圖像的關鍵區域。Vision Transformer 為圖像分類提供了全新的架構選擇。'},
    {title:'進階應用', content:'超解析度（Super-Resolution）：從低解析度圖像重建高解析度細節。風格轉換：將一張圖像的風格應用於另一張圖像。圖像修復（Inpainting）：合理填補圖像中缺失的區域。全景分割（Panoptic Segmentation）：同時進行語義分割和實例分割。3D 視覺：點雲處理、神經渲染（NeRF）。'},
  ],
};

TOPIC_CONTENT['語音處理與識別'] = {
  en: 'Speech Processing and Recognition',
  sections: [
    {title:'語音特徵提取', content:'原始音訊訊號經過預強調、分幀、加窗後進行短時傅立葉變換（STFT），得到頻譜圖（Spectrogram）。梅爾頻譜圖（Mel-spectrogram）使用梅爾刻度模擬人耳頻率解析度。梅爾頻率倒譜係數（MFCC）是傳統語音識別中最常用的特徵。Fbank（濾波銀行特徵）在深度學習時代更受歡迎。'},
    {title:'聲學模型', content:'傳統方法：GMM-HMM 混合模型是深度學習前的語音識別標準架構。CTC（Connectionist Temporal Classification）解決輸入輸出長度不對齊問題，常用於端到端語音識別。LAS（Listen, Attend and Spell）使用注意力機制實現序列到序列的語音識別。Transducer (RNN-T) 提供流式語音識別的解決方案。Whisper 是 OpenAI 的多語言語音識別模型。'},
    {title:'語音合成與應用', content:'TTS 系統將文字轉換為語音。傳統方法：串聯合成、參數合成。深度學習方法：Tacotron（注意力序列到序列）、FastSpeech（非自回歸）、WaveNet（原始音訊生成）、HiFi-GAN（神經聲碼器）。應用包含語音助理（Siri、Google Assistant）、語音翻譯、無障礙技術、互動語音應答（IVR）。情感語音合成和語音複製是前沿研究方向。'},
  ],
};

TOPIC_CONTENT['推薦系統'] = {
  en: 'Recommender Systems',
  sections: [
    {title:'協同過濾', content:'基於用戶的協同過濾：找到與目標用戶興趣相似的其他用戶，推薦他們喜歡的項目。基於項目的協同過濾：找到與目標項目相似的其他項目，推薦給用戶。相似度計算使用 Pearson 相關係數或餘弦相似度。矩陣分解（Matrix Factorization）將用戶-項目評分矩陣分解為用戶潛在因子和項目潛在因子。'},
    {title:'基於內容的推薦', content:'根據項目的內容特徵（文字、標籤、類別等）和用戶的歷史偏好進行推薦。建立用戶配置文件（User Profile）和項目特徵向量的匹配度。不需要其他用戶的資料，沒有冷啟動問題。但推薦多樣性較低，容易陷入資訊繭房。混合方法結合協同過濾和基於內容的推薦。'},
    {title:'深度學習推薦', content:'神經協同過濾（Neural Collaborative Filtering, NCF）使用多層感知機建模用戶-項目交互。Wide & Deep 模型結合記憶力和泛化能力。DeepFM 自動學習特徵交互。DIN（Deep Interest Network）建模用戶動態興趣。多任務學習（MMOE）同時優化多個目標（點擊率、轉換率、觀看時長）。YouTube DNN 是工業級深度學習推薦系統的經典案例。'},
  ],
};

TOPIC_CONTENT['貝氏網路'] = {
  en: 'Bayesian Network',
  sections: [
    {title:'圖模型表示', content:'貝氏網路使用有向無環圖（DAG）表示隨機變量間的條件獨立關係。節點代表隨機變量，有向邊代表因果或依賴關係。每個節點有條件機率表（CPT），給定父節點狀態下的機率分布。聯合機率分解為 P(X₁,...,Xₙ) = ∏P(Xᵢ|Parents(Xᵢ))。網絡結構可以透過專家知識或結構學習演算法獲得。'},
    {title:'推論方法', content:'精確推論（Exact Inference）：變數消元法（Variable Elimination）逐一消去非查詢變數。信念傳播（Belief Propagation）在樹狀結構上進行訊息傳遞。近似推論（Approximate Inference）：蒙地卡羅方法（如吉布斯採樣）在大型網路上實用。重要性採樣（Importance Sampling）透過加權樣本近似後驗分布。'},
    {title:'學習', content:'參數學習在已知網絡結構下估計 CPT。最大似然估計（MLE）使用資料頻率。貝氏估計加入 Dirichlet 先驗。結構學習尋找最佳網絡結構，使用基於約束（條件獨立性檢驗）或基於分數（BIC、BDeu）的方法。EM演算法處理含有隱藏變數的學習問題。應用包含醫療診斷、基因網路分析、風險評估、故障診斷。'},
  ],
};

TOPIC_CONTENT['馬可夫鏈'] = {
  en: 'Markov Chain',
  sections: [
    {title:'基本概念', content:'馬可夫鏈是離散時間隨機過程，滿足馬可夫性質（無記憶性）：P(X_{t+1}|X_t, X_{t-1}, ..., X₁) = P(X_{t+1}|X_t)。未來狀態只與當前狀態有關。轉移機率矩陣 P 定義了從一個狀態到另一個狀態的機率。正則（Regular）馬可夫鏈經過足夠步數後達到平穩分布。n 步轉移機率由 Pⁿ 給出。'},
    {title:'狀態分類', content:'互通（Communicating）：兩個狀態可以互相到達。不可約（Irreducible）：所有狀態屬於同一互通類。週期性（Periodicity）：狀態返回自身的步數有固定週期。遍歷性（Ergodicity）：不可約、非週期、正常返的馬可夫鏈有唯一的平穩分布。吸收狀態（Absorbing State）：一旦進入就不會離開的狀態。'},
    {title:'平穩分布', content:'當馬可夫鏈滿足 π = πP 時，π 是平穩分布（Stationary Distribution）。從任何初始分布出發，經過足夠多次轉移後，狀態分布趨近於 π。細緻平衡條件（Detailed Balance）：πᵢP_{ij} = πⱼP_{ji}，滿足時 π 是平穩分布。PageRank 演算法將網頁重要性建模為馬可夫鏈的平穩分布。MCMC（馬可夫鏈蒙地卡羅）方法利用馬可夫鏈採樣複雜分布。'},
  ],
};

TOPIC_CONTENT['隱馬可夫模型'] = {
  en: 'Hidden Markov Model',
  sections: [
    {title:'模型定義', content:'HMM 假設存在不可觀測的隱藏狀態序列（馬可夫鏈），每個時間步的隱藏狀態產生一個可觀測的輸出。λ = (A, B, π)：A 是狀態轉移機率矩陣，B 是觀測機率矩陣，π 是初始狀態分布。三個基本問題：評估（Forward-Backward 演算法計算觀測序列機率）、解碼（Viterbi 演算法找出最可能隱藏狀態序列）、學習（Baum-Welch 演算法估計參數）。'},
    {title:'Viterbi 演算法', content:'Viterbi 使用動態規劃找出最可能的隱藏狀態序列。定義 δ_t(i) 為 t 時刻處於狀態 i 的最佳路徑機率。遞迴：δ_t(j) = max_i [δ_{t-1}(i) a_{ij}] b_j(o_t)。回溯保存的最佳狀態序列即為解碼結果。時間複雜度 O(T·N²)，其中 T 是序列長度，N 是狀態數。在語音辨識、詞性標註、生物序列分析中廣泛應用。'},
    {title:'應用', content:'語音辨識：HMM 是傳統語音辨識系統的核心，每個音素用一個 HMM 建模。詞性標註：隱藏狀態是詞性標籤，觀測是詞彙。生物資訊學：DNA 序列中編碼區/非編碼區的識別。手勢識別：隱藏狀態是手勢類型。異常偵測：正常行為用 HMM 建模，低機率序列視為異常。在深度學習時代，HMM 常與神經網路結合使用（Hybrid HMM-NN 或 CTC 模型）。'},
  ],
};

TOPIC_CONTENT['EM演算法'] = {
  en: 'Expectation-Maximization Algorithm',
  sections: [
    {title:'演算法框架', content:'EM 演算法是處理含有隱藏變數機率模型參數估計的迭代方法。E 步（Expectation）：在當前參數下計算隱藏變數的後驗機率，進而計算完整資料對數似然函數的期望（Q 函數）。M 步（Maximization）：最大化 Q 函數更新模型參數。重複 E 步和 M 步直到收斂。理論保證每次迭代提高對數似然函數值。'},
    {title:'高斯混合模型 (GMM)', content:'GMM 假設資料由 K 個高斯分布的加權混合生成，每個樣本所屬的高斯分量是隱藏變數。E 步：計算每個樣本屬於每個高斯分量的責任度（Responsibility）。M 步：根據責任度重新估計各分量的均值、協方差和權重。GMM 在聚類、密度估計、語音辨識中廣泛應用。與 K-Means 相比，GMM 提供軟分配（機率）和橢圓形簇。'},
    {title:'應用', content:'GMM 聚類：比 K-Means 更靈活，可處理不同形狀和大小。隱馬可夫模型（HMM）訓練：Baum-Welch 演算法就是 EM 的應用。缺失資料處理：EM 可以在含有缺失值的資料上估計模型參數。因子分析：潛在變數模型的參數估計。圖像分割：GMM 對像素顏色建模實現分割。基因關聯分析：EM 估計單倍型頻率。PLSA（機率潛在語義分析）：文本主題建模。'},
  ],
};

TOPIC_CONTENT['聯邦學習'] = {
  en: 'Federated Learning',
  sections: [
    {title:'核心思想', content:'聯邦學習讓多個客戶端在本地保留資料的前提下協作訓練模型。FedAvg（聯邦平均）演算法：伺服器將初始模型分發給客戶端，客戶端在本地資料上訓練數輪後將模型更新傳回，伺服器聚合平均更新。這種方法保護資料隱私，因為原始資料從未離開客戶端。通訊效率是主要挑戰之一。'},
    {title:'挑戰', content:'統計異質性：客戶端資料分布不獨立同分布（Non-IID），如不同手機用戶的照片風格各異。系統異質性：客戶端硬體、網路狀況、可用電量差異大。通訊效率：頻繁傳送模型更新耗費頻寬。隱私保護：模型更新仍可能洩漏部分資訊，需結合差分隱私（Differential Privacy）或安全多方計算（SMPC）。容錯性：客戶端可能隨時離線。'},
    {title:'方法與應用', content:'FedProx：在 FedAvg 中加入近端項處理 Non-IID 資料。SCAFFOLD：使用方差降低技術加速收斂。FEDL：結合模型壓縮減少通訊量。個性化聯邦學習（pFL）：為每個客戶端建立個人化模型。應用：行動鍵盤預測（Google Gboard）、智慧醫療（不同醫院的協作診斷）、金融風控、IoT 設備預測維護、推薦系統（保護用戶隱私的個人化推薦）。'},
  ],
};


TOPIC_CONTENT['電腦下棋'] = {
  en: 'Game AI / Computer Chess',
  sections: [
    {title:'搜尋演算法', content:'Minimax 演算法：在博弈樹中模擬雙方最優對弈，假設對手會最小化我方效用。Alpha-Beta 剪枝透過剪除不可能影響最終決策的分支大幅減少搜尋空間。MCTS（蒙地卡羅樹搜尋）：透過隨機模擬估計節點價值，在搜尋過程中平衡探索與利用。AlphaGo 結合 MCTS 和深度神經網路，以 4-1 擊敗世界冠軍李世石。'},
    {title:'AlphaZero', content:'AlphaZero 完全從自我對弈（Self-Play）中學習，不使用任何人類棋譜或領域知識。神經網路同時輸出策略（Policy）和價值（Value）。MCTS 使用神經網路的策略引導搜尋，價值評估葉節點。訓練目標是最小化策略損失和價值損失。AlphaZero 在圍棋、西洋棋、將棋上都達到了超人類水準。MuZero 進一步在無需環境模型的情況下學習遊戲規則。'},
    {title:'應用', content:'經典桌遊：圍棋（AlphaGo/AlphaZero）、西洋棋（Stockfish + NNUE）、將棋（AlphaZero）、五子棋（Gomoku AI）。電子遊戲：Atari 遊戲（DQN）、即時策略遊戲（AlphaStar for StarCraft II）、MOBA（OpenAI Five for Dota 2）。撲克：Libratus 和 Pluribus 在德州撲克中擊敗人類職業選手。'},
  ],
};

TOPIC_CONTENT['世界模型'] = {
  en: 'World Models',
  sections: [
    {title:'認知架構', content:'世界模型讓 Agent 學習環境的內部模型，能夠預測行為的未來結果。Dreamer 演算法：1）學習世界模型（從歷史序列預測未來）。2）在學習的潛在空間中進行行為學習（Actor-Critic）。3）使用想像軌跡（Imagined Trajectories）更新策略。基於模型的強化學習（MBRL）比無模型方法樣本效率更高。'},
    {title:'方法', content:'DreamerV1/V2/V3：在潛在空間中學習世界模型，使用重建損失、獎賞預測、折扣預測等多個目標。DayDreamer：將 Dreamer 應用到真實機器人上。I2A（Imagination-Augmented Agents）：將想像路徑編碼後饋入策略網路。Plan2Explore：透過內在動機驅動的探索學習世界模型，再進行規劃。JEPA（聯合嵌入預測架構）：Yann LeCun 提出的世界模型架構，在表示空間中進行預測。'},
    {title:'應用與展望', content:'機器人控制：機械臂操作、移動機器人導航。自動駕駛：預測交通參與者的行為。遊戲 AI：在《我的世界》中學習複雜技能。影片預測：預測未來的影格序列。強化學習加速：學習的世界模型取代真實環境進行策略最佳化。世界模型被認為是邁向通用 AI 的關鍵組件，讓 AI 能像人類一樣想像和規劃。'},
  ],
};

TOPIC_CONTENT['爬山演算法'] = {
  en: 'Hill Climbing',
  sections: [
    {title:'演算法原理', content:'爬山演算法是最簡單的局部搜尋最佳化方法。從隨機初始解出發，每次向鄰域中目標函數值更好的方向移動，直到無法改善（到達峰值或谷底）。優點是簡單快速，缺點是容易陷入局部最佳解。收斂速度和最終品質高度依賴初始解的位置。在解決旅行商問題、八皇后問題等 NP 問題時常被用作基線方法。'},
    {title:'變體', content:'簡單爬山法（Steepest Ascent）：探索所有鄰居後選擇最好的移動。隨機爬山法（Stochastic）：從改善移動中隨機選擇一個。首次改善法（First-choice）：找到第一個改善移動就執行。隨機重啟爬山法：從不同的隨機初始點重新爬山，取最佳結果。模擬退火（Simulated Annealing）是爬山演算法的推廣，允許以逐漸降低的機率接受更差的解。'},
    {title:'與機器學習的關聯', content:'梯度下降法可以視為爬山演算法在連續空間中的推廣（沿梯度方向而非離散鄰域移動）。神經網路的訓練本質上是高維空間中的最佳化問題。SGD 的隨機性能幫助逃脫不良局部極小值。超參數最佳化（如隨機搜尋、網格搜尋、貝氏最佳化）也是爬山思想的應用。遺傳演算法和粒子群最佳化是更複雜的全域搜尋方法。'},
  ],
};

TOPIC_CONTENT['搜尋法'] = {
  en: 'Search Algorithms',
  sections: [
    {title:'暴力搜尋', content:'廣度優先搜尋（BFS）：逐層擴展，保證找到最短路徑，但記憶體需求高。深度優先搜尋（DFS）：沿一條路徑走到黑，記憶體需求低但可能陷入無限深路徑。疊代加深深度優先搜尋（IDDFS）結合 BFS 的最優性和 DFS 的空間效率。雙向搜尋同時從起點和終點進行 BFS，大幅減少搜尋空間。'},
    {title:'啟發式搜尋', content:'A* 演算法結合實際距離和估計距離（啟發函數 h(n)）來引導搜尋。只要 h(n) 是可容許的（不高估實際成本），A* 保證找到最優解。貪婪最佳優先搜尋只用啟發函數，速度快但不保證最優。IDA*（迭代加深 A*）在記憶體受限環境下實現 A* 的效果。在機器學習中，A* 被用於序列預測（如 Beam Search 的變體）。'},
    {title:'與機器學習的關係', content:'Beam Search：在序列生成模型中（如 GPT），從多個候選路徑中保留機率最高的 k 個，是寬度受限的啟發式搜尋。MCTS 是 AlphaZero 等遊戲 AI 的核心。特徵選擇、超參數最佳化本質上是搜尋問題。貝氏最佳化在超參數搜尋中結合了機率模型和效用函數。神經網路結構搜尋（NAS）在架構空間中搜尋最佳網路設計。對抗性搜尋應用於生成對抗攻擊樣本。'},
  ],
};

TOPIC_CONTENT['PyTorch'] = {
  en: 'PyTorch',
  sections: [
    {title:'核心概念', content:'Tensor：PyTorch 的基本資料結構，類似 NumPy 陣列但支援 GPU 加速。自動微分（Autograd）：透過記錄計算圖自動計算梯度，是深度學習訓練的基礎。動態計算圖：每次前向傳播時建構新的計算圖，靈活且易於除錯。torch.nn 模組提供神經網路層的建構積木。torch.optim 提供各種最佳化器（SGD、Adam、AdamW）。'},
    {title:'訓練流程', content:'1）定義模型架構（繼承 nn.Module）。2）定義損失函數和最佳化器。3）迭代資料：前向傳播計算預測和損失，反向傳播計算梯度，最佳化器更新參數。4）驗證評估。DataLoader 提供批次載入和資料打亂。GPU 加速只需 .to(\"cuda\")。分散式訓練（DDP）支援多 GPU 訓練。混合精度訓練（AMP）使用 float16 加速並減少記憶體。'},
    {title:'生態系統', content:'torchvision：影像資料集、模型（ResNet、ViT）、轉換工具。torchaudio：音訊處理工具。torchtext：NLP 資料處理。HuggingFace Transformers：基於 PyTorch 的預訓練模型庫。PyTorch Lightning：高階訓練框架減少樣板代碼。TensorBoard 整合用於訓練可視化。PyTorch 在學術界和研究領域佔據主導地位。TorchScript 和 TorchServe 支持生產部署。'},
  ],
};

TOPIC_CONTENT['TensorFlow'] = {
  en: 'TensorFlow',
  sections: [
    {title:'核心架構', content:'TensorFlow 最初使用靜態計算圖（定義後執行），TensorFlow 2.0 轉向 Eager Execution（動態圖，類似 PyTorch）。Keras 是 TensorFlow 的高階 API，提供使用者友善的模型建構介面。tf.data 提供高效的資料管線。SavedModel 格式用於模型序列化和部署。TensorBoard 是強大的可視化工具。TF Serving 和 TF Lite 支援伺服器和行動端部署。'},
    {title:'分散式訓練', content:'TensorFlow 支援多種分散式策略：MirroredStrategy（單機多 GPU）、MultiWorkerMirroredStrategy（多機多 GPU）、TPUStrategy（Google TPU）。Parameter Server 架構處理超大規模模型訓練。All-Reduce 演算法（NCCL）高效同步梯度。TFX（TensorFlow Extended）是全端 ML 生產管線。'},
    {title:'應用', content:'TensorFlow 在工業生產環境中廣泛部署。Google 內部大量使用 TensorFlow（搜尋、廣告、翻譯）。TPU（Tensor Processing Unit）是 Google 專為 TensorFlow 設計的 AI 加速晶片。TensorFlow Lite 在行動裝置和邊緣裝置上執行 ML 模型。TensorFlow.js 在瀏覽器中訓練和執行神經網路。TensorFlow Extended（TFX）提供完整的 ML 管線管理解決方案。'},
  ],
};

TOPIC_CONTENT['scikit-learn'] = {
  en: 'scikit-learn',
  sections: [
    {title:'簡介', content:'scikit-learn 是 Python 生態系統中最廣泛使用的機器學習經典庫，基於 NumPy、SciPy 和 matplotlib 構建。提供統一的 API（fit/predict/transform）和豐富的演算法庫。資料預處理、特徵工程、模型選擇、評估指標、管線（Pipeline）一站式完成。BSD 開源授權，社群活躍，文件完善。'},
    {title:'主要功能', content:'分類：SVM、隨機森林、KNN、樸素貝葉斯、邏輯斯諦回歸。回歸：線性回歸、Ridge、Lasso、Elastic Net。聚類：K-Means、DBSCAN、階層式聚類、GMM。降維：PCA、t-SNE、Truncated SVD。模型選擇：交叉驗證、GridSearchCV、RandomizedSearchCV。評估指標：準確率、精確率、召回率、F1、MSE、R²。管線（Pipeline）將多個步驟串聯。'},
    {title:'最佳實務', content:'使用 StandardScaler 或 MinMaxScaler 標準化特徵。OneHotEncoder 處理類別特徵。使用 train_test_split 劃分資料。K-fold 交叉驗證評估模型穩定性。GridSearchCV 或 RandomizedSearchCV 進行超參數調優。管線（Pipeline）確保預處理步驟在交叉驗證中正確應用。ColumnTransformer 處理不同類型的特徵。'},
  ],
};

TOPIC_CONTENT['Gym'] = {
  en: 'Gym (Gymnasium)',
  sections: [
    {title:'環境介面', content:'Gym（現為 Gymnasium）提供了標準化的強化學習環境介面。核心 API：reset() 重置環境回傳初始觀測；step(action) 執行動作並回傳 (observation, reward, terminated, truncated, info)。觀測可以是向量（CartPole）或圖像（Atari）。動作空間可以是離散（左/右）或連續（關節角度）。環境渲染支援人類可視化或 RGB 陣列。'},
    {title:'內建環境', content:'經典控制：CartPole（平衡桿子）、MountainCar（爬山）、Acrobot（擺盪）、Pendulum（擺錘）。Atari 遊戲：Breakout、Pong、Space Invaders 等 60+ 經典遊戲（使用 Arcade Learning Environment）。MuJoCo：Walker2d、Hopper、HalfCheetah、Ant、Humanoid 等連續控制機器人環境。Box2D：BipedalWalker、LunarLander。Toy Text：FrozenLake、Taxi、CliffWalking。'},
    {title:'自訂環境', content:'繼承 gym.Env 類別，實作 reset() 和 step() 方法。定義 observation_space 和 action_space 為 Space 物件（Discrete、Box、MultiDiscrete、Dict）。可選擇實作 render() 方法支援可視化。註冊環境後可使用 gym.make() 建立。客製化環境廣泛應用於機器人控制、推薦系統、資源排程、對話管理等領域的強化學習研究。Wrappers 模式可以輕鬆修改環境行為（如幀堆疊、獎賞縮放、時間限制）。'},
  ],
};

TOPIC_CONTENT['microgpt'] = {
  en: 'microgpt',
  sections: [
    {title:'極簡實作', content:'microgpt 是 Andrej Karpathy 創作的極簡 GPT 實作，僅用約 200 行純 Python 程式碼（無 PyTorch/TensorFlow 之外的外部依賴），從頭實現了 GPT 的訓練和推理。包含 Transformer 的所有核心組件：Token Embedding、Positional Encoding、Multi-Head Self-Attention、Feed-Forward Network、Layer Normalization。在 CPU 上即可訓練一個字元級語言模型。'},
    {title:'教學價值', content:'microgpt 是學習 Transformer 架構的極佳教材。將複雜的 GPT 概念精簡為可讀性極高的原始碼。逐行對應 Transformer 論文的「Attention Is All You Need」。展示了為什麼要使用 LayerNorm（而非 BatchNorm）、為什麼需要殘差連接、因果注意力如何實現自回歸生成。Karpathy 的 YouTube 教學影片與程式碼配對，逐步解釋每個元件的原理。'},
    {title:'擴展', content:'nanoGPT (2023)：Karpathy 在 microgpt 基礎上的 PyTorch 實作，支援 GPU 訓練和 OpenWebText 資料集。更實用但仍保持簡潔（約 300 行核心程式碼）。支援權重初始化、學習率排程、梯度累積、混合精度訓練等實戰技術。在單 GPU 上可以訓練小規模 GPT 模型。microgpt 的精神影響了許多教學性質的深度學習專案。'},
  ],
};

TOPIC_CONTENT['mini-openclaw'] = {
  en: 'mini-openclaw',
  sections: [
    {title:'極簡 Agent', content:'mini-openclaw 是一個用約 400 行 Python 實現的極簡 AI Agent，模擬 Claude/OpenClaw 的設計理念。核心功能包含：LLM 對話管理（支援 OpenAI API 和 Ollama 本地模型）、工具呼叫（Tool Use）機制、多 Agent 協作架構、對話記憶管理、任務排程。展示了現代 AI Agent 系統的核心設計模式。'},
    {title:'架構', content:'Agent 循環：接收用戶輸入 → LLM 思考輸入 → 決定直接回答或使用工具 → 執行工具並觀察結果 → 繼續思考直到完成。工具以 JSON Schema 定義，Agent 可以動態調用。記憶管理：最近 N 輪對話記憶 + 摘要記憶。多 Agent 模式：經理 Agent 分配任務給子 Agent，子 Agent 執行後回報結果。支援平行工具執行和依賴管理。'},
    {title:'教學價值', content:'mini-openclaw 將複雜的 Agent 概念簡化為可讀性極高的教學程式碼。展示了 LLM Agent 的核心設計模式：ReAct（Reasoning + Acting）循環、工具呼叫、記憶管理、多 Agent 協作。適合學習現代 AI Agent 系統的原理和實作，也是實驗新 Agent 架構的基礎框架。專案包含詳細的文件和測試案例。'},
  ],
};

TOPIC_CONTENT['Geoffrey Hinton'] = {
  en: 'Geoffrey Hinton',
  sections: [
    {title:'學術生涯', content:'Geoffrey Hinton（1947–）是英國出生的加拿大電腦科學家，被譽為「深度學習之父」。1986 年共同發表了反傳遞演算法（Backpropagation）的現代版本。2012 年帶領學生 Alex Krizhevsky 和 Ilya Sutskever 開發 AlexNet，在 ImageNet 比賽中大幅領先，開啟了深度學習革命。2013 年加入 Google，2018 年獲得圖靈獎。2023 年離開 Google以公開討論 AI 風險。'},
    {title:'重要貢獻', content:'反傳遞演算法（1986）使多層神經網路的訓練成為可能。徑向基函數網路（RBF Networks）。受限波茲曼機（RBM）和深度信念網路（DBN）的對比分歧（Contrastive Divergence）訓練演算法（2006）。Dropout 正則化技術（2012）防止神經網路過擬合。膠囊網路（Capsule Network）試圖克服 CNN 的限制。在語音辨識和影像識別領域的開創性應用。'},
    {title:'觀點與影響', content:'Hinton 長期相信大規模神經網路是實現 AI 的正確道路，在符號 AI 主導的年代堅持這一觀點。他認為反向傳播演算法雖然成功但可能不是大腦的學習方式，呼籲探索新的學習機制。2023 年他公開表達對 AI 可能帶來的生存風險的擔憂。作為超過 200 名博士生的導師，培養了深度學習領域的許多領軍人物，包括 Ilya Sutskever、Alex Krizhevsky、Yann LeCun（博士後）、Ruslan Salakhutdinov。'},
  ],
};

TOPIC_CONTENT['Yann LeCun'] = {
  en: 'Yann LeCun',
  sections: [
    {title:'學術生涯', content:'Yann LeCun（1960–）是法國裔美國電腦科學家，被譽為「卷積神經網路之父」。1989 年在貝爾實驗室提出卷積神經網路 LeNet，用於手寫郵遞區號辨識，這是 CNN 的第一個實際應用。1998 年發表了 LeNet-5 架構。2003 年成為紐約大學教授。2013 年加入 Facebook 擔任首席 AI 科學家，建立 FAIR（Facebook AI Research）。2018 年獲得圖靈獎。2024 年離開 Meta。'},
    {title:'重要貢獻', content:'卷積神經網路（CNN）的早期發展和推廣。權重共享（Weight Sharing）和局部感受野（Local Receptive Fields）的核心想法。LeNet-5 網路架構（卷積→池化→卷積→池化→全連接）。光字元辨識（OCR）的商業應用。壓縮表示學習（Compressed Representation Learning）。JEPA（Joint Embedding Predictive Architecture）世界模型架構。能量模型（Energy-Based Models）理論框架。對比學習在自監督表示學習中的應用。'},
    {title:'觀點', content:'LeCun 一直是監督式學習的批評者，倡導自監督學習和世界模型作為邁向通用 AI 的關鍵。他反對強化學習是實現智慧的必經之路，認為世界模型和內在動機更重要。對 AI 生存風險的觀點比其他 AI 領袖更樂觀，認為擔憂遙遠的風險會分散對近期問題（偏見、隱私、錯誤資訊）的關注。他提出了一個個人化的「Cognitive Architecture」作為 AI 研究方向。'},
  ],
};

TOPIC_CONTENT['Yoshua Bengio'] = {
  en: 'Yoshua Bengio',
  sections: [
    {title:'學術生涯', content:'Yoshua Bengio 是加拿大電腦科學家，蒙特婁大學教授，Mila（蒙特婁學習演算法研究所）創辦人兼科學總監。與 Hinton 和 LeCun 並稱為「深度學習三巨頭」，共同獲得 2018 年 ACM 圖靈獎。他的實驗室是全球最大的深度學習學術研究機構之一，培養了許多 AI 領域的頂尖人才。長期擔任 NeurIPS 大會主席和程式委員會成員。'},
    {title:'重要貢獻', content:'序列模型中的機率語言模型和神經機器翻譯。注意力機制的前期工作（與機器翻譯結合）。生成對抗網路（GAN）的理論分析。變分自編碼器（VAE）的理論貢獻。表徵學習和轉移學習。深層架構的訓練困難研究。元學習（Learning to Learn）的理論基礎。在醫療 AI 和氣候變化應用方面的領導工作。對 AI 安全性和倫理學的積極倡導。'},
    {title:'觀點與倡導', content:'Bengio 對 AI 的安全性和可解釋性問題特別重視，倡導負責任的 AI 發展。他呼籲更多資源投入 AI 安全研究，並提出了基於因果關係的 AI 系統設計理念。認為深度學習需要突破性的理論進步才能實現真正的通用智慧。積極參與 AI 政策制定，是 Montreal Declaration for Responsible AI 的主要推動者。在 COVID-19 疫情期間領導 AI 加速藥物發現的研究。'},
  ],
};

TOPIC_CONTENT['Andrej Karpathy'] = {
  en: 'Andrej Karpathy',
  sections: [
    {title:'職業生涯', content:'Andrej Karpathy 是當代最具影響力的深度學習研究者和教育家之一。史丹福大學博士（李飛飛指導），論文主題為卷積神經網路在視覺和語言中的應用。2015 年共同創辦 OpenAI。2017 年加入 Tesla 擔任 AI 總監，領導 Autopilot 自動駕駛系統的電腦視覺團隊。2023 年回歸 OpenAI。2024 年離開 OpenAI 後專注於個人專案和 AI 教育。以 YouTube 頻道和部落格文章深入淺出地教授深度學習。'},
    {title:'教育貢獻', content:'史丹福大學 CS231n（卷積神經網路與視覺辨識）課程是全球知名的深度視覺課程，課程影片和講義在網路上廣泛流傳。Zero-to-Hero 系列 YouTube 影片從微積分/梯度開始逐步講解到 GPT 訓練。microgpt（200 行 Python 實現 GPT）和 nanoGPT（實用級 PyTorch GPT 實作）是學習 Transformer 的經典教材。C 語言版的 GPT 實現（llm.c）展示了從零開始的極致簡潔。他的教學理念是「不要只使用函式庫，從原理理解一切」。'},
    {title:'技術貢獻', content:'在 Tesla 領導了純視覺（純相機）自動駕駛方案，去除了雷達感測器。設計了 Tesla 的 HydraNet 多任務學習網路架構。OpenAI 早期成員，參與了 GPT-1 和 GPT-2 的研究。開發了 ImageNet 分類的 ConvNetJS 瀏覽器內深度學習框架。Arxiv Sanity 預印本推薦系統。Rust 和 C 語言的積極倡導者，展示了低層語言在 AI 應用中的可能性。'},
  ],
};

function findCodeForTopic(topicName) {
  var map = {
    '機器學習的歷史':['ml/regression/linear.py','ml/classify/knn.py'],
    '監督式學習':['ml/regression/linear.py','ml/classify/svm.py','ml/classify/knn.py','ml/classify/decision_tree.py','ml/sklearn/iris_classify.py'],
    '非監督式學習':['ml/cluster/kmean.py','ml/cluster/dbscan.py','ml/pca/pca.py'],
    '強化學習':['reinforce/cartpole/cartpoleAllMethod/cartpole.py','reinforce/walker2d/train.py','reinforce/bipedalwalker/train.py','game/breakout/train.py','reinforce/qtable/frozenlake.py'],
    '生成式AI':['image/mnist_gan_gen/train.py','image/mnist_diff_gen/train.py','nn/microgpt/microgpt.py','mini-llm/v1-pretrain/mini-llm.py'],
    '模型評估':[],
    '損失函數':[],
    '分類演算法':['ml/classify/knn.py','ml/classify/logistic.py','ml/classify/svm.py','ml/classify/decision_tree.py','ml/classify/naive_bayes.py','ml/sklearn/iris_classify.py'],
    '線性回歸':['ml/regression/linear.py','ml/regression/polynomial.py','ml/regression/ridge.py','ml/regression/lasso.py'],
    'K-近鄰':['ml/classify/knn.py'],
    '決策樹':['ml/classify/decision_tree.py'],
    '隨機森林':[],
    '支持向量機':['ml/classify/svm.py'],
    '樸素貝葉斯':['ml/classify/naive_bayes.py','prob_model/bayesnet/bayesnet.py'],
    '正則化':['ml/regression/ridge.py','ml/regression/lasso.py'],
    '異常偵測':[],
    '聚類演算法':['ml/cluster/kmean.py','ml/cluster/dbscan.py','ml/cluster/hierarchical.py','ml/cluster/gmm.py','ml/sklearn/cluster.py'],
    'K-均值':['ml/cluster/kmean.py'],
    '主成分分析':['ml/pca/pca.py'],
    '深度學習':['nn/nn0/nn0py/nn0.py','nn/nn1tensor/tensor.py','nn/nn2kv/chargpt.py','nn/nn0/nn0c/nn0.c','nn/nn0/nn0rs/src/main.rs'],
    '神經網路':['nn/nn0/nn0py/nn0.py','nn/nn0/nn0js/nn0.js','nn/nn0/nn0c/nn0.c','nn/nn0/nn0rs/src/nn0.rs','nn/nn1tensor/nn.py'],
    '反傳遞演算法':['nn/nn0/nn0py/nn0.py','nn/nn0/nn0py/examples/ex1-grad.py','nn/nn1tensor/tensor.py'],
    '梯度下降法':['nn/gd/gd.py','nn/gd/gdRegression.py','ml/regression/linear.py'],
    'Adam':[],
    '卷積神經網路':['nn/nn0/nn0py/cnn0.py','image/mnist_recognize/train.py'],
    '循環神經網路':[],
    'Transformer':['nn/nn0/nn0py/gpt0.py','nn/microgpt/microgpt.py','nn/nn1tensor/gpt.py','nn/nn2kv/chargpt.py'],
    'GPT':['nn/nn0/nn0py/gpt0.py','nn/microgpt/microgpt.py','nn/nn1tensor/main.py','nn/nn2kv/chargpt.py','mini-llm/v1-pretrain/mini-llm.py'],
    '大型語言模型':['mini-llm/v1-pretrain/mini-llm.py','mini-llm/v2-finetune/model.py','mini-llm/v2-finetune/pretrain.py','mini-llm/v2-finetune/finetune.py','mini-llm/v3-distill/gen_data_distill.py'],
    '多模態模型':[],
    '散射網路':['image/mnist_diff_gen/train.py'],
    '生成對抗網路':['image/mnist_gan_gen/train.py'],
    '自然語言處理':['nn/nn0/nn0py/gpt0.py','nn/microgpt/microgpt.py','mini-llm/v1-pretrain/mini-llm.py'],
    '電腦視覺':['image/mnist_recognize/train.py','nn/nn0/nn0py/cnn0.py'],
    '影像處理與識別':['image/mnist_recognize/train.py','image/mnist_gan_gen/train.py','image/mnist_diff_gen/train.py'],
    '語音處理與識別':['voice/supertonic/tts.py'],
    '推薦系統':[],
    '貝氏網路':['prob_model/bayesnet/bayesnet.py'],
    '馬可夫鏈':['prob_model/markov/markov_pagerank.py','music/musicgen.py'],
    '隱馬可夫模型':['prob_model/hmm/viterbi_pos_tagging.py'],
    'EM演算法':['prob_model/em/em_gmm.py'],
    '聯邦學習':[],
    '電腦下棋':['game/gomoku/alphazero/train.py','game/gomoku/classical/gomoku.py','game/tictactoe/alphazero/train.py','game/tictactoe/qlearning/train.py'],
    '世界模型':[],
    '爬山演算法':[],
    '搜尋法':[],
    'PyTorch':['nn/nn0/nn0py/nn0.py','nn/nn1tensor/tensor.py','nn/nn2kv/chargpt.py','mini-llm/v1-pretrain/mini-llm.py','image/mnist_recognize/train.py'],
    'TensorFlow':[],
    'scikit-learn':['ml/sklearn/classify.py','ml/sklearn/cluster.py','ml/sklearn/iris_classify.py'],
    'Gym':['game/breakout/train.py','reinforce/cartpole/cartpoleAllMethod/cartpole.py','reinforce/walker2d/train.py','reinforce/qtable/frozenlake.py'],
    'microgpt':['nn/microgpt/microgpt.py'],
    'mini-openclaw':['agent/v0-chat/chat0.py','agent/v2-agent-xml/agent0.py','agent/v5-agent-team/agent0team.py'],
    'Geoffrey Hinton':['nn/nn0/nn0py/nn0.py'],
    'Yann LeCun':['nn/nn0/nn0py/cnn0.py'],
    'Yoshua Bengio':[],
    'Andrej Karpathy':['nn/microgpt/microgpt.py','nn/nn0/nn0py/nn0.py'],
  };
  return map[topicName] || [];
}

var curTab = '概念';
var curTopic = null;

function showCard(item) {
  var h = '<div class="card" onclick="openTopic(\'' + item.name.replace(/'/g, "\\'") + '\')">';
  h += '<h3>' + item.name + '</h3>';
  if (item.desc) h += '<p>' + item.desc + '</p>';
  if (item.tags) {
    h += '<div class="tags">';
    for (var i = 0; i < item.tags.length; i++) {
      h += '<span class="tag">' + item.tags[i] + '</span>';
    }
    h += '</div>';
  }
  var codeLinks = findCodeForTopic(item.name);
  if (codeLinks.length > 0) {
    h += '<div class="code-links">';
    for (var j = 0; j < Math.min(codeLinks.length, 3); j++) {
      var shortName = codeLinks[j].split('/').pop();
      h += '<a href="' + CODE + '/' + codeLinks[j] + '" target="_blank">' + shortName + '</a>';
    }
    h += '</div>';
  }
  h += '</div>';
  return h;
}

function renderConcepts() {
  var html = '';
  var keys = Object.keys(TOPICS);
  for (var k = 0; k < keys.length; k++) {
    var key = keys[k];
    var items = TOPICS[key];
    var count = 0;
    for (var i = 0; i < items.length; i++) {
      if (items[i]._hidden) continue;
      count++;
    }
    if (count === 0) continue;
    html += '<h2 class="section-title">' + key + ' <span style="font-size:0.8rem;color:var(--text-secondary);font-weight:400">(' + count + ')</span></h2><div class="grid">';
    for (var i = 0; i < items.length; i++) {
      if (items[i]._hidden) continue;
      html += showCard(items[i]);
    }
    html += '</div>';
  }
  document.getElementById('content').innerHTML = html;
}

function renderCodes() {
  var html = '<h2 class="section-title">程式碼範例 <span style="font-size:0.8rem;color:var(--text-secondary);font-weight:400">(' + CODES.length + ' 個專案)</span></h2>';
  html += '<div class="grid">';
  for (var i = 0; i < CODES.length; i++) {
    var c = CODES[i];
    html += '<div class="card" style="cursor:default">';
    html += '<h3>' + c.name + '</h3>';
    if (c.desc) html += '<p>' + c.desc + '</p>';
    html += '<div class="code-links">';
    html += '<a href="' + CODE + '/' + c.path + '" target="_blank">📁 目錄</a>';
    for (var j = 0; j < c.files.length; j++) {
      html += '<a href="' + CODE + '/' + c.path + '/' + c.files[j] + '" target="_blank">' + c.files[j] + '</a>';
    }
    html += '</div>';
    html += '</div>';
  }
  html += '</div>';
  document.getElementById('content').innerHTML = html;
}

function renderReader(topicName) {
  var topic = null;
  var keys = Object.keys(TOPICS);
  for (var k = 0; k < keys.length; k++) {
    for (var i = 0; i < TOPICS[keys[k]].length; i++) {
      if (TOPICS[keys[k]][i].name === topicName) {
        topic = TOPICS[keys[k]][i];
        break;
      }
    }
    if (topic) break;
  }
  if (!topic) { renderConcepts(); return; }

  var content = TOPIC_CONTENT[topicName];
  var html = '<div class="reader-nav">';
  html += '<button class="back-btn" onclick="closeReader()">← 回到主題列表</button>';
  if (topic.file) {
    html += '<a href="' + WIKI + '/' + topic.file + '" target="_blank"><button>📄 Wiki 原文</button></a>';
  }
  html += '</div>';

  html += '<div class="reader-header">';
  html += '<h1>' + topic.name + '</h1>';
  if (content && content.en) html += '<div class="en-name">' + content.en + '</div>';
  if (topic.tags) {
    html += '<div class="meta">標籤：' + topic.tags.join(', ') + '</div>';
  }
  html += '</div>';

  html += '<div class="reader-content">';
  if (content && content.sections) {
    for (var s = 0; s < content.sections.length; s++) {
      var sec = content.sections[s];
      html += '<h2>' + sec.title + '</h2>';
      html += '<p>' + sec.content + '</p>';
    }
  }
  html += '</div>';

  var codeLinks = findCodeForTopic(topicName);
  if (codeLinks.length > 0) {
    html += '<div class="reader-footer">';
    html += '<h3>相關程式碼</h3>';
    html += '<div class="external-links">';
    for (var j = 0; j < codeLinks.length; j++) {
      var shortName = codeLinks[j].split('/').pop();
      html += '<a href="' + CODE + '/' + codeLinks[j] + '" target="_blank">📄 ' + shortName + '</a>';
    }
    html += '</div>';
    html += '</div>';
  }

  var related = findRelated(topicName);
  if (related.length > 0) {
    html += '<div class="reader-footer">';
    html += '<h3>相關主題</h3>';
    html += '<div class="related-tags">';
    for (var r = 0; r < related.length; r++) {
      html += '<a href="javascript:void(0)" onclick="openTopic(\'' + related[r].replace(/'/g, "\\'") + '\')">' + related[r] + '</a>';
    }
    html += '</div>';
    html += '</div>';
  }

  document.getElementById('content').innerHTML = html;
  curTopic = topicName;
  window.scrollTo({ top: 0, behavior: 'smooth' });
}

function findRelated(name) {
  var related = [];
  var myTags = CATEGORY_TAGS[name] || [];
  if (myTags.length === 0) return [];
  var keys = Object.keys(CATEGORY_TAGS);
  for (var i = 0; i < keys.length; i++) {
    if (keys[i] === name) continue;
    var tags = CATEGORY_TAGS[keys[i]];
    for (var j = 0; j < tags.length; j++) {
      if (myTags.indexOf(tags[j]) !== -1) {
        related.push(keys[i]);
        break;
      }
    }
  }
  return related.slice(0, 6);
}

function openTopic(name) {
  curTopic = name;
  renderReader(name);
}

function closeReader() {
  curTopic = null;
  render(curTab);
}

function render(tab) {
  curTab = tab;
  if (tab === '概念') renderConcepts();
  else renderCodes();
}

function doSearch() {
  var q = document.getElementById('search').value.toLowerCase().trim();
  if (q === '') {
    render(curTab);
    return;
  }
  if (curTab === '概念') {
    var keys = Object.keys(TOPICS);
    for (var k = 0; k < keys.length; k++) {
      for (var i = 0; i < TOPICS[keys[k]].length; i++) {
        var item = TOPICS[keys[k]][i];
        var match = item.name.toLowerCase().indexOf(q) !== -1 ||
                    (item.desc && item.desc.toLowerCase().indexOf(q) !== -1) ||
                    (item.tags && item.tags.join(' ').toLowerCase().indexOf(q) !== -1);
        item._hidden = !match;
      }
    }
    renderConcepts();
  } else {
    var html = '<h2 class="section-title">搜尋結果</h2><div class="grid">';
    var count = 0;
    for (var i = 0; i < CODES.length; i++) {
      var c = CODES[i];
      var match = c.name.toLowerCase().indexOf(q) !== -1 ||
                  (c.desc && c.desc.toLowerCase().indexOf(q) !== -1) ||
                  c.files.join(' ').toLowerCase().indexOf(q) !== -1;
      if (match) {
        count++;
        html += '<div class="card" style="cursor:default">';
        html += '<h3>' + c.name + '</h3>';
        if (c.desc) html += '<p>' + c.desc + '</p>';
        html += '<div class="code-links">';
        html += '<a href="' + CODE + '/' + c.path + '" target="_blank">📁 目錄</a>';
        for (var j = 0; j < c.files.length; j++) {
          html += '<a href="' + CODE + '/' + c.path + '/' + c.files[j] + '" target="_blank">' + c.files[j] + '</a>';
        }
        html += '</div></div>';
      }
    }
    if (count === 0) {
      html = '<div class="empty-state"><div class="icon">🔍</div><p>沒有符合「' + q + '」的程式碼範例</p></div>';
    }
    html += '</div>';
    document.getElementById('content').innerHTML = html;
  }
}

function initTabs() {
  var tabs = document.querySelectorAll('.tab');
  for (var i = 0; i < tabs.length; i++) {
    tabs[i].onclick = function() {
      for (var j = 0; j < tabs.length; j++) tabs[j].classList.remove('active');
      this.classList.add('active');
      curTab = this.getAttribute('data-tab');
      document.getElementById('search').value = '';
      for (var k = 0; k < Object.keys(TOPICS).length; k++) {
        var key = Object.keys(TOPICS)[k];
        for (var i2 = 0; i2 < TOPICS[key].length; i2++) {
          TOPICS[key][i2]._hidden = false;
        }
      }
      render(curTab);
    };
  }
}

function toggleTheme() {
  document.body.classList.toggle('dark');
  var btn = document.getElementById('themeBtn');
  btn.textContent = document.body.classList.contains('dark') ? '☀' : '☾';
}

window.onload = function() {
  document.getElementById('c0').textContent = Object.keys(TOPICS).reduce(function(a, k) { return a + TOPICS[k].length; }, 0);
  document.getElementById('c1').textContent = CODES.length;
  initTabs();
  render('概念');
};
