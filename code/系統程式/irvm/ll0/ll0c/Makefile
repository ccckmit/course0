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
all: fact.s fact.o ll0c

# ==========================================
# 3. 建置規則 (Build Rules)
# ==========================================

# 將 fact.c 編譯為組合語言 fact.s
fact.s: fact.c
	$(RV_CC) $(RV_CFLAGS) -S fact.c -o fact.s

# 將 fact.c 編譯為目的檔 fact.o
fact.o: fact.c
	$(RV_CC) $(RV_CFLAGS) -c fact.c -o fact.o

# 編譯 ll0c 工具
ll0c: ll0c.c
	$(HOST_CC) $(HOST_CFLAGS) ll0c.c ../ll0.c -o ll0c

# ==========================================
# 4. 測試執行指令 (Run Targets)
# ==========================================

# 執行 llvm 原生 objdump
run-ll0c: ll0c fact.o
	set -x
	$(RV_CC) $(RV_CFLAGS) -c fact.s -o fact.o
	$(RV_OBJDUMP) -h -d fact.o
	./ll0c fact.ll -o fact2.s
	$(RV_CC) $(RV_CFLAGS) -c fact2.s -o fact2.o
	$(RV_OBJDUMP) -h -d fact2.o

# 一鍵執行你原本 shell script 的所有測試流程
test: fact.o fact.s run-ll0c

# ==========================================
# 5. 清理檔案 (Clean)
# ==========================================
clean:
	rm -f fact.s fact.o fact2.o rv0objdump rv0as rv0vm