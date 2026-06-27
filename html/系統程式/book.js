var WIKI = 'https://github.com/ccckmit/course0/blob/main/wiki/%E7%B3%BB%E7%B5%B1%E7%A8%8B%E5%BC%8F';
var CODE = 'https://github.com/ccckmit/course0/blob/main/code/%E7%B3%BB%E7%B5%B1%E7%A8%8B%E5%BC%8F';

var TOPICS = {
  '系統程式概論': [
    {name:'系統程式的歷史', file:'系統程式的歷史.md', desc:'從 ENIAC 到 UNIX — 系統程式的發展脈絡', tags:['History','UNIX','OS']},
  ],
  '程式語言': [
    {name:'程式語言', file:'程式語言.md', desc:'從機器語言到高階語言，編譯與直譯的演化', tags:['Programming Languages','Compiled','Interpreted']},
    {name:'C語言', file:'C語言.md', desc:'Dennis Ritchie 創造的系統程式語言', tags:['C','Pointer','K&R']},
    {name:'C++', file:'C++.md', desc:'物件導向、模板、現代 C++ 標準', tags:['C++','OOP','Templates']},
    {name:'Rust', file:'Rust.md', desc:'Mozilla 的記憶體安全系統語言', tags:['Rust','Ownership','Borrow Checker']},
    {name:'Go', file:'Go.md', desc:'Google 的簡潔並行語言 (Goroutine/Channel)', tags:['Go','Concurrency','Goroutine']},
    {name:'Zig', file:'zig語言.md', desc:'C 的現代替代品 — 編譯期計算、無隱藏控制流', tags:['Zig','Comptime','C Alternative']},
    {name:'Haskell', file:'Haskell.md', desc:'純函數式語言 — 惰性求值、強型別、Monad', tags:['Haskell','Functional','Lazy']},
    {name:'LISP', file:'LISP.md', desc:'最古老的高階語言 — S-expression、代碼即資料', tags:['LISP','Macro','REPL']},
    {name:'Prolog', file:'Prolog.md', desc:'邏輯程式語言 — 事實、規則、回溯推理', tags:['Prolog','Logic','Backtracking']},
    {name:'BASIC', file:'BASIC.md', desc:'初學者通用符號指令碼 — 個人電腦革命的推手', tags:['BASIC','Beginner','Interactive']},
  ],
  '編譯器與直譯器': [
    {name:'編譯器', file:'編譯器.md', desc:'詞法分析、語法分析、IR、最佳化、程式碼生成', tags:['Compiler','Lexer','Parser','Optimization']},
    {name:'直譯器', file:'直譯器.md', desc:'直譯器架構、AST 執行、REPL 實作', tags:['Interpreter','AST','REPL']},
    {name:'c4', file:'c4.md', desc:'四個函數的迷你 C 編譯器 — 自舉的奇蹟', tags:['c4','Mini Compiler','Self-hosting']},
    {name:'LLVM', file:'LLVM.md', desc:'編譯器基礎設施 — LLVM IR、Clang、最佳化管道', tags:['LLVM','IR','Clang','Optimization']},
    {name:'管線處理與編譯器', file:'管線處理與編譯器.md', desc:'編譯器如何為管線 CPU 最佳化指令排程', tags:['Pipeline','Compiler Optimization','Scheduling']},
  ],
  '作業系統核心': [
    {name:'作業系統', file:'作業系統.md', desc:'行程管理、記憶體管理、檔案系統、I/O 的綜合', tags:['OS','Kernel','Process','Memory']},
    {name:'Unix', file:'Unix.md', desc:'Unix 哲學 — 一切皆檔案、組合、簡潔', tags:['UNIX','Pipe','File System','Kernel']},
    {name:'Unix V6', file:'Unix_v6.md', desc:'1975 年經典 Unix — PDP-11、原始碼可讀', tags:['V6','PDP-11','UNIX']},
    {name:'xv6', file:'xv6.md', desc:'MIT RISC-V 教學作業系統 — Unix V6 的現代重現', tags:['xv6','Teaching OS','RISC-V']},
    {name:'RISC-V', file:'RISC-V.md', desc:'開源指令集架構 — RV32I/RV64I、特權層級', tags:['RISC-V','ISA','Open Source']},
    {name:'組合語言', file:'組合語言.md', desc:'RISC-V 組合語言 — 指令格式、呼叫慣例', tags:['Assembly','RISC-V','Calling Convention']},
  ],
  '記憶體管理': [
    {name:'記憶體管理', file:'記憶體管理.md', desc:'分配策略、分頁、分段、夥伴系統、slab', tags:['Memory','Paging','Allocation']},
    {name:'垃圾搜集', file:'垃圾搜集.md', desc:'Mark-Sweep、Copying、Generational、Reference Counting', tags:['GC','Memory','Automatic']},
    {name:'虛擬記憶體', file:'虛擬記憶體.md', desc:'位址轉換、多層頁表、TLB、頁面置換', tags:['Virtual Memory','Page Table','TLB']},
  ],
  '行程與排程': [
    {name:'行程與執行緒', file:'行程與執行緒.md', desc:'PCB、TCB、上下文切換、核心級 vs 使用者級執行緒', tags:['Process','Thread','Context Switch']},
    {name:'排程', file:'排程.md', desc:'FCFS、SJF、RR、CFS、即時排程演算法', tags:['Scheduling','FCFS','Round Robin','CFS']},
    {name:'同步與通訊', file:'同步與通訊.md', desc:'Mutex、Semaphore、Condition Variable、死結、IPC', tags:['Synchronization','Deadlock','IPC']},
  ],
  '檔案系統與網路': [
    {name:'檔案系統', file:'檔案系統.md', desc:'inode、superblock、日誌、Ext4、Btrfs', tags:['File System','inode','Journaling']},
    {name:'網路', file:'網路.md', desc:'OSI 模型、TCP/IP、區域網路、拓撲', tags:['Network','OSI','LAN','Topology']},
    {name:'TCP/IP', file:'TCP-IP協定.md', desc:'四層模型、TCP 三次握手、流量控制、壅塞控制', tags:['TCP','IP','Protocol']},
    {name:'HTTP', file:'HTTP.md', desc:'HTTP/1.1→2→3、REST、快取、HTTPS', tags:['HTTP','REST','SSL/TLS']},
    {name:'Socket', file:'Socket網路介面.md', desc:'Berkeley Socket API、TCP/UDP、epoll、非阻塞 I/O', tags:['Socket','TCP','UDP','epoll']},
  ],
  '系統工具': [
    {name:'Linux 系統呼叫', file:'Linux系統呼叫.md', desc:'使用者態/核心態切換、syscall 流程、常用系統呼叫', tags:['Syscall','Kernel','Trap']},
    {name:'POSIX', file:'POSIX.md', desc:'IEEE 1003.1 標準 — 可攜式作業系統介面', tags:['POSIX','Portability','Standard']},
    {name:'Shell 腳本', file:'Shell腳本.md', desc:'Bash 程式設計 — 變數、控制結構、管線、工作控制', tags:['Shell','Bash','Scripting']},
    {name:'連結器與載入器', file:'連結器與載入器.md', desc:'ELF 格式、符號解析、重定位、動態連結', tags:['Linker','Loader','ELF','Dynamic Linking']},
    {name:'訊號處理', file:'訊號處理.md', desc:'Signal、sigaction、非同步安全函數、訊號集', tags:['Signal','Async','Handler']},
  ],
  '安全與虛擬化': [
    {name:'密碼學與資訊安全', file:'密碼學與資訊安全.md', desc:'AES、RSA、TLS、對稱/非對稱加密、雜湊', tags:['Cryptography','AES','RSA','TLS']},
    {name:'區塊鏈與比特幣', file:'區塊鏈與比特幣.md', desc:'去中心化帳本、工作量證明、Merkle Tree、挖礦', tags:['Blockchain','Bitcoin','PoW','Merkle']},
    {name:'虛擬機', file:'虛擬機.md', desc:'系統虛擬化、程序虛擬化、容器、Docker', tags:['VM','Container','Docker','Hypervisor']},
    {name:'QEMU', file:'QEMU.md', desc:'開源模擬器 — 跨架構執行、KVM 加速', tags:['QEMU','Emulation','KVM']},
    {name:'Kubernetes', file:'Kubernetes.md', desc:'容器編排平台 — Pod、Service、Deployment、叢集管理', tags:['K8s','Container','Orchestration']},
  ],
  '平行與雲端': [
    {name:'並行計算與異構平台', file:'並行計算與異構平台.md', desc:'ILP/TLP/DLP、SIMD、GPU、FPGA 異構運算', tags:['Parallel','Heterogeneous','SIMD','GPU']},
    {name:'AI 的加速技術', file:'AI的加速技術.md', desc:'矩陣加速、AVX-512、Tensor Core、模型量化', tags:['AI','Acceleration','SIMD','Tensor Core']},
    {name:'雲端技術', file:'雲端技術.md', desc:'SaaS/PaaS/IaaS、虛擬化、容器、邊緣運算', tags:['Cloud','SaaS','PaaS','IaaS']},
  ],
  '開發工具': [
    {name:'Make / CMake', file:'Make_CMake.md', desc:'建置系統 — Makefile 規則、CMake 跨平台建置', tags:['Make','CMake','Build System']},
    {name:'GDB / LLDB', file:'GDB_LLDB.md', desc:'除錯工具 — 中斷點、記憶體檢視、堆疊追蹤', tags:['GDB','LLDB','Debugger']},
  ],
  '人物': [
    {name:'Dennis Ritchie', file:'Dennis_Ritchie.md', desc:'C 語言與 Unix 的共同發明人，圖靈獎得主', tags:['Dennis Ritchie','C','UNIX','Turing Award']},
    {name:'Ken Thompson', file:'Ken_Thompson.md', desc:'Unix 共同發明人、B 語言、正規表示式、圖靈獎', tags:['Ken Thompson','UNIX','B','Regex']},
    {name:'Linus Torvalds', file:'Linus_Torvalds.md', desc:'Linux 核心與 Git 的創造者，開源領袖', tags:['Linus Torvalds','Linux','Git']},
    {name:'Fabrice Bellard', file:'Fabrice_Bellard.md', desc:'QEMU、TinyCC、FFmpeg 的創造者，程式天才', tags:['Bellard','QEMU','TinyCC']},
  ],
};

var CODES = [
  {name:'c4 編譯器', path:'c/computer0/compiler/c4', files:['c4.c','hello.c'], desc:'四個函數的迷你 C 編譯器 — 自舉'},
  {name:'c0 編譯器', path:'c/computer0/compiler/c0/_version/c0c_long', files:['main.c','lexer.c','parser.c','ast.c','codegen.c'], desc:'C 語言編譯器 (lexer→parser→ast→codegen)'},
  {name:'BASIC 直譯器', path:'c/computer0/interpreter/basic', files:['basic.py','bas/fib.bas','bas/gcd.bas'], desc:'BASIC 語言直譯器與範例程式'},
  {name:'LISP 直譯器', path:'c/computer0/interpreter/lisp', files:['lisp.py','factorial.lisp'], desc:'LISP 直譯器 (Python 實作)'},
  {name:'Prolog 直譯器', path:'c/computer0/interpreter/prolog', files:['prolog.py','pl/fact.pl','pl/family.pl'], desc:'Prolog 邏輯程式語言直譯器'},
  {name:'py0i 直譯器', path:'c/computer0/interpreter/py0i', files:['py0i.py','py/fact.py'], desc:'Python 子集直譯器'},
  {name:'js0 編譯器', path:'c/computer0/compiler/js0', files:['js0i.js','js0i.rs'], desc:'JavaScript 子集編譯器 (JS 與 Rust 雙版本)'},
  {name:'mini-riscv-os2', path:'c/computer0/os/mini-riscv-os2/01-HelloOs', files:['os.c','start.s','Makefile'], desc:'迷你 RISC-V 作業系統 — 第一階段'},
  {name:'xv6', path:'c/computer0/os/xv6/kernel', files:['main.c','proc.c','fs.c','syscall.c'], desc:'MIT RISC-V 教學作業系統核心'},
  {name:'Web Server', path:'c/computer0/network/webserver0/http', files:['webserver.c'], desc:'簡易 HTTP Web 伺服器'},
  {name:'SSL/TLS', path:'c/computer0/crpyto/ssl0/src', files:['ssl.c','aes.c','rsa.c','sha.c'], desc:'SSL/TLS 協定實作 (AES/RSA/SHA)'},
  {name:'HTTPS Server', path:'c/computer0/crpyto/https0', files:['httpd_ssl0.c'], desc:'HTTPS 安全網頁伺服器'},
  {name:'Blockchain', path:'c/computer0/tool/blockchain', files:['blockchain0.c'], desc:'區塊鏈基礎實作'},
  {name:'Git', path:'c/computer0/tool/git0', files:['git0.c'], desc:'簡易版 Git 版本控制系統'},
  {name:'Telnet', path:'c/computer0/network/telnet0', files:['telnet_server.c','telnet_client.c'], desc:'Telnet 伺服器與客戶端'},
  // --- Rust 專案 ---
  {name:'mini-riscv-os (Rust)', path:'rust/riscv/mini-riscv-os/src', files:['main.rs','lib.rs','os_code.rs'], desc:'Rust 版的迷你 RISC-V 作業系統'},
  {name:'rvemu4 (Rust)', path:'rust/riscv/rvemu4/src', files:['main.rs','cpu.rs','memory.rs','elf.rs'], desc:'RISC-V 模擬器 (Rust)'},
  {name:'sql4 (Rust)', path:'rust/database/sql4/src', files:['main.rs','interface/mod.rs','parser/mod.rs','table/mod.rs'], desc:'SQL 資料庫引擎 (Rust)'},
  {name:'ssl4 (Rust)', path:'rust/crypto/ssl4/src', files:['lib.rs','server.rs','client.rs','cert.rs'], desc:'SSL/TLS 協定 (Rustls)'},
  {name:'lli4 (Rust)', path:'rust/compiler/lli4/src', files:['main.rs','parser.rs','ir.rs','interp.rs'], desc:'LLVM IR 直譯器 (Rust)'},
  {name:'btree (Rust)', path:'rust/database/btree/src', files:['main.rs','tree.rs','node.rs','storage.rs'], desc:'BTree 儲存引擎 (Rust)'},
  {name:'db6 (Rust)', path:'rust/database/db6/src', files:['main.rs','lib.rs','engine/','kv/','fts/'], desc:'統一資料庫引擎 (Rust)'},
  {name:'vi4 (Rust)', path:'rust/tool/vi4/src', files:['main.rs'], desc:'終端機文字編輯器 (Rust)'},
  // --- rust/riscv/ (更多) ---
  {name:'rv4 (Rust)', path:'rust/riscv/rv4/src', files:['main.rs','lib.rs','vm.rs','memory.rs','elf.rs'], desc:'RISC-V 模擬器 RV32I/RV64I'},
  {name:'rvboard4 (Rust)', path:'rust/riscv/rvboard4', files:['src/lib.rs','simulator/src/main.rs','examples/led.rs'], desc:'RISC-V 開發板模擬'},
  {name:'verilog RISC-V', path:'rust/riscv/verilog/single_cycle', files:['rv64i_cpu.v','tb_rv64i.v'], desc:'RISC-V 單週期 CPU Verilog 實作'},
  {name:'xv6-rust-octopus', path:'rust/riscv/xv6-rust-octopus/kernel/src', files:['main.rs','proc.rs','fs.rs','syscall.rs'], desc:'xv6 作業系統 Rust 移植版'},
  {name:'xv7-rust-octopus', path:'rust/riscv/xv7-rust-octopus/kernel/src', files:['main.rs','proc.rs','fs.rs','net/eth.rs'], desc:'xv7 作業系統 Rust 移植版（含網路）'},
  {name:'xv8-rust-posix', path:'rust/riscv/xv8-rust-posix/posix/tools/src', files:['lib.rs'], desc:'完整 POSIX 相容 Rust 作業系統'},
  // --- rust/compiler/ ---
  {name:'rustc4 (Rust)', path:'rust/compiler/rustc4/src', files:['main.rs','lexer.rs','parser.rs','ast.rs','codegen.rs'], desc:'Rust → LLVM IR 編譯器'},
  {name:'objdump (Rust)', path:'rust/compiler/objdump/src', files:['main.rs'], desc:'ELF 檔案分析工具'},
  // --- rust/database/ ---
  {name:'lsm (Rust)', path:'rust/database/lsm/src', files:['main.rs','lib.rs','engine.rs','lsm/'], desc:'LSM-Tree 儲存引擎'},
  {name:'fts (Rust)', path:'rust/database/fts/src', files:['main.rs','lib.rs'], desc:'全文搜尋引擎 (BM25)'},
  {name:'inodefs (Rust)', path:'rust/database/inodefs/src', files:['fs.rs','dir.rs','disk.rs','bitmaps.rs'], desc:'inode 虛擬檔案系統'},
  {name:'swisstable (Rust)', path:'rust/database/swisstable/src', files:['lib.rs'], desc:'瑞士表雜湊實作'},
  {name:'patricia-trie (Rust)', path:'rust/database/patricia-trie/src', files:['lib.rs','main.rs'], desc:'Patricia Trie 字典樹'},
  {name:'redblacktree (Rust)', path:'rust/database/redblacktree/src', files:['lib.rs','main.rs'], desc:'左傾紅黑樹實作'},
  // --- rust/crypto/ ---
  {name:'keygen (Rust)', path:'rust/crypto/keygen/src', files:['main.rs'], desc:'RSA/ECDSA 金鑰與憑證產生器'},
  // --- rust/tool/ ---
  {name:'lz4 (Rust)', path:'rust/tool/lz4/src', files:['main.rs'], desc:'LZ4 壓縮/解壓縮'},
  // --- rust/media/ ---
  {name:'jpeg (Rust)', path:'rust/media/jpeg/src', files:['main.rs','encoder.rs','decoder.rs'], desc:'JPEG 影像編解碼器'},
  {name:'mp3rs (Rust)', path:'rust/media/mp3rs/src', files:['main.rs','decoder.rs','encoder.rs','types.rs'], desc:'MP3 音訊編解碼器'},
  // --- rust/web/ ---
  {name:'js4 (Rust)', path:'rust/web/js4/src', files:['main.rs','lib.rs'], desc:'JavaScript 引擎 (tokenizer→AST→interpreter)'},
];

var TOPIC_CONTENT = {};

TOPIC_CONTENT['系統程式的歷史'] = {
  en: 'History of System Programming',
  sections: [
    {title:'黎明期 (1940-1950)', content:'ENIAC (1946) 以接線板程式設計，沒有系統軟體。EDVAC 引入儲存程式概念。早期程式設計師直接使用機器碼，後來發展出組合語言助記符。Fortran (1957) 是第一個高階語言編譯器。'},
    {title:'批次處理與作業系統萌芽 (1950-1960)', content:'Fortran Monitor System、IBM IBSYS 等最早的作業系統出現。程式以打孔卡片提交，批次執行。Atlas (1962) 引入了虛擬記憶體和中斷的概念。'},
    {title:'分時系統與 Unix 革命 (1960-1970)', content:'CTSS 和 Multics 引入了分時系統。1969 年，Ken Thompson 和 Dennis Ritchie 在貝爾實驗室創造了 Unix。Unix 以 C 語言重寫 (1973)，成為第一個可移植的作業系統，其簡潔設計影響深遠。'},
    {title:'個人電腦與開源時代 (1980-至今)', content:'IBM PC 與 MS-DOS (1981) 開創了個人電腦時代。GNU 專案 (1983) 和 Linux (1991) 推動了開源作業系統的發展。現代系統程式涵蓋編譯器、直譯器、虛擬機、容器等多個領域。'},
  ],
};

TOPIC_CONTENT['程式語言'] = {
  en: 'Programming Languages',
  sections: [
    {title:'機器語言與組合語言', content:'最早的程式以二進位機器碼編寫，直接對應 CPU 指令。組合語言以助記符代替數值操作碼，引入標籤、巨集等抽象，但仍與硬體緊密耦合。每種 CPU 架構有各自的組合語言。'},
    {title:'高階語言的誕生', content:'Fortran (1957) 是第一個高階語言，專注數值計算。LISP (1958) 引入函數式程式設計和垃圾搜集。COBOL (1959) 服務商業資料處理。C (1972) 成為系統程式的主流語言。'},
    {title:'編譯 vs 直譯', content:'編譯語言在執行前將原始碼轉換為機器碼（如 C、Rust），執行效率高。直譯語言在執行時逐行分析執行（如 Python、JavaScript），開發週期快。現代語言常混合兩者：JIT 編譯、位元組碼虛擬機。'},
    {title:'現代語言趨勢', content:'Rust 強調記憶體安全無需 GC。Go 簡化並行程式設計。Zig 作為 C 的現代替代。TypeScript 為 JavaScript 加入型別系統。各語言在安全性、效能、開發體驗之間做出不同取捨。'},
  ],
};

TOPIC_CONTENT['C語言'] = {
  en: 'C Language',
  sections: [
    {title:'歷史與哲學', content:'C 語言由 Dennis Ritchie 於 1972 年在貝爾實驗室創造，用於重寫 Unix 核心。C 的設計哲學是「信任程式設計師」，提供低階記憶體存取能力，語言核心極簡，功能透過標準函式庫擴展。K&R C (1978) 和 ANSI C (1989) 是兩個關鍵標準。'},
    {title:'指標與記憶體', content:'C 最強大的功能是指標 — 儲存記憶體位址的變數。指標允許直接記憶體操作、動態分配 (malloc/free)、陣列/字串操作。但指標也是 C 最容易出錯的地方：懸空指標、緩衝區溢位、記憶體洩漏。'},
    {title:'C 與系統程式', content:'C 是系統程式的主要語言：作業系統核心 (Linux、Windows)、嵌入式系統、編譯器、資料庫 (SQLite) 都以 C 或 C++ 撰寫。其效率接近組合語言，抽象程度又足夠支援大型專案。'},
  ],
};

TOPIC_CONTENT['C++'] = {
  en: 'C++',
  sections: [
    {title:'從 C 到物件導向', content:'C++ 由 Bjarne Stroustrup 於 1979 年開始開發，最初稱為「C with Classes」。在 C 的基礎上加入類別、繼承、多型、封裝等物件導向特性，保持對 C 的向前相容和零成本抽象原則。'},
    {title:'模板與泛型', content:'C++ 模板實作了編譯期泛型程式設計。STL (Standard Template Library) 提供容器 (vector、map、set)、演算法 (sort、find)、迭代器。模板元程式設計可在編譯期執行計算。Modern C++ (C++11/14/17/20) 引入了 auto、lambda、smart pointer、concept 等特性。'},
  ],
};

TOPIC_CONTENT['Rust'] = {
  en: 'Rust',
  sections: [
    {title:'所有權系統', content:'Rust 的核心創新是所有權 (Ownership) 系統：每個值有唯一所有者，所有權可轉移 (move) 或借用 (borrow)。借用檢查器 (Borrow Checker) 在編譯期確保無懸空參考和資料競爭。這使得 Rust 在無垃圾搜集的情況下保證記憶體安全。'},
    {title:'所有權規則', content:'三個核心規則：1) Rust 中每個值都有一個所有者。2) 同時間只能有一個可變借用或多個不可變借用。3) 引用必須始終有效。違反規則在編譯期報錯，而非執行時崩潰。'},
    {title:'零成本抽象', content:'Rust 提供高階語言的表達力（迭代器、closure、trait、模式匹配）但抽象在編譯期消除，最終產生的機器碼與手寫 C 相當。這使 Rust 成為系統程式的新興選擇：Firefox、Linux 核心、Windows 元件都開始採用 Rust。'},
  ],
};

TOPIC_CONTENT['Go'] = {
  en: 'Go',
  sections: [
    {title:'設計理念', content:'Go 由 Robert Griesemer、Rob Pike、Ken Thompson 在 Google 設計 (2009)。目標是解決大型分散式系統的開發效率問題。特性：簡潔語法、快速編譯、內建並行支援、垃圾搜集。刻意省略了繼承、泛型 (直到 Go 1.18)、異常處理等複雜特性。'},
    {title:'Goroutine 與 Channel', content:'Goroutine 是 Go 的輕量級執行緒，由 Go runtime 排程，啟動成本極低（數 KB 堆疊）。Channel 是 Goroutine 之間的通訊機制，遵循「透過通訊共享記憶體，而非共享記憶體來通訊」的哲學。select 語句支援多 channel 非同步處理。'},
  ],
};

TOPIC_CONTENT['Zig'] = {
  en: 'Zig',
  sections: [
    {title:'設計哲學', content:'Zig 由 Andrew Kelley 於 2015 年創造，定位為 C 的現代替代品。核心理念：顯式優於隱式、無隱藏控制流、編譯期計算 (comptime)。沒有預處理器、沒有運算子重載、沒有異常，保持與 C 的 ABI 相容。'},
    {title:'Comptime', content:'Zig 的 comptime 機制允許在編譯期執行任意程式碼。這是 Zig 替代 C 巨集和 C++ 模板的方式。編譯期計算可用於生成程式碼、執行型別檢查、計算常數。comptime 使得 Zig 在無巨集的情況下仍保持強大表達力。'},
  ],
};

TOPIC_CONTENT['Haskell'] = {
  en: 'Haskell',
  sections: [
    {title:'純函數式', content:'Haskell 是純函數式語言 (1987)，函數無副作用，相同輸入總是產生相同輸出。所有變數不可變（immutable）。這使得 Haskell 程式易於推理和測試，但也需要不同的思維方式來處理 I/O 和狀態。'},
    {title:'惰性求值', content:'Haskell 預設採用惰性求值（Lazy Evaluation）：表達式在需要其值時才計算。這允許處理無限資料結構（如全部自然數），並能避免不必要的計算。但惰性求值也使得效能分析和空間使用變得複雜。'},
    {title:'Monad', content:'Monad 是 Haskell 處理副作用的方式。IO Monad 封裝輸入輸出，Maybe Monad 處理可能失敗的計算，List Monad 處理非確定性計算。雖然 Monad 的數學概念對初學者具挑戰性，但它提供了優雅且型別安全的副作用管理。'},
  ],
};

TOPIC_CONTENT['LISP'] = {
  en: 'LISP',
  sections: [
    {title:'S-Expression', content:'LISP 使用 S-Expression (符號表達式) 表示程式和資料，統一形式為 (operator operand1 operand2 ...)。例如 (+ 1 2) 計算 1+2。這種一致性使得 LISP 程式可以像資料一樣操作 — 代碼即資料 (Code as Data) 的特性。'},
    {title:'REPL 與互動開發', content:'LISP 是最早提供 REPL (Read-Eval-Print Loop) 的語言之一。開發者可以即時輸入表達式並獲得結果，動態修改執行中的程式。這種互動式開發風格在當時極具革命性，至今仍影響著許多現代語言。'},
  ],
};

TOPIC_CONTENT['Prolog'] = {
  en: 'Prolog',
  sections: [
    {title:'宣告式程式設計', content:'Prolog 以宣告式而非命令式的方式描述問題。程式由事實 (fact) 和規則 (rule) 組成，查詢 (query) 透過邏輯推論得到答案。程式設計師描述「什麼為真」，而非「如何得到答案」。'},
    {title:'回溯與統一', content:'Prolog 的執行引擎透過回溯 (backtracking) 和合一 (unification) 來解答查詢。當一個查詢嘗試失敗時，引擎自動回溯到上一個選擇點嘗試另一條路徑。模式匹配的合一機制使得 Prolog 程式碼極其簡潔。'},
  ],
};

TOPIC_CONTENT['BASIC'] = {
  en: 'BASIC',
  sections: [
    {title:'歷史', content:'BASIC (Beginner\'s All-purpose Symbolic Instruction Code) 由 John Kemeny 和 Thomas Kurtz 於 1964 年在達特茅斯學院創造。設計目標是讓非科學背景的學生也能輕鬆學習程式設計。語法接近英語，無需編譯即可執行。'},
    {title:'個人電腦革命', content:'BASIC 隨 Altair 8800 (1975)、Apple II (1977)、IBM PC (1981) 等個人電腦普及，成為數百萬人的第一個程式語言。Microsoft 的 Altair BASIC 是比爾蓋茲和保羅艾倫的第一個產品。雖然現代已較少使用，BASIC 的影響體現在 Visual Basic 和 VBA 等後繼語言中。'},
  ],
};

TOPIC_CONTENT['編譯器'] = {
  en: 'Compiler',
  sections: [
    {title:'編譯器架構', content:'編譯器將高階語言轉換為低階程式碼，典型分為前端和後端。前端包含詞法分析 (Lexer) → 語法分析 (Parser) → 語意分析 → 中間表示 (IR)。後端包含最佳化 → 程式碼生成 → 目標機器碼。這種分離設計使同一前端可支援多種目標架構。'},
    {title:'詞法分析', content:'詞法分析器將原始碼字串切割為記號 (Token) 序列：關鍵字、識別字、運算子、常數等。通常使用正規表示式和有限狀態機實作。工具如 Lex/Flex 可從規則定義自動生成詞法分析器。'},
    {title:'語法分析', content:'語法分析器根據語法規則將 Token 序列組織為樹狀結構。兩種主要方法：自頂向下 (遞迴下降、LL) 適合手寫分析器；自底向上 (LR、LALR) 適合工具生成 (Yacc/Bison)。輸出為抽象語法樹 (AST)。'},
    {title:'最佳化與程式碼生成', content:'最佳化階段在 IR 上執行多種轉換：常數折疊、死碼消除、循環展開、內聯展開、暫存器分配。程式碼生成將最佳化的 IR 轉換為目標機器指令，處理指令選擇、排程、暫存器分配等問題。'},
  ],
};

TOPIC_CONTENT['直譯器'] = {
  en: 'Interpreter',
  sections: [
    {title:'直譯器架構', content:'直譯器直接執行原始碼或位元組碼，無需編譯為機器碼。基本架構：Lexer → Parser → AST → Executor (Tree-walking)。更高效的做法是編譯為位元組碼然後在虛擬機上執行（如 Python、Java）。'},
    {title:'REPL 與互動性', content:'直譯語言的核心優勢是互動性：REPL (Read-Eval-Print-Loop) 允許開發者逐一執行表達式並即時看到結果。這極大加速了探索性程式設計和除錯。LISP、Python、JavaScript 的 REPL 都是其生態的重要部分。'},
  ],
};

TOPIC_CONTENT['c4'] = {
  en: 'c4 — C in Four Functions',
  sections: [
    {title:'什麼是 c4', content:'c4 是由 Robert Swier 撰寫的迷你 C 編譯器，自稱 "C in four functions"。全部程式碼僅約 500 行，包含了 Lexer、Parser、Code Generator、Virtual Machine 四個核心功能。儘管微小，c4 能編譯並執行自己的原始碼（自舉）。'},
    {title:'實作方式', content:'c4 採用遞迴下降解析，即時產生虛擬機器指令，無明確的 AST 結構。VM 是一個簡單的堆疊機，支援運算、變數、控制流、函數呼叫。c4 是學習編譯器實作的絕佳教材，在 GitHub 上廣泛被 fork 和研究。'},
  ],
};

TOPIC_CONTENT['LLVM'] = {
  en: 'LLVM',
  sections: [
    {title:'LLVM 架構', content:'LLVM 由 Chris Lattner 於 2000 年在 UIUC 開始開發，現已成為編譯器基礎設施的事實標準。三階段設計：前端 (Clang) 將原始碼轉為 LLVM IR，最佳化器對 IR 做無關機器的轉換，後端將 IR 轉為目標機器碼。'},
    {title:'LLVM IR', content:'LLVM IR (Intermediate Representation) 採用 SSA (Static Single Assignment) 形式，每個變數只賦值一次。IR 有三種表示法：人類可讀的文字格式、緊湊的位元組碼格式、記憶體中 C++ 物件。支援強型別系統和豐富的指令集。'},
    {title:'最佳化管道', content:'LLVM 提供一系列最佳化 pass：-O1/-O2/-O3/-Os/-Oz 管道。關鍵 Pass 包括：內聯 (Inlining)、GVN (全域值編號)、SCCP (稀疏條件常數傳播)、迴圈最佳化 (LoopUnroll、LoopVectorize)、自動向量化 (Auto-Vectorization)。'},
  ],
};

TOPIC_CONTENT['管線處理與編譯器'] = {
  en: 'Pipeline Processing and Compilers',
  sections: [
    {title:'管線簡介', content:'現代 CPU 使用管線 (Pipeline) 並行執行指令，典型分為 IF (擷取)、ID (解碼)、EX (執行)、MEM (記憶體)、WB (寫回) 五級。理想情況下每個時脈週期完成一條指令，但結構、資料、控制三種危險會造成停頓。'},
    {title:'編譯器的最佳化', content:'編譯器可以透過指令重排來減少管線停頓。例如將載入指令提前以隱藏記憶體延遲、在分支指令後插入獨立指令填充分支延遲槽。現代編譯器也進行循環展開、軟體管線化 (Software Pipelining) 等高級排程最佳化。'},
  ],
};

TOPIC_CONTENT['作業系統'] = {
  en: 'Operating System',
  sections: [
    {title:'作業系統的功能', content:'作業系統是管理電腦硬體和軟體資源的系統軟體。核心功能包括：行程管理 (建立/排程/終止行程)、記憶體管理 (分配/保護/虛擬記憶體)、檔案系統 (儲存/組織/存取控制)、I/O 管理 (裝置驅動/中斷處理)、網路和安全性。'},
    {title:'作業系統類型', content:'批次處理系統 (早期大型機)、分時系統 (Unix、Multics)、即時系統 (RTOS、VxWorks)、嵌入式系統、個人電腦 OS (Windows、macOS)、行動 OS (iOS、Android)、伺服器/雲端 OS (Linux、Windows Server)。'},
    {title:'核心架構', content:'單核心 (Monolithic Kernel) — 所有服務在核心空間，效率高但耦合度高（如 Linux）。微核心 (Microkernel) — 最小化核心功能，其他服務在使用者空間（如 Minix）。混合核心結合兩者優點（如 Windows NT）。'},
  ],
};

TOPIC_CONTENT['Unix'] = {
  en: 'Unix',
  sections: [
    {title:'Unix 哲學', content:'Unix 的設計原則：1) 小即是美 — 每個工具做好一件事。2) 一切皆檔案 — 統一 I/O 介面。3) 組合勝過擴展 — 管線連接小工具。4) 純文字作為通用介面。5) 可移植性 — C 語言實作。這些原則至今仍指導著系統軟體設計。'},
    {title:'核心架構', content:'Unix 核心提供行程管理 (fork/exec)、記憶體管理 (分頁/交換)、檔案系統 (inode-based)、訊號處理、行程間通訊 (pipe/signal/shared memory)。系統呼叫是使用者程式與核心的介面，透過軟體中斷 (trap) 切換特權層級。'},
  ],
};

TOPIC_CONTENT['Unix V6'] = {
  en: 'Unix Version 6',
  sections: [
    {title:'經典 Unix', content:'UNIX Version 6 (1975) 是第一個廣泛發布的 Unix 版本，運行於 PDP-11 迷你電腦。其原始碼約 9,000 行 C 語言和 1,000 行組合語言，以現代標準來看極其簡潔可讀。Lions 的註解版原始碼成為作業系統課程的經典教材。'},
    {title:'檔案系統', content:'V6 檔案系統使用 inode 結構，每個檔案由一個 inode 描述（包含權限、擁有者、大小、13 個區塊指標）。目錄是包含 (檔名, inode號碼) 配對的特殊檔案。這種結構成為 Unix 類檔案系統的基礎。'},
  ],
};

TOPIC_CONTENT['xv6'] = {
  en: 'xv6',
  sections: [
    {title:'教學作業系統', content:'xv6 是 MIT 為作業系統課程開發的教學作業系統，以 ANSI C 重寫並現代化 Unix V6。原始碼約 8,000 行，完整實作了行程、記憶體管理、檔案系統、管線、訊號等核心功能。支援 RISC-V 多處理器架構。'},
    {title:'系統呼叫', content:'xv6 提供簡潔的系統呼叫介面：fork (建立行程)、exec (執行程式)、pipe (建立管線)、read/write (I/O)、kill (發送訊號)、sbrk (調整堆積大小)。每個系統呼叫透過 ecall 指令進入核心，在核心中完成實際工作後返回使用者空間。'},
  ],
};

TOPIC_CONTENT['RISC-V'] = {
  en: 'RISC-V',
  sections: [
    {title:'開源 ISA', content:'RISC-V 由加州大學柏克萊分校於 2010 年開發，是第一個完全開放的指令集架構 (ISA)。採用模塊化設計：基礎整數指令集 (RV32I/RV64I) + 標準擴展 (M 乘法、A 原子、F/D 浮點、C 壓縮指令)。免授權費，任何人可用於任何目的。'},
    {title:'特權層級', content:'RISC-V 定義三種特權層級：機器模式 (M-mode) — 最高權限，執行最低層韌體；監督者模式 (S-mode) — 執行作業系統核心；使用者模式 (U-mode) — 執行程式。支援多層次異常處理和虛擬記憶體。'},
  ],
};

TOPIC_CONTENT['組合語言'] = {
  en: 'Assembly Language',
  sections: [
    {title:'RISC-V 指令格式', content:'RISC-V 使用六種統一的 32 位元指令格式：R-type (add, sub, and, or)、I-type (addi, lw)、S-type (sw)、B-type (beq, bne)、U-type (lui, auipc)、J-type (jal)。統一的格式簡化了 CPU 的解碼邏輯。'},
    {title:'控制流與函數呼叫', content:'RISC-V 的條件分支使用 beq/bne/blt/bge 等指令，比較兩個暫存器後跳轉。函數呼叫使用 jal (Jump and Link) 儲存回傳位址到 ra 暫存器。呼叫慣例定義了引數暫存器 (a0-a7)、保存暫存器 (s0-s11)、暫存暫存器 (t0-t6) 的分工。'},
  ],
};

TOPIC_CONTENT['記憶體管理'] = {
  en: 'Memory Management',
  sections: [
    {title:'記憶體分配策略', content:'固定分割區 — 記憶體預先分割為固定大小區塊，簡單但有內部碎片。可變分割區 — 動態分配，有外部碎片問題。夥伴系統 (Buddy System) — 以 2 的冪次分割，合併相鄰空閒塊。Slab 分配器 — 為特定物件類型預先分配快取。'},
    {title:'分頁與分段', content:'分頁 (Paging) — 將虛擬和實體記憶體分為固定大小的頁，消除外部碎片。分段 (Segmentation) — 按邏輯區塊（程式碼、資料、堆疊）分割，支援共享和保護。現代系統主要使用分頁，或分頁+分段混合 (x86)。'},
  ],
};

TOPIC_CONTENT['垃圾搜集'] = {
  en: 'Garbage Collection',
  sections: [
    {title:'追蹤式 GC', content:'Mark-Sweep (標記清除)：從根集合開始標記所有可達物件，然後回收未標記物件。Mark-Compact (標記壓縮)：標記後將存活物件壓縮到一端，消除碎片。Copying (複製)：將存活物件複製到另一半空間，優點是分配快速（bump allocation）。'},
    {title:'世代式 GC', content:'觀察到大多數物件很快死亡（朝生夕滅）。將物件分為年輕代和老年代，年輕代頻繁 GC，老年代較少 GC。物件經數次 GC 存活後晉升到老年代。世代式 GC 在 JVM、V8 等現代 runtime 中廣泛使用。'},
  ],
};

TOPIC_CONTENT['虛擬記憶體'] = {
  en: 'Virtual Memory',
  sections: [
    {title:'位址轉換', content:'虛擬位址由 CPU 產生，透過頁表 (Page Table) 轉換為實體位址。x86-64 使用四層頁表 (PML4→PDPT→PD→PT→Offset)。每個行程有自己的頁表，實現記憶體隔離。TLB (Translation Lookaside Buffer) 快取最近使用的轉換以加速。'},
    {title:'頁面置換演算法', content:'當實體記憶體不足時，需置換頁面。FIFO (先進先出) 簡單但可能置換頻繁使用的頁面。LRU (最近最少使用) 利用局部性原理，實作成本較高。Clock (第二次機會) 演算法是 LRU 的近似，使用參考位元。現代 Linux 使用多層次 LRU 演算法。'},
  ],
};

TOPIC_CONTENT['行程與執行緒'] = {
  en: 'Processes and Threads',
  sections: [
    {title:'行程', content:'行程 (Process) 是執行中的程式實例，擁有獨立的虛擬位址空間、檔案描述符、訊號處理器等資源。行程控制區塊 (PCB) 儲存行程狀態、程式計數器、暫存器、排程資訊。行程間切換成本高，需要 TLB 刷新。'},
    {title:'執行緒', content:'執行緒 (Thread) 是 CPU 排程的最小單位，同一行程的執行緒共享位址空間和資源，每個執行緒有獨立的堆疊和暫存器。執行緒切換成本低。核心級執行緒由 OS 核心管理，使用者級執行緒由使用者空間 runtime 管理。'},
    {title:'上下文切換', content:'上下文切換 (Context Switch) 是暫停當前行程/執行緒並恢復另一個的過程。需要保存和恢復暫存器、程式計數器、堆疊指標等。頻繁的上下文切換會降低效能。Linux 透過 CFS 和組排程盡量減少不必要的切換。'},
  ],
};

TOPIC_CONTENT['排程'] = {
  en: 'Scheduling',
  sections: [
    {title:'排程目標', content:'排程器需要平衡多個目標：CPU 利用率 (越高越好)、吞吐量 (單位時間完成的工作量)、周轉時間 (從提交到完成的時間)、等待時間 (在就緒佇列中的時間)、響應時間 (從提交到首次執行的時間)。不同場景對這些目標的權重不同。'},
    {title:'經典演算法', content:'FCFS (先來先服務) — 簡單但平均等待時間長。SJF (最短工作優先) — 最小化平均等待時間但可能餓死長工作。Priority (優先權排程) — 高優先權先執行，可能導致低優先權餓死。Round Robin (輪詢) — 每個行程獲得固定時間片，互動式系統常用。'},
    {title:'Linux CFS', content:'Linux 的 Completely Fair Scheduler (CFS) 使用紅黑樹管理可執行行程，以 vruntime (虛擬運行時間) 作為鍵值。每次選擇 vruntime 最小的行程執行。CFS 確保所有行程公平獲得 CPU 時間，並支援 nice 值設定優先級。'},
  ],
};

TOPIC_CONTENT['同步與通訊'] = {
  en: 'Synchronization and Communication',
  sections: [
    {title:'同步機制', content:'Mutex (互斥鎖) — 保護臨界區，一次只有一個執行緒能進入。Semaphore (信號量) — 計數同步，可控制多個資源的存取。Condition Variable (條件變數) — 等待特定條件滿足。Read-Write Lock (讀寫鎖) — 區分讀取和寫入，允許多個讀取者並行。'},
    {title:'死結', content:'死結 (Deadlock) 的四個必要條件：互斥、持有並等待、不可剝奪、循環等待。處理策略：預防 (打破四個條件之一)、避免 (銀行家演算法)、檢測與恢復。現代系統主要採用預防和檢測策略。'},
    {title:'行程間通訊', content:'IPC (Inter-Process Communication) 方式：PIPE (管線) — 親屬行程間單向通訊。FIFO/Named Pipe — 無親屬關係行程也可用。Shared Memory — 最快的 IPC，需同步機制。Message Queue — 訊息傳遞。Signal — 非同步事件通知。Socket — 跨機器通訊。'},
  ],
};

TOPIC_CONTENT['檔案系統'] = {
  en: 'File Systems',
  sections: [
    {title:'檔案系統結構', content:'典型 Unix 檔案系統分為：superblock (儲存整體資訊：inode 數量、區塊大小)、inode table (每個檔案/目錄一個 inode，儲存中繼資料和區塊指標)、data blocks (實際資料)。目錄是包含 (名稱, inode號碼) 項目的特殊檔案。'},
    {title:'日誌式檔案系統', content:'日誌 (Journaling) 防止系統崩潰導致的檔案系統損壞。在寫入實際資料前先將操作記錄到日誌。Ext3/Ext4 使用三種模式：Journal (全部日誌)、Ordered (僅日誌中繼資料，先寫入資料)、Writeback (僅日誌中繼資料)。現代檔案系統如 Ext4、XFS、Btrfs 都支援日誌。'},
  ],
};

TOPIC_CONTENT['網路'] = {
  en: 'Networking',
  sections: [
    {title:'網路分類', content:'網路按規模分為：PAN (個人區域網路，~1m)、LAN (區域網路，~100m)、MAN (都會網路，~10km)、WAN (廣域網路，全球)。按拓撲分為：匯流排、星狀、環狀、網狀、樹狀。乙太網路 (Ethernet) 是區域網路的主流技術。'},
    {title:'OSI 模型', content:'OSI 七層模型 (由上到下)：應用層、表現層、會話層、傳輸層、網路層、資料鏈路層、實體層。TCP/IP 四層模型（應用層、傳輸層、網路層、網路介面層）更符合實際網際網路架構。'},
  ],
};

TOPIC_CONTENT['TCP/IP'] = {
  en: 'TCP/IP',
  sections: [
    {title:'TCP/IP 協定族', content:'TCP/IP 四層模型：應用層 (HTTP, FTP, SMTP, DNS)、傳輸層 (TCP 可靠連線導向, UDP 不可靠無連線)、網路層 (IP 定址與路由)、網路介面層 (實體網路)。IP 協定負責將封包從來源送到目的地，TCP 在 IP 之上提供可靠傳輸。'},
    {title:'TCP 特性', content:'TCP 提供連線導向的可靠傳輸。三次握手建立連線：SYN → SYN-ACK → ACK。流量控制使用滑動視窗 (Sliding Window)。壅塞控制包含慢啟動 (Slow Start)、壅塞避免 (Congestion Avoidance)、快速重傳 (Fast Retransmit)。TCP 保證資料按順序送達且無遺失。'},
  ],
};

TOPIC_CONTENT['HTTP'] = {
  en: 'HTTP',
  sections: [
    {title:'HTTP 演進', content:'HTTP/0.9 (1991) — 僅 GET 方法，純文字。HTTP/1.0 (1996) — 加入 Header、狀態碼、多方法。HTTP/1.1 (1997) — 持久連線、管線化、分塊編碼。HTTP/2 (2015) — 多路複用、伺服器推送、頭部壓縮。HTTP/3 (2022) — 基於 QUIC (UDP)，降低連線建立延遲。'},
    {title:'RESTful API', content:'REST (Representational State Transfer) 是基於 HTTP 的 API 設計風格。資源透過 URL 標識，操作使用 HTTP 方法：GET (讀取)、POST (建立)、PUT (更新)、DELETE (刪除)。狀態碼表示結果：200 OK、201 Created、404 Not Found、500 Server Error。'},
  ],
};

TOPIC_CONTENT['Socket'] = {
  en: 'Socket Programming',
  sections: [
    {title:'Socket API', content:'Berkeley Socket API 是網路程式設計的標準介面。TCP Server 流程：socket() → bind() → listen() → accept() → read()/write() → close()。TCP Client 流程：socket() → connect() → read()/write() → close()。SOCK_STREAM 表示 TCP，SOCK_DGRAM 表示 UDP。'},
    {title:'高效 I/O 模型', content:'阻塞 I/O — 行程在系統呼叫時休眠。非阻塞 I/O — 立即返回，需輪詢。I/O 多路複用 (select/poll/epoll) — 單執行緒監控多個檔案描述符。epoll 是 Linux 高效方案，使用事件驅動和回呼機制，適合大量並發連線。'},
  ],
};

TOPIC_CONTENT['Linux 系統呼叫'] = {
  en: 'Linux System Calls',
  sections: [
    {title:'系統呼叫流程', content:'使用者程式透過系統呼叫請求核心服務。流程：應用程式調用 glibc 包裹函數 → 軟體中斷 (int 0x80 或 syscall 指令) → 切換到核心態 → 執行對應核心函數 → 返迴使用者態。x86-64 使用 syscall/sysret 指令切換，透過暫存器傳遞參數和系統呼叫號碼。'},
    {title:'常用系統呼叫', content:'檔案操作：open、read、write、close、lseek。行程管理：fork、execve、exit、waitpid、getpid。記憶體：mmap、munmap、brk、mprotect。網路：socket、connect、bind、listen、accept。程序控制：kill、signal、sigaction、nanosleep。'},
  ],
};

TOPIC_CONTENT['POSIX'] = {
  en: 'POSIX',
  sections: [
    {title:'POSIX 標準', content:'POSIX (Portable Operating System Interface) 是 IEEE 1003.1 標準，定義了作業系統應該提供的 API。目標是讓應用程式可在不同 POSIX 相容系統間移植。包含：流程控制 (fork, exec)、執行緒 (pthread)、檔案 I/O、訊號、socket 等。'},
    {title:'核心概念', content:'一切皆檔案 — 檔案、目錄、裝置、socket、管線都以檔案描述符操作。標準串流 — stdin/stdout/stderr。管線 (Pipe) — 連接行程的 I/O 通道。訊號 (Signal) — 非同步事件通知。Shell — 命令列介面與腳本執行環境。'},
  ],
};

TOPIC_CONTENT['Shell 腳本'] = {
  en: 'Shell Scripting',
  sections: [
    {title:'Shell 程式設計', content:'Shell 腳本是 Unix 系統管理的核心工具。變數：name="value"、$name、$1-$9 (位置參數)。控制結構：if/then/else/elif、for/in/do/done、while/do/done、case/esac。函數定義：function name { commands; }。'},
    {title:'管線與重定向', content:'管線 (|) 將前一個指令的 stdout 連接到下個指令的 stdin。重定向：> (覆蓋寫入)、>> (附加寫入)、< (讀取)、2> (錯誤輸出)。EOF 標記 (<<) 用於內嵌輸入。Job Control：& (背景執行)、Ctrl+Z (暫停)、jobs/fg/bg。'},
  ],
};

TOPIC_CONTENT['連結器與載入器'] = {
  en: 'Linkers and Loaders',
  sections: [
    {title:'ELF 格式', content:'ELF (Executable and Linkable Format) 是 Linux 的標準可執行檔格式。主要部分：ELF Header (辨識、架構、進入點)、Section Header Table (.text 程式碼、.data 資料、.bss 未初始化、.rodata 唯讀)、Program Header Table (載入資訊)。readelf 和 objdump 是 ELF 分析工具。'},
    {title:'連結過程', content:'編譯器產生目的檔 (.o)，包含尚未解析的符號參考。連結器的工作：符號解析 (將符號參考連接到符號定義)、重定位 (調整位址以匹配最終記憶體佈局)。靜態連結將所有目的檔和函式庫合併為單一可執行檔。動態連結在載入或執行時解析共享函式庫。'},
  ],
};

TOPIC_CONTENT['訊號處理'] = {
  en: 'Signal Processing',
  sections: [
    {title:'訊號概念', content:'訊號是 Unix/Linux 的非同步事件通知機制。常見訊號：SIGINT (Ctrl+C)、SIGKILL (強制終止)、SIGSEGV (記憶體存取錯誤)、SIGTERM (優雅終止)、SIGCHLD (子行程狀態變更)。每個訊號有預設處理動作：終止、忽略、核心傾印、停止、繼續。'},
    {title:'訊號 API', content:'signal() 設定訊號處理函數，但行為因平台而異。sigaction() 是 POSIX 標準介面，提供更精確的控制。kill() 發送訊號到行程。sigprocmask() 管理訊號遮罩。pause()、sigsuspend() 等待訊號。非同步訊號安全函數 (Async-Signal-Safe Functions) 可在處理函數中安全呼叫。'},
  ],
};

TOPIC_CONTENT['密碼學與資訊安全'] = {
  en: 'Cryptography and Information Security',
  sections: [
    {title:'對稱加密', content:'DES (56 位元金鑰，已不安全) → AES (128/192/256 位元金鑰，目前標準)。AES 使用 SPN (Substitution-Permutation Network) 結構，透過位元組代換、行位移、列混合、輪金鑰加等操作進行多輪加密。對稱加密速度快，適合大量資料加密。'},
    {title:'非對稱加密', content:'RSA 基於大整數分解的困難度：選擇兩個大質數 p,q → n=pq → 選擇 e → 計算 d = e⁻¹ mod φ(n)。公鑰 (n, e) 加密，私鑰 (n, d) 解密。金鑰長度通常 2048–4096 位元。ECDSA 基於橢圓曲線，相同安全強度下金鑰更短。'},
    {title:'TLS 協定', content:'TLS (Transport Layer Security) 在 TCP 之上提供加密通訊。握手階段：客戶端和伺服器協商密碼套件、交換公鑰憑證、透過非對稱加密交換對稱金鑰。記錄層使用對稱加密保護傳輸資料。HTTPS = HTTP over TLS。'},
  ],
};

TOPIC_CONTENT['區塊鏈與比特幣'] = {
  en: 'Blockchain and Bitcoin',
  sections: [
    {title:'區塊鏈結構', content:'區塊鏈由一系列區塊 (Block) 組成，每個區塊包含多筆交易。區塊頭包含：前一個區塊的 hash、Merkle Tree 根 hash、時間戳、難度目標、nonce。Merkle Tree 將多筆交易的 hash 組織為樹狀結構，高效驗證交易是否存在。'},
    {title:'工作量證明', content:'比特幣使用 PoW (Proof of Work) 共識機制。礦工需找到 nonce 使區塊 hash 小於目標值。這個過程需要大量計算，但驗證只需一次 hash。難度目標每 2016 個區塊 (約 2 週) 調整一次，維持平均 10 分鐘出塊速度。'},
  ],
};

TOPIC_CONTENT['虛擬機'] = {
  en: 'Virtual Machines',
  sections: [
    {title:'系統虛擬化', content:'Hypervisor 在硬體和作業系統之間建立抽象層。Type 1 (裸機) — 直接運行在硬體上（如 VMware ESXi、KVM）。Type 2 (託管) — 執行在作業系統之上（如 VirtualBox、VMware Workstation）。硬體輔助虛擬化 (Intel VT-x/AMD-V) 透過 CPU 支援提升效能。'},
    {title:'程序虛擬化', content:'程序虛擬機為單一應用程式提供執行環境。JVM (Java Virtual Machine) 執行 Java 位元組碼，JIT 編譯提升效能。CLR (.NET)、V8 (JavaScript)、WASM (WebAssembly) 都是程序虛擬機。容器 (Container) 共享主機核心，比傳統 VM 更輕量。'},
  ],
};

TOPIC_CONTENT['QEMU'] = {
  en: 'QEMU',
  sections: [
    {title:'簡介', content:'QEMU 由 Fabrice Bellard 開發，是功能強大的開源機器模擬器和虛擬化工具。支援多種目標架構（x86、ARM、RISC-V、MIPS 等）的完整系統模擬。可用於開發、測試、除錯作業系統和嵌入式軟體。'},
    {title:'使用方式', content:'系統模擬：qemu-system-riscv64 -machine virt -bios fw_jump.bin -kernel vmlinux。使用者模式模擬：qemu-riscv64-linux-user ./program。硬體加速 (Linux KVM)：qemu-system-x86_64 -enable-kvm -m 2G ubuntu.qcow2。GDB 遠端除錯：-s -S 標誌。'},
  ],
};

TOPIC_CONTENT['Kubernetes'] = {
  en: 'Kubernetes',
  sections: [
    {title:'架構', content:'Kubernetes (K8s) 由 Google 於 2014 年開源，是容器編排的事實標準。控制平面 (Control Plane) 包含：API Server (所有操作入口)、Scheduler (分配 Pod 到節點)、Controller Manager (維持期望狀態)、etcd (分散式鍵值儲存)。工作節點 (Node) 執行 Pod，由 kubelet 管理。'},
    {title:'核心抽象', content:'Pod — 最小部署單位，一個或多個容器共享網路和儲存。Service — 提供穩定的網路端點，透過標籤選擇器找到對應 Pod。Deployment — 管理 Pod 的聲明式更新。ConfigMap/Secret — 設定管理。PersistentVolume — 儲存抽象。'},
  ],
};

TOPIC_CONTENT['並行計算與異構平台'] = {
  en: 'Parallel Computing and Heterogeneous Platforms',
  sections: [
    {title:'並行層級', content:'指令級並行 (ILP) — 管線、超純量、亂序執行。執行緒級並行 (TLP) — 多核心、多執行緒。資料級並行 (DLP) — SIMD、向量處理器。任務級並行 — 多處理器、叢集。Flynn 分類法：SISD、SIMD、MISD、MIMD。'},
    {title:'異構運算', content:'CPU + GPU + FPGA/ASIC 協同工作。GPU 擅長資料平行任務（矩陣運算、影像處理）。FPGA 可重新配置，適合低延遲管線。TPU 專為 TensorFlow 設計的 ASIC。OpenCL 和 CUDA 是異構程式設計的主流框架。Amdahl 定律限制了加速比的上限。'},
  ],
};

TOPIC_CONTENT['AI 的加速技術'] = {
  en: 'AI Acceleration Technologies',
  sections: [
    {title:'矩陣乘法加速', content:'神經網路的核心運算是矩陣乘法。CPU 使用 SIMD 指令 (AVX-512、ARM NEON) 一次處理多個資料。GPU 使用數千個 CUDA 核心平行計算。NVIDIA Tensor Core 專為 4×4 矩陣乘法加速設計，可達每秒百兆次運算。'},
    {title:'模型最佳化', content:'模型量化 (Quantization) 將 FP32 權重降為 INT8/FP16，減少記憶體並加速推理。剪枝 (Pruning) 移除不重要的神經元連接。知識蒸餾 (Knowledge Distillation) 用大模型教導小模型。TensorRT 和 ONNX Runtime 是常見的推理加速框架。'},
  ],
};

TOPIC_CONTENT['雲端技術'] = {
  en: 'Cloud Computing',
  sections: [
    {title:'服務模型', content:'SaaS (Software as a Service) — 使用完整應用（如 Gmail）。PaaS (Platform as a Service) — 部署自己的應用（如 Heroku）。IaaS (Infrastructure as a Service) — 管理虛擬伺服器和網路（如 AWS EC2）。FaaS (Function as a Service) — 無伺服器架構。'},
    {title:'雲端部署', content:'公有雲 (Public Cloud) — AWS/GCP/Azure，共用基礎設施。私有雲 (Private Cloud) — 專用基礎設施。混合雲 (Hybrid Cloud) — 結合兩者。邊緣運算 (Edge Computing) — 將計算推向資料來源端。容器和 Kubernetes 是現代雲端部署的標準方式。'},
  ],
};

TOPIC_CONTENT['Make / CMake'] = {
  en: 'Make and CMake',
  sections: [
    {title:'Makefile', content:'Make 是最經典的建置工具。Makefile 由規則組成：target: dependencies → recipe。自動變數：$@ (目標)、$< (第一個依賴)、$^ (所有依賴)。模式規則使用 % 萬用字元。.PHONY 標記虛擬目標。常見目標：all、clean、install、test。'},
    {title:'CMake', content:'CMake 是跨平台建置系統生成器。使用 CMakeLists.txt 描述專案。核心指令：cmake_minimum_required、project、add_executable、add_library、target_link_libraries。建置：mkdir build && cd build && cmake .. && make。CMake 支援多種產生器（Make、Ninja、Visual Studio）。'},
  ],
};

TOPIC_CONTENT['GDB / LLDB'] = {
  en: 'GDB and LLDB',
  sections: [
    {title:'GDB 基本指令', content:'run — 啟動程式。break — 設定中斷點 (break main, break file.c:42)。next — 跳過一行。step — 進入函數。print — 印出變數值。backtrace — 顯示呼叫堆疊。info registers — 顯示暫存器。watch — 監控變數變化。x — 檢視記憶體 (x/4xw addr)。'},
    {title:'LLDB', content:'LLDB 是 LLVM 專案的除錯器，指令與 GDB 相似但更現代。等效指令：run、breakpoint set、thread step-over、frame variable、bt。LLDB 整合 LLVM 生態，支援表達式求值時的 JIT 編譯，對 C++ 和 Rust 除錯有更好的支援。'},
  ],
};

TOPIC_CONTENT['Dennis Ritchie'] = {
  en: 'Dennis Ritchie',
  sections: [
    {title:'生平', content:'Dennis Ritchie (1941-2011) 是美國電腦科學家，貝爾實驗室研究員。他創造了 C 程式語言，並與 Ken Thompson 共同開發了 Unix 作業系統。1983 年獲得圖靈獎。他的工作奠定了現代系統程式的基礎。'},
    {title:'主要貢獻', content:'C 語言 (1972) — 成為系統程式的主流語言，影響了 C++、Java、C#、Go、Rust 等幾乎所有後繼語言。Unix — 與 Thompson 共同開發，引入「一切皆檔案」和組合哲學。K&R C 書籍是經典教材。'},
  ],
};

TOPIC_CONTENT['Ken Thompson'] = {
  en: 'Ken Thompson',
  sections: [
    {title:'生平', content:'Ken Thompson (1943-) 是美國電腦科學家，貝爾實驗室研究員。他共同創造了 Unix 作業系統，開發了 B 語言（C 的前身），並在正規表示式和編譯器領域有重要貢獻。1983 年與 Ritchie 共同獲得圖靈獎。'},
    {title:'主要貢獻', content:'Unix — 與 Ritchie 共同開發，Tim Thompson 設計了檔案系統、管線、簡單 shell。B 語言 — C 的前身，簡化了系統程式設計。正規表示式 — 開發了 grep (global regular expression print) 工具。Thompson 建構攻擊證明可以在編譯器中植入後門。近年於 Google 參與 Go 語言設計。'},
  ],
};

TOPIC_CONTENT['Linus Torvalds'] = {
  en: 'Linus Torvalds',
  sections: [
    {title:'生平', content:'Linus Torvalds (1969-) 是芬蘭裔美國軟體工程師，Linux 核心和 Git 的創造者。1991 年在大學時期發布了 Linux 核心的第一個版本。Linux 已成為伺服器、超級電腦、嵌入式設備的主流作業系統。'},
    {title:'主要貢獻', content:'Linux 核心 (1991) — 從個人專案成長為全球最大的開源專案，數千名開發者貢獻，運行在從 Android 手機到 TOP500 超級電腦的各類設備上。Git (2005) — 分散式版本控制系統，徹底改變了軟體開發協作方式。Linus 以直率坦誠的溝通風格著稱。'},
  ],
};

TOPIC_CONTENT['Fabrice Bellard'] = {
  en: 'Fabrice Bellard',
  sections: [
    {title:'生平', content:'Fabrice Bellard (1972-) 是法國程式設計師，以創造多個影響深遠的開源軟體專案而聞名。他擅長編譯器、模擬器和多媒體技術。被認為是當代最有天賦的程式設計師之一。'},
    {title:'主要成就', content:'QEMU (2005) — 最流行的開源機器模擬器，支援多種架構的完整系統模擬。TinyCC (TCC) — 極小的 C 編譯器，可編譯自舉，啟動極快。FFmpeg — 共同創造了最廣泛使用的多媒體處理工具。他也曾用 JavaScript 實作 PC 模擬器在瀏覽器中啟動 Linux。'},
  ],
};

var CATEGORY_TAGS = {
  '系統程式的歷史': ['歷史', '概論'],
  '程式語言': ['概論', '語言'],
  'C語言': ['語言', '系統'],
  'C++': ['語言', '物件導向'],
  'Rust': ['語言', '安全', '系統'],
  'Go': ['語言', '並行'],
  'Zig': ['語言', '系統'],
  'Haskell': ['語言', '函數式'],
  'LISP': ['語言', '函數式', '歷史'],
  'Prolog': ['語言', '邏輯'],
  'BASIC': ['語言', '歷史'],
  '編譯器': ['工具', '語言處理'],
  '直譯器': ['工具', '語言處理'],
  'c4': ['編譯器', '教學'],
  'LLVM': ['編譯器', '工具'],
  '管線處理與編譯器': ['編譯器', '架構'],
  '作業系統': ['OS', '核心'],
  'Unix': ['OS', '歷史'],
  'Unix V6': ['OS', '歷史'],
  'xv6': ['OS', '教學'],
  'RISC-V': ['架構', 'ISA'],
  '組合語言': ['語言', '低階'],
  '記憶體管理': ['記憶體', 'OS'],
  '垃圾搜集': ['記憶體', '語言處理'],
  '虛擬記憶體': ['記憶體', 'OS'],
  '行程與執行緒': ['OS', '並行'],
  '排程': ['OS', '效能'],
  '同步與通訊': ['OS', '並行'],
  '檔案系統': ['OS', '儲存'],
  '網路': ['網路', '基礎'],
  'TCP/IP': ['網路', '協定'],
  'HTTP': ['網路', '協定', 'Web'],
  'Socket': ['網路', '程式設計'],
  'Linux 系統呼叫': ['OS', '程式設計'],
  'POSIX': ['標準', 'OS'],
  'Shell 腳本': ['工具', '自動化'],
  '連結器與載入器': ['工具', '編譯器'],
  '訊號處理': ['OS', '程式設計'],
  '密碼學與資訊安全': ['安全', '加密'],
  '區塊鏈與比特幣': ['安全', '分散式'],
  '虛擬機': ['虛擬化', '工具'],
  'QEMU': ['虛擬化', '工具'],
  'Kubernetes': ['雲端', '容器'],
  '並行計算與異構平台': ['並行', '架構'],
  'AI 的加速技術': ['AI', '效能'],
  '雲端技術': ['雲端', '基礎設施'],
  'Make / CMake': ['工具', '建置'],
  'GDB / LLDB': ['工具', '除錯'],
  'Dennis Ritchie': ['人物', 'C', 'Unix'],
  'Ken Thompson': ['人物', 'Unix'],
  'Linus Torvalds': ['人物', 'Linux', 'Git'],
  'Fabrice Bellard': ['人物', 'QEMU'],
};

function findCodeForTopic(topicName) {
  var map = {
    'C語言':['c/computer0/compiler/c4/c4.c','c/computer0/compiler/c0/_version/c0c_long/main.c'],
    '編譯器':['c/computer0/compiler/c4/c4.c','c/computer0/compiler/js0/js0i.js','c/computer0/compiler/c0/_version/c0c_long/main.c','rust/compiler/lli4/src/main.rs','rust/compiler/rustc4/src/main.rs'],
    'c4':['c/computer0/compiler/c4/c4.c'],
    '直譯器':['c/computer0/interpreter/basic/basic.py','c/computer0/interpreter/lisp/lisp.py','c/computer0/interpreter/prolog/prolog.py','c/computer0/interpreter/py0i/py0i.py'],
    'BASIC':['c/computer0/interpreter/basic/basic.py','c/computer0/interpreter/basic/bas/fib.bas'],
    'LISP':['c/computer0/interpreter/lisp/lisp.py','c/computer0/interpreter/lisp/factorial.lisp'],
    'Prolog':['c/computer0/interpreter/prolog/prolog.py','c/computer0/interpreter/prolog/pl/fact.pl'],
    '作業系統':['c/computer0/os/mini-riscv-os2/01-HelloOs/os.c','c/computer0/os/xv6/kernel/main.c','rust/riscv/mini-riscv-os/src/main.rs','rust/riscv/xv6-rust-octopus/kernel/src/main.rs','rust/riscv/xv7-rust-octopus/kernel/src/main.rs','rust/riscv/xv8-rust-posix/posix/tools/src/lib.rs'],
    'xv6':['c/computer0/os/xv6/kernel/main.c','c/computer0/os/xv6/kernel/proc.c','c/computer0/os/xv6/kernel/fs.c','rust/riscv/xv6-rust-octopus/kernel/src/main.rs','rust/riscv/xv6-rust-octopus/kernel/src/proc.rs','rust/riscv/xv6-rust-octopus/kernel/src/fs.rs','rust/riscv/xv7-rust-octopus/kernel/src/main.rs'],
    'RISC-V':['c/computer0/os/mini-riscv-os2/01-HelloOs/os.c','c/computer0/os/mini-riscv-os2/01-HelloOs/start.s','rust/riscv/mini-riscv-os/src/main.rs','rust/riscv/rvemu4/src/cpu.rs','rust/riscv/rv4/src/vm.rs','rust/riscv/rvboard4/src/lib.rs','rust/riscv/verilog/single_cycle/rv64i_cpu.v','rust/riscv/xv6-rust-octopus/kernel/src/main.rs','rust/riscv/xv7-rust-octopus/kernel/src/main.rs'],
    '組合語言':['c/computer0/os/mini-riscv-os2/01-HelloOs/start.s','rust/riscv/verilog/single_cycle/rv64i_cpu.v'],
    '網路':['c/computer0/network/webserver0/http/webserver.c','c/computer0/network/telnet0/telnet_server.c'],
    'HTTP':['c/computer0/network/webserver0/http/webserver.c','c/computer0/crpyto/https0/httpd_ssl0.c'],
    'Socket':['c/computer0/network/telnet0/telnet_server.c','c/computer0/network/telnet0/telnet_client.c'],
    '密碼學與資訊安全':['c/computer0/crpyto/ssl0/src/ssl.c','c/computer0/crpyto/ssl0/src/aes.c','c/computer0/crpyto/ssl0/src/rsa.c','rust/crypto/ssl4/src/lib.rs','rust/crypto/keygen/src/main.rs'],
    '區塊鏈與比特幣':['c/computer0/tool/blockchain/blockchain0.c'],
    '系統程式的歷史':[],
    '程式語言':[],
    'C++':[],
    'Rust':['rust/riscv/mini-riscv-os/src/main.rs','rust/compiler/lli4/src/main.rs','rust/crypto/ssl4/src/lib.rs','rust/database/sql4/src/main.rs','rust/riscv/rv4/src/main.rs','rust/riscv/rvboard4/src/lib.rs','rust/riscv/xv6-rust-octopus/kernel/src/main.rs','rust/riscv/xv7-rust-octopus/kernel/src/main.rs','rust/riscv/xv8-rust-posix/posix/tools/src/lib.rs','rust/compiler/rustc4/src/main.rs','rust/compiler/objdump/src/main.rs','rust/database/lsm/src/main.rs','rust/database/fts/src/main.rs','rust/database/inodefs/src/fs.rs','rust/database/swisstable/src/lib.rs','rust/database/patricia-trie/src/main.rs','rust/database/redblacktree/src/main.rs','rust/crypto/keygen/src/main.rs','rust/tool/lz4/src/main.rs','rust/media/jpeg/src/main.rs','rust/media/mp3rs/src/main.rs','rust/web/js4/src/main.rs'],
    'Go':[],
    'Zig':[],
    'Haskell':[],
    'LLVM':['rust/compiler/lli4/src/ir.rs','rust/compiler/lli4/src/interp.rs','rust/compiler/rustc4/src/codegen.rs'],
    '管線處理與編譯器':[],
    'Unix':[],
    'Unix V6':[],
    '記憶體管理':[],
    '垃圾搜集':[],
    '虛擬記憶體':[],
    '行程與執行緒':[],
    '排程':[],
    '同步與通訊':[],
    '檔案系統':['rust/database/inodefs/src/fs.rs'],
    'TCP/IP':[],
    'Linux 系統呼叫':[],
    'POSIX':['rust/riscv/xv8-rust-posix/posix/tools/src/lib.rs'],
    'Shell 腳本':[],
    '連結器與載入器':[],
    '訊號處理':[],
    '虛擬機':['rust/riscv/rv4/src/vm.rs'],
    'QEMU':[],
    'Kubernetes':[],
    '並行計算與異構平台':[],
    'AI 的加速技術':[],
    '雲端技術':[],
    'Make / CMake':[],
    'GDB / LLDB':[],
    'Dennis Ritchie':[],
    'Ken Thompson':[],
    'Linus Torvalds':[],
    'Fabrice Bellard':[],
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
