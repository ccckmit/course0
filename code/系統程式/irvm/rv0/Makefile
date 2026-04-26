SHELL := /bin/bash -x
# ==========================================
# 1. 定義變數與編譯器路徑
# ==========================================
# RISC-V 交叉編譯器設定
LLVM_BIN   = /opt/homebrew/opt/llvm/bin
RV_CC      = $(LLVM_BIN)/clang
RV_OBJDUMP = $(LLVM_BIN)/llvm-objdump
RV_CFLAGS  = --target=riscv64 -march=rv64g -mabi=lp64d -g

# 本機端 (Host) 工具的編譯器設定
HOST_CC    = clang
HOST_CFLAGS= -O2 -Wall

# ==========================================
# 2. 主要目標 (Targets)
# ==========================================

# 預設行為：編譯所有東西
all: fact.s fact.o objdump as vm

# 分別建置三個執行檔的快捷指令
objdump: rv0objdump
as: rv0as
vm: rv0vm

# ==========================================
# 3. 建置規則 (Build Rules)
# ==========================================

# 將 fact.c 編譯為組合語言 fact.s
fact.s: fact.c
	$(RV_CC) $(RV_CFLAGS) -S fact.c -o fact.s

# 將 fact.c 編譯為目的檔 fact.o
fact.o: fact.c
	$(RV_CC) $(RV_CFLAGS) -c fact.c -o fact.o

# 編譯 rv0objdump 工具
rv0objdump: rv0objdump.c
	$(HOST_CC) $(HOST_CFLAGS) rv0objdump.c -o rv0objdump

# 編譯 rv0as 工具
rv0as: rv0as.c
	$(HOST_CC) $(HOST_CFLAGS) rv0as.c -o rv0as

# 編譯 rv0vm 工具
rv0vm: rv0vm.c
	$(HOST_CC) $(HOST_CFLAGS) rv0vm.c -o rv0vm

# ==========================================
# 4. 測試執行指令 (Run Targets)
# ==========================================

# 執行 llvm 原生 objdump
run-dump: rv0objdump fact.o
	$(RV_OBJDUMP) -h -d fact.o
	./rv0objdump -h -d fact.o

# 執行自製的 rv0as 並驗證
run-as: rv0as fact.s rv0objdump
	$(RV_CC) $(RV_CFLAGS) -c fact.s -o fact.o
	$(RV_OBJDUMP) -h -d fact.o
	./rv0as fact.s -o fact2.o
	./rv0objdump -h -d fact2.o
	./rv0vm fact2.o

# 執行自製的虛擬機
run-vm: rv0vm fact.o
	set -x
	./rv0vm fact.o

# 一鍵執行你原本 shell script 的所有測試流程
test: fact.o fact.s run-llvm-objdump run-objdump run-as run-vm

# ==========================================
# 5. 清理檔案 (Clean)
# ==========================================
clean:
	rm -f fact.s fact.o fact2.o rv0objdump rv0as rv0vm