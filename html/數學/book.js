var WIKI = 'https://github.com/ccckmit/course0/blob/main/wiki/%E6%95%B8%E5%AD%B8';
var CODE = 'https://github.com/ccckmit/course0/blob/main/code/%E6%95%B8%E5%AD%B8';

var TOPICS = {
  '數學基礎': [
    {name:'數學的歷史', file:'數學的歷史.md', desc:'從古希臘幾何到現代抽象數學的發展歷程', tags:['History','Overview']},
    {name:'集合論', file:'集合論.md', desc:'現代數學的基礎語言 — 集合、運算、ZFC 公理', tags:['Set Theory','Foundation','ZFC']},
    {name:'邏輯學', file:'邏輯學.md', desc:'數理邏輯 — 命題邏輯、謂詞邏輯、Gödel 不完備定理', tags:['Logic','Propositional','Predicate','Godel']},
    {name:'數論', file:'數論.md', desc:'整數性質的研究 — 質數、同餘、RSA 密碼學', tags:['Number Theory','Prime','Modular']},
  ],
  '代數與幾何': [
    {name:'代數學', file:'代數學.md', desc:'群、環、體等抽象代數結構及其在電腦科學的應用', tags:['Algebra','Group','Ring','Field']},
    {name:'幾何學', file:'幾何學.md', desc:'空間中點線面體的性質與關係 — 計算幾何的基礎', tags:['Geometry','Euclidean','Computational Geometry']},
    {name:'微分幾何', file:'微分幾何.md', desc:'曲線曲面的局部幾何 — 曲率、流形、黎曼幾何', tags:['Differential Geometry','Manifold','Curvature']},
    {name:'拓撲學', file:'拓撲學.md', desc:'連續變形下不變的性質 — 連通性、緊緻性、同倫', tags:['Topology','Continuity','Homotopy']},
  ],
  '微積分與分析': [
    {name:'微積分', file:'微積分.md', desc:'微分與積分 — 變化率與累積量的核心工具', tags:['Calculus','Derivative','Integral']},
    {name:'複變函數', file:'複變函數.md', desc:'複數域上的解析函數 — Cauchy 積分公式、留數定理', tags:['Complex Analysis','Analytic','Residue']},
    {name:'泛函分析', file:'泛函分析.md', desc:'無限維向量空間及其映射 — Banach/Hilbert 空間', tags:['Functional Analysis','Banach','Hilbert']},
    {name:'測度論', file:'測度論.md', desc:'集合大小的嚴格定義 — Lebesgue 積分、機率論基礎', tags:['Measure Theory','Lebesgue','Integration']},
    {name:'變分學', file:'變分學.md', desc:'泛函極值問題 — Euler-Lagrange 方程、最速降線', tags:['Calculus of Variations','Euler-Lagrange','Functional']},
  ],
  '機率與統計': [
    {name:'機率統計', file:'機率統計.md', desc:'隨機現象的規律 — 機率論、統計推論、假設檢定', tags:['Probability','Statistics','Hypothesis Testing']},
    {name:'隨機微積分', file:'隨機微積分.md', desc:'隨機過程的積分理論 — Ito 積分、Black-Scholes 模型', tags:['Stochastic Calculus','Ito','Brownian Motion']},
  ],
  '應用數學': [
    {name:'數值分析', file:'數值分析.md', desc:'計算機上求解數學問題的近似方法與誤差分析', tags:['Numerical Analysis','Floating Point','Approximation']},
    {name:'密碼學', file:'密碼學.md', desc:'資訊保密與安全的數學 — 對稱/非對稱加密、簽章', tags:['Cryptography','Encryption','RSA','AES']},
    {name:'物理數學', file:'物理數學.md', desc:'數學方法在物理學的應用 — 力學、電磁學、量子力學', tags:['Mathematical Physics','Mechanics','Quantum']},
  ],
  '物理學中的數學': [
    {name:'狹義相對論', file:'狹義相對論.md', desc:'時間與空間的相對性 — Lorentz 變換、質能等價', tags:['Special Relativity','Lorentz','E=mc^2']},
    {name:'廣義相對論', file:'廣義相對論.md', desc:'重力作為時空彎曲 — Einstein 場方程、黑洞', tags:['General Relativity','Gravity','Black Hole']},
    {name:'量子力學', file:'量子力學.md', desc:'微觀粒子行為的數學描述 — 薛丁格方程、疊加態', tags:['Quantum Mechanics','Schrodinger','Wave Function']},
    {name:'量子電腦', file:'量子電腦.md', desc:'利用量子疊加與糾纏的計算 — Shor/Grover 演算法', tags:['Quantum Computing','Qubit','Shor','Grover']},
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
  // --- 02-代數 ---
  {name:'群公理驗證', path:'02-代數/_ccc', files:['group.py','group_axioms.py'], desc:'Python 驗證群封閉性、結合律、單位元、反元素'},
  {name:'體公理驗證', path:'02-代數/_ccc', files:['field_axioms.py','field_rational.py'], desc:'有理數體的公理驗證'},
  {name:'有理數類別', path:'02-代數/_ccc', files:['rational_number.py'], desc:'有理數的分數表示與運算'},
  // --- 03-幾何 ---
  {name:'2D 幾何向量', path:'03-幾何/_ccc/2d', files:['geometry2d_vector.py','geometry2d_points.py'], desc:'平面幾何的向量與座標運算'},
  {name:'N 維幾何', path:'03-幾何/_ccc/nd', files:['geometry_nd.py','transform.py'], desc:'N 維空間點、向量與變換'},
  {name:'幾何證明', path:'03-幾何/_ccc/geo_proof', files:['geo_proof1.py','geo_proof2.py'], desc:'自動化幾何定理證明'},
  // --- 04-微積分 ---
  {name:'微分與導數', path:'04-微積分/_ccc/02-梯度優化/01-微分', files:['diff.py','diff.c'], desc:'數值與符號微分實作'},
  {name:'梯度下降', path:'04-微積分/_ccc/02-梯度優化/03-梯度下降法', files:['gd.py','gdRegression.py','gdArray.py'], desc:'梯度下降最佳化演算法'},
  {name:'microGrad 自動微分', path:'04-微積分/_ccc/02-梯度優化/04-反傳遞算法/02-microGrad/micrograd', files:['engine.py','nn.py'], desc:'Karpathy 風格微型自動微分引擎'},
  {name:'macroGrad 自動微分', path:'04-微積分/_ccc/02-梯度優化/04-反傳遞算法/03-macroGrad/macrograd', files:['engine.py'], desc:'自製自動微分框架'},
  {name:'符號自動微分', path:'04-微積分/_ccc/03-符號自動微分', files:['symGrad.py'], desc:'符號計算梯度的方法'},
  {name:'torchdiy v1 MLP', path:'04-微積分/_ccc/02-梯度優化/05-torchdiy/_more/v1mlp/dtorch', files:['nn.py'], desc:'DIY PyTorch 線性層與 MLP'},
  {name:'torchdiy v5 Transformer', path:'04-微積分/_ccc/02-梯度優化/05-torchdiy/_more/v5transformer/dtorch', files:['transformer.py'], desc:'DIY Transformer 自注意力機制'},
  // --- 05-機率統計 ---
  {name:'機率分布 (statr)', path:'05-機率統計/statr/statr', files:['distributions.py'], desc:'常態、t、卡方、F、二項、Poisson 分布'},
  {name:'描述統計 (statr)', path:'05-機率統計/statr/statr', files:['stats.py'], desc:'平均數、中位數、變異數、共變異數'},
  {name:'假設檢定 (statr)', path:'05-機率統計/statr/statr', files:['tests.py'], desc:'t 檢定、z 檢定、卡方檢定、ANOVA'},
  {name:'中央極限定理', path:'05-機率統計/_bak/02-clt', files:['clt1.py'], desc:'CLT 數值模擬展示'},
  // --- 05b-隨機微積分 ---
  {name:'Ito 積分', path:'05b-隨機微積分/stoc4py', files:['ito.py'], desc:'Ito 隨機積分定義與計算'},
  {name:'布朗運動模擬', path:'05b-隨機微積分/stoc4py', files:['process.py'], desc:'Wiener 過程路徑模擬'},
  {name:'Black-Scholes 定價', path:'05b-隨機微積分/examples', files:['ex3_black_scholes.py'], desc:'選擇權定價的 Black-Scholes 模型'},
  // --- 06-資訊理論 ---
  {name:'Shannon 熵', path:'06-資訊理論/_ccc/01-entropy', files:['entropy1.py','convex1.py'], desc:'資訊熵與凸性計算'},
  {name:'Huffman 編碼', path:'06-資訊理論/_ccc/02-huffmanCode', files:['huffmanCode1.py'], desc:'霍夫曼無失真壓縮演算法'},
  {name:'Hamming 碼', path:'06-資訊理論/_ccc/03-hammingCode', files:['hammingCode1.py'], desc:'漢明錯誤更正碼'},
  // --- 07-線性代數 ---
  {name:'LU 行列式', path:'07-線性代數/_ccc', files:['det_lu.py'], desc:'LU 分解求行列式值'},
  {name:'QR 特徵值演算法', path:'07-線性代數/_ccc', files:['eig_with_qr1.py'], desc:'QR 迭代求特徵值'},
  {name:'SVD 奇異值分解', path:'07-線性代數/_ccc', files:['svd_with_eig1.py'], desc:'透過特徵值分解計算 SVD'},
  {name:'Householder QR', path:'07-線性代數/_ccc/QR', files:['householderQR.py'], desc:'Householder 反射 QR 分解'},
  // --- 08-傅立葉 ---
  {name:'DFT 離散傅立葉', path:'08-傅立葉/_ccc', files:['dft.py'], desc:'離散傅立葉變換實作'},
  {name:'FFT 快速傅立葉', path:'08-傅立葉/_ccc/轉換法/02-fourier/01-fft', files:['fft1.py'], desc:'Cooley-Tukey 快速傅立葉演算法'},
  {name:'Walsh 轉換', path:'08-傅立葉/_ccc/轉換法/03-walsh', files:['walsh.py'], desc:'Walsh-Hadamard 轉換'},
  // --- 08b-複變函數 ---
  {name:'複變函數繪圖', path:'08b-複變函數/01-draw', files:['draw_z2.py','draw_z2_3d.py'], desc:'複數函數 f(z)=z^2 的 2D/3D 視覺化'},
  // --- 09-向量微積分 ---
  {name:'向量場', path:'09-向量微積分', files:['vector_field.py'], desc:'向量場類別與基本運算'},
  {name:'向量場繪圖', path:'09-向量微積分/draw', files:['vector_field_draw.py','grad_field_draw.py'], desc:'梯度/旋度/散度場視覺化'},
  {name:'Green 定理', path:'09-向量微積分/theorem', files:['green_theorem.py'], desc:'格林定理數值驗證'},
  {name:'Stokes 定理', path:'09-向量微積分/theorem', files:['stoke_theorem.py'], desc:'斯托克斯定理數值驗證'},
  {name:'Gauss 散度定理', path:'09-向量微積分/theorem', files:['gauss_theorem.py'], desc:'高斯散度定理數值驗證'},
  // --- 09d-拓樸學 ---
  {name:'Mobius 帶', path:'09d-拓樸學', files:['mobius_tape.py','mobius_tape_animation.py'], desc:'莫比烏斯帶 3D 視覺化與動畫'},
  {name:'Klein 瓶', path:'09d-拓樸學', files:['klein_bottle.py','klein_bottle_animation.py'], desc:'克萊因瓶 3D 視覺化與動畫'},
  // --- 09e-泛函分析 ---
  {name:'交換子運算', path:'09e-泛函分析/01-量子算子理論', files:['01-commutator.py'], desc:'量子力學交換子計算'},
  {name:'Dirac 符號', path:'09e-泛函分析/01-量子算子理論', files:['03-ket.py'], desc:'Bra-Ket 符號與基底展開'},
  // --- 10-微分方程 ---
  {name:'ODE 數值解', path:'10-微分方程', files:['ode1.py'], desc:'常微分方程式數值求解'},
  {name:'SymPy 微分方程', path:'10-微分方程/_more', files:['sympy1_sin.py','sympy5_pde1.py'], desc:'SymPy 符號解 ODE 與 PDE'},
  {name:'1D 波動方程', path:'10-微分方程/_more/1dwave', files:['1dwave_eq_solve.py','1dwave_show.py'], desc:'一維波動方程數值解與視覺化'},
  // --- 11-微分幾何 ---
  {name:'Frenet-Serret 框架', path:'11-微分幾何/_bookcode/03', files:['3.1-Frenet-Serret.py'], desc:'空間曲線的 Frenet-Serret 公式'},
  {name:'Riemann 幾何引擎', path:'11-微分幾何/_dgeom/06-riemann/dgeom', files:['riemann.py','relativity.py'], desc:'黎曼幾何核心：度規、曲率、測地線'},
  {name:'黑洞光線追蹤', path:'11-微分幾何/_bookcode/12', files:['12.1-BlackHoleRayTracer.py'], desc:'Schwarzschild 黑洞光線彎曲模擬'},
  {name:'數值/符號幾何', path:'11-微分幾何/_dgeom/07-num_sym/dgeom/num', files:['calculus.py'], desc:'微分幾何的數值與符號計算工具'},
  // --- 12-物理數學 ---
  {name:'經典力學', path:'12-物理數學/01-經典物理', files:['01-mechanics.py'], desc:'SymPy 經典力學分析'},
  {name:'Schwarzschild 度規', path:'12-物理數學/02-相對論', files:['schwarzschild.py'], desc:'Schwarzschild 黑洞度規推導'},
  {name:'FLRW 宇宙學', path:'12-物理數學/02-相對論', files:['flrw.py'], desc:'Friedmann 宇宙模型'},
  {name:'Kerr 黑洞', path:'12-物理數學/02-相對論', files:['kerr.py'], desc:'Kerr 旋轉黑洞度規推導'},
  {name:'量子力學', path:'12-物理數學/03-量子力學', files:['03-quantum.py','03d-hydrogen.py'], desc:'量子態、算符、氫原子'},
  // --- _ai/lean4py ---
  {name:'lean4py 邏輯', path:'_ai/lean4py/lean4py', files:['logic.py'], desc:'命題邏輯與謂詞邏輯的形式化'},
  {name:'lean4py 集合論', path:'_ai/lean4py/lean4py', files:['sets.py'], desc:'集合論的公理化形式化'},
  {name:'lean4py 代數結構', path:'_ai/lean4py/lean4py', files:['algebra.py'], desc:'群與環的代數結構形式化'},
  {name:'lean4py 定理證明器', path:'_ai/lean4py/lean4py', files:['prover.py'], desc:'自動定理證明 (tableaux)'},
  // --- _sympy ---
  {name:'SymPy 多項式', path:'_sympy/01-basic/04-polynomial', files:['poly1_factor.py'], desc:'SymPy 多項式分解與 Grobner 基'},
  {name:'SymPy RSA', path:'_sympy/01-basic/07-cryptography', files:['crypt2_rsa.py'], desc:'SymPy RSA 加密/解密示範'},
  {name:'SymPy 微分幾何', path:'_sympy/02-dgeom/02-diffgeom', files:['diffgeom.ipynb'], desc:'SymPy 微分幾何互動筆記本'},
  {name:'SymPy 廣義相對論', path:'_sympy/02-dgeom/03-relativity', files:['schwarzschild.ipynb'], desc:'SymPy Schwarzschild 時空互動筆記本'},
];

var TOPIC_CONTENT = {};

TOPIC_CONTENT['數學的歷史'] = {
  en: 'History of Mathematics',
  sections: [
    {title:'古希臘時期', content:'古希臘數學家將數學從實用計算提升為理論科學。Thales 提出幾何定理的邏輯證明。Pythagoras 學派發現勾股定理和無理數。Euclid 的《幾何原本》建立了公理化體系的典範。Archimedes 在面積和體積計算上使用窮竭法，預示了積分思想。'},
    {title:'近代數學的誕生', content:'17 世紀 Newton 和 Leibniz 獨立發明微積分，為科學革命提供數學工具。18 世紀 Euler 在分析、數論、圖論等領域做出開創性貢獻。19 世紀 Gauss、Riemann、Galois 等人將數學推向更高度的抽象。Cantor 的集合論為現代數學奠定了基礎。'},
    {title:'現代數學', content:'20 世紀 Hilbert 提出 23 個問題指引了數學發展。Gödel 的不完備定理揭示了形式系統的內在限制。電腦的出現催生了計算數學、數值分析和資訊理論。數學在密碼學、機器學習、量子計算等領域發揮了關鍵作用。'},
  ],
};

TOPIC_CONTENT['集合論'] = {
  en: 'Set Theory',
  sections: [
    {title:'基本概念', content:'集合是數學中最基本的概念，由確定且互異的對象組成。常用的集合表示法包含列舉法和描述法。子集、聯集、交集、差集、補集構成基本的集合運算。冪集是集合所有子集構成的集合。笛卡兒積生成有序對的集合。'},
    {title:'Russell 悖論與公理化', content:'樸素集合論允許定義「所有不屬於自身的集合的集合」而導致 Russell 悖論。ZFC 公理系統透過正則公理（Foundation Axiom）禁止集合屬於自身來避免悖論。選擇公理（Axiom of Choice）是 ZFC 中最具爭議的公理，等價於 Zorn 引理和良序定理。'},
    {title:'基數與序數', content:'Cantor 提出基數（Cardinal Number）比較集合的大小。可數集（如整數、有理數）與實數集具有不同的基數。連續統假設（Continuum Hypothesis）說實數的基數是自然數基數的下一個，已被證明獨立於 ZFC。序數（Ordinal Number）推廣了自然數到超限的情況。'},
  ],
};

TOPIC_CONTENT['邏輯學'] = {
  en: 'Logic',
  sections: [
    {title:'命題邏輯', content:'命題邏輯研究命題之間的邏輯關係。基本運算包含 AND (∧)、OR (∨)、NOT (¬)、IMPLIES (→)、IFF (↔)。真值表列出所有邏輯組合。邏輯等價（De Morgan 定律、分配律等）可用於公式化簡。命題邏輯是完備且可判定的。'},
    {title:'謂詞邏輯', content:'謂詞邏輯引入量詞 ∀（全稱量詞）和 ∃（存在量詞），以及謂詞和函數符號。可以表達命題邏輯無法表達的語句（如「所有人都是會死的」）。Gödel 完備性定理：謂詞邏輯中的有效公式都是可證明的。但謂詞邏輯是不可判定的。'},
    {title:'Gödel 不完備定理', content:'1931 年 Gödel 證明：任何足夠強的一致形式系統（如皮亞諾算術）都包含一個無法在系統內證明或否證的命題（第一不完備定理）。這樣的系統也無法證明自身的一致性（第二不完備定理）。這些結果從根本上限制了形式化數學的能力，也影響了計算機科學的基礎。'},
  ],
};

TOPIC_CONTENT['數論'] = {
  en: 'Number Theory',
  sections: [
    {title:'質數', content:'質數是大於 1 且只能被 1 和自身整除的自然數。算術基本定理：每個整數可以唯一分解為質因數的乘積。質數分布定理描述了質數的漸近密度。Euclid 證明了質數有無限多個。Riemann ζ 函數與質數分布有深刻聯繫。RSA 密碼系統的安全性依賴於大數分解的困難。'},
    {title:'同餘理論', content:'同餘關係 a ≡ b (mod m) 表示 m 整除 a-b。同餘運算保持加法、減法和乘法。Fermat 小定理：a^(p-1) ≡ 1 (mod p)（p 為質數）。中國剩餘定理：一組兩兩互質的同餘式有唯一解。Euler 定理推廣了 Fermat 小定理。離散對數問題是 ElGamal 加密的基礎。'},
    {title:'應用', content:'RSA：利用大數分解的困難性實現公鑰加密。Diffie-Hellman 密鑰交換基於離散對數問題。橢圓曲線密碼學（ECC）使用更短的密鑰長度。數論在雜湊函數、數位簽章、零知識證明等密碼學協定中扮演核心角色。在電腦科學中，數論也用於檢查素數、隨機數生成和程式碼優化。'},
  ],
};

TOPIC_CONTENT['代數學'] = {
  en: 'Algebra',
  sections: [
    {title:'群論', content:'群是具有封閉性、結合律、單位元和反元素的代數結構 (G, *)。群的例子包含整數加法群、置換群、矩陣群。子群、正規子群、商群是分析群結構的基本工具。同態和同構建立群之間的映射。Klein 四元群、循環群、對稱群是重要的群類型。Lagrange 定理：子群的階整除群的階。'},
    {title:'環論', content:'環是具有加法（形成交換群）和乘法（結合律）兩種運算的代數結構。整數、多項式、矩陣都是環的例子。理想是環的特殊子結構，用於構建商環。整環、體、域的層次結構構成了代數的核心。多項式環是代數幾何和代數數論的基礎。'},
    {title:'體論與應用', content:'體是加法、減法、乘法、除法都封閉的代數結構。有理數體 Q、實數體 R、複數體 C 和有限體 GF(p) 是典型例子。Galois 理論建立了體擴張與群論之間的橋樑。在電腦科學中，有限體用於編碼理論（Reed-Solomon 碼）和密碼學（AES 的 GF(2^8) 運算）。'},
  ],
};

TOPIC_CONTENT['幾何學'] = {
  en: 'Geometry',
  sections: [
    {title:'Euclid 幾何', content:'Euclid 的《幾何原本》以 5 條公設為基礎推導出全部幾何命題。第五公設（平行公設）的獨立性問題導致了非歐幾何的誕生。平面幾何研究三角形、圓形、多邊形的性質。勾股定理是幾何學中最基本的定理。相似形和全等形是幾何證明的重要工具。'},
    {title:'解析幾何', content:'笛卡兒提出座標系，用代數方法研究幾何問題。點與有序數對一一對應，曲線由方程式表示。向量空間提供了統一的幾何語言。內積定義長度和角度。變換矩陣實現旋轉、平移、縮放等幾何操作。解析幾何是計算幾何、電腦圖學的基礎。'},
    {title:'計算幾何', content:'計算幾何研究幾何問題的演算法。凸包（Convex Hull）是包裹點集的最小凸多邊形。Voronoi 圖將平面劃分為離最近站點的多邊形區域。Delaunay 三角剖分最大化最小角。碰撞檢測在機器人和遊戲物理中至關重要。幾何雜湊用於模式識別和 3D 匹配。'},
  ],
};

TOPIC_CONTENT['微分幾何'] = {
  en: 'Differential Geometry',
  sections: [
    {title:'曲線論', content:'參數化曲線 γ(t) 的切向量、法向量、副法向量構成 Frenet-Serret 框架。曲率 κ 度量曲線偏離直線的程度，撓率 τ 度量曲線偏離平面的程度。Frenet-Serret 公式描述了框架沿曲線的變化：dT/ds = κN，dN/ds = -κT + τB，dB/ds = -τN。'},
    {title:'曲面論', content:'曲面的第一基本形式（度量）定義了曲面上的長度和角度。第二基本形式描述了曲面在空間中的彎曲方式。Gauss 曲率 K 是內蘊幾何量（Gauss 絕妙定理），平均曲率 H 依賴於嵌入方式。測地線是曲面上距離最短的路徑。Theorema Egregium 說 Gauss 曲率由第一基本形式完全確定。'},
    {title:'黎曼幾何', content:'黎曼流形是配備了度量張量 g 的光滑流形。Christoffel 符號描述了協變導數。Riemann 曲率張量 R^ρ_{σμν} 全面描述了流形的彎曲。Ricci 曲率和純量曲率是曲率張量的縮併。測地線方程 ∇_γ′ γ′ = 0 由變分法導出。Einstein 場方程 G_μν = 8πT_μν 將時空曲率與物質能量聯繫起來。'},
  ],
};

TOPIC_CONTENT['拓撲學'] = {
  en: 'Topology',
  sections: [
    {title:'點集拓樸', content:'拓樸空間由集合 X 和一組開子集構成，滿足空集和 X 為開集、有限交保開、任意並保開等公理。連續映射保持開集的原像為開集。緊緻性（任意開覆蓋有有限子覆蓋）和連通性是拓樸學的核心性質。Hausdorff 分離性保證極限的唯一性。度量空間是拓樸空間最重要的例子。'},
    {title:'代數拓樸', content:'基本群 π₁(X) 是空間中環路的同倫類構成的群，反映了空間的「洞」結構。同調群 H_n(X) 用鏈複形定義，計算空間的 n 維洞。上同調群賦予代數結構。Mayer-Vietoris 序列將空間分解為簡單部分的同調聯繫起來。Brouwer 不動點定理和 Borsuk-Ulam 定理是代數拓樸的重要應用。'},
    {title:'幾何應用', content:'拓樸不變量如同倫群、同調群、Euler 示性數用於區分不同空間。球面、環面、Klein 瓶、實投影平面等基本曲面可以透過連通和分類。紐結理論研究三維空間中的封閉曲線嵌入。拓樸資料分析（TDA）使用持續同調分析高維資料的形狀。Morse 理論透過臨界點研究流形的拓樸。'},
  ],
};

TOPIC_CONTENT['微積分'] = {
  en: 'Calculus',
  sections: [
    {title:'微分學', content:'導數 f\'(x) = lim_{h→0} (f(x+h)-f(x))/h 描述函數的瞬時變化率。微分法則：和差積商法則、連鎖律。高階導數刻畫函數的更高階行為。導數的應用包含切線斜率、極值判定（一階/二階導數檢驗）、L\'Hôpital 法則求極限。Taylor 展開將函數表示為多項式級數。'},
    {title:'積分學', content:'定積分 ∫_a^b f(x)dx 是黎曼和的極限，代表曲線下的面積。微積分基本定理建立了微分和積分的橋樑：∫_a^b f\'(x)dx = f(b)-f(a)。換元積分法和分部積分法是計算積分的核心技巧。瑕積分處理無界區間或無界函數。積分應用包含面積、體積、弧長、旋轉體表面積。'},
    {title:'多變數微積分', content:'偏導數 ∂f/∂x 度量一個變數變化時的變化率。梯度向量 ∇f 指向函數增加最快的方向。多重積分將積分推廣到多維區域。Green 定理、Stokes 定理、Gauss 散度定理是向量微積分的核心定理，統一了微分和積分在高維的關係。Lagrange 乘數法求解約束最佳化問題。'},
  ],
};

TOPIC_CONTENT['複變函數'] = {
  en: 'Complex Analysis',
  sections: [
    {title:'解析函數', content:'複變函數 f(z) 在點 z₀ 可微意味著極限 lim_{h→0} (f(z₀+h)-f(z₀))/h 存在且與 h 的趨近路徑無關。Cauchy-Riemann 方程 ∂u/∂x = ∂v/∂y，∂u/∂y = -∂v/∂x 是可微的必要充分條件。解析函數具有驚人的性質：無窮可微、滿足 Cauchy 積分公式、可以展開為冪級數。'},
    {title:'Cauchy 積分理論', content:'Cauchy 積分定理：解析函數在簡單封閉曲線上的積分為零。Cauchy 積分公式將函數在區域內的值表示為邊界積分。留數定理將封閉曲線積分化為內部奇點留數的和，是計算實積分的有力工具。Liouville 定理：有界整函數必為常數。代數基本定理可由 Liouville 定理證明。'},
    {title:'應用', content:'保角映射（Conformal Mapping）保持角度，用於求解拉普拉斯方程的邊界值問題（流體力學、靜電學）。傅立葉和拉普拉斯變換在複平面上分析系統穩定性。ζ 函數的正則化用於量子場論。複變函數在訊號處理、控制理論、量子力學中有廣泛應用。'},
  ],
};

TOPIC_CONTENT['泛函分析'] = {
  en: 'Functional Analysis',
  sections: [
    {title:'賦範空間', content:'賦範向量空間配備了度量向量長度的範數 ||x||。Banach 空間是完備的賦範空間（Cauchy 序列必收斂）。L^p 空間、C[0,1] 空間是 Banach 空間的典型例子。Hölder 不等式和 Minkowski 不等式是 L^p 空間的基本工具。有限維空間的所有範數等價。'},
    {title:'Hilbert 空間', content:'Hilbert 空間是完備的內積空間，內積 ⟨x,y⟩ 誘導範數。L^2 空間和 ℓ^2 空間是 Hilbert 空間。正交投影、正交補空間、Gram-Schmidt 正交化是基本工具。Riesz 表示定理：Hilbert 空間上的連續線性泛函可由內積唯一表示。Fourier 級數是 Hilbert 空間中正交展開的典型例子。'},
    {title:'線性算子理論', content:'有界線性算子 T: X→Y 保持線性結構且有界。譜理論推廣了特徵值概念到無限維空間。緊算子的譜結構類似有限維。自伴算子的譜為實數，在量子力學中對應可觀測量。Banach-Steinhaus 定理（一致有界原理）、開映射定理、閉圖像定理是泛函分析的三大基本定理。'},
  ],
};

TOPIC_CONTENT['測度論'] = {
  en: 'Measure Theory',
  sections: [
    {title:'σ-代數與測度', content:'測度是將集合映射到非負實數（或 ∞）的函數，是長度、面積、體積的抽象推廣。σ-代數是對可數並和補集封閉的集合族。測度的基本性質包含：空集測度為零、可列可加性、單調性、次可加性。Borel 測度定義在拓樸空間的 Borel σ-代數上。Lebesgue 測度是歐氏空間中符合直覺的測度。'},
    {title:'Lebesgue 積分', content:'Lebesgue 積分將函數按值域劃分，比 Riemann 積分能處理更多病態函數。可測函數是原像為可測集的函數。簡單函數逼近法用於構造 Lebesgue 積分。控制收斂定理（Dominated Convergence Theorem）和單調收斂定理給出了積分與極限交換的條件。Fubini 定理將重積分化為累次積分。'},
    {title:'機率論基礎', content:'機率測度是總測度為 1 的測度。隨機變數是可測函數。期望值是 Lebesgue 積分。幾乎必然收斂、依機率收斂、分布收斂等收斂模式在機率論中至關重要。Radon-Nikodym 導數對應機率密度函數。條件期望是 L² 空間上的正交投影。Kolmogorov 一致性定理保證了隨機過程的存在性。'},
  ],
};

TOPIC_CONTENT['變分學'] = {
  en: 'Calculus of Variations',
  sections: [
    {title:'泛函極值', content:'變分學處理泛函 J[y] = ∫ L(x, y, y\')dx 的極值問題，其中 y 是未知函數。一階變分 δJ = 0 給出必要條件。Euler-Lagrange 方程 ∂L/∂y - d/dx(∂L/∂y\') = 0 是變分學的核心方程，其解為候選極值函數。自然邊界條件處理端點自由的問題。多函數和多變數的推廣得到偏微分方程。'},
    {title:'經典問題', content:'最速降線問題（Brachistochrone）：質點在重力作用下沿何種曲線下降最快？答案為擺線。最小旋轉曲面：在兩點間旋轉生成面積最小的曲線，解為懸鏈線。等周問題：固定周長圍出面積最大的圖形為圓。測地線：曲面上距離最短的路徑滿足測地線方程。'},
    {title:'與物理學的關係', content:'最小作用量原理（Hamilton 原理）：物理系統的真實演化使作用量泛函取極值。Lagrangian 力學由 Euler-Lagrange 方程推導牛頓力學。Hamiltonian 力學透過 Legendre 變換給出另一種表述。Noether 定理：每個連續對稱對應一個守恆量。變分法在量子力學（路徑積分）、廣義相對論（Einstein-Hilbert 作用量）、控制理論中起核心作用。'},
  ],
};

TOPIC_CONTENT['機率統計'] = {
  en: 'Probability and Statistics',
  sections: [
    {title:'機率論基礎', content:'機率空間 (Ω, F, P) 由樣本空間、事件 σ-代數、機率測度構成。條件機率 P(A|B) = P(A∩B)/P(B)。Bayes 定理 P(A|B) = P(B|A)P(A)/P(B) 是統計推論的基礎。隨機變數的分布由累積分布函數（CDF）描述。期望值 E[X]、變異數 Var(X)、共變異數 Cov(X,Y) 是基本特徵量。大數法則和中央極限定理是機率論的兩大基石。'},
    {title:'統計推論', content:'參數估計：點估計（MLE、矩估計）和區間估計（信賴區間）。假設檢定：虛無假設 H₀ 與對立假設 H₁，p 值、型 I/型 II 錯誤。t 檢定比較均值，卡方檢定比較類別資料，ANOVA 比較多組均值。迴歸分析建立變數間的關係模型。貝氏統計引入先驗分布更新為後驗分布。'},
    {title:'應用', content:'機器學習中的機率模型（Naive Bayes、GMM、HMM）。A/B 測試在產品開發中比較方案效果。統計品質控制在製造業中監控產品品質。流行病學中計算疾病風險和治療效果。金融風險管理和時間序列預測。社會科學中的調查分析和因果推論。深度學習中的 Batch Normalization 和 Dropout 有統計學基礎。'},
  ],
};

TOPIC_CONTENT['隨機微積分'] = {
  en: 'Stochastic Calculus',
  sections: [
    {title:'布朗運動', content:'布朗運動（Wiener 過程）W(t) 是連續時間隨機過程：W(0)=0，增量獨立且 W(t)-W(s) ∼ N(0, t-s)。布朗運動的軌跡幾乎必然連續但處處不可微。二次變分 [W,W](t) = t 是隨機微積分的關鍵差異。隨機微分方程 dX = μdt + σdW 廣泛應用於金融和物理建模。'},
    {title:'Ito 積分', content:'Ito 積分 ∫_0^T f(t,ω) dW_t 是針對布朗運動路徑的隨機積分。與 Riemann-Stieltjes 積分不同，Ito 積分在求和點取左端點，導致 Ito 公式（隨機版本的連鎖律）：df(W_t) = f\'(W_t)dW_t + ½f\'\'(W_t)dt。Ito 公式中的額外項 ½f\'\'dt 來自布朗運動的非零二次變分。'},
    {title:'Black-Scholes 模型', content:'Black-Scholes 模型假設股價遵循幾何布朗運動 dS = μSdt + σSdW。Black-Scholes 偏微分方程 ∂V/∂t + ½σ²S²∂²V/∂S² + rS∂V/∂S - rV = 0。透過 Ito 公式和無套利定價推導。歐式選擇權有封閉解：C(S,t) = SN(d₁) - Ke^{-r(T-t)}N(d₂)。隨機微積分也是利率模型（Vasicek、CIR）和信用風險模型的基礎。'},
  ],
};

TOPIC_CONTENT['數值分析'] = {
  en: 'Numerical Analysis',
  sections: [
    {title:'浮點數與誤差', content:'IEEE 754 浮點數標準以二進位科學記號表示實數。單精度（32-bit）和雙精度（64-bit）是最常見的格式。進位誤差、截斷誤差、捨入誤差的區別和傳播。數值穩定性：前向誤差和後向誤差分析。條件數衡量問題對輸入誤差的敏感度。災難性對消和避免方法。Kahan 求和演算法減少累積誤差。'},
    {title:'數值線性代數', content:'LU 分解求解線性方程組 Ax=b。QR 分解用於最小平方問題和特徵值計算。奇異值分解（SVD）用於矩陣近似和降維。共軛梯度法（Conjugate Gradient）是大規模稀疏系統的迭代解法。Jacobi 和 Gauss-Seidel 迭代是古典迭代方法。功率迭代法求最大特徵值。Rayleigh 商迭代加速收斂。'},
    {title:'數值微積分', content:'數值微分使用有限差分逼近導數，但受步驟大小和捨入誤差的權衡限制。數值積分（求積分法）：Newton-Cotes 公式（梯形法、Simpson 法）、Gauss 求積法最大化代數精度。Runge-Kutta 方法求解常微分方程初值問題。有限差分法和有限元素法求解偏微分方程。快速傅立葉變換（FFT）是數值計算的基石。'},
  ],
};

TOPIC_CONTENT['密碼學'] = {
  en: 'Cryptography',
  sections: [
    {title:'對稱加密', content:'對稱加密使用相同密鑰進行加密和解密。AES（Advanced Encryption Standard）是現今最廣泛使用的對稱加密演算法，使用 SPN（置換-置換網路）結構。DES（已被破解）和 3DES 是前代標準。串流密碼（如 RC4、ChaCha20）逐位元加密。分組密碼的工作模式（ECB、CBC、CTR、GCM）影響安全性與功能。'},
    {title:'非對稱加密', content:'公鑰密碼學使用一對公私鑰。RSA 基於大整數分解的困難性，用於加密和簽章。橢圓曲線密碼學（ECC）以更短的密鑰長度提供相同安全強度。Diffie-Hellman 密鑰交換協定允許雙方在公開通道上協商共享密鑰。量子電腦對 RSA 和 ECC 構成威脅（Shor 演算法），推動了後量子密碼學的發展。'},
    {title:'雜湊函數與應用', content:'密碼學雜湊函數將任意長度訊息映射為固定長度輸出，具有抗碰撞性、抗原像性、抗第二原像性。SHA-256 和 SHA-3 是當前標準。HMAC 用於訊息認證。數位簽章（DSA、ECDSA）提供不可否認性。區塊鏈使用 SHA-256 實現工作量證明。零知識證明允許一方證明知道某個秘密而不洩露秘密本身。'},
  ],
};

TOPIC_CONTENT['物理數學'] = {
  en: 'Mathematical Physics',
  sections: [
    {title:'經典力學的數學', content:'牛頓力學使用向量微積分描述質點運動。Lagrangian 力學 L = T - V 透過變分法導出 Euler-Lagrange 方程。Hamiltonian 力學使用正則坐標 (p,q) 和 Hamilton 方程 dq/dt = ∂H/∂p，dp/dt = -∂H/∂q。Poisson 括號 {f,g} 是 Hamiltonian 力學的代數結構。連續介質力學使用張量分析描述變形和應力。'},
    {title:'電磁學的數學', content:'Maxwell 方程組以向量微積分統一了電學和磁學。∇·E = ρ/ε₀、∇·B = 0、∇×E = -∂B/∂t、∇×B = μ₀J + μ₀ε₀∂E/∂t。勢函數 φ 和向量勢 A 簡化了 Maxwell 方程。電磁波方程從 Maxwell 方程推導。狹義相對論以 Lorentz 變換統一電磁學和力學。規範場論將電磁交互作用解釋為 U(1) 規範對稱性。'},
    {title:'量子力學的數學', content:'量子態由 Hilbert 空間中的向量 |ψ⟩ 描述。可觀測量對應自伴算子。薛丁格方程 iℏ∂|ψ⟩/∂t = Ĥ|ψ⟩ 控制態的演化。位置和動量滿足對易關係 [x̂,p̂] = iℏ。角動量算子、自旋算子滿足 SU(2) 代數。微擾理論近似求解無法精確解的系統。路徑積分表述（Feynman）從另一個角度理解量子力學。'},
  ],
};

TOPIC_CONTENT['狹義相對論'] = {
  en: 'Special Relativity',
  sections: [
    {title:'基本假設', content:'狹義相對論基於兩個基本假設：1）相對性原理 — 物理定律在所有慣性參考系中形式相同。2）光速不變原理 — 真空中的光速在任何慣性系中恆為 c（約 3×10⁸ m/s），與光源運動無關。這些假設導致了時間膨脹、長度收縮、同時性的相對性等反直覺結論。'},
    {title:'Lorentz 變換', content:'Lorentz 變換聯繫了兩個慣性系的時空坐標。沿 x 方向的變換：t\' = γ(t - vx/c²)，x\' = γ(x - vt)，其中 γ = 1/√(1-v²/c²)。Lorentz 變換保持時空間隔 ds² = -c²dt² + dx² + dy² + dz² 不變。四維向量（四維位移、四維動量）在 Lorentz 變換下協變。Minkowski 時空用 (+, -, -, -) 度規描述相對論的幾何。'},
    {title:'質能等價', content:'著名公式 E = mc² 是狹義相對論的直接推論。靜止質量 m₀ 的物體具有靜止能量 E₀ = m₀c²。相對論性能量 E = γm₀c²，動量 p = γm₀v。四維動量 (E/c, p) 的模長平方等於 (m₀c)²。質能等價解釋了核反應中質量虧損轉化為巨大的能量。相對論動力學修正了牛頓力學在高速情況下的偏差。'},
  ],
};

TOPIC_CONTENT['廣義相對論'] = {
  en: 'General Relativity',
  sections: [
    {title:'彎曲時空', content:'重力不是傳統意義上的力，而是時空本身的彎曲。物質和能量告訴時空如何彎曲（Einstein 場方程 G_μν = 8πG/c⁴ T_μν），時空的彎曲告訴物質如何運動（測地線方程）。愛因斯坦的電梯思想實驗和高斯/黎曼的曲面幾何是廣義相對論的觀念和數學先驅。等效原理（慣性質量和重力質量等價）是理論的出發點。'},
    {title:'Einstein 場方程', content:'Einstein 場方程將時空曲率（Einstein 張量 G_μν）與物質能量分布（能量動量張量 T_μν）聯繫起來。Ricci 張量 R_μν 和 Ricci 純量 R 由於 Riemann 曲率張量縮併得到。宇宙常數 Λ 項最初被 Einstein 加入以獲得靜態宇宙解，但現在用來解釋宇宙加速膨脹。Schwarzschild 解是最簡單的黑洞解。FLRW 度規描述均勻各向同性的宇宙。'},
    {title:'預言與驗證', content:'廣義相對論的預言包括：水星近日點進動（每世紀 43 角秒）、光線在重力場中的彎曲（1919 年日食觀測確認）、重力紅移（Pound-Rebka 實驗）、重力波（2015 年 LIGO 首次直接觀測）、黑洞的存在（2019 年 Event Horizon Telescope 拍攝 M87 黑洞影像）。GPS 衛星需要同時考慮狹義和廣義相對論效應進行時鐘校正。'},
  ],
};

TOPIC_CONTENT['量子力學'] = {
  en: 'Quantum Mechanics',
  sections: [
    {title:'基本概念', content:'量子態向量 |ψ⟩ 描述系統的狀態。疊加原理：系統可以同時處於多個狀態的線性組合。測量導致態向量坍縮到某個本徵態。不確定性原理 Δx·Δp ≥ ℏ/2 說明位置和動量不能同時精確測量。波粒二象性：微觀實體同時表現出粒子和波動特性。量子糾纏：兩個粒子的量子態不可分離。'},
    {title:'薛丁格方程', content:'時間相關薛丁格方程 iℏ∂ψ/∂t = Ĥψ 描述量子態隨時間的演化。時間無關薛丁格方程 Ĥψ = Eψ 給出定態能量本徵值。無限深勢井、調和振子、氫原子是可精確求解的經典問題。波函數 ψ(x) 的模平方 |ψ(x)|² 解釋為粒子的機率密度。邊界條件導致能量量子化。'},
    {title:'量子力學的數學結構', content:'Hilbert 空間（通常是 L²）是量子態的狀態空間。自伴算子對應可觀測物理量。么正算子描述量子態的演化（包括量子閘）。對易關係 [Â,B̂] = iℏ 反映可觀測量的相容性。自旋由 Pauli 矩陣描述，滿足 SU(2) 代數。密度算子描述混合態。路徑積分表述等價於薛丁格方程。量子測量理論（POVM）將測量推廣到非正交投影。'},
  ],
};

TOPIC_CONTENT['量子電腦'] = {
  en: 'Quantum Computing',
  sections: [
    {title:'量子位元', content:'量子位元（Qubit）是量子電腦的基本資訊單元。與傳統位元只能是 0 或 1 不同，量子位元可以是 |0⟩ 和 |1⟩ 的疊加態 α|0⟩ + β|1⟩，其中 |α|² + |β|² = 1。Bloch 球面直觀表示單量子位元的狀態。多量子位元系統使用張量積構成，維度隨位元數指數增長。量子糾纏是多量子位元系統的核心資源。'},
    {title:'量子閘與電路', content:'量子閘是作用於量子位元的么正變換。單量子位元閘包含 Pauli 閘（X、Y、Z）、Hadamard 閘 H、相位閘 S、T 閘。CNOT 閘是雙量子位元控制反閘，可產生糾纏態。Toffoli 閘（CCNOT）是通用量子閘。量子電路由量子閘序列組成。量子測量將量子態坍縮為經典位元。量子傅立葉變換（QFT）是許多量子演算法的關鍵組件。'},
    {title:'量子演算法', content:'Shor 演算法（1994）在量子電腦上多項式時間分解大整數，威脅 RSA 密碼學。Grover 搜尋演算法（1996）平方根級加速未排序資料庫搜尋。量子優化（QAOA、VQE）在近期量子裝置上有實際應用前景。量子錯誤更正碼（Surface Code、Steane Code）克服量子退相干。近期目標：含噪的中等規模量子（NISQ）裝置的實用演算法。'},
  ],
};

function findCodeForTopic(topicName) {
  var map = {
    '數學的歷史':[],
    '集合論':['_ai/lean4py/lean4py/sets.py'],
    '邏輯學':['_ai/lean4py/lean4py/logic.py','_ai/lean4py/lean4py/prover.py'],
    '數論':['_sympy/01-basic/07-cryptography/crypt2_rsa.py'],
    '代數學':['02-代數/_ccc/group.py','02-代數/_ccc/field_axioms.py','_ai/lean4py/lean4py/algebra.py'],
    '幾何學':['03-幾何/_ccc/2d/geometry2d_vector.py','03-幾何/_ccc/nd/geometry_nd.py','03-幾何/_ccc/geo_proof/geo_proof1.py'],
    '微分幾何':['11-微分幾何/_bookcode/03/3.1-Frenet-Serret.py','11-微分幾何/_dgeom/06-riemann/dgeom/riemann.py','_sympy/02-dgeom/02-diffgeom/diffgeom.ipynb'],
    '拓撲學':['09d-拓樸學/mobius_tape.py','09d-拓樸學/klein_bottle.py'],
    '微積分':['04-微積分/_ccc/02-梯度優化/01-微分/diff.py','04-微積分/_ccc/02-梯度優化/03-梯度下降法/gd.py','09-向量微積分/theorem/green_theorem.py'],
    '複變函數':['08b-複變函數/01-draw/draw_z2.py','08b-複變函數/01-draw/draw_z2_3d.py'],
    '泛函分析':['09e-泛函分析/01-量子算子理論/01-commutator.py','09e-泛函分析/01-量子算子理論/03-ket.py'],
    '測度論':[],
    '變分學':[],
    '機率統計':['05-機率統計/statr/statr/distributions.py','05-機率統計/statr/statr/tests.py','05-機率統計/_bak/02-clt/clt1.py'],
    '隨機微積分':['05b-隨機微積分/stoc4py/ito.py','05b-隨機微積分/stoc4py/process.py','05b-隨機微積分/examples/ex3_black_scholes.py'],
    '數值分析':['07-線性代數/_ccc/det_lu.py','07-線性代數/_ccc/eig_with_qr1.py','07-線性代數/_ccc/svd_with_eig1.py'],
    '密碼學':['06-資訊理論/_ccc/02-huffmanCode/huffmanCode1.py','_sympy/01-basic/07-cryptography/crypt2_rsa.py'],
    '物理數學':['12-物理數學/01-經典物理/01-mechanics.py','12-物理數學/03-量子力學/03-quantum.py','_sympy/03-physics/03-quantum.py'],
    '狹義相對論':['12-物理數學/02-相對論/schwarzschild.py'],
    '廣義相對論':['11-微分幾何/_bookcode/12/12.1-BlackHoleRayTracer.py','12-物理數學/02-相對論/flrw.py','_sympy/02-dgeom/03-relativity/schwarzschild.ipynb'],
    '量子力學':['12-物理數學/03-量子力學/03-quantum.py','09e-泛函分析/01-量子算子理論/01-commutator.py','_sympy/03-physics/03-quantum.py'],
    '量子電腦':[],
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
