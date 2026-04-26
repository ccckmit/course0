# RISC-V

RISC-V 是一個開放的、精簡的指令集架構（Instruction Set Architecture，ISA），由加州大學伯克利分校於 2010 年設計。RISC-V 的設計理念是簡潔、模組化和可擴展，這使其成為學術研究、工業應用和開源硬體的理想選擇。與傳統的封閉式指令集（如 x86、ARM）不同，RISC-V 允許任何人免費使用、修改和商業化，這種開放性正在徹底改變處理器設計的生態系統。

## RISC-V 的發展歷程

```
┌─────────────────────────────────────────────────────────────┐
│                    RISC-V 發展時間線                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   2010      設計開始                                        │
│             UC Berkeley, Krste Asanović                     │
│                                                             │
│   2014      RISC-V v2.0 發布                               │
│             基礎指令集標準化                                │
│                                                             │
│   2015      RISC-V 基金會成立                               │
│             50+ 會員，包括 Western Digital, Google         │
│                                                             │
│   2017      Linux 支援 RISC-V                              │
│             開始進入主流系統                                │
│                                                             │
│   2019      RISC-V Foundation 更名為 RISC-V International    │
│             300+ 會員                                      │
│                                                             │
│   2021      高效能核心發布                                  │
│             SiFive U74, Alibaba X910                       │
│                                                             │
│   2024      商業化加速                                      │
│             AI 加速器、資料中心、嵌入式                     │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## RISC-V 架構特點

### 開放性

```
┌─────────────────────────────────────────────────────────────┐
│                  開放 vs 封閉指令集                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   封閉指令集 (x86, ARM)：                                    │
│   - 需要授權才能使用                                        │
│   - 專利費用昂貴                                            │
│   - 供應商鎖定                                              │
│   - 改動受限                                                │
│                                                             │
│   RISC-V：                                                  │
│   - 免費使用，無授權費用                                    │
│   - 開放標準                                                │
│   - 可自由改動和擴展                                        │
│   - 無供應商鎖定                                            │
│   - 社區驅動創新                                             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 模組化設計

```python
# RISC-V 擴展模組
risc_v_extensions = {
    "基本擴展 (must have)": {
        "I": "整數基礎指令 (Integer)",
        "E": "嵌入式中使用的整數指令 (16 個暫存器)",
        "G": "基本 + 整數乘除 + 單精度浮點 + 雙精度浮點"
    },
    
    "標準擴展": {
        "M": "整數乘除指令 (mul, div, rem)",
        "A": "原子操作指令 (amo, lr, sc)",
        "F": "單精度浮點 (float)",
        "D": "雙精度浮點 (double)",
        "C": "壓縮指令 (16-bit)",
        "V": "向量指令 (vector)",
        "B": "位元操作指令",
        "P": "packed SIMD 指令",
        "H": "超管理器指令 (hypervisor)"
    },
    
    "命名方式": {
        "RV32I": "32-bit 整數",
        "RV64I": "64-bit 整數",
        "RV32GC": "32-bit + G + C = RV32IMAFDC",
        "RV64GC": "64-bit + G + C = RV64IMAFDC"
    }
}
```

## 暫存器架構

### RV64GC 暫存器

```python
# RISC-V 暫存器表格
risc_v_registers = {
    "通用暫存器 (32個)": {
        "x0 (zero)": "永遠為 0",
        "x1 (ra)": "返回位址 (return address)",
        "x2 (sp)": "堆疊指標 (stack pointer)",
        "x3 (gp)": "全域指標 (global pointer)",
        "x4 (tp)": "執行緒指標 (thread pointer)",
        "x5-x7 (t0-t2)": "暫存暫存器 (temporary)",
        "x8 (s0/fp)": "儲存暫存器0 / 框架指標",
        "x9 (s1)": "儲存暫存器1",
        "x10-x11 (a0-a1)": "函式參數0-1 / 返回值",
        "x12-x17 (a2-a7)": "函式參數2-7",
        "x18-x27 (s2-s11)": "儲存暫存器2-11",
        "x28-x31 (t3-t6)": "暫存暫存器3-6"
    },
    
    "浮點暫存器 (32個, 如果有 F/D 擴展)": {
        "ft0-ft7": "浮點暫存暫存器",
        "fs0-fs1": "浮點儲存暫存器",
        "fa0-fa1": "浮點函式參數/返回值",
        "fa2-fa7": "浮點函式參數",
        "fs2-fs11": "浮點儲存暫存器",
        "ft8-ft11": "浮點暫存暫存器"
    },
    
    "特殊暫存器": {
        "pc": "程式計數器 (program counter)",
        "mstatus": "Machine 模式狀態",
        "mepc": "Machine 異常 PC",
        "mtvec": "Machine 陷阱向量",
        "mie": "Machine 中斷使能"
    }
}
```

## 指令格式與類別

### 指令格式

```
RISC-V 指令格式 (6種)：

R-type (register):
  31        25  24  20  19  15  14  12  11  7  6   0
  ┌──────────┬─────────┬───────┬────────┬────────┐
  │ funct7   │  rs2    │  rs1  │ funct3 │  rd   │ opcode│
  └──────────┴─────────┴───────┴────────┴────────┘
  例如: add a0, a1, a2

I-type (immediate):
  31        25  24  20  19  15  14  12  11  7  6   0
  ┌──────────┬─────────┬───────┬────────┬────────┐
  │  imm[11:0]│  rs1   │ funct3│   rd   │ opcode│
  └──────────┴─────────┴───────┴────────┴────────┘
  例如: lw a0, 0(sp)

S-type (store):
  31        25  24  20  19  15  14  12  11  7  6   0
  ┌──────────┬─────────┬───────┬────────┬────────┐
  │imm[11:5]│  rs2   │  rs1  │ funct3 │imm[4:0]│ opcode│
  └──────────┴─────────┴───────┴────────┴────────┘
  例如: sw a0, 0(sp)

B-type (branch):
  31        25  24  20  19  15  14  12  11  7  6   0
  ┌──────────┬─────────┬───────┬────────┬────────┐
  │imm[12|10:5]│ rs2  │  rs1  │ funct3 │imm[4:1|11]│ opcode│
  └──────────┴─────────┴───────┴────────┴────────┘
  例如: beq a0, a1, label

U-type (upper immediate):
  31                       12  11  7  6   0
  ┌─────────────────────────────┬────────┬────────┐
  │         immediate          │   rd   │ opcode│
  └─────────────────────────────┴────────┴────────┘
  例如: lui a0, 0x12345

J-type (jump):
  31                       12  11  7  6   0
  ┌─────────────────────────────┬────────┬────────┐
  │         immediate          │   rd   │ opcode│
  └─────────────────────────────┴────────┴────────┘
  例如: jal a0, label
```

### 指令類別

```python
# RISC-V 指令集
risc_v_instructions = {
    "整數運算指令": {
        "加法": ["add", "addi", "addw", "addiw"],
        "減法": ["sub", "subw"],
        "邏輯": ["and", "andi", "or", "ori", "xor", "xori"],
        "位移": ["sll", "slli", "srl", "srli", "sra", "srai"],
        "比較": ["slt", "slti", "sltu", "sltiu"],
        "乘除": ["mul", "mulh", "div", "divu", "rem", "remu"]
    },
    
    "載入/儲存指令": {
        "載入": ["lb", "lbu", "lh", "lhu", "lw", "lwu", "ld"],
        "儲存": ["sb", "sh", "sw", "sd"],
        "浮點載入/儲存": ["flw", "fsw", "fld", "fsd"]
    },
    
    "分支指令": {
        "條件分支": ["beq", "bne", "blt", "bge", "bltu", "bgeu"],
        "無條件跳躍": ["jal", "jalr"]
    },
    
    "控制轉移": {
        "跳躍": ["jal", "jalr"],
        "回歸": ["ret (jalr x0, 0(x1))"]
    },
    
    "系統指令": {
        "環境呼叫": ["ecall", "ebreak"],
        "CSR 存取": ["csrr", "csrw", "csrs", "csrc"]
    },
    
    "浮點指令 (F/D 擴展)": {
        "算術": ["fadd.s", "fsub.s", "fmul.s", "fdiv.s", "fsqrt.s"],
        "轉換": ["fcvt.s.w", "fcvt.w.s", "fcvt.s.d", "fcvt.d.s"],
        "比較": ["feq.s", "flt.s", "fle.s"],
        "載入/儲存": ["flw", "fsw"]
    }
}
```

## 組譯範例

```assembly
# RISC-V 64-bit 組譯範例

# 簡單的函式：計算陣列總和
# a0 = 陣列指標, a1 = 元素數量, 返回 a0 = 總和

    .text
    .globl sum_array

sum_array:
    # 初始化
    li a2, 0          # a2 = sum = 0
    li a3, 0          # a3 = i = 0
    
loop:
    # 檢查是否完成
    bge a3, a1, end   # if i >= n, goto end
    
    # 取得 arr[i]
    slli a4, a3, 3    # a4 = i * 8 (64-bit)
    add a4, a0, a4    # a4 = &arr[i]
    ld a5, 0(a4)      # a5 = arr[i]
    
    # sum += arr[i]
    add a2, a2, a5    # sum += arr[i]
    
    # i++
    addi a3, a3, 1
    
    # 繼續迴圈
    j loop

end:
    # 返回 sum
    mv a0, a2         # a0 = sum
    ret

# C 函式呼叫
# long result = sum_array(array, 10);

    # 設定參數
    la a0, array      # a0 = array指標
    li a1, 10         # a1 = 10
    # 呼叫
    call sum_array    # 呼叫函式
    # result 在 a0 中

# 標準 C 函式庫呼叫
# printf("Hello, RISC-V!\n");

    la a0, fmt_str    # a0 = 字串指標
    call printf
```

## RISC-V 特權架構

```python
# RISC-V 特權等級
privilege_modes = {
    "U (User/Application)": {
        "等級": 0,
        "用途": "應用程式執行",
        "可訪問": "一般暫存器, 使用者級 CSR"
    },
    
    "S (Supervisor)": {
        "等級": 1,
        "用途": "作業系統核心",
        "可訪問": "Supervisor 級 CSR, trap handling"
    },
    
    "M (Machine)": {
        "等級": 3,
        "用途": " firmware, bootloader, 硬體",
        "可訪問": "所有 CSR, 完全控制"
    }
}

# 特權 CSR
privilege_csrs = {
    "Machine Mode": {
        "mstatus": "Machine 狀態",
        "mie": "Machine 中斷使能",
        "mip": "Machine 中斷待處理",
        "mepc": "Machine 例外 PC",
        "mcause": "例外原因",
        "mtvec": "Machine 陷阱向量",
        "mscratch": "Machine Scratch"
    },
    
    "Supervisor Mode": {
        "sstatus": "Supervisor 狀態",
        "sie": "Supervisor 中斷使能",
        "sip": "Supervisor 中斷待處理",
        "sepc": "Supervisor 例外 PC",
        "scause": "Supervisor 例外原因",
        "stvec": "Supervisor 陷阱向量",
        "satp": "Supervisor 地址翻譯"
    }
}
```

## 組譯器工具

```bash
# RISC-V 工具鏈安裝
# Ubuntu/Debian
sudo apt install gcc-riscv64-linux-gnu
# 或使用 crossbuild

# 組譯
riscv64-unknown-elf-as -o test.o test.s

# 連結
riscv64-unknown-elf-ld -o test test.o

# 使用 GCC 編譯 C
riscv64-unknown-elf-gcc -o test test.c

# 反組譯
riscv64-unknown-elf-objdump -d test

# 使用 Spike 模擬器
spike pk test
```

## RISC-V 生態系統

```python
# RISC-V 軟體生態
ecosystem = {
    "模擬器": {
        "Spike": "RISC-V ISA 模擬器",
        "QEMU": "完整系統模擬",
        "OVPsim": "快速模擬器"
    },
    
    "作業系統": {
        "Linux": "完整支援 RISC-V",
        "FreeRTOS": "嵌入式即時作業系統",
        "Zephyr": "物聯網 OS"
    },
    
    "編譯器": {
        "GCC": "riscv64-unknown-elf-gcc",
        "LLVM": "已支援 RISC-V",
        "Rust": "已支援 RISC-V"
    },
    
    "處理器核心": {
        "Rocket": " Berkeley 開源核心",
        "BOOM": "超純量開源核心",
        "SiFive U74": "商業高效能核心",
        "阿里 X910": "雲端伺服器核心"
    },
    
    "開發板": {
        "SiFive HiFive": "開發板",
        "Kendryte K210": "AI 視覺晶片",
        "StarFive VisionFive": "單板電腦"
    }
}
```

## 實際應用場景

```python
# RISC-V 應用領域
applications = {
    "嵌入式系統": {
        "特點": "低功耗、成本敏感",
        "產品": "微控制器、IoT 設備"
    },
    
    "資料中心": {
        "特點": "高效能、雲端運算",
        "產品": "伺服器 CPU、AI 加速器"
    },
    
    "AI/ML": {
        "特點": "向量擴展、硬體加速",
        "產品": "神經網路加速器、GPU"
    },
    
    "安全晶片": {
        "特點": "可自訂、開放審計",
        "產品": "安全處理器、TPM"
    },
    
    "學術研究": {
        "特點": "靈活、可擴展",
        "產品": "研究晶片、實驗架構"
    }
}
```

## 與其他 ISA 的比較

```python
# ISA 比較
comparison = {
    "x86_64": {
        "類型": "CISC (複雜指令集)",
        "優勢": "生態成熟、相容性高",
        "劣勢": "專利封閉、功耗較高"
    },
    
    "ARM": {
        "類型": "RISC",
        "優勢": "行動市場佔有率高、授權靈活",
        "劣勢": "授權費用、不如 RISC-V 開放"
    },
    
    "RISC-V": {
        "類型": "RISC (開放)",
        "優勢": "完全開放、模組化、無授權費",
        "劣勢": "生態較新、軟體支援待完善"
    }
}
```

## 相關主題

- [組合語言](組合語言.md) - 低階程式設計
- [編譯器](編譯器.md) - RISC-V 編譯器後端
- [LLVM](LLVM.md) - RISC-V 編譯器支援
- [C語言](C語言.md) - RISC-V C 程式設計