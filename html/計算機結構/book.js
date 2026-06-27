var WIKI = 'https://github.com/ccckmit/course0/blob/main/wiki/%E8%A8%88%E7%AE%97%E6%A9%9F%E7%B5%90%E6%A7%8B';
var CODE = 'https://github.com/ccckmit/course0/blob/main/code/%E8%A8%88%E7%AE%97%E6%A9%9F%E7%B5%90%E6%A7%8B';

var TOPICS = {
  '計算機基礎': [
    {name:'數位邏輯', file:'數位邏輯.md', desc:'基本邏輯閘、布林代數、組合邏輯與序向邏輯', tags:['Logic Gates','Boolean Algebra']},
    {name:'記憶體', file:'記憶體.md', desc:'記憶體金字塔、DRAM、SRAM、非揮發性記憶體', tags:['DRAM','SRAM','Memory Hierarchy']},
    {name:'ALU', file:'ALU.md', desc:'算術邏輯單元 — 加法器、乘法器、移位器', tags:['Adder','Multiplier','Shifter']},
  ],
  '歷史與發展': [
    {name:'計算機結構的歷史', file:'計算機結構的歷史.md', desc:'從 ENIAC 到現代處理器的四十年發展史', tags:['History','von Neumann','Microprocessor']},
  ],
  'CPU架構': [
    {name:'CPU架構', file:'CPU架構.md', desc:'控制單元、ALU、暫存器檔案、指令週期', tags:['Control Unit','Data Path']},
    {name:'指令集架構', file:'指令集架構.md', desc:'RISC vs CISC、指令格式、定址模式', tags:['ISA','RISC','CISC']},
    {name:'暫存器', file:'暫存器.md', desc:'CPU 內部最快記憶體、RISC-V/x86 暫存器模型', tags:['Register','Calling Convention']},
    {name:'組合語言', file:'組合語言.md', desc:'x86-64、RISC-V、ARM64 與 Hack 組合語言', tags:['Assembly','x86','RISC-V']},
    {name:'異常與中斷', file:'異常與中斷.md', desc:'Exception/Interrupt/Trap/Fault 處理流程', tags:['Interrupt','Exception','CSR']},
  ],
  '管線與效能': [
    {name:'管線處理', file:'管線處理.md', desc:'五級管線、結構/資料/控制危險', tags:['Pipeline','Hazard']},
    {name:'分支預測', file:'分支預測.md', desc:'靜態/動態預測、飽和計數器、gshare', tags:['Branch Prediction','Speculative']},
    {name:'超純量與亂序執行', file:'超純量與亂序執行.md', desc:'Tomasulo、重排緩衝區、指令級並行', tags:['Superscalar','OoO','Tomasulo']},
  ],
  '記憶體階層': [
    {name:'記憶體層次', file:'記憶體層次.md', desc:'快取組織、TLB、NUMA、替換策略', tags:['Cache','TLB','NUMA']},
    {name:'快取', file:'快取.md', desc:'直接映射、關聯式快取、寫入策略', tags:['Cache Mapping','Write Policy']},
    {name:'虛擬記憶體', file:'虛擬記憶體.md', desc:'分頁機制、MMU、TLB、分頁錯誤', tags:['Virtual Memory','Paging','MMU']},
    {name:'快取一致性', file:'快取一致性.md', desc:'MESI/MOESI 協定、監聽式與目錄式協定', tags:['Cache Coherence','MESI']},
  ],
  '平行計算': [
    {name:'平行運算', file:'平行運算.md', desc:'Flynn 分類、Amdahl 定律、OpenMP/MPI', tags:['Parallel','SIMD','Amdahl']},
    {name:'GPU架構', file:'GPU架構.md', desc:'SM、CUDA 核心、SIMT、NVIDIA 架構演進', tags:['GPU','SIMT','CUDA']},
    {name:'CUDA', file:'CUDA.md', desc:'Grid/Block/Thread、記憶體模型、Warp 排程', tags:['CUDA','Warp','Shared Memory']},
  ],
  '硬體設計語言': [
    {name:'Verilog', file:'Verilog.md', desc:'模組、assign、always、測試平台', tags:['Verilog','HDL','Synthesis']},
    {name:'HDL', file:'HDL.md', desc:'Verilog/VHDL/SystemVerilog 比較', tags:['VHDL','SystemVerilog']},
  ],
  'EDA 與工具': [
    {name:'EDA', file:'EDA.md', desc:'電子設計自動化流程、邏輯合成、實體設計', tags:['EDA','Synthesis','Place & Route']},
    {name:'開放原始碼EDA', file:'開放原始碼EDA.md', desc:'Yosys、OpenROAD、GTKWave、OpenPDK', tags:['Open Source','Yosys','OpenROAD']},
    {name:'Icarus Verilog', file:'IcarusVerilog.md', desc:'開源 Verilog 模擬器使用', tags:['Icarus','Simulation']},
    {name:'Verilator', file:'Verilator.md', desc:'編譯式 SystemVerilog 模擬', tags:['Verilator','C++']},
  ],
  '架構比較': [
    {name:'x86 與 ARM 架構', file:'x86與ARM架構.md', desc:'CISC vs RISC、暫存器對比、SIMD', tags:['x86','ARM','CISC','RISC']},
    {name:'Nand2Tetris', file:'Nand2Tetris.md', desc:'從 NAND 到俄羅斯方塊的完整計算機建構', tags:['Nand2Tetris','Hack','Jack']},
    {name:'QEMU', file:'QEMU.md', desc:'開源機器模擬器與虛擬化工具', tags:['QEMU','Emulation','Virtualization']},
  ],
  '人物': [
    {name:'John Hennessy', file:'John_Hennessy.md', desc:'RISC 革命先驅、2017 圖靈獎、MIPS 架構', tags:['Hennessy','RISC','Turing Award']},
    {name:'David Patterson', file:'David_Patterson.md', desc:'RISC/RISC-V 開創者、RAID 發明人、2017 圖靈獎', tags:['Patterson','RISC-V','RAID']},
  ],
};

var CODES = [
  {name:'布林邏輯閘', path:'nand2tetris/01', files:['Not.hdl','And.hdl','Or.hdl','Xor.hdl','Mux.hdl'], desc:'Nand2Tetris 第 1 章 — 15 種基本邏輯閘實作'},
  {name:'加法器與 ALU', path:'nand2tetris/02', files:['HalfAdder.hdl','FullAdder.hdl','Add16.hdl','ALU.hdl'], desc:'Nand2Tetris 第 2 章 — 半加器、全加器、ALU'},
  {name:'記憶體元件', path:'nand2tetris/03', files:['Bit.hdl','Register.hdl','RAM8.hdl','PC.hdl'], desc:'Nand2Tetris 第 3 章 — Bit、暫存器、RAM、程式計數器'},
  {name:'組合語言程式', path:'nand2tetris/04', files:['Mult.asm','Fill.asm','drawA.asm'], desc:'Nand2Tetris 第 4 章 — Hack 組合語言範例'},
  {name:'Hack 處理器', path:'nand2tetris/05', files:['CPU.hdl','Memory.hdl','Computer.hdl'], desc:'Nand2Tetris 第 5 章 — CPU、記憶體、完整電腦'},
  {name:'組譯器', path:'nand2tetris/06', files:['asm.cpp','dasm.c','vm.c'], desc:'Nand2Tetris 第 6 章 — 組譯器、反組譯器、虛擬機器'},
  {name:'VM 翻譯器 I', path:'nand2tetris/07', files:['vm2asm.c'], desc:'Nand2Tetris 第 7 章 — 堆疊算術與記憶體存取翻譯'},
  {name:'VM 翻譯器 II', path:'nand2tetris/08', files:['vm2asm.c'], desc:'Nand2Tetris 第 8 章 — 程式流程與函數呼叫翻譯'},
  {name:'Jack 編譯器', path:'nand2tetris/11', files:['jack2vm.c','Compiler.py'], desc:'Nand2Tetris 第 11 章 — Jack 高階語言編譯器'},
  {name:'Jack 作業系統', path:'nand2tetris/12', files:['Math.jack','Memory.jack','Screen.jack','String.jack'], desc:'Nand2Tetris 第 12 章 — Jack OS 標準函式庫'},
  {name:'Hack CPU (Verilog)', path:'verilog/hackcpu', files:['computer.v','alu.v','memory.v'], desc:'以 Verilog 實作 Hack CPU，用 Icarus 模擬'},
  {name:'MCU0 CPU', path:'verilog/mcu0', files:['mcu0m.v'], desc:'MCU0 最小 16 位元 CPU Verilog 實作'},
  {name:'EDA 自動化流程', path:'eda/eda0', files:['eda0.py'], desc:'簡易 EDA 流程 — 解析、綜合、佈局、繞線'},
  {name:'SPICE 電路模擬', path:'eda/spice0', files:['spice0.c','spice0.py'], desc:'迷你 SPICE 電路模擬器 (C/Python 雙版本)'},
  {name:'Verilog 轉 C 編譯器', path:'eda/verilog0c', files:['verilog0c.c'], desc:'將 Verilog 子集編譯為 C 程式碼'},
];

var TOPIC_CONTENT = {};

TOPIC_CONTENT['數位邏輯'] = {
  en: 'Digital Logic',
  sections: [
    {title:'基本邏輯閘', content:'數位電路的基礎是邏輯閘：AND（與閘）、OR（或閘）、NOT（反閘）、NAND（反及閘）、NOR（反或閘）、XOR（互斥或閘）。每個閘實現一個布林函數，輸入輸出為 0 或 1。NAND 和 NOR 被稱為通用閘，因為任何布林函數都可以僅用 NAND（或僅用 NOR）來實現。'},
    {title:'布林代數', content:'布林代數是數位邏輯的數學基礎。基本定律包括交換律 (A·B = B·A)、結合律 (A·(B·C) = (A·B)·C)、分配律 (A·(B+C) = A·B + A·C)、吸收律 (A + A·B = A) 以及迪摩根定律 (¬(A·B) = ¬A + ¬B, ¬(A+B) = ¬A · ¬B)。'},
    {title:'組合邏輯 vs 序向邏輯', content:'組合邏輯的輸出僅取決於當前輸入，沒有記憶功能，如加法器、多工器。序向邏輯的輸出取決於當前輸入和歷史狀態，需要正反器（Flip-Flop）來儲存狀態，如計數器、暫存器、有限狀態機。'},
    {title:'多工器與解多工器', content:'多工器（MUX）根據選擇訊號從多個輸入中選出一個輸出。解多工器（DMUX）將一個輸入導向到多個輸出中的一個。它們是構成更複雜電路的基本元件，廣泛應用於資料選擇和路由。'},
  ],
};

TOPIC_CONTENT['記憶體'] = {
  en: 'Memory',
  sections: [
    {title:'記憶體金字塔', content:'計算機記憶體按照速度與容量形成金字塔結構：暫存器（最快，最小）→ L1/L2/L3 快取 → 主記憶體（DRAM）→ SSD → HDD → 磁帶（最慢，最大）。越靠近 CPU 的層級速度越快、成本越高、容量越小。'},
    {title:'DRAM 與 SRAM', content:'DRAM（動態隨機存取記憶體）每個位元由一個電容和一個電晶體組成，需定期刷新，密度高、成本低，用作主記憶體。SRAM（靜態隨機存取記憶體）每個位元需 6 個電晶體，無需刷新，速度更快但密度低，用作快取記憶體。'},
    {title:'局部性原理', content:'程式執行時表現出兩種局部性：時間局部性（最近存取的資料很可能在短時間內再次存取，如迴圈變數）和空間局部性（存取某個位置後，附近位置很可能被存取，如陣列遍歷）。局部性原理是快取能夠有效工作的關鍵。'},
  ],
};

TOPIC_CONTENT['ALU'] = {
  en: 'Arithmetic Logic Unit',
  sections: [
    {title:'ALU 基本結構', content:'ALU 是 CPU 中負責算術和邏輯運算的核心單元。基本結構包含算術電路（加法、減法）、邏輯電路（AND、OR、XOR、NOT）、移位電路（左移、右移）以及旗標輸出（零旗標、負旗標、進位旗標、溢位旗標）。'},
    {title:'加法器設計', content:'半加器計算兩個位元相加的和 (S = A XOR B) 與進位 (C = A AND B)。全加器增加進位輸入，可串接形成 N 位元漣波進位加法器（延遲 O(N)）。進位前瞻加法器使用額外邏輯提前計算進位，將延遲降至 O(log N)。'},
    {title:'乘法器與移位器', content:'陣列乘法器直接使用加法器陣列，結構規則但面積大。Wallace 樹乘法器使用壓縮器減少部分積，速度更快。Booth 編碼可同時處理多位元減少部分積數量。桶形移位器可以在單一週期內完成任意位數的移位操作。'},
  ],
};

TOPIC_CONTENT['計算機結構的歷史'] = {
  en: 'History of Computer Architecture',
  sections: [
    {title:'機電計算時代', content:'1940 年代之前，計算依賴機械或機電裝置。打孔卡片系統用於資料處理。Mark I（1944）是第一部機電電腦，ENIAC（1946）使用 18,000 個真空管，重達 30 噸，每秒可執行 5,000 次加法。'},
    {title:'儲存程式概念', content:'馮紐曼（von Neumann）在 1945 年提出儲存程式概念：程式和資料共用同一記憶體空間。這一設計成為現代電腦的基礎，稱為馮紐曼架構。其核心包括記憶體、控制單元、算術邏輯單元、輸入和輸出五大部件。'},
    {title:'電晶體與積體電路', content:'1947 年電晶體發明（Shockley / Bardeen / Brattain），1958 年 Kilby 發明積體電路。1960-70 年代，IBM System/360 引入了相容系列概念。Intel 4004（1971）是第一部微處理器，僅有 2,300 個電晶體，執行頻率 740 kHz。'},
    {title:'微處理器革命', content:'從 Intel 8080、8086 到 x86 系列的發展，摩爾定律驅動電晶體數量指數增長。RISC 架構（1980s）由 Patterson 和 Hennessy 提出，簡化指令集以提升效能。現代處理器已包含數百億個電晶體，採用多核心、深管線、亂序執行等先進技術。'},
  ],
};

TOPIC_CONTENT['CPU架構'] = {
  en: 'CPU Architecture',
  sections: [
    {title:'CPU 內部結構', content:'CPU 由控制單元（CU）、算術邏輯單元（ALU）、暫存器檔案和快取組成。控制單元負責指令擷取、解碼和產生控制訊號。ALU 執行算術和邏輯運算。暫存器檔案提供最快的資料存取。快取緩解 CPU 與主記憶體之間的速度差距。'},
    {title:'指令週期', content:'CPU 執行每條指令經歷五個階段：擷取（Fetch）— 從記憶體讀取指令；解碼（Decode）— 解釋指令含義；執行（Execute）— ALU 進行運算；記憶體存取（Memory）— 讀寫資料；寫回（Writeback）— 將結果寫入暫存器。'},
    {title:'控制單元設計', content:'控制單元產生所有必要的控制訊號來協調 CPU 各部件。硬佈線控制單元使用組合邏輯，速度快但設計複雜。微程式控制單元使用微指令 ROM，設計靈活但速度較慢。現代 RISC CPU 傾向使用硬佈線控制以獲得最佳效能。'},
  ],
};

TOPIC_CONTENT['指令集架構'] = {
  en: 'Instruction Set Architecture',
  sections: [
    {title:'RISC vs CISC', content:'RISC（精簡指令集電腦）使用固定長度的簡單指令，每條指令通常在一週期內完成。CISC（複雜指令集電腦）使用變長指令，單條指令可完成複雜操作。代表性 RISC 架構：ARM、RISC-V、MIPS。代表性 CISC 架構：x86。現代 x86 處理器內部將 CISC 指令解碼為 RISC 微操作。'},
    {title:'RISC-V 指令格式', content:'RISC-V 使用六種基本指令格式，全部為 32 位元固定長度：R-type（暫存器-暫存器運算）、I-type（立即數運算/載入）、S-type（儲存）、B-type（條件分支）、U-type（高立即數）、J-type（跳轉）。統一的格式簡化了解碼邏輯。'},
    {title:'定址模式', content:'CPU 提供多種定址模式來存取運算元：暫存器直接定址（操作數在暫存器中）、立即數定址（操作數在指令中）、基底加偏移定址（暫存器值 + 偏移量，支援陣列和結構）、PC 相對定址（常用於分支指令）。'},
  ],
};

TOPIC_CONTENT['暫存器'] = {
  en: 'Registers',
  sections: [
    {title:'暫存器角色', content:'暫存器是 CPU 內部最快的記憶體，存取延遲幾乎為零。它們位於記憶體金字塔的最頂端，用於儲存正在處理的資料、指令位址和處理器狀態。暫存器的數量和類型直接影響 CPU 的效能和程式設計模型。'},
    {title:'RISC-V 暫存器', content:'RISC-V 架構有 32 個通用暫存器 x0–x31。x0 恆為 0。按照呼叫慣例：a0–a7（函數引數/回傳值）、t0–t6（暫存暫存器）、s0–s11（被呼叫者保存）、sp（堆疊指標）、ra（回傳位址）、gp（全域指標）、tp（執行緒指標）。'},
    {title:'x86 暫存器', content:'x86-64 架構有 16 個通用暫存器：RAX（累加器）、RBX（基底）、RCX（計數）、RDX（資料）、RSI/RDI（源/目的索引）、RBP（基底指標）、RSP（堆疊指標）、R8–R15（額外通用）。每個暫存器可存取 64/32/16/8 位元子集。'},
    {title:'函數呼叫慣例', content:'呼叫者保存暫存器（如 RISC-V 的 t0–t6, a0–a7）：呼叫者需在呼叫前保存，被呼叫者可自由使用。被呼叫者保存暫存器（如 s0–s11）：被呼叫者若使用需先保存並在返回前恢復。堆疊指標（sp）和回傳位址（ra）有特殊處理規則。'},
  ],
};

TOPIC_CONTENT['組合語言'] = {
  en: 'Assembly Language',
  sections: [
    {title:'發展歷程', content:'組合語言是機器碼的人類可讀表示，每條指令直接對應一條機器碼。從最早的機器碼程式設計，到引入助記符和標籤，再到標準化的呼叫慣例，組合語言經歷了長期的發展。即使在今天，組合語言在作業系統核心、嵌入式系統和效能關鍵程式碼中仍不可或缺。'},
    {title:'x86-64 指令集', content:'x86-64 提供豐富的指令：資料移動（mov、push、pop）、算術（add、sub、imul、idiv）、邏輯（and、or、xor、not）、分支（cmp、je、jne、jmp）、以及 SIMD 指令（SSE/AVX）。指令長度 1–15 bytes 不等，採用變長編碼。'},
    {title:'RISC-V 指令集', content:'RISC-V 採用精簡設計，指令格式統一為 32 位元。六大格式涵蓋所有指令類型。典型指令如 add x1, x2, x3（x1 = x2 + x3）、lw x1, 0(x2)（從記憶體載入字）、sw x1, 0(x2)（儲存字到記憶體）。'},
    {title:'Hack 組合語言', content:'Nand2Tetris 的 Hack 平台使用兩種指令：A 指令 @value 設定 A 暫存器為 15 位元常數，C 指令 dest=comp;jump 同時計算和跳轉。C 指令的 comp 欄位可執行 ALU 運算，dest 指定儲存位置，jump 控制條件跳轉。'},
  ],
};

TOPIC_CONTENT['異常與中斷'] = {
  en: 'Exceptions and Interrupts',
  sections: [
    {title:'基本概念', content:'異常和中斷是 CPU 處理非同步和同步事件的機制。中斷（Interrupt）由外部裝置發起（如鍵盤、網卡、計時器），非同步發生。異常（Exception）由指令執行引起，同步發生。Trap 是刻意觸發的異常（如系統呼叫），Fault 可修復後重試，Abort 不可恢復。'},
    {title:'中斷處理流程', content:'典型中斷處理流程：1) 裝置發送中斷訊號到中斷控制器。2) 中斷控制器通知 CPU。3) CPU 保存當前執行上下文（PC、狀態暫存器等）。4) 查詢中斷向量表找到對應處理常式。5) 跳轉執行中斷服務常式（ISR）。6) 恢復上下文，返回中斷點繼續執行。'},
    {title:'RISC-V CSR', content:'RISC-V 使用控制狀態暫存器（CSR）管理異常和中斷。關鍵 CSR 包括：mstatus（機器模式狀態）、mie（中斷致能）、mip（中斷待決）、mtvec（中斷向量表基底）、mepc（異常 PC）、mcause（異常原因）、mtval（異常附加資訊）。支援 Direct（直接跳轉到單一處理常式）和 Vectored（向量式中斷）兩種模式。'},
  ],
};

TOPIC_CONTENT['管線處理'] = {
  en: 'Pipeline Processing',
  sections: [
    {title:'五級管線', content:'現代 CPU 將指令執行分為五個管線階段：IF（指令擷取）、ID（指令解碼/暫存器讀取）、EX（執行/ALU 運算）、MEM（記憶體存取）、WB（寫回暫存器）。理想情況下，每個時脈週期完成一條指令，CPI ≈ 1，比非管線設計提升約 5 倍吞吐量。'},
    {title:'結構危險', content:'結構危險發生在多條指令同時需要同一硬體資源時。例如 IF 和 MEM 都需要存取記憶體。解決方法包括：複製資源（分離指令快取和資料快取）、插入停頓（氣泡）等。'},
    {title:'資料危險', content:'資料危險（Data Hazard）分為三類：RAW（讀後寫）— 後續指令需等待前一指令寫入結果；WAR（寫後讀）— 亂序執行時可能發生；WAW（寫後寫）— 亂序執行時可能發生。主要解決方案：轉發（Forwarding/Bypassing）將 ALU 結果直接送給後續指令、插入停頓、編譯器指令重排。'},
    {title:'控制危險', content:'控制危險由分支指令造成，因為下一條指令的位址在分支結果確定前未知。解決方法包括：分支預測、延遲分支槽（Delay Slot）、條件執行。分支預測錯誤的代價隨管線深度增加而增大。'},
  ],
};

TOPIC_CONTENT['分支預測'] = {
  en: 'Branch Prediction',
  sections: [
    {title:'靜態分支預測', content:'靜態預測在編譯時決定分支方向，不依賴執行時資訊。常見策略：總是預測不跳轉、總是預測跳轉、根據分支方向預測（向後跳轉預測為迴圈，向前跳轉預測為不跳轉）。GCC 的 likely/unlikely 巨集（__builtin_expect）讓程式設計師提示分支方向。'},
    {title:'動態分支預測', content:'動態預測使用分支歷史來預測未來行為。最簡單的是 1 位元預測器：記錄上次結果，下次預測相同方向。2 位元飽和計數器有四個狀態（強預測跳轉/弱預測跳轉/弱預測不跳轉/強預測不跳轉），需要連續兩次預測錯誤才會改變強預測方向，對迴圈尾部的誤判更具抵抗力。'},
    {title:'進階預測器', content:'gshare 預測器使用分支位址和全域歷史暫存器的 XOR 來索引預測表，能捕捉不同分支間的相關性。混合預測器（Tournament）組合多種預測器，由選擇器選擇當前最準確的預測器。現代 CPU 的預測器準確率超過 99%，包含數萬個條目和複雜的預測演算法。'},
  ],
};

TOPIC_CONTENT['超純量與亂序執行'] = {
  en: 'Superscalar and Out-of-Order Execution',
  sections: [
    {title:'超純量架構', content:'超純量處理器每時脈週期可發射多條指令到多個執行單元。常見配置為 4 路或 6 路超純量，搭配多個 ALU、浮點單元、載入/儲存單元。指令級並行（ILP）是超純量效能的關鍵限制因素。'},
    {title:'Tomasulo 演算法', content:'Tomasulo 演算法由 Robert Tomasulo 提出，支援硬體暫存器重新命名和亂序執行。核心思想是使用預約站（Reservation Station）將指令與執行單元解耦，透過公共資料匯流排（CDB）廣播結果。暫存器重新命名消除了 WAR 和 WAW 危險。'},
    {title:'重排緩衝區 (ROB)', content:'ROB 確保亂序執行的指令能按原始程式碼順序提交。當指令執行完成後，它寫入 ROB 條目而非直接修改架構狀態。只有當 ROB 中最舊的指令完成時，其結果才提交到暫存器檔案。這保證在異常發生時，處理器能恢復到精確的例外狀態。'},
    {title:'執行單元延遲', content:'不同指令的執行延遲差異很大：整數 ALU 通常 1 週期、整數乘法 3–5 週期、浮點加法 3–4 週期、浮點乘法 5 週期、浮點除法 10–20 週期、載入指令 2–4 週期（快取命中）。排程器需管理這些差異，最大化利用率。'},
  ],
};

TOPIC_CONTENT['記憶體層次'] = {
  en: 'Memory Hierarchy',
  sections: [
    {title:'層次結構原則', content:'記憶體層次的核心原則是速度與成本的權衡。越快的記憶體越貴、容量越小；越慢的記憶體越便宜、容量越大。資料從慢速層複製到快速層，利用局部性原理減少慢速存取。設計目標是讓 CPU 經常能從最快層獲取資料。'},
    {title:'快取組織', content:'快取組織分為三種：直接映射（Direct-Mapped）— 每個記憶體區塊只能映射到一個快取行，簡單但衝突率高；N 路組相聯（N-way Set-Associative）— 每個集合包含 N 個行，折衷方案；完全相聯（Fully Associative）— 任何區塊可放在任何位置，靈活但硬體複雜。'},
    {title:'替換策略與寫入策略', content:'替換策略決定快取滿時替換哪一行：LRU（最近最少使用）、LFU（最不常使用）、隨機替換。寫入策略：Write-Through（寫穿）— 同時寫入快取和下一層記憶體，簡單但產生較多寫入流量；Write-Back（寫回）— 只寫入快取，髒行在替換時才寫回下層，效率更高但實現複雜。'},
    {title:'TLB 與 NUMA', content:'TLB（Translation Lookaside Buffer）是虛擬位址到實體位址轉換的快取，加速分頁查詢。NUMA（Non-Uniform Memory Access）在多處理器系統中，每個處理器對本地記憶體的存取延遲低於遠端記憶體，程式設計時需考慮記憶體放置位置。'},
  ],
};

TOPIC_CONTENT['快取'] = {
  en: 'Cache Memory',
  sections: [
    {title:'快取層次', content:'現代 CPU 整合多層快取：L1 快取分為指令快取（I-Cache）和資料快取（D-Cache），存取延遲 2–4 週期；L2 快取統一儲存指令和資料，延遲約 10 週期；L3 快取由多核心共享，延遲約 30–40 週期。快取越大，命中率越高，但延遲也越大。'},
    {title:'快取行結構', content:'快取行是快取與記憶體間資料傳輸的最小單位，通常為 64 bytes。每行包含：有效位元標記該行是否有效、標籤位元儲存記憶體位址的部分資訊、資料區塊儲存實際資料。部分架構還有髒位元（Write-Back 策略）和 LRU 狀態位元。'},
    {title:'命中率與缺失代價', content:'快取命中率 = 快取命中次數 / 總存取次數。L1 命中率通常 > 95%，L2 > 90%。快取缺失的代價是從下一層讀取資料的延遲。平均存取時間 = 命中時間 + 缺失率 × 缺失代價。最佳化目標是最小化平均存取時間。'},
  ],
};

TOPIC_CONTENT['虛擬記憶體'] = {
  en: 'Virtual Memory',
  sections: [
    {title:'分頁機制', content:'虛擬記憶體將程式的虛擬位址空間映射到實體記憶體。基本單位為頁（通常 4 KB）。虛擬位址分為頁號（VPN）和頁內偏移（Offset）。透過頁表將 VPN 轉換為實體頁框號（PPN），加上偏移得到實體位址。'},
    {title:'x86-64 四層分頁', content:'x86-64 使用四層分頁表：PML4（頁映射層級 4）→ PDPT（頁目錄指標表）→ PD（頁目錄）→ PT（頁表）→ 4 KB 頁。每層表有 512 個條目，覆蓋 48 位元虛擬位址空間（256 TB）。頁表條目包含 Present、R/W、U/S、XD（禁止執行）等權限控制位元。'},
    {title:'MMU 與 TLB', content:'記憶體管理單元（MMU）負責位址轉換。轉換流程：先查 TLB（VPN → PPN 的快取），若命中則快速得到實體位址；若缺失則進行完整的頁表走訪（page walk），並將結果填入 TLB。當頁表修改時，需刷新對應的 TLB 條目。分頁錯誤（Page Fault）發生在目標頁不在實體記憶體中時，由作業系統負責換頁。'},
  ],
};

TOPIC_CONTENT['快取一致性'] = {
  en: 'Cache Coherence',
  sections: [
    {title:'問題背景', content:'在多核心系統中，每個核心有私有 L1/L2 快取。當不同核心快取同一記憶體位置的不同副本時，若一個核心修改了資料，其他核心的副本就過時了。快取一致性協議確保所有核心看到一致的記憶體視圖。'},
    {title:'MESI 協定', content:'MESI 是最廣泛使用的快取一致性協定。每個快取行處於四種狀態之一：Modified（已修改，該行僅在此快取中且與主記憶體不同）、Exclusive（專有，該行僅在此快取中且與主記憶體一致）、Shared（共享，多個快取有副本且一致）、Invalid（無效，該行資料無效）。狀態轉換由本地 CPU 操作和總線監聽觸發。'},
    {title:'MOESI 與目錄式協定', content:'MOESI 在 MESI 基礎上增加 Owned 狀態：允許 Modified 的資料被共享，避免 Write-Back 後再讀取。監聽式協定（Snooping）透過總線廣播—適合小系統。目錄式協定維護一個集中目錄追蹤每行的共享者—適合大規模多處理器系統，減少總線流量。'},
    {title:'記憶體模型', content:'記憶體模型定義多核心對共享變數存取的可見性規則。順序一致性（SC）最直觀但限制最佳化。TSO（Total Store Order，如 x86）允許寫入延遲但保證程式順序。弱記憶體模型（如 ARM、RISC-V）允許更多重排，需程式設計師使用記憶體屏障（Fence）來確保正確性。'},
  ],
};

TOPIC_CONTENT['平行運算'] = {
  en: 'Parallel Computing',
  sections: [
    {title:'平行化層級', content:'運算可從多個層級進行平行化：位元層級平行（增加字長）、指令層級平行（管線/超純量）、執行緒層級平行（多核心）、處理程序層級平行（多處理器）、節點層級平行（叢集）。層級越高，平行化的粒度越粗。'},
    {title:'Flynn 分類法', content:'Flynn 分類根據指令流和資料流的數量將計算機分為四類：SISD（單指令單資料，傳統單核）、SIMD（單指令多資料，如 GPU、向量處理器）、MISD（多指令單資料，罕見，如容錯系統）、MIMD（多指令多資料，如多核 CPU、叢集）。'},
    {title:'Amdahl 與 Gustafson 定律', content:'Amdahl 定律指出加速比受序列部分限制：Speedup = 1 / ((1-P) + P/N)，其中 P 為可平行化比例，N 為處理器數。當 N→∞ 時，加速比趨近 1/(1-P)。Gustafson 定律則指出問題規模可隨處理器數增加而擴展，提供了更樂觀的視角。'},
    {title:'OpenMP 與 MPI', content:'OpenMP 是共享記憶體平行程式設計的標準，使用編譯器指示（#pragma omp parallel for）將迴圈自動平行化。MPI（Message Passing Interface）是分散式記憶體平行程式設計標準，程式透過訊息傳遞（send/receive）在節點間交換資料，適用於大規模叢集。'},
  ],
};

TOPIC_CONTENT['GPU架構'] = {
  en: 'GPU Architecture',
  sections: [
    {title:'CPU vs GPU', content:'CPU 設計注重低延遲和序列執行，擁有少量但強大的核心（4–16 核）、大快取、複雜控制單元。GPU 設計注重高吞吐量，擁有數千個小型核心、小快取、簡單控制單元。GPU 的核心優勢來自大量簡單核心平行工作，特別適合資料平行任務。'},
    {title:'SM 與 CUDA 核心', content:'GPU 的基本運算單元是串流多處理器（SM）。每個 SM 包含多個 CUDA 核心（整數/浮點運算單元）、共享記憶體、暫存器檔案、排程器、指令快取。從 Fermi 架構（2010）到 Hopper 架構（2022），SM 的設計不斷進化，增加了 Tensor Core（張量核心）加速 AI 運算、RT Core（光線追蹤核心）加速圖形渲染。'},
    {title:'SIMT 執行模型', content:'SIMT（Single Instruction, Multiple Threads）是 GPU 的獨特執行模型。多個執行緒以 32 為一組（稱為 Warp）執行同一條指令，但每個執行緒可處理不同的資料。當 Warp 內執行緒分支時（如 if-else），不同路徑需序列化執行，稱為分支分歧。最小化分支分歧是 GPU 程式最佳化的關鍵。'},
  ],
};

TOPIC_CONTENT['CUDA'] = {
  en: 'CUDA',
  sections: [
    {title:'執行層次', content:'CUDA 程式的執行層次從上到下：Grid（網格）→ Block（執行緒塊）→ Thread（執行緒）。核心函數（Kernel）以 <<<num_blocks, threads_per_block>>> 啟動。每個執行緒可透過內建變數獲取其全域 ID。Block 內的執行緒可透過共享記憶體協作，Block 間則需透過全域記憶體通訊。'},
    {title:'記憶體模型', content:'CUDA 提供多層記憶體：全域記憶體（Global Memory）— 所有執行緒可存取，容量大（數 GB）但延遲高（數百週期）；共享記憶體（Shared Memory）— Block 內共享，速度接近暫存器，容量小（通常 48 KB）；常數記憶體（Constant Memory）— 快取唯讀。正確使用共享記憶體是 GPU 程式效能的關鍵。'},
    {title:'Warp 排程', content:'Warp 由 32 個執行緒組成，是 GPU 的最小調度單位。SM 中的 Warp Scheduler 每個週期選擇一個可執行 Warp 發射到 CUDA 核心。Warp 執行緒執行同一指令，利用資料平行性隱藏記憶體延遲。當 Warp 因記憶體存取或同步操作停頓時，排程器立即切換到其他可執行 Warp，實現零成本上下文切換。'},
  ],
};

TOPIC_CONTENT['Verilog'] = {
  en: 'Verilog',
  sections: [
    {title:'模組結構', content:'Verilog 使用模組（module）作為基本設計單元。每個模組有輸入和輸出埠，內部可包含 wire（線網）、reg（暫存器）、參數、以及子模組實例化。模組透過埠連接形成層次化設計。關鍵字 module 和 endmodule 定義模組邊界。'},
    {title:'組合邏輯描述', content:'組合邏輯可透過 assign 連續指派語句描述，輸出隨輸入變化立即更新。例：assign Y = A & B。也可使用 always @(*) 或 always @(敏感列表) 區塊搭配 if-else 或 case 語句來描述更複雜的組合邏輯。'},
    {title:'時序邏輯描述', content:'時序邏輯使用 always @(posedge clk) 區塊，在時脈上升沿觸發。在此區塊中賦值的變數需宣告為 reg 類型。標準寫法採用非阻塞賦值（<=）避免競賽條件。同步重置信號通常寫在敏感列表中。'},
    {title:'測試平台', content:'測試平台（Testbench）用於驗證設計的正確性。使用 initial 區塊設定初始值和時序，$monitor 監控變數變化，$dumpvars 產生 VCD 波形檔案供 GTKWave 檢視。$display 類似 C 的 printf，用於輸出調試資訊。'},
  ],
};

TOPIC_CONTENT['HDL'] = {
  en: 'Hardware Description Languages',
  sections: [
    {title:'Verilog', content:'Verilog 是目前使用最廣泛的硬體描述語言之一，語法類似 C 語言。支援閘層級（and, or, nand）、資料流層級（assign）和行為層級（always）三種描述風格。Verilog-2005 是最後一次 IEEE 標準更新。'},
    {title:'VHDL', content:'VHDL（VHSIC Hardware Description Language）語法類似 Ada，強調嚴格的型別系統。基本結構由 entity（定義介面）和 architecture（定義功能）組成。使用 process 區塊描述時序行為，signal 和 variable 區分訊號與區域變數。在歐洲和航太/國防領域較受歡迎。'},
    {title:'SystemVerilog', content:'SystemVerilog 是 Verilog 的超集，大幅增強了驗證能力。新增 interface（簡化模組連接）、assertion（斷言驗證）、class（OOP 支援隨機驗證）、packed/unpacked struct 等特性。已成為 ASIC/FPGA 驗證的主流語言。'},
    {title:'可合成 vs 不可合成', content:'並非所有 HDL 結構都能轉為硬體電路。可合成的結構：assign、always @(posedge clk) 的有限模式、if-else、case、算術運算。不可合成的結構：initial、$display、$monitor、#delay、遞迴函數。這些主要用於測試平台和行為模擬。'},
  ],
};

TOPIC_CONTENT['EDA'] = {
  en: 'Electronic Design Automation',
  sections: [
    {title:'EDA 發展歷程', content:'1960 年代：圖形電路輸入系統。1970 年代：HDL 語言出現（Verilog 1984, VHDL 1987）。1980 年代：EDA 商業化，Synopsys、Cadence 等公司成立。1990 年代：深次微米技術帶來時序、功耗和訊號完整性挑戰。2000 年代後：先進節點（7nm/5nm）需要更複雜的物理模擬和多物理場分析。'},
    {title:'標準設計流程', content:'典型數位晶片設計流程：系統規格 → 架構設計 → RTL 編寫（Verilog/VHDL）→ 邏輯合成（RTL→閘級網表）→ 實體設計（佈局/時脈樹/繞線）→ 版圖驗證（DRC/LVS）→ GDSII 輸出 → 光罩製造。每個階段都有專用工具和檢查點。'},
    {title:'主要工具類別', content:'模擬器：ModelSim/VCS/Icarus（功能模擬）。合成器：Design Compiler/Genus/Yosys（RTL 到閘級）。實體設計：IC Compiler/Innovus（佈局繞線）。版圖驗證：Calibre/ICV（DRC/LVS/RCX）。功率分析：PrimeTime PX/RedHawk。'},
  ],
};

TOPIC_CONTENT['開放原始碼EDA'] = {
  en: 'Open Source EDA',
  sections: [
    {title:'開源 EDA 生態', content:'近年來開源 EDA 工具快速發展，形成了從 RTL 到 GDSII 的完整生態鏈。應用層 → 驗證層 → 模擬層 → 綜合層 → 實體設計 → 工藝資料。開源工具降低了晶片設計的進入門檻，使學術界和小型團隊也能參與 ASIC 設計。'},
    {title:'Yosys', content:'Yosys 是當前最受歡迎的開源邏輯合成工具，由 Claire Wolf 開發。支援 Verilog-2005 輸入，可合成為多種目標格式。Yosys 使用內部的中間表示（RTLIL）進行多種最佳化，支援正規化驗證。配合 ABC（Berkeley 開源邏輯合成工具）進行技術映射。'},
    {title:'OpenROAD', content:'OpenROAD 提供完整的開源 RTL-to-GDS 流程，包含：OpenSTA（靜態時序分析）、RePlAce（全域佈局）、TritonCTS（時脈樹合成）、TritonRoute（詳細繞線）、KLayout（版圖檢視）。搭配 OpenPDK 製程設計套件（Sky130、GF180）可實現完整的 ASIC 設計流程。'},
  ],
};

TOPIC_CONTENT['Icarus Verilog'] = {
  en: 'Icarus Verilog',
  sections: [
    {title:'簡介', content:'Icarus Verilog（iverilog）是輕量級的開源 Verilog 模擬器，支援 Verilog-2005 及部分 SystemVerilog。它將 Verilog 原始碼編譯為 vvp 中間格式，再由 vvp 執行模擬。簡單、快速、跨平台，非常適合教育和功能驗證。'},
    {title:'基本用法', content:'編譯：iverilog -o test.vvp test.v。模擬：vvp test.vvp。產生波形：在測試平台中使用 $dumpvars，然後用 gtkwave test.vcd 檢視波形。常用選項：-Wall（顯示警告）、-I./include（包含目錄）、-DSIMULATION（定義編譯巨集）。'},
    {title:'範例', content:'典型的 8 位元 ALU 測試：編寫 Verilog 模組定義 ALU 運算（add、sub、and、or、xor、sll、srl），編寫測試平台產生所有 OP 組合的測試向量，用 iverilog 編譯模擬，檢查波形輸出是否正確。'},
  ],
};

TOPIC_CONTENT['Verilator'] = {
  en: 'Verilator',
  sections: [
    {title:'編譯式模擬', content:'Verilator 採用編譯式方法，將 SystemVerilog 設計轉換為高效能的 C++ 模擬模型。速度比傳統事件驅動模擬器快 10–100 倍，特別適合需要大量回歸測試的場景。但 Verilator 假設設計是可綜合的，不支援非同步行為和部分 SystemVerilog 驗證結構。'},
    {title:'使用流程', content:'基本流程：verilator --cc --exe --build -j top.v sim_main.cpp。--cc 生成 C++ 程式碼，--exe 指定主程式，--build 自動編譯。輸出為可執行檔。--trace 啟用波形輸出（FST/VCD）。-O3 啟用最佳化。'},
    {title:'應用場景', content:'Verilator 廣泛應用於：開放原始碼處理器驗證（如 Rocket Chip、BOOM）、大型 SoC 模擬、回歸測試套件。由於其速度和容量優勢，許多商業公司也在前端設計中使用 Verilator 進行早期軟硬體協同驗證。'},
  ],
};

TOPIC_CONTENT['x86 與 ARM 架構'] = {
  en: 'x86 and ARM Architectures',
  sections: [
    {title:'x86 (CISC)', content:'x86 架構始於 Intel 8086（1978），經 80386（IA-32，1985）到 AMD64（x86-64，2003）。採用 CISC 設計哲學，提供豐富的指令和定址模式。指令長度 1–15 bytes 可變。主要玩家：Intel 和 AMD。x86 在桌上型電腦和伺服器市場佔據主導地位。'},
    {title:'ARM (RISC)', content:'ARM 架構起源於 Acorn RISC Machine（1985），最初設計為低功耗嵌入式處理器。隨著智慧型手機的普及，ARM 成為行動裝置霸主。蘋果 M1/M2/M3 晶片將 ARM 推向高效能計算領域。ARM64（AArch64）是現代 64 位元 ARM 架構，提供 31 個通用暫存器和統一的 NEON SIMD 單元。'},
    {title:'架構對比', content:'x86-64：16 個通用暫存器（RAX–R15）、CISC 指令集（1–15 bytes）、SSE/AVX SIMD。ARM64：31 個通用暫存器（X0–X30）、RISC 指令集（固定 4 bytes）、NEON SIMD。x86 擅長高效能計算，ARM 擅長能效比。現代趨勢是兩者互相借鑑：x86 引入更多 RISC 特性，ARM 擴展高效能能力。'},
  ],
};

TOPIC_CONTENT['Nand2Tetris'] = {
  en: 'Nand2Tetris',
  sections: [
    {title:'課程概述', content:'Nand2Tetris 由 Noam Nisan 和 Shimon Schocken 開發，是一門從底層建構完整計算機系統的經典課程。從最基本的 NAND 閘開始，逐步建構邏輯閘、ALU、CPU、記憶體，再到組合語言、虛擬機器、高階語言（Jack）、編譯器、作業系統，最終能在自建電腦上執行 Tetris 遊戲。共 12 章，分為硬體和軟體兩大部分。'},
    {title:'Hack 平台', content:'Hack 是課程中設計的 16 位元計算機平台。硬體規格：16K 指令 ROM、8K 資料 RAM、螢幕記憶體映射、鍵盤。CPU 有兩個暫存器（A 和 D）。指令集只有兩種格式：A 指令（@value，設定 A 暫存器）和 C 指令（dest=comp;jump，同時計算和跳轉）。儘管簡單，Hack 平台是圖靈完備的。'},
    {title:'Jack 語言', content:'Jack 是課程設計的簡單物件導向語言，語法類似 Java。支援類別、方法、陣列、字串、繼承（單一繼承）。編譯器將 Jack 原始碼翻譯為 VM 虛擬機器碼，再由 VM 翻譯器轉為 Hack 組合語言。Jack 作業系統提供 Math、Memory、Screen、Output、String、Array、Keyboard、Sys 等標準類別庫。'},
  ],
};

TOPIC_CONTENT['QEMU'] = {
  en: 'QEMU',
  sections: [
    {title:'簡介', content:'QEMU 是功能強大的開源機器模擬器和虛擬化工具。支援多種架構（x86、ARM、RISC-V、MIPS 等）的完整系統模擬。可用於開發、測試、除錯作業系統和嵌入式軟體。QEMU 還支援硬體加速虛擬化（KVM，僅 Linux），在接近原生速度下執行虛擬機。'},
    {title:'基本用法', content:'系統模擬：qemu-system-x86_64 -m 2G -cdrom ubuntu.iso。無頭模式（無視窗）：-nographic。RISC-V 模擬：qemu-system-riscv64 -machine virt -bios fw_jump.bin -kernel vmlinux。GDB 遠端除錯：-S -gdb tcp::1234。指令追蹤：-d exec -D qemu.log。'},
    {title:'磁碟映像管理', content:'qemu-img 指令用於管理虛擬磁碟映像。支援多種格式：qcow2（QEMU 寫入時複製，節省空間）、raw（原始格式，效能好）、vmdk（VMware 相容）。建立 qcow2 映像：qemu-img create -f qcow2 disk.qcow2 10G。格式轉換：qemu-img convert -f raw -O qcow2 input.raw output.qcow2。'},
  ],
};

TOPIC_CONTENT['John Hennessy'] = {
  en: 'John Hennessy',
  sections: [
    {title:'生平', content:'John Hennessy 是史丹佛大學名譽教授、Alphabet 公司董事長。2017 年與 David Patterson 共同獲得圖靈獎（電腦科學的最高榮譽）。曾任史丹佛大學校長（2000–2016），在任期內推動了線上教育平台 edX 的發展。'},
    {title:'主要貢獻', content:'Hennessy 是 RISC 架構的先驅之一。他領導開發了 MIPS 架構，並創立 MIPS Computer Systems。與 Patterson 合著的《Computer Architecture: A Quantitative Approach》被譽為計算機結構的聖經，以量化方法分析和比較架構設計。其研究證明簡化指令集可以實現更高的效能，推動了 RISC 從學術概念到商業成功的轉變。'},
  ],
};

TOPIC_CONTENT['David Patterson'] = {
  en: 'David Patterson',
  sections: [
    {title:'生平', content:'David Patterson 是加州大學柏克萊分校名譽教授，曾擔任 Google 傑出工程師。2017 年與 John Hennessy 共同獲得圖靈獎。他的研究影響了現代計算機設計的多個方面。'},
    {title:'主要貢獻', content:'Patterson 的貢獻包括：與 Hennessy 共同開創 RISC 革命，參與 MIPS 架構開發，在柏克萊領導發起開源 RISC-V ISA。他也是 RAID（Redundant Array of Inexpensive Disks）儲存系統的發明人之一。在 Google 參與了 TPU（Tensor Processing Unit）的設計，將 RISC 理念應用到 AI 加速器。合著《Computer Organization and Design RISC-V Edition》，推動 RISC-V 的教育和產業應用。'},
  ],
};

var CATEGORY_TAGS = {
  '數位邏輯': ['數位電路', '基礎'],
  '記憶體': ['記憶體', '硬體'],
  'ALU': ['算術', '硬體'],
  '計算機結構的歷史': ['歷史', '概論'],
  'CPU架構': ['CPU', '核心'],
  '指令集架構': ['ISA', '指令'],
  '暫存器': ['CPU', '記憶體'],
  '組合語言': ['組合語言', '程式設計'],
  '異常與中斷': ['CPU', '系統'],
  '管線處理': ['管線', '效能'],
  '分支預測': ['管線', '效能'],
  '超純量與亂序執行': ['管線', '效能', '進階'],
  '記憶體層次': ['記憶體', '快取'],
  '快取': ['記憶體', '快取'],
  '虛擬記憶體': ['記憶體', '作業系統'],
  '快取一致性': ['多核心', '快取'],
  '平行運算': ['平行', '多核心'],
  'GPU架構': ['GPU', '平行'],
  'CUDA': ['GPU', '平行', '程式設計'],
  'Verilog': ['硬體設計', 'Verilog'],
  'HDL': ['硬體設計', '語言'],
  'EDA': ['EDA', '工具'],
  '開放原始碼EDA': ['EDA', '開源'],
  'Icarus Verilog': ['工具', '模擬'],
  'Verilator': ['工具', '模擬'],
  'x86 與 ARM 架構': ['架構', '比較'],
  'Nand2Tetris': ['教育', '完整系統'],
  'QEMU': ['工具', '模擬'],
  'John Hennessy': ['人物', 'RISC'],
  'David Patterson': ['人物', 'RISC'],
};

function findCodeForTopic(topicName) {
  var map = {
    '數位邏輯':['nand2tetris/01/Not.hdl','nand2tetris/01/And.hdl','nand2tetris/01/Or.hdl','nand2tetris/01/Xor.hdl','nand2tetris/01/Mux.hdl'],
    '記憶體':['nand2tetris/03/a/Bit.hdl','nand2tetris/03/a/Register.hdl','nand2tetris/03/a/RAM8.hdl'],
    'ALU':['nand2tetris/02/HalfAdder.hdl','nand2tetris/02/FullAdder.hdl','nand2tetris/02/Add16.hdl','nand2tetris/02/ALU.hdl'],
    'CPU架構':['nand2tetris/05/CPU.hdl','nand2tetris/05/Memory.hdl','nand2tetris/05/Computer.hdl'],
    '指令集架構':['nand2tetris/06/asm.cpp','nand2tetris/05/CPU.hdl'],
    '暫存器':['nand2tetris/03/a/Register.hdl','nand2tetris/03/a/PC.hdl'],
    '組合語言':['nand2tetris/04/mult/Mult.asm','nand2tetris/04/fill/Fill.asm'],
    'Verilog':['verilog/hackcpu/alu.v','verilog/hackcpu/computer.v','verilog/mcu0/mcu0m.v'],
    'HDL':['verilog/hackcpu/gate.v','verilog/hackcpu/mux.v','verilog/hackcpu/alu.v'],
    'Nand2Tetris':['nand2tetris/01/Xor.hdl','nand2tetris/05/CPU.hdl','nand2tetris/06/asm.cpp','nand2tetris/11/jack2vm.c','nand2tetris/12/Math.jack'],
    'EDA':['eda/eda0/eda0.py','eda/spice0/spice0.c','eda/verilog0c/verilog0c.c'],
    '開放原始碼EDA':['eda/eda0/eda0.py','eda/spice0/spice0.py'],
    'Icarus Verilog':['verilog/hackcpu/computer.v','verilog/hackcpu/alu.v'],
    'Verilator':['eda/verilog0c/verilog0c.c'],
    'x86 與 ARM 架構':['nand2tetris/06/asm.cpp'],
    'GPU架構':[],
    'CUDA':[],
    '平行運算':[],
    '快取一致性':[],
    '快取':[],
    '記憶體層次':[],
    '虛擬記憶體':[],
    '管線處理':[],
    '分支預測':[],
    '超純量與亂序執行':[],
    '異常與中斷':[],
    '計算機結構的歷史':[],
    'John Hennessy':[],
    'David Patterson':[],
    'QEMU':[],
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
