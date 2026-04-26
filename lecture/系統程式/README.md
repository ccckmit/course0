# 系統程式

本課程介紹電腦系統的底層運作，包含編譯器、作業系統、網路等核心技術。

## 內容架構

### 1. 編譯器 (Compiler)
* 詞法分析與語法分析
    * [code/系統程式/compiler/](../code/系統程式/compiler/) - 編譯器實作

### 2. 直譯器 (Interpreter)
* 位元碼直譯器
    * [code/系統程式/interpreter/](../code/系統程式/interpreter/) - 直譯器實作

### 3. 虛擬機器與中介表示 (IR + VM)
* **IR (Intermediate Representation)**：中介表示設計
* **VM (Virtual Machine)**：虛擬機器實作
* RISC-V 指令集
* LLVM IR
* 組譯器 (Assembler) 與反組譯器 (Objdump)
    * [code/系統程式/irvm/](../code/系統程式/irvm/) - IRVM 實作
        * `rv0/` - RISC-V 指令集實作
        * `ll0/` - LLVM IR 相關
        * `objdump/` - 反組譯器
        * `py0/` - Python VM
        * `qd0/` - 快速原型 VM

### 4. 作業系統 (OS)
* 行程管理與排程
* 記憶體管理
* 檔案系統
    * [code/系統程式/os/](../code/系統程式/os/) - OS 專案 (xv6, xv7, mini-riscv-os2)

### 5. 網路 (Network)
* Socket 程式設計
* 協定實作
    * [code/系統程式/network/](../code/系統程式/network/) - 網路程式

### 6. 密碼學與安全 (Crypto)
* 基礎密碼學
* SSL/TLS
    * [code/系統程式/crpyto/](../code/系統程式/crpyto/) - 密碼學實作

### 7. 媒體處理 (Media)
* 影片編碼 (MP4, MPEG)
* 音訊處理
    * [code/系統程式/media/](../code/系統程式/media/) - 媒體處理

### 8. 圖形使用者介面 (GUI)
* 視窗程式設計
    * [code/系統程式/gui/](../code/系統程式/gui/) - GUI 實作

### 9. 文字資料處理 (Text Data)
* Git 實作
* 區塊鏈
* SQL 資料庫
* 正規表達式
* 壓縮演算法
    * [code/系統程式/textdata/](../code/系統程式/textdata/) - 文字資料處理

### 10. 工具 (Tool)
* 開發工具
    * [code/系統程式/tool/](../code/系統程式/tool/) - 工具實作

## 使用技術
- 主要：C 語言
- 輔助：Python, Rust

## 相關資源
- Wiki: [_wiki/系統程式/](../_wiki/) - 系統軟體理論補充