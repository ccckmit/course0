# 系統程式

系統程式探討從高階語言到機器碼的完整轉換過程。

## 概念

- [系統程式的歷史](系統程式的歷史.md) - 從 ENIAC 到現代系統軟體 | tags: UNIX, GCC, Linux
- [程式語言](程式語言.md) - 人類與電腦溝通的媒介 | tags: 編譯器, 直譯器, 型別系統
- [編譯器](編譯器.md) - 將高階語言翻譯為機器碼的程式 | tags: 編譯器理論, 中間表示
- [作業系統](作業系統.md) - 管理硬體資源並提供服務的系統軟體 | tags: 核心, 行程, 記憶體
- [虛擬機](虛擬機.md) - 軟體模擬的硬體執行環境 | tags: 虛擬化, hypervisor
- [網路](網路.md) - 電腦之間的資料交換技術 | tags: TCP/IP, 協定
- [雲端技術](雲端技術.md) - 透過網路提供運算資源的技術 | tags: IaaS, PaaS, SaaS
- [形式文法](形式文法.md) - 描述語言語法的數學工具 | tags: Chomsky階層, 產生式
- [有限狀態機](有限狀態機.md) - 具有有限狀態的抽象機器 | tags: DFA, NFA, 正規語言
- [直譯器](直譯器.md) - 程式執行與語言實現 | tags: bytecode, eval
- [Lambda演算](Lambda演算.md) - 函數式計算理論 | tags: Church編碼, Y_Combinator
- [圖靈機](圖靈機.md) - 通用計算模型 | tags: 停機問題, Church-Turing論點
- [函數式編程](函數式編程.md) - 以函數為主的程式設計範式 | tags: 高階函數, 不可變性
- [Noam_Chomsky](Noam_Chomsky.md) - 語言學家與認知科學家 | tags: 轉換生成語法
- [LISP](LISP.md) - 最早的函數式程式語言 | tags: Scheme, Clojure
- [Prolog](Prolog.md) - 邏輯程式語言 | tags: 知識表示, 專家系統
- [並行計算與異構平台](並行計算與異構平台.md) - 多執行緒、GPU、FPGA | tags: CUDA, OpenCL
- [AI的加速技術](AI的加速技術.md) - TPU、TensorRT、模型量化 | tags: 深度學習, 硬體加速
- [管線處理與編譯器](管線處理與編譯器.md) - 指令管線、LLVM 優化 | tags: pipeline, 超純量

## 主題

- [計算理論](計算理論.md) - 可計算性與複雜度理論 | tags: P vs NP, NPC
- [RISC-V](RISC-V.md) - 開放指令集架構 | tags: 精簡指令, 開源
- [LLVM](LLVM.md) - 編譯器框架與工具鏈 | tags: Clang, IR
- [Linux系統呼叫](Linux系統呼叫.md) - 使用者與核心溝通的介面 | tags: 系統呼叫表
- [行程與執行緒](行程與執行緒.md) - 程式執行的基本單位 | tags: 排程, 上下文切換
- [記憶體管理](記憶體管理.md) - 記憶體配置與回收 | tags: GC, 分頁
- [垃圾搜集](垃圾搜集.md) - 自動記憶體管理機制 | tags: GC, 標記清除, 分代
- [虛擬記憶體](虛擬記憶體.md) - 擴展位址空間的記憶體管理 | tags: 分頁, 頁表, TLB
- [檔案系統](檔案系統.md) - 資料的組織與存取 | tags: Ext4, inode
- [TCP-IP協定](TCP-IP協定.md) - 網路傳輸協定 | tags: 三次握 hand
- [HTTP](HTTP.md) - Web 傳輸協定 | tags: REST, API
- [C語言](C語言.md) - 系統程式設計的基礎語言 | tags:指標, 記憶體操作
- [排程](排程.md) - CPU 資源分配演算法 | tags: CFS, 即時排程
- [同步與通訊](同步與通訊.md) - 多行程/執行緒間的協調 | tags: mutex, semaphore
- [Socket網路介面](Socket網路介面.md) - 網路程式設計的核心 API | tags: TCP, UDP, epoll
- [密碼學與資訊安全](密碼學與資訊安全.md) - 資料保護與加密技術 | tags: AES, RSA, TLS
- [區塊鏈與比特幣](區塊鏈與比特幣.md) - 去中心化帳本技術 | tags: PoW, 智慧合約, DeFi
- [版本控制](版本控制.md) - 程式碼版本管理 | tags: Git, 分支策略
- [Unix](Unix.md) - 經典作業系統家族 | tags: POSIX, Shell
- [Unix_v6](Unix_v6.md) - 第一個廣泛分發的 Unix 版本 | tags: PDP-11, Bell_Labs

## 人物

- [Linus_Torvalds](Linus_Torvalds.md) - Linux 與 Git 發明者 | 日期: 2026-04-07
- [Ken_Thompson](Ken_Thompson.md) - Unix 共同發明者 | 日期: 2026-04-07
- [Dennis_Ritchie](Dennis_Ritchie.md) - C 語言與 Unix 發明者 | 日期: 2026-04-07
- [Fabrice_Bellard](Fabrice_Bellard.md) - QEMU 與 FFmpeg 發明者 | 日期: 2026-04-08

## 工具

- [QEMU](QEMU.md) - 開源模擬器與虛擬化平台
- [Docker](Docker.md) - 容器化平台
- [Kubernetes](Kubernetes.md) - 容器編排系統
- [Git](../軟體工程/Git.md) - 分散式版本控制
- [c4](c4.md) - 四函式 C 編譯器
- [xv6](xv6.md) - MIT 教學用 Unix 作業系統
- [Rust](Rust.md) - 安全並發的系統程式語言
- [zig語言](zig語言.md) - 現代系統程式語言 | tags: comptime, 零成本抽象

## 日誌

- 參見 [log.md](log.md)