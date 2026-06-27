var WIKI = 'https://github.com/ccckmit/course0/blob/main/wiki/%E8%A8%88%E7%AE%97%E7%90%86%E8%AB%96';
var CODE = 'https://github.com/ccckmit/course0/blob/main/code/%E8%A8%88%E7%AE%97%E7%90%86%E8%AB%96';

var TOPICS = {
  '計算理論基礎': [
    {name:'計算理論', file:'計算理論.md', desc:'可計算性與複雜度理論的核心概念', tags:['Computability','Complexity','P vs NP']},
    {name:'圖靈機', file:'圖靈機.md', desc:'通用計算模型 — 無限磁帶、停機問題', tags:['Turing Machine','Halting Problem','Church-Turing']},
    {name:'自動機理論', file:'自動機理論.md', desc:'形式語言與計算模型的分層體系', tags:['Automata','Formal Language','PDA']},
  ],
  '形式語言與自動機': [
    {name:'有限狀態機', file:'有限狀態機.md', desc:'正則語言與 DFA/NFA 的數學模型', tags:['FSM','DFA','NFA','Regular Language']},
    {name:'形式文法', file:'形式文法.md', desc:'描述語言語法的數學工具與 Chomsky 階層', tags:['Grammar','CFG','Chomsky Hierarchy','BNF']},
  ],
  '計算複雜度': [
    {name:'NP完全性', file:'NP完全性.md', desc:'NP 完全問題與 P vs NP 千年難題', tags:['NP-Complete','Complexity','SAT','Reduction']},
  ],
  '函數式計算': [
    {name:'Lambda演算', file:'Lambda演算.md', desc:'函數式計算的數學基礎 — Church 編碼、Y Combinator', tags:['Lambda Calculus','Church Encoding','Y Combinator']},
    {name:'函數式編程', file:'函數式編程.md', desc:'以函數為核心的程式設計範式', tags:['Functional Programming','Immutable','Higher-order']},
  ],
  '資訊理論': [
    {name:'資訊理論', file:'資訊理論.md', desc:'資訊量的數學理論 — 熵、編碼、通道容量', tags:['Information Theory','Entropy','Shannon','Huffman']},
  ],
  '人物': [
    {name:'Noam_Chomsky', file:'Noam_Chomsky.md', desc:'語言學家與認知科學家 — 形式文法之父', tags:['Chomsky','Linguistics','Universal Grammar']},
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
  // --- 有限狀態機 ---
  {name:'FSM 核心類別', path:'finiteStateMachine', files:['finiteStateMachine.py'], desc:'通用有限狀態機實作，使用字串鍵轉移表'},
  {name:'FSM aⁿ', path:'finiteStateMachine', files:['an.py'], desc:'接受 aⁿ 的 DFA — 接受任意多個 a'},
  {name:'FSM aⁿbᵐ', path:'finiteStateMachine', files:['anbm.py'], desc:'接受 aⁿbᵐ 的 DFA — a 後接 b'},
  {name:'FSM 偶數個 0', path:'finiteStateMachine', files:['even0.py'], desc:'接受二進位字串中 0 的個數為偶數的 DFA'},
  // --- 形式文法 ---
  {name:'文法生成器', path:'grammar', files:['grammar.py'], desc:'隨機展開非終止符產生字串的通用引擎'},
  {name:'文法 aⁿbⁿ', path:'grammar', files:['anbn.py'], desc:'CFG 產生 aⁿbⁿ（上下文無關語言範例）'},
  {name:'文法 aⁿbᵐ', path:'grammar', files:['anbm.py'], desc:'CFG 產生 aⁿbᵐ'},
  {name:'文法 aⁿbⁿcⁿ', path:'grammar', files:['anbncn.py'], desc:'CSG 產生 aⁿbⁿcⁿ（上下文有關語言範例）'},
  {name:'英文句子文法', path:'grammar', files:['english.py','english2.py','english3.py'], desc:'CFG 產生簡單英文句子（含形容詞、關係子句）'},
  {name:'算術表達式文法', path:'grammar', files:['exp.py'], desc:'CFG 產生算術表達式'},
  // --- λ 演算 ---
  {name:'λ 演算完整實作', path:'lambda/01-full', files:['lambdaCalculus.py'], desc:'Church 編碼完整實作：布林、數字、列表、Y Combinator'},
  {name:'λ 演算精簡版', path:'lambda/02-short', files:['lambdaCalculus.py'], desc:'精簡 Church 編碼，Y Combinator 計算階乘'},
  {name:'λ 演算直譯器', path:'lambda/03-interpreter', files:['lambda.py'], desc:'λ 表達式解析器、β-歸約、Call-by-Name 求值'},
  {name:'λ 演算範例', path:'lambda/03-interpreter/example', files:['one.lc','and.lc','fact.lc'], desc:'λ 演算直譯器的測試範例檔案'},
  // --- 圖靈機 ---
  {name:'圖靈機模擬器', path:'turingMachine', files:['TuringMachine.py'], desc:'通用圖靈機類別，含 aⁿbⁿcⁿ 測試'},
  // --- 直譯器 ---
  {name:'BASIC 直譯器', path:'interpreter/basic', files:['basic.py'], desc:'行號式 BASIC 直譯器，支援 LET/PRINT/INPUT/GOTO/IF'},
  {name:'BASIC 範例程式', path:'interpreter/basic/bas', files:['gcd.bas','guess.bas','fib.bas','sum.bas','hello.bas'], desc:'BASIC 範例：最大公因數、猜數字、費氏數列'},
  {name:'JS 直譯器', path:'interpreter/js0i', files:['js0i.js'], desc:'ES6 JavaScript 直譯器，Pratt Parser + Tree-walking'},
  {name:'Lisp 直譯器', path:'interpreter/lisp', files:['lisp.py'], desc:'S-expression 直譯器，101 行精簡實作'},
  {name:'Prolog 直譯器', path:'interpreter/prolog', files:['prolog.py'], desc:'邏輯編程直譯器，含 unification + 回溯搜尋'},
  {name:'Prolog 範例', path:'interpreter/prolog/pl', files:['family.pl','qsort.pl','fact.pl','hanoi.pl'], desc:'Prolog 範例：家族關係、快速排序、河內塔'},
  {name:'Python 直譯器', path:'interpreter/py0i', files:['py0i.py'], desc:'自我託管 Python 直譯器，1017 行 AST 求值'},
];

var TOPIC_CONTENT = {};

TOPIC_CONTENT['計算理論'] = {
  en: 'Theory of Computation',
  sections: [
    {title:'什麼是計算？', content:'計算理論探討計算的本質、能力與極限。核心問題是：什麼問題可以被自動化地解決？Hilbert 在 1900 年提出的 Entscheidungsproblem（判定問題）直接催生了這個領域。1936 年，Turing 和 Church 分別以圖靈機和 λ 演算形式化了「有效計算」的概念。Church-Turing 論點主張：任何直觀上可有效計算的函數都能被圖靈機計算。'},
    {title:'計算模型階層', content:'不同的計算模型對應不同的語言類別，形成 Chomsky 階層：Type 3（正則語言/有限狀態機）、Type 2（上下文無關語言/下推自動機）、Type 1（上下文有關語言/線性有界自動機）、Type 0（遞歸可枚舉語言/圖靈機）。每種模型在表達能力和計算資源需求之間做出權衡。正則表達式、程式語言文法、自然語言處理分別對應不同的階層。'},
    {title:'可計算性與複雜度', content:'可計算性理論研究哪些問題是可計算的。停機問題是最著名的不可計算問題。Rice 定理說任何非平凡的語義性質都是不可判定的。複雜度理論研究問題的固有困難度，將問題分為 P（多項式時間可解）、NP（多項式時間可驗證）、NP-完全（NP 中最困難的問題）等類別。P vs NP 是電腦科學中最重要的未解決問題。'},
  ],
};

TOPIC_CONTENT['圖靈機'] = {
  en: 'Turing Machine',
  sections: [
    {title:'形式定義', content:'圖靈機由 7-元組 M = (Q, Σ, Γ, δ, q₀, q_accept, q_reject) 定義。Q 是有限狀態集合，Σ 是輸入字母表，Γ 是磁帶字母表（包含空白符號 _），δ: Q×Γ → Q×Γ×{L,R} 是轉移函數，q₀ 是起始狀態，q_accept 和 q_reject 是接受/拒絕狀態。轉移函數 δ(q, a) = (p, b, L/R) 表示：在狀態 q 讀到 a，寫入 b，移動磁頭到左/右，進入狀態 p。'},
    {title:'計算過程', content:'圖靈機擁有無限長的磁帶，讀寫頭可以在磁帶上左右移動。初始時輸入字串放在磁帶最左端，其餘為空白。每一步，圖靈機讀取當前磁頭位置的符號，根據轉移函數決定寫入哪個符號、移動方向、和下個狀態。如果進入接受或拒絕狀態就停機。著名的 aⁿbⁿ 識別範例展示了圖靈機如何使用狀態來計數。'},
    {title:'停機問題', content:'停機問題：是否存在一個演算法可以判定任意給定的圖靈機在任意輸入上是否會停機？Turing 用對角線法證明不存在這樣的演算法。證明假設存在這樣的 HALT 函數，然後構造一個 PARADOX 機：如果 HALT 說它會停機就進入無窮迴圈，反之則停機。矛盾證明了 HALT 不可能存在。這揭示了計算的內在局限性。'},
    {title:'通用圖靈機', content:'通用圖靈機（UTM）可以模擬任何其他圖靈機的行為。UTM 接受兩個輸入：被模擬圖靈機的描述（編碼為符號串）和該圖靈機的輸入。UTM 的概念預示了儲存程式電腦（von Neumann 結構）的誕生。Church-Turing 論點認為一切有效計算過程都可被圖靈機模擬。'},
  ],
};

TOPIC_CONTENT['自動機理論'] = {
  en: 'Automata Theory',
  sections: [
    {title:'核心問題', content:'自動機理論的核心問題是：什麼語言可以被什麼樣的自動機識別？語言、自動機、文法的對應關係構成了形式語言理論的基石。有限自動機 (FA) 識別正則語言，下推自動機 (PDA) 識別上下文無關語言，線性有界自動機 (LBA) 識別上下文有關語言，圖靈機 (TM) 識別遞歸可枚舉語言。'},
    {title:'有限自動機', content:'確定性有限自動機 (DFA) 由五元組 (Q, Σ, δ, q₀, F) 定義。非確定性有限自動機 (NFA) 允許一個輸入對應多個可能的轉移，以及 ε-轉移（不消耗輸入的轉移）。DFA 和 NFA 的表達能力等價（子集構造法）。正則表達式也與 FA 等價（Thompson 構造法將 regex 轉為 NFA）。'},
    {title:'下推自動機', content:'下推自動機 (PDA) 在 FA 的基礎上增加一個無限棧，可以儲存和讀取符號。PDA 的計算依賴於棧的頂端符號來決定轉移。PDA 和上下文無關文法 (CFG) 等價：任何 CFG 生成的語言都可以被某個 PDA 識別，反之亦然。aⁿbⁿ 和匹配括號是需要棧記憶的典型語言。'},
  ],
};

TOPIC_CONTENT['有限狀態機'] = {
  en: 'Finite State Machine',
  sections: [
    {title:'DFA 與 NFA', content:'確定性有限狀態機 (DFA) 的每個狀態對每個輸入符號恰好有一個轉移。非確定性有限狀態機 (NFA) 允許從一個狀態對同一輸入有多個轉移，以及 ε-轉移。雖然 NFA 看起來比 DFA 強大，但兩者表達能力完全相同：任何 NFA 都可以通過子集構造法轉換為等價的 DFA。DFA 的狀態數可能指數增長（相對於 NFA）。'},
    {title:'正則語言', content:'能被有限狀態機識別的語言稱為正則語言。正則語言封閉於聯集、交集、補集、串接、Kleene 星號等運算。正則表達式是描述正則語言的便利語法。Myhill-Nerode 定理提供了判斷語言是否為正則的充要條件，並給出最小 DFA 的唯一性。 pumping lemma 用於證明某些語言不是正則的。'},
    {title:'應用與限制', content:'有限狀態機的應用包括：詞法分析器（lex/flex）、網路協定狀態機（TCP）、數位電路設計、遊戲 AI 狀態行為。限流狀態機的關鍵限制是缺乏記憶能力：無法識別 aⁿbⁿ（需要計數）、無法匹配嵌套括號。這些限制推動了下推自動機和圖靈機的發展。'},
  ],
};

TOPIC_CONTENT['形式文法'] = {
  en: 'Formal Grammar',
  sections: [
    {title:'Chomsky 階層', content:'Chomsky 在 1956 年將形式文法分為四個層次：Type 0（無限制文法）對應圖靈機和遞歸可枚舉語言；Type 1（上下文有關文法）使用 αAβ → αγβ 形式規則，對應線性有界自動機；Type 2（上下文無關文法）使用 A → γ 形式規則，對應下推自動機和大多數程式語言的語法；Type 3（正則文法）使用 A → aB 或 A → a 規則，對應有限狀態機。'},
    {title:'上下文無關文法', content:'CFG 由四元組 G = (N, Σ, P, S) 定義，其中 N 是非終止符集合，Σ 是終止符集合，P 是產生式規則，S 是起始符號。BNF (Backus-Naur Form) 是描述 CFG 的標準記法。算術表達式 E → E+T | T, T → T*F | F, F → (E) | id 是經典範例。parse tree 展示了推導的結構。LL(k) 和 LR(k) 是兩大類解析方法。'},
    {title:'解析方法', content:'自上而下解析（Top-Down Parsing）從起始符號開始，嘗試匹配輸入字串，如遞迴下降解析和 LL 解析。自下而上解析（Bottom-Up Parsing）從輸入字串歸約到起始符號，如 LR 解析。LL(1) 使用一個 lookahead token 決定產生式。LR(1) 使用 LR 自動機。LALR(1)（Yacc/Bison 使用）是 LR(1) 的簡化。'},
  ],
};

TOPIC_CONTENT['NP完全性'] = {
  en: 'NP-Completeness',
  sections: [
    {title:'P 與 NP', content:'P 類包含所有可以在多項式時間內由確定性圖靈機解決的問題（如排序、最短路徑、線性規劃）。NP 類包含所有可以在多項式時間內由非確定性圖靈機解決的問題，等價於解可在多項式時間內驗證的問題（如 SAT 的滿足指派、Hamiltonian 迴路、TSP 路徑）。顯然 P ⊆ NP，但 P = NP 是否成立是懸而未決的千年難題。'},
    {title:'NP-完全性與歸約', content:'一個問題 L 是 NP-完全的如果：1) L ∈ NP，2) 每個 NP 問題都可以多項式時間歸約到 L。Cook-Levin 定理（1971）證明 SAT 是第一個 NP-完全問題。歸約是證明 NP-完全性的核心工具：如果問題 A 可以歸約到 B，則 B 至少和 A 一樣難。經典 NPC 問題包括 3-SAT、Vertex Cover、Hamiltonian Cycle、TSP、Subset Sum、Clique。'},
    {title:'應對策略', content:'NP-完全問題雖然困難，但在實際中仍有方法應對：精確演算法（分支限界、整數規劃）、參數化演算法（將複雜度集中在某個參數上）、近似演算法（如 Vertex Cover 的 2-近似）、隨機演算法（隨機化期望多項式時間）、特殊情況分析（樹狀結構、二分圖等限制可簡化問題）。'},
  ],
};

TOPIC_CONTENT['Lambda演算'] = {
  en: 'Lambda Calculus',
  sections: [
    {title:'基本語法與歸約', content:'λ 演算由 Alonzo Church 於 1936 年提出。語法只有三種表達式：變數 x、抽象 λx.M、應用 M N。三種轉換規則：α-轉換（重新命名綁定變數）允許 λx.x ≡ λy.y；β-歸約 (λx.M)N → M[x:=N] 是函數應用；η-轉換 λx.Mx → M（當 x 不在 M 中自由出現）。β-歸約是計算的核心步驟。'},
    {title:'Church 編碼', content:'Church 編碼將所有資料結構表示為函數。Church 布林值：TRUE = λx.λy.x，FALSE = λx.λy.y，IF = λp.λa.λb.p a b。Church 數字：ZERO = λf.λx.x，ONE = λf.λx.f x，SUCC = λn.λf.λx.f (n f x)。算術運算如 ADD = λm.λn.λf.λx.m f (n f x)。所有資料都是閉包。'},
    {title:'Y Combinator', content:'Y Combinator Y = λf.(λx.f (x x)) (λx.f (x x)) 在 λ 演算中實現遞迴。由於 λ 演算中的函數沒有名字，無法直接引用自身。Y 組合子可以計算任何遞迴函數的不動點：Y f = f (Y f)。在嚴格求值語言中需使用 Z 組合子（η-展開版本）。Y Combinator 展示了 λ 演算的表達能力與圖靈機等價。'},
  ],
};

TOPIC_CONTENT['函數式編程'] = {
  en: 'Functional Programming',
  sections: [
    {title:'核心概念', content:'函數式編程以 λ 演算為理論基礎。核心原則包括：一等函數（函數可以作為參數傳遞和作為返回值）、不可變性（資料一旦建立不允許修改）、純函數（無副作用、相同輸入產生相同輸出）。這些原則使得函數式程式易於測試、並行和推理。'},
    {title:'高階函數', content:'高階函數是操作其他函數的函數。map 對列表每個元素應用函數。filter 選出滿足條件的元素。reduce/fold 將整個列表折疊為一個值。compose 將多個函數串聯。currying 將多參數函數轉換為一系列單參數函數。這些模式在函數式語言中無處不在，且已滲透到主流語言（JavaScript 的 Array.map/filter/reduce）。'},
    {title:'語言與應用', content:'Haskell 是純函數式、惰性求值的代表。Lisp 方言（Scheme、Clojure）是動態函數式的典範。ML/OCaml/F# 加入型別推論和模式匹配。Scala 融合 OO 和 FP。函數式特性已進入主流：Java Stream API、C# LINQ、Python list comprehension、JavaScript arrow functions。React 使用不可變狀態和純組件。函數式架構（Elm、Redux）提供可預測的狀態管理。'},
  ],
};

TOPIC_CONTENT['資訊理論'] = {
  en: 'Information Theory',
  sections: [
    {title:'熵與資訊量', content:'Shannon 在 1948 年提出資訊熵 H(X) = -Σ p(x) log₂ p(x) 量化隨機變數的不確定性。熵是編碼一個符號所需的最小平均位元數。均勻分布時熵最大，確定性分布時熵為零。二元熵函數在 p=0.5 時達到最大值 1 bit。交叉熵 H(p,q) 和 KL 散度 D_KL(p‖q) 衡量兩個分布的差異。'},
    {title:'編碼理論', content:'Huffman 編碼是最優前綴碼，透過貪婪演算法構建編碼樹。平均碼長接近熵。算術編碼將整個訊息編碼為一個 [0,1) 區間內的數字，可達到位元級編碼效率。Lempel-Ziv（LZ77/LZ78）是字典編碼的基礎（ZIP、PNG、GIF）。通道編碼（Hamming 碼、Reed-Solomon 碼、LDPC 碼）在傳輸中糾錯。'},
    {title:'通道容量', content:'Shannon 通道編碼定理給出了在雜訊通道上可靠通訊的最大速率（通道容量）。對於 AWGN 通道，容量 C = B log₂(1 + S/N)。通道編碼定理保證存在編碼方案可以任意低的錯誤率逼近容量，但未給出具體構造方法。Turbo 碼和 LDPC 碼在實際中逼近了 Shannon 極限。'},
  ],
};

TOPIC_CONTENT['Noam_Chomsky'] = {
  en: 'Noam Chomsky',
  sections: [
    {title:'語言學貢獻', content:'Chomsky 在 1957 年《句法結構》中提出轉換生成語法（Transformational-Generative Grammar），徹底改變了語言學。他區分深層結構（語義解釋）和表層結構（語音實現），轉換規則將前者映射到後者。1959 年對 Skinner《口語行為》的評論終結了行為主義在語言學的主導地位。他主張人類天生具有語言習得裝置（LAD）和普遍語法（UG）。'},
    {title:'Chomsky 階層', content:'Chomsky 在形式語言理論的奠基性工作中提出了文法的四層階層。每一層對應一種自動機和一個語言類別：Type 0（無限制→圖靈機）、Type 1（上下文有關→線性有界自動機，使用 αAβ → αγβ 規則）、Type 2（上下文無關→下推自動機，A → γ 規則，對應 BNF 和程式語言語法）、Type 3（正則→有限狀態機，A → aB 或 A → a 規則，對應正則表達式）。這對編譯器設計、計算語言學和理論電腦科學產生了深遠影響。'},
    {title:'公共知識份子', content:'Chomsky 也是美國最具影響力的公共知識份子之一。他對越戰的批評、媒體操控的理論（《製造共識》）、以及對美國外交政策的持續批判影響深遠。作為認知科學的奠基人之一，他將語言學與心理學、哲學聯繫起來，反對實證主義，支持心靈主義和內在論。他的工作雖然在學術界備受尊敬，但也引發了持續的爭議和討論。'},
  ],
};

function findCodeForTopic(topicName) {
  var map = {
    '計算理論':['turingMachine/TuringMachine.py','finiteStateMachine/finiteStateMachine.py','grammar/grammar.py'],
    '圖靈機':['turingMachine/TuringMachine.py'],
    '自動機理論':['finiteStateMachine/finiteStateMachine.py','finiteStateMachine/an.py','finiteStateMachine/anbm.py'],
    '有限狀態機':['finiteStateMachine/finiteStateMachine.py','finiteStateMachine/an.py','finiteStateMachine/anbm.py','finiteStateMachine/even0.py'],
    '形式文法':['grammar/grammar.py','grammar/anbn.py','grammar/anbncn.py','grammar/english.py','grammar/exp.py'],
    'NP完全性':[],
    'Lambda演算':['lambda/01-full/lambdaCalculus.py','lambda/02-short/lambdaCalculus.py','lambda/03-interpreter/lambda.py'],
    '函數式編程':['lambda/01-full/lambdaCalculus.py','interpreter/lisp/lisp.py'],
    '資訊理論':[],
    'Noam_Chomsky':[],
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
    html += '</div></div>';
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
