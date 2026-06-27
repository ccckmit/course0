var WIKI = 'https://github.com/ccckmit/course0/blob/main/wiki/%E6%BC%94%E7%AE%97%E6%B3%95';
var CODE = 'https://github.com/ccckmit/course0/blob/main/code/%E6%BC%94%E7%AE%97%E6%B3%95';

var TOPICS = {
  '概念': [
    {name:'演算法的歷史', file:'演算法的歷史.md', desc:'從歐幾里得到量子計算 — 跨越四千年的演算法發展史', tags:['計算理論','發展']},
    {name:'時間複雜度', file:'時間複雜度.md', desc:'分析演算法執行時間隨輸入規模增長的趨勢', tags:['Big-O','效率分析']},
    {name:'空間複雜度', file:'空間複雜度.md', desc:'評估演算法執行所需的記憶體資源', tags:['Big-O','記憶體']},
    {name:'資料結構', file:'資料結構.md', desc:'資料的組織、儲存與操作方式', tags:['陣列','樹','圖','雜湊']},
  ],
  '排序與搜尋': [
    {name:'排序演算法', file:'排序演算法.md', desc:'從氣泡排序到快速排序，比較排序與非比較排序', tags:['QuickSort','MergeSort','HeapSort']},
    {name:'搜尋法', file:'搜尋法.md', desc:'線性搜尋、二分搜尋、BFS/DFS、A* 路徑搜尋', tags:['BinarySearch','BFS','DFS']},
    {name:'雜湊法', file:'雜湊法.md', desc:'利用雜湊函數達到 O(1) 平均查找', tags:['HashTable','BloomFilter']},
  ],
  '圖論與網路': [
    {name:'圖形演算法', file:'圖形演算法.md', desc:'BFS、DFS、最短路徑、最小生成樹、強連通分量', tags:['Graph','Dijkstra','Kruskal']},
    {name:'網路流', file:'網路流.md', desc:'最大流最小割定理、Ford-Fulkerson 與 Dinic 演算法', tags:['MaxFlow','MinCut']},
  ],
  '演算法設計策略': [
    {name:'分割擊破法', file:'分割擊破法.md', desc:'分而治之 — Merge Sort、Quick Sort、Strassen', tags:['Divide-Conquer','遞迴']},
    {name:'動態規劃法', file:'動態規劃法.md', desc:'最優子結構與重疊子問題 — 背包問題、LCS、編輯距離', tags:['DP','Memoization']},
    {name:'貪婪法', file:'貪婪法.md', desc:'每一步選擇局部最優 — Huffman、MST、活動選擇', tags:['Greedy']},
    {name:'遞迴法', file:'遞迴法.md', desc:'函數呼叫自身解決問題 — 河內塔、費波那契、樹遍歷', tags:['Recursion']},
    {name:'迭代法', file:'迭代法.md', desc:'重複逼近 — 牛頓法、梯度下降、PageRank', tags:['Iteration','Convergence']},
    {name:'列舉法', file:'列舉法.md', desc:'窮舉所有可能性 — 排列、組合、子集枚舉', tags:['Brute-Force','Backtracking']},
    {name:'暴力法', file:'暴力法.md', desc:'直接枚舉所有可能解，作為 baseline', tags:['Brute-Force']},
    {name:'前處理法', file:'前處理法.md', desc:'資料預先處理以加速後續查詢 — 前綴和、排序預處理', tags:['Preprocessing']},
    {name:'查表法', file:'查表法.md', desc:'以空間換取時間 — 預先計算結果快速查詢', tags:['Space-Time','Lookup']},
    {name:'化簡法', file:'化簡法.md', desc:'將未知問題轉化為已知問題求解', tags:['Reduction']},
    {name:'逼近法', file:'逼近法.md', desc:'NP-困難問題的近似解與近似比保證', tags:['Approximation','NP-Hard']},
    {name:'近似演算法', file:'近似演算法.md', desc:'NP-困難問題的多項式時間近似解', tags:['Approximation','VertexCover','TSP']},
  ],
  '最佳化演算法': [
    {name:'梯度下降法', file:'梯度下降法.md', desc:'沿負梯度方向迭代尋找最小值 — GD、SGD、Adam', tags:['Optimization','MachineLearning']},
    {name:'爬山演算法', file:'爬山演算法.md', desc:'局部搜尋 — 移動到更好的鄰居', tags:['LocalSearch']},
    {name:'遺傳演算法', file:'遺傳演算法.md', desc:'模擬天擇的演化式最佳化', tags:['GA','Evolution']},
    {name:'蒙地卡羅法', file:'蒙地卡羅法.md', desc:'隨機抽樣求解 — π 估計、數值積分、MCMC', tags:['MonteCarlo','Sampling']},
    {name:'隨機演算法', file:'隨機演算法.md', desc:'利用隨機性提升效率 — Las Vegas 與 Monte Carlo', tags:['Randomized']},
  ],
  '進階主題': [
    {name:'字串演算法', file:'字串演算法.md', desc:'字串匹配 — KMP、Boyer-Moore、Rabin-Karp、Trie', tags:['String','KMP']},
    {name:'計算幾何', file:'計算幾何.md', desc:'幾何演算法 — 凸包、旋轉卡尺、最近點對', tags:['Geometry','ConvexHull']},
    {name:'線性規劃', file:'線性規劃.md', desc:'線性約束下最佳化目標函數 — 單純形法、對偶理論', tags:['LP','Simplex']},
  ],
  '計算理論': [
    {name:'有限狀態機', file:'有限狀態機.md', desc:'DFA/NFA 與正則語言的計算模型', tags:['Automata','Regular']},
    {name:'形式語法', file:'形式語法.md', desc:'CFG、CSG 與 Chomsky 階層的形式語言', tags:['Grammar','Chomsky']},
    {name:'Lambda Calculus', file:'λ演算.md', desc:'Church 的 λ 演算與可計算性理論基礎', tags:['Lambda','Church']},
    {name:'圖靈機', file:'圖靈機.md', desc:'通用計算模型與可計算性的極限', tags:['Turing','Undecidability']},
  ],
  '數值與轉換': [
    {name:'函數式編程', file:'函數式編程.md', desc:'高階函數、不可變性、組合子與 Y Combinator', tags:['FP','Lambda']},
    {name:'轉換法', file:'轉換法.md', desc:'傅立葉轉換、Walsh 轉換與 JPEG 壓縮', tags:['Fourier','FFT','Transform']},
  ],
};

var CODES = [
  {name:'時間複雜度', path:'method/00-bigO', files:['bubbleSort.py','matrixMul.py','lsearch.py'], desc:'Big-O 分析 — O(n)、O(n²)、O(n³) 範例'},
  {name:'查表法', path:'method/01-查表法', files:['fibonacci_lookup.py','CnkRLookup.py','power2n.py'], desc:'以空間換取時間的查表最佳化'},
  {name:'列舉法', path:'method/02a-列舉法', files:['truthTable.py','permutation.py','allCombination.py'], desc:'真值表、排列、組合窮舉'},
  {name:'雜湊法', path:'method/02b-雜湊法', files:['hash1.py','bloomFilter.py','sha256.py'], desc:'雜湊表、布隆過濾器、SHA-256'},
  {name:'暴力法', path:'method/02c-暴力法', files:['bruteForce1.py','mining.py'], desc:'暴力搜尋與比特幣挖礦'},
  {name:'迭代法', path:'method/03-迭代法', files:['newton.py','jacobi1.py','pageRankGpt1.py'], desc:'牛頓法、Jacobi、PageRank'},
  {name:'遞迴法', path:'method/04a-遞迴法', files:['fibonacci.py','hanoi.py','gcd.py'], desc:'遞迴經典範例'},
  {name:'函數式編程', path:'method/04b-函數式編程', files:['fp.py','ycombinator1.py'], desc:'高階函數與 Y Combinator'},
  {name:'Lambda Calculus', path:'method/04c-LambdaCalculus', files:['lambdaCalculus.py','bool.py','numerial.py'], desc:'Church 編碼與 λ 演算'},
  {name:'爬山演算法', path:'method/05c-爬山演算法', files:['hillClimbing1.py','hillClimbingN.py'], desc:'局部搜尋最佳化'},
  {name:'梯度下降法', path:'method/05e-梯度下降法', files:['gd.py','gdRegression.py','sgd1.py'], desc:'GD、SGD、反向傳播'},
  {name:'貪婪法', path:'method/05f-貪婪法', files:['huffmanCode1.py','kruskal1.py','prims.py'], desc:'Huffman、MST'},
  {name:'搜尋法', path:'method/06-搜尋法', files:['graph_search.py','stateSearch.py','astar.py'], desc:'BFS、DFS、A* 搜尋'},
  {name:'分割擊破法', path:'method/07-分割擊破法', files:['mergesort.py','binSearch.py','strassen.py'], desc:'Merge Sort、Strassen'},
  {name:'動態規劃法', path:'method/08-動態規劃法', files:['CnkDynamic.py','editDistance.py'], desc:'DP 表格法與編輯距離'},
  {name:'蒙地卡羅法', path:'method/09b-蒙地卡羅法', files:['monteCarloPi.py','ndBall.py','gibbs.py'], desc:'π 估計、MCMC、Gibbs'},
  {name:'遺傳演算法', path:'method/09c-遺傳演算法', files:['geneticAlgorithm.py','keyGa.py'], desc:'GA 框架與字串猜測'},
  {name:'前處理法', path:'method/13-前處理法', files:['sort.py','sort_search.py','hash.py'], desc:'排序/索引/雜湊前處理'},
  {name:'化簡法', path:'method/15c-化約法', files:['knapsack1.py','queen1.py','tsp1.py'], desc:'化約為整數規劃'},
  {name:'逼近法', path:'method/16-逼近法', files:['e.py','sterling.py','taylor.py','integration.py'], desc:'數值逼近與泰勒展開'},
  {name:'轉換法', path:'method/18-轉換法', files:['fft1.py','dft.py','walsh.py'], desc:'傅立葉轉換與 Walsh 轉換'},
  {name:'有限狀態機', path:'theory/finiteStateMachine', files:['finiteStateMachine.py','anbm.py','even0.py'], desc:'DFA 實作'},
  {name:'形式語法', path:'theory/grammar', files:['grammar.py','anbn.py','anbncn.py','english.py'], desc:'CFG 與 CSG 產生器'},
  {name:'λ 演算 (理論)', path:'theory/lambda/01-full', files:['lambdaCalculus.py'], desc:'完整 Church 編碼實作 / 合併至 Lambda Calculus'},
  {name:'圖靈機', path:'theory/turingMachine', files:['TuringMachine.py'], desc:'圖靈機模擬器'},
];

var TOPIC_CONTENT = {};

TOPIC_CONTENT['演算法的歷史'] = {
  en: 'History of Algorithms',
  sections: [
    {title:'古代演算法', content:'最早的演算法可追溯至西元前 1800 年的巴比倫人，他們已發展出求平方根的方法。西元前 300 年，歐幾里得提出了求最大公因數的演算法（輾轉相除法），這是史上最著名的演算法之一。'},
    {title:'中世紀與文藝復興', content:'波斯數學家 Al-Khwarizmi（約 780-850）在其著作中系統性地介紹了代數方法，"Algorithm" 一詞即源自其名字。中國剩餘定理、秦九韶的"天元術"等也是這一時期的重要貢獻。'},
    {title:'近代計算理論的誕生', content:'19 世紀，布爾（Boole）創立了布爾代數，為計算的數學基礎鋪路。20 世紀初，圖靈（Turing）提出圖靈機模型，丘奇（Church）發展 λ 演算，兩者共同奠定了可計算性的理論基礎。哥德爾的不完備定理則揭示了形式系統的內在限制。'},
    {title:'電子電腦時代', content:'1940-50 年代，ENIAC 的誕生開啟了電子計算時代。馮紐曼架構成為現代電腦的標準設計。丹齊格（Dantzig）提出了單純形法（1947），漢明（Hamming）發明了錯誤更正碼（1948）。'},
    {title:'演算法學科的形成', content:'1960-70 年代是演算法發展的黃金時期：Quicksort（Hoare, 1962）、Heapsort（Williams, 1964）、Dijkstra 最短路徑（1959）、KMP 字串匹配（1977）、NP-完備性理論（Cook-Levin, 1971）等經典成果相繼問世。'},
    {title:'大數據與機器學習時代', content:'21 世紀，演算法面臨大規模資料的挑戰。MapReduce（2004）開啟了分散式運算的新範式。深度學習（CNN/RNN/Transformer）和強化學習在圖像辨識、自然語言處理、圍棋等領域取得突破性進展。'},
  ],
};

TOPIC_CONTENT['時間複雜度'] = {
  en: 'Time Complexity',
  sections: [
    {title:'定義', content:'時間複雜度描述演算法執行時間隨輸入規模增長的趨勢。通常使用 Big-O 記號來表示最壞情況下的漸進上界。'},
    {title:'常見複雜度等級', content:'O(1) < O(log n) < O(n) < O(n log n) < O(n²) < O(n³) < O(2ⁿ) < O(n!)。分析時忽略常數因子和低階項，關注主導項的增長趨勢。'},
    {title:'O(1) — 常數時間', content:'無論輸入規模大小，執行時間固定不變。例如陣列隨機存取、雜湊表查找。'},
    {title:'O(log n) — 對數時間', content:'每次操作都將問題規模減半。例如已排序陣列的二分搜尋、平衡二元搜尋樹操作。'},
    {title:'O(n) — 線性時間', content:'執行時間與輸入規模成正比。例如線性搜尋、找最大值。'},
    {title:'O(n²) — 平方時間', content:'常見於雙層迴圈的演算法，如氣泡排序、選擇排序。'},
    {title:'分析要點', content:'最壞情況 vs 平均情況 vs 最好情況；忽略常數因子；忽略低階項。'},
  ],
};

TOPIC_CONTENT['空間複雜度'] = {
  en: 'Space Complexity',
  sections: [
    {title:'定義', content:'空間複雜度評估演算法執行所需的記憶體資源，包括輸入空間、輔助空間和輸出空間三個部分。'},
    {title:'O(1) — 常數空間', content:'使用固定數量的變數，不隨輸入規模增長。例如交換兩個變數、計算陣列總和。'},
    {title:'O(n) — 線性空間', content:'建立與輸入規模成比例的輔助結構。例如複製陣列、前綴和陣列。'},
    {title:'遞迴空間', content:'遞迴演算法的空間複雜度包括呼叫堆疊的深度。例如二分搜尋遞迴版 O(log n)，費波那契遞迴 O(n)。'},
    {title:'空間-時間權衡', content:'許多演算法可以在空間和時間之間取捨：使用更多記憶體來加快執行速度（如查表法、記憶化），或使用較少記憶體但接受較慢的執行時間。'},
  ],
};

TOPIC_CONTENT['資料結構'] = {
  en: 'Data Structures',
  sections: [
    {title:'陣列 (Array)', content:'連續記憶體空間，固定大小。隨機存取 O(1)，插入/刪除 O(n)。靜態陣列大小固定，動態陣列（如 Python list）可自動擴展。'},
    {title:'鏈結串列 (Linked List)', content:'由節點組成，每個節點包含資料和指向下一個節點的指標。插入/刪除 O(1)（已知位置），搜尋 O(n)。'},
    {title:'堆疊 (Stack)', content:'後進先出（LIFO）結構。push/pop 皆為 O(1)。用於函數呼叫管理、表達式求值。'},
    {title:'佇列 (Queue)', content:'先進先出（FIFO）結構。enqueue/dequeue 皆為 O(1)。用於 BFS、任務排程。'},
    {title:'二元搜尋樹 (BST)', content:'每個節點左子樹值小於父節點，右子樹值大於父節點。平均 O(log n) 搜尋。'},
    {title:'圖 (Graph)', content:'由頂點和邊組成。表示方式：鄰接矩陣（稠密圖）、鄰接串列（稀疏圖）。'},
  ],
};

TOPIC_CONTENT['排序演算法'] = {
  en: 'Sorting Algorithms',
  sections: [
    {title:'分類', content:'排序演算法分為比較排序（Comparison Sort）與非比較排序。比較排序的理論下限為 O(n log n)。'},
    {title:'O(n²) 排序', content:'氣泡排序（Bubble Sort）：重複走訪數列，相鄰比較交換。選擇排序（Selection Sort）：每次選最小元素放到前面。插入排序（Insertion Sort）：將元素插入已排序部分。'},
    {title:'O(n log n) 排序', content:'合併排序（Merge Sort）：分治策略，穩定排序，O(n) 額外空間。快速排序（Quick Sort）：選 pivot 分區，平均最快。堆積排序（Heap Sort）：利用最大堆積。'},
    {title:'非比較排序', content:'計數排序（Counting Sort）：O(n+k)，適用範圍有限的整數。基數排序（Radix Sort）：按位數排序。桶排序（Bucket Sort）：均勻分布資料。'},
    {title:'穩定性', content:'穩定排序保留相等元素的相對順序。穩定：Bubble、Insertion、Merge、Counting。不穩定：Selection、Quick、Heap。'},
  ],
};

TOPIC_CONTENT['搜尋法'] = {
  en: 'Search Algorithms',
  sections: [
    {title:'線性搜尋', content:'從頭到尾逐一比對，時間 O(n)。適用於未排序或小型資料集。可擴展為回傳所有匹配位置。'},
    {title:'二分搜尋', content:'在已排序陣列中，每次將搜尋範圍縮小一半。時間 O(log n)。需預先排序。'},
    {title:'圖形搜尋', content:'廣度優先搜尋（BFS）使用佇列，找到最短路徑。深度優先搜尋（DFS）使用堆疊或遞迴，用於拓撲排序、連通分量。'},
    {title:'A* 搜尋', content:'啟發式搜尋演算法，結合 Dijkstra 的最短路徑保證與貪婪最佳優先搜尋的效率。使用 f(n) = g(n) + h(n) 評估節點。'},
  ],
};

TOPIC_CONTENT['圖形演算法'] = {
  en: 'Graph Algorithms',
  sections: [
    {title:'圖的表示', content:'鄰接矩陣：2D 陣列，適合稠密圖。鄰接串列：字典/串列，適合稀疏圖。邊串列：適合枚舉所有邊。'},
    {title:'BFS 與 DFS', content:'BFS（廣度優先）：使用佇列，用於最短路徑（無權重圖）。DFS（深度優先）：使用堆疊/遞迴，用於拓撲排序、連通分量、環檢測。'},
    {title:'最短路徑', content:'Dijkstra：非負權重，O((V+E)log V)。Bellman-Ford：可處理負權重，O(VE)。Floyd-Warshall：全點對，O(V³)。'},
    {title:'最小生成樹', content:'Kruskal：將邊按權重排序，使用 Union-Find。Prim：類似 Dijkstra，每次加入最小跨接邊。'},
  ],
};

TOPIC_CONTENT['動態規劃法'] = {
  en: 'Dynamic Programming',
  sections: [
    {title:'核心概念', content:'動態規劃適用於具備最優子結構（問題的最優解包含子問題的最優解）和重疊子問題（子問題被重複計算）的性質的問題。'},
    {title:'實作方法', content:'自頂向下（Top-down）：遞迴 + 記憶化（memoization），只計算需要的子問題。自底向上（Bottom-up）：迭代填表，計算所有子問題，無遞迴開銷。'},
    {title:'經典問題', content:'費波那契數列（O(2ⁿ) → O(n)）、0/1 背包問題（O(nW)）、最長公共子序列 LCS（O(nm)）、編輯距離（Levenshtein）、矩陣鏈乘積。'},
  ],
};

TOPIC_CONTENT['分割擊破法'] = {
  en: 'Divide and Conquer',
  sections: [
    {title:'核心概念', content:'將原問題分割成數個規模較小的子問題，遞迴解決子問題，再將子問題的解合併為原問題的解。三步驟：分割（Divide）→ 解決（Conquer）→ 合併（Merge）。'},
    {title:'合併排序', content:'將陣列遞迴分成兩半，分別排序後合併。時間 O(n log n)，空間 O(n)。'},
    {title:'快速排序', content:'選擇 pivot 將陣列分為小於和大於 pivot 的兩部分，遞迴排序。平均 O(n log n)，最壞 O(n²)。'},
    {title:'Strassen 矩陣乘法', content:'將矩陣分為四個子矩陣，透過 7 次乘法取代 8 次乘法。時間 O(n^2.807)，優於傳統 O(n³)。'},
  ],
};

TOPIC_CONTENT['貪婪法'] = {
  en: 'Greedy Algorithm',
  sections: [
    {title:'核心概念', content:'每一步都選擇當前看起來最優的選項，期望最終能得到全域最優解。三個組成部分：選擇函數、可行性檢查、目標函數更新。'},
    {title:'經典問題', content:'找零錢問題（需注意幣制）、活動選擇（按結束時間排序）、Huffman 編碼（最小堆積建樹）、Kruskal 最小生成樹（邊排序 + Union-Find）。'},
    {title:'貪婪 vs DP', content:'貪婪法無法保證全域最優（如找零錢問題在任意幣制下），而動態規劃總是能找到最優解但成本較高。'},
  ],
};

TOPIC_CONTENT['遞迴法'] = {
  en: 'Recursion',
  sections: [
    {title:'核心概念', content:'函數直接或間接呼叫自身。包含基本情況（終止條件）和遞迴情況（縮小問題規模）。'},
    {title:'經典範例', content:'費波那契數列（樸素 O(2ⁿ) vs 記憶化 O(n)）、階乘、河內塔（2ⁿ-1 步）、二元樹遍歷（前/中/後序）。'},
    {title:'尾端遞迴', content:'遞迴呼叫為函數的最後一個操作，可被編譯器優化為迭代。階乘的尾遞迴版本可避免堆疊溢出。'},
    {title:'遞迴 vs 迭代', content:'遞迴程式碼簡潔但可能有堆疊溢出和效能開銷；迭代效率高但程式碼較複雜。'},
  ],
};

TOPIC_CONTENT['迭代法'] = {
  en: 'Iteration',
  sections: [
    {title:'核心概念', content:'重複套用某個運算或程序，逐步逼近目標解。通用框架：初始猜測 → 迭代更新 → 收斂檢查。'},
    {title:'定點迭代法', content:'將方程式改寫為 x = g(x) 的形式，反覆代入直到收斂。收斂條件為 |g\'(x)| < 1。'},
    {title:'牛頓法', content:'利用切線逼近求根：x_{n+1} = x_n - f(x_n)/f\'(x_n)。在根附近二次收斂。'},
    {title:'梯度下降法', content:'沿負梯度方向更新參數以最小化目標函數。學習率決定每一步的大小。'},
  ],
};

TOPIC_CONTENT['列舉法'] = {
  en: 'Enumeration',
  sections: [
    {title:'核心概念', content:'窮舉所有可能的候選解，從中挑選正確答案。也稱暴力搜尋。數學基礎為組合數學（2ⁿ 子集、n! 排列）。'},
    {title:'子集枚舉', content:'使用位元遮罩（bit mask）技術：for mask in range(1 << n)。可枚舉所有子集。'},
    {title:'排列枚舉', content:'使用遞迴回溯生成所有 n! 種排列。藉由剪枝（pruning）優化。'},
    {title:'N-皇后問題', content:'經典回溯法應用：在 N×N 棋盤上放置 N 個皇后，使其互不攻擊。'},
  ],
};

TOPIC_CONTENT['暴力法'] = {
  en: 'Brute Force',
  sections: [
    {title:'核心概念', content:'直接枚舉所有可能性，逐一檢查是否為解。雖然效率低，但實作簡單，常作為其他演算法的基準（baseline）。'},
    {title:'應用場景', content:'字串比對（O(nm)）、最近點對（O(n²)）、密碼破解（指數級）、0/1 背包問題。當問題規模很小或作為效能基準時適用。'},
  ],
};

TOPIC_CONTENT['前處理法'] = {
  en: 'Preprocessing',
  sections: [
    {title:'核心概念', content:'在主要計算之前先對資料進行預處理，將資料組織成更方便查詢的形式，提升後續演算法的效率。'},
    {title:'常見技術', content:'排序預處理（O(n log n) 排序 + O(log n) 查詢）。前綴和（O(n) 構建 + O(1) 區間求和）。前綴最大/最小值。離散化（將連續值映射為整數）。'},
    {title:'KMP 前處理', content:'KMP 字串匹配演算法通過預先計算部分匹配表（LPS 陣列），將字串比對從 O(nm) 優化至 O(n+m)。'},
  ],
};

TOPIC_CONTENT['查表法'] = {
  en: 'Lookup Table',
  sections: [
    {title:'核心概念', content:'預先計算結果並儲存在表格中，執行時直接查表取得答案。以空間換取時間的經典範例，將 O(n) 以上的運算降低為 O(1)。'},
    {title:'應用', content:'費波那契查找表（記憶化）、冪次預算表（如 2ⁿ）、三角函數表、S-Box（密碼學）、CRC 表。'},
    {title:'壓縮技術', content:'大型查找表可透過對稱性壓縮（如只儲存半個正弦表）、差分編碼、壓縮索引等方式減少記憶體佔用。'},
  ],
};

TOPIC_CONTENT['化簡法'] = {
  en: 'Reduction',
  sections: [
    {title:'核心概念', content:'將一個複雜的未知問題轉化為一個已知的、已解決的問題。求解已知問題後，再將解映射回原問題。'},
    {title:'經典化簡', content:'排序化簡搜尋：先排序再二分搜尋。歐幾里得演算法：GCD(a,b) = GCD(b, a mod b)。NP-完備性證明：將問題 A 化簡為已知的 NP-完備問題 B。'},
  ],
};

TOPIC_CONTENT['逼近法'] = TOPIC_CONTENT['近似演算法'] = {
  en: 'Approximation Algorithms',
  sections: [
    {title:'核心概念', content:'對 NP-困難問題，在多項式時間內求接近最優的解。近似比（Approximation Ratio）定義為近似解與最優解的比值上界。'},
    {title:'頂點覆蓋', content:'貪婪法選擇最高度頂點可達 2-近似比。基於最大匹配的演算法也可達到 2-近似比。'},
    {title:'集合覆蓋', content:'貪婪法每輪選擇覆蓋最多未覆蓋元素的子集。近似比為 O(log n)。'},
    {title:'旅行推銷員問題 (TSP)', content:'基於最小生成樹的演算法可達 2-近似比。Christofides 演算法可達 1.5-近似比。'},
  ],
};

TOPIC_CONTENT['梯度下降法'] = {
  en: 'Gradient Descent',
  sections: [
    {title:'核心概念', content:'沿負梯度方向迭代更新參數以最小化目標函數。梯度指向函數值增加最快的方向，負梯度即下降最快的方向。'},
    {title:'主要變體', content:'批量梯度下降（Batch GD）：使用全部資料計算梯度。隨機梯度下降（SGD）：每次使用一個樣本，效率高但有雜訊。小批量梯度下降（Mini-batch GD）：折衷方案。'},
    {title:'進階優化器', content:'動量法（Momentum）加速收斂。Adam 結合動量和自適應學習率。學習率衰減（step decay、指數衰減、餘弦退火）有助於收斂。'},
  ],
};

TOPIC_CONTENT['爬山演算法'] = {
  en: 'Hill Climbing',
  sections: [
    {title:'核心概念', content:'從初始點出發，反覆移動到更好的鄰居位置，直到抵達局部最優點。類似於在迷霧中爬山，只能感知周圍一步的範圍。'},
    {title:'變體', content:'簡單爬山法：選擇第一個更好的鄰居。最陡上升法：在所有鄰居中選最好的。隨機爬山法：隨機選擇鄰居。'},
    {title:'局限性', content:'容易陷入局部最優。處理方法：隨機重啟（random restart）、模擬退火（以機率接受較差的解）、加入擾動。'},
  ],
};

TOPIC_CONTENT['遺傳演算法'] = {
  en: 'Genetic Algorithm',
  sections: [
    {title:'核心概念', content:'模擬自然界天擇機制的演化式最佳化演算法。維護一個候選解族群，透過選擇、交配、突變等操作產生新一代。'},
    {title:'主要步驟', content:'初始化族群 → 計算適應度 → 選擇父母（錦標賽/輪盤賭）→ 交配（單點/雙點/均勻）→ 突變（位元翻轉/交換）→ 取代 → 重複。'},
    {title:'應用', content:'背包問題、旅行推銷員問題（TSP）、函數最佳化、超參數調整、程式演化。'},
  ],
};

TOPIC_CONTENT['蒙地卡羅法'] = {
  en: 'Monte Carlo Method',
  sections: [
    {title:'核心概念', content:'利用隨機抽樣來解決確定性問題。根據大數法則，抽樣數量越多，結果越精確。誤差以 O(1/√n) 的速度下降。'},
    {title:'典型應用', content:'圓周率 π 估計：隨機投點到正方形中，計算落在內切圓內的比例。數值積分：對高維度積分特別有效。'},
    {title:'MCMC', content:'馬可夫鏈蒙地卡羅法（MCMC）用於從複雜分布中抽樣。Metropolis-Hastings 和 Gibbs 抽樣是兩種主要方法。'},
  ],
};

TOPIC_CONTENT['隨機演算法'] = {
  en: 'Randomized Algorithms',
  sections: [
    {title:'核心概念', content:'在決策過程中引入隨機性。分為兩大類：拉斯維加斯演算法（Las Vegas）總是正確，執行時間隨機；蒙地卡羅演算法（Monte Carlo）執行時間固定，可能給出錯誤答案。'},
    {title:'隨機化快速排序', content:'隨機選擇 pivot，避免最壞情況 O(n²) 的發生。期望時間 O(n log n)。'},
    {title:'Karger 最小割', content:'隨機收縮邊直到只剩兩個頂點，重複多次取最小值。成功機率可透過重複提升。'},
  ],
};

TOPIC_CONTENT['雜湊法'] = {
  en: 'Hashing',
  sections: [
    {title:'核心概念', content:'使用雜湊函數將鍵映射到陣列索引，達到 O(1) 平均查找、插入、刪除。雜湊函數需滿足：確定性、均勻分布、O(1) 計算。'},
    {title:'碰撞處理', content:'鍊結法（Separate Chaining）：每個桶用鏈結串列儲存。開放定址法（Open Addressing）：線性探測、二次探測、雙重雜湊。'},
    {title:'負載因子', content:'負載因子 = 元素數 / 桶數。超過門檻（通常 0.75）時需重新雜湊（rehash），擴展陣列大小並重新映射所有元素。'},
  ],
};

TOPIC_CONTENT['字串演算法'] = {
  en: 'String Algorithms',
  sections: [
    {title:'暴力比對', content:'從文字串每個位置開始與模式串逐一比較。時間 O(nm)，實作簡單。'},
    {title:'KMP 演算法', content:'Knuth-Morris-Pratt 利用部分匹配表（LPS）避免重複比較。預處理 O(m)，比對 O(n+m)。'},
    {title:'Boyer-Moore', content:'從右向左比對，利用壞字元和好後綴規則跳過不必要的比對。在實際應用中通常比 KMP 更快。'},
    {title:'Rabin-Karp', content:'使用滾動雜湊（rolling hash）在 O(n+m) 期望時間內比對。適合多模式比對。'},
    {title:'Trie (字典樹)', content:'前綴樹結構，適合字典查找、自動完成、拼字檢查。'},
  ],
};

TOPIC_CONTENT['網路流'] = {
  en: 'Network Flow',
  sections: [
    {title:'核心概念', content:'研究如何在網路中有效地傳輸流量。最大流最小割定理：最大流的流量等於最小割的容量。'},
    {title:'Ford-Fulkerson', content:'反覆尋找增廣路徑並增加流量。時間 O(E × max_flow)。使用 BFS 找增廣路徑的版本稱為 Edmonds-Karp，時間 O(VE²)。'},
    {title:'Dinic 演算法', content:'使用分層圖和當前弧優化，時間 O(V²E)。在特定圖上可達 O(E√V)。'},
    {title:'應用', content:'二分圖最大匹配、運輸問題、通訊網路容量規劃、影像分割（Graph Cut）。'},
  ],
};

TOPIC_CONTENT['計算幾何'] = {
  en: 'Computational Geometry',
  sections: [
    {title:'核心概念', content:'研究幾何問題的高效演算法。應用於電腦圖學、機器人學、地理資訊系統、電腦視覺。'},
    {title:'凸包', content:'Graham Scan：O(n log n)，基於極角排序和堆疊掃描。Jarvis March（禮物包裹）：O(nh)，h 為凸包頂點數。Andrew\'s Algorithm：O(n log n)，先排序後掃描。'},
    {title:'旋轉卡尺', content:'在凸多邊形上旋轉兩條平行線，用於計算直徑（最遠點對）、寬度、最小包圍矩形。'},
    {title:'最近點對', content:'使用分割擊破法，O(n log n)。將點按 x 座標排序，遞迴找出左右最近點對，再檢查分界線附近的候選點。'},
  ],
};

TOPIC_CONTENT['線性規劃'] = {
  en: 'Linear Programming',
  sections: [
    {title:'核心概念', content:'在一組線性不等式約束下，最大化或最小化一個線性目標函數。由喬治·丹齊格（George Dantzig）於 1947 年提出。'},
    {title:'單純形法', content:'在可行域的多面體頂點間移動，每次沿改善最多的邊移動，直到無法改善為止。最壞情況指數時間但平均高效。'},
    {title:'對偶理論', content:'每個線性規劃問題都有對應的對偶問題。弱對偶定理：對偶問題的目標值 ≤ 原問題的目標值（最大化）。強對偶定理：若原問題有最優解，則對偶問題也有最優解且目標值相等。'},
    {title:'整數線性規劃', content:'限制變數必須為整數。求解難度遠高於一般線性規劃（NP-困難），常用分支定界法（Branch and Bound）。'},
  ],
};

TOPIC_CONTENT['有限狀態機'] = {
  en: 'Finite State Machine',
  sections: [
    {title:'核心概念', content:'有限狀態機（FSM）是一種包含有限數量的狀態、狀態間轉換規則、以及接受條件的計算模型。DFA（確定性有限自動機）在每個狀態對每個輸入符號都有唯一的轉移。'},
    {title:'應用', content:'詞法分析（Lexer）、通訊協定、數位電路設計、字串匹配、遊戲 AI 狀態管理。'},
    {title:'限制', content:'有限狀態機無法處理需要計數到任意大的問題（如 aⁿbⁿ 的括號匹配），這需要更強大的下推自動機（PDA）。'},
  ],
};

TOPIC_CONTENT['形式語法'] = {
  en: 'Formal Grammar',
  sections: [
    {title:'核心概念', content:'形式語法由一組產生規則（Production Rules）定義，描述如何從起始符號推導出語言的合法字串。Chomsky 階層將語法分為四類：Type-0（無限制）、Type-1（上下文相關）、Type-2（上下文無關）、Type-3（正則）。'},
    {title:'上下文無關語法 (CFG)', content:'用於描述程式語言的語法結構。產生式形式為 A → α，左側為單一非終止符。範例：S → aSb | ε 產生 aⁿbⁿ。'},
    {title:'應用', content:'程式語言編譯器（語法分析）、自然語言處理（句法分析）、資料序列化格式（JSON/XML）、RNA 結構預測。'},
  ],
};

TOPIC_CONTENT['Lambda Calculus'] = {
  en: 'Lambda Calculus',
  sections: [
    {title:'核心概念', content:'λ 演算由 Alonzo Church 在 1930 年代提出，是研究函數定義、函數應用和遞迴的形式系統。所有計算都可以歸約為這三種操作：變數（Variable）、抽象（Abstraction）、應用（Application）。'},
    {title:'Church 編碼', content:'使用純函數來表示布林值（TRUE = λx.λy.x）、數字（0 = λf.λx.x）、列表等資料結構。一切皆為函數。'},
    {title:'Y Combinator', content:'在 λ 演算中實現遞迴的固定點組合子。Y = λf.(λx.f(x x))(λx.f(x x))。在不具備命名函數的純 λ 演算中，Y Combinator 是實現遞迴的關鍵。'},
    {title:'β-歸約', content:'函數應用的計算規則：(λx.M)N → M[x:=N]，即將參數 N 代入函數體 M 中的 x。歸約策略包括先求值參數（應用序）和延遲求值（正常序）。'},
  ],
};

TOPIC_CONTENT['圖靈機'] = {
  en: 'Turing Machine',
  sections: [
    {title:'核心概念', content:'圖靈機由 Alan Turing 在 1936 年提出，包含一條無限長的紙帶、一個讀寫頭、以及一組狀態轉換規則。儘管結構簡單，圖靈機可以模擬任何計算機的計算過程，是通用計算模型的理論基礎。'},
    {title:'組成部分', content:'無限長的紙帶（tape）分為格子，每個格子包含一個符號。讀寫頭（head）每次讀取或寫入一個符號，並可向左或向右移動。狀態寄存器儲存當前狀態，轉換函數 δ(state, symbol) → (new_state, new_symbol, direction) 定義行為。'},
    {title:'停機問題', content:'圖靈證明不存在一個通用演算法能判斷任意圖靈機是否會停機。停機問題的不可判定性是計算理論中最根本的極限結果之一。'},
  ],
};

TOPIC_CONTENT['函數式編程'] = {
  en: 'Functional Programming',
  sections: [
    {title:'核心概念', content:'函數式編程（FP）是一種以函數為核心的程式範式。強調不可變性（Immutability）、純函數（Pure Function）、高階函數（Higher-Order Function）。與命令式編程不同，FP 避免狀態變化和副作用。'},
    {title:'高階函數', content:'接收函數作為參數或回傳函數的函數。map、filter、reduce 是經典範例。例如 map(f, [1,2,3]) 將函數 f 套用到列表每個元素。'},
    {title:'Y Combinator', content:'在沒有命名函數的純函數式語言中，Y Combinator 實現了遞迴。Z Combinator 是 Y Combinator 在嚴格求值語言（如 Python）中的變體。'},
    {title:'與 λ 演算的關係', content:'函數式編程直接源於 λ 演算。Church 編碼可用純函數表示所有資料結構，體現了函數式編程"一切皆為函數"的哲學。'},
  ],
};

TOPIC_CONTENT['轉換法'] = {
  en: 'Transform Methods',
  sections: [
    {title:'核心概念', content:'轉換法將資料從一個域映射到另一個域，在新域中處理後再轉換回來。最著名的例子是傅立葉轉換（Fourier Transform），將時域訊號轉換為頻域表示。'},
    {title:'傅立葉轉換 (FFT)', content:'快速傅立葉轉換（FFT）在 O(n log n) 時間內計算離散傅立葉轉換（DFT）。應用包括：音訊/影像處理、頻譜分析、卷積加速、JPEG 壓縮（離散餘弦轉換 DCT）。'},
    {title:'Walsh-Hadamard 轉換', content:'使用方波而非正弦波的轉換方法，計算僅需加減法（無乘法），適合硬體實現。應用於影像壓縮、錯誤更正碼、量子計算。'},
  ],
};

var CATEGORY_TAGS = {
  '演算法的歷史': ['概論', '歷史'],
  '時間複雜度': ['基礎', '效率'],
  '空間複雜度': ['基礎', '記憶體'],
  '資料結構': ['基礎', '資料組織'],
  '排序演算法': ['排序', '經典'],
  '搜尋法': ['搜尋', '圖形'],
  '雜湊法': ['搜尋', '資料結構'],
  '圖形演算法': ['圖論', '最短路徑'],
  '網路流': ['圖論', '最佳化'],
  '分割擊破法': ['設計策略', '遞迴'],
  '動態規劃法': ['設計策略', '最佳化'],
  '貪婪法': ['設計策略', '最佳化'],
  '遞迴法': ['設計策略', '基礎'],
  '迭代法': ['設計策略', '數值'],
  '列舉法': ['設計策略', '暴力'],
  '暴力法': ['設計策略', '暴力'],
  '前處理法': ['設計策略', '最佳化'],
  '查表法': ['設計策略', '最佳化'],
  '化簡法': ['設計策略', '理論'],
  '逼近法': ['設計策略', 'NP-困難'],
  '近似演算法': ['設計策略', 'NP-困難'],
  '梯度下降法': ['最佳化', '機器學習'],
  '爬山演算法': ['最佳化', '搜尋'],
  '遺傳演算法': ['最佳化', '演化'],
  '蒙地卡羅法': ['隨機', '模擬'],
  '隨機演算法': ['隨機', '概率'],
  '字串演算法': ['字串', '比對'],
  '計算幾何': ['幾何', '圖形'],
  '線性規劃': ['最佳化', '數學'],
  '有限狀態機': ['計算理論', '自動機'],
  '形式語法': ['計算理論', '語言'],
  'Lambda Calculus': ['計算理論', '函數式'],
  '圖靈機': ['計算理論', '可計算性'],
  '函數式編程': ['程式範式', '函數式'],
  '轉換法': ['數值', '訊號處理'],
};

function findCodeForTopic(topicName) {
  var map = {
    '排序演算法':['method/05b-改良法/bubbleSort.py','method/07-分割擊破法/mergeSort/mergesort.py','method/13-前處理法/01-sort/sort.py'],
    '搜尋法':['method/06-搜尋法/graph_search.py','method/06-搜尋法/04-framework/stateSearch.py','method/06-搜尋法/05a-Astar/astar.py'],
    '圖形演算法':['method/06-搜尋法/02-search/graph_search.py','method/05f-貪婪法/03-minimumSpanningTree/kruskal1.py'],
    '動態規劃法':['method/08-動態規劃法/editDistance/editDistance.py','method/08-動態規劃法/combinatorial/CnkDynamic.py'],
    '分割擊破法':['method/07-分割擊破法/mergeSort/mergesort.py','method/07-分割擊破法/binSearch/binSearch.py','method/07-分割擊破法/matrixMul/strassen.py'],
    '貪婪法':['method/05f-貪婪法/01-huffmanCode/huffmanCode1.py','method/05f-貪婪法/03-minimumSpanningTree/kruskal1.py'],
    '遞迴法':['method/04a-遞迴法/01-basic/fibonacci.py','method/04a-遞迴法/01-basic/hanoi.py','method/04a-遞迴法/01-basic/gcd.py'],
    '迭代法':['method/03-迭代法/01b-newton/newton.py','method/03-迭代法/03-pageRank/pageRankGpt1.py'],
    '梯度下降法':['method/05e-梯度下降法/03-梯度下降法/gd.py','method/05e-梯度下降法/03-梯度下降法/gdRegression.py','method/05e-梯度下降法/A1-隨機梯度下降法/sgd1.py'],
    '爬山演算法':['method/05c-爬山演算法/hillClimbing1/hillClimbing1.py','method/05c-爬山演算法/hillClimbingN/hillClimbingN.py'],
    '遺傳演算法':['method/09c-遺傳演算法/geneticAlgorithm.py','method/09c-遺傳演算法/keyGa.py'],
    '蒙地卡羅法':['method/09b-蒙地卡羅法/01-pi/monteCarloPi.py','method/09b-蒙地卡羅法/05-markov/gibbs.py','method/09b-蒙地卡羅法/05b-MCMC/02-Metropolis/metroBayes1.py'],
    '隨機演算法':['method/09a-隨機法/random1.py','method/09b-蒙地卡羅法/01-pi/monteCarloPi.py'],
    '字串演算法':['method/06-搜尋法','method/02a-列舉法/03-permutation/permutation.py'],
    '線性規劃':['method/15a-委託法/ipDelegate.py','method/15c-化約法/整數規劃/knapsack1.py'],
    '計算幾何':['method/07-分割擊破法/mergeSort/mergesort.py'],
    '前處理法':['method/13-前處理法/01-sort/sort.py','method/13-前處理法/03-hash/hash.py'],
    '查表法':['method/01-查表法/fiboanacci/fibonacci_lookup.py','method/01-查表法/combinatorial/CnkRLookup.py'],
    '列舉法':['method/02a-列舉法/01-truthTable/truthTable.py','method/02a-列舉法/03-permutation/permutation.py'],
    '暴力法':['method/02c-暴力法/solve/bruteForce1.py','method/02c-暴力法/bitcoin/mining.py'],
    '雜湊法':['method/02b-雜湊法/hashtable/hash1.py','method/02b-雜湊法/bloomFilter/bloomFilter.py'],
    '化簡法':['method/15c-化約法/整數規劃/knapsack1.py','method/15c-化約法/整數規劃/tsp1.py'],
    '近似演算法':['method/16-逼近法/01-e/e.py','method/16-逼近法/07-integration/integration.py'],
    '逼近法':['method/16-逼近法/01-e/e.py','method/16-逼近法/02-sterling/sterling.py','method/16-逼近法/06-taylor/taylor.py'],
    '空間複雜度':['method/00-bigO/bubbleSort.py','method/00-bigO/matrixMul.py'],
    '時間複雜度':['method/00-bigO/bubbleSort.py','method/00-bigO/lsearch.py','method/00-bigO/matrixMul.py'],
    '資料結構':['method/02b-雜湊法/hashtable/hash1.py','method/04a-遞迴法/01-basic/gcd.py'],
    '網路流':['method/03-迭代法/01a-equation','method/05f-貪婪法/03-minimumSpanningTree'],
    '有限狀態機':['theory/finiteStateMachine/finiteStateMachine.py','theory/finiteStateMachine/anbm.py','theory/finiteStateMachine/even0.py'],
    '形式語法':['theory/grammar/grammar.py','theory/grammar/anbn.py','theory/grammar/anbncn.py','theory/grammar/english.py'],
    'Lambda Calculus':['method/04c-LambdaCalculus/lambdaCalculus.py','method/04c-LambdaCalculus/01-bool/bool.py','method/04c-LambdaCalculus/04-numerial/numerial.py'],
    '圖靈機':['theory/turingMachine/TuringMachine.py'],
    '函數式編程':['method/04b-函數式編程/01-fp/fp.py','method/04b-函數式編程/07-combinator/02-ycombinator/ycombinator1.py'],
    '轉換法':['method/18-轉換法/02-fourier/01-fft/fft1.py','method/18-轉換法/02-fourier/DFT.py','method/18-轉換法/03-walsh/walsh.py'],
    '演算法的歷史':[],
  };
  return map[topicName] || [];
}

var curTab = '概念';
var curTopic = null;

function showCard(item) {
  var h = '<div class="card" onclick="openTopic(\'' + item.name + '\')">';
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
    html += '<div class="card">';
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
      html += '<a href="javascript:void(0)" onclick="openTopic(\'' + related[r] + '\')">' + related[r] + '</a>';
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
        html += '<div class="card">';
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
