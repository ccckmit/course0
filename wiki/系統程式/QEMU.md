# QEMU

QEMU（Quick Emulator）是一個功能強大的開源虛擬化與模擬解決方案，可模擬多種 CPU 架構和系統平台。它支援使用者模式模擬和完整系統模擬兩種模式，是開發、測試和執行跨平台軟體的重要工具。

## QEMU 核心功能

### 使用者模式模擬

使用者模式模擬讓您可以在本機系統上執行其他架構的單一程式：

```
功能特點：
- 無需完整作業系統
- 跨架構執行使用者程式
- 自動轉換系統呼叫
- 動態二進位翻譯

支援架構：
- x86 → ARM, RISC-V, MIPS
- ARM → x86, RISC-V
- RISC-V → x86, ARM
```

```bash
# 執行 RISC-V 執行檔
qemu-riscv64 -L /path/to/riscv64/sysroot ./hello

# 執行 ARM 執行檔
qemu-arm -L /path/to/arm/sysroot ./hello

# 執行 x86_64 程式（在 ARM 主機上）
qemu-x86_64 ./program

# 顯示幫助
qemu-riscv64 --help
```

### 系統模式模擬

系統模式模擬完整電腦硬體，包括 CPU、記憶體、週邊設備等：

```
模擬的硬體元件：
- CPU 處理器（多種架構）
- 記憶體管理單元（MMU）
- 顯示卡（VGA, virtio）
- 網路卡（virtio, e1000）
- 儲存控制器（IDE, SCSI, virtio）
- USB 控制器
- 音效卡（AC97, HDA）
- 序列埠、並列埠
```

```bash
# 模擬 RISC-V 完整系統
qemu-system-riscv64 \
    -machine virt \
    -m 2G \
    -kernel vmlinuz \
    -initrd initrd.img \
    -append "root=/dev/vda" \
    -drive file=rootfs.img,format=raw

# 模擬 ARM 開發板
qemu-system-arm \
    -machine vexpress-a9 \
    -kernel zImage \
    -dtb vexpress-v2p-ca9.dtb \
    -sd sdcard.img \
    -append "root=/dev/mmcblk0" \
    -nographic

# 模擬 x86_64 PC
qemu-system-x86_64 \
    -m 4G \
    -hda disk.img \
    -cdrom os.iso \
    -boot d

# 模擬 MIPS 系統
qemu-system-mips \
    -machine malta \
    -kernel vmlinux \
    -initrd rootfs.img
```

## 進階使用

### KVM 加速

使用硬體虛擬化加速，大幅提升效能：

```bash
# 檢查 KVM 是否可用
ls /dev/kvm

# 使用 KVM 加速（需要主客端架構相同）
qemu-system-x86_64 \
    -enable-kvm \
    -m 4G \
    -hda disk.img

# ARM KVM
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a57 \
    -enable-kvm \
    -m 2G \
    -kernel Image

# RISC-V KVM
qemu-system-riscv64 \
    -machine virt \
    -enable-kvm \
    -m 4G \
    -kernel Image
```

### 網路設定

```bash
# 使用使用者模式網路（NAT）
qemu-system-x86_64 -net user,hostfwd=tcp::2222-:22 ...

# 橋接模式（需要 root）
qemu-system-x86_64 -net bridge,br=br0 ...

# TAP 網路
qemu-system-x86_64 -net tap,ifname=tap0,script=no ...
```

### 儲存設備

```bash
# 使用 QCOW2 格式（精簡配置）
qemu-img create -f qcow2 disk.img 40G

# 使用原始映像檔
qemu-img create -f raw disk.raw 10G

# 轉換格式
qemu-img convert -f raw -O qcow2 input.img output.img

# 附加儲存
qemu-system-x86_64 \
    -hda main.img \
    -hdb second.img \
    -cdrom installer.iso
```

### 顯示設定

```bash
# 無圖形介面（序列控制台）
qemu-system-x86_64 -nographic ...

# 使用 SDL 顯示
qemu-system-x86_64 -display sdl ...

# 使用 GTK 顯示
qemu-system-x86_64 -display gtk ...

# 無顯示（僅執行）
qemu-system-x86_64 -display none ...

# VNC 遠端顯示
qemu-system-x86_64 -vnc :0 ...
```

### VirtIO 設備

VirtIO 是準虛擬化驅動程式，提供高效能：

```bash
# VirtIO 區塊設備
qemu-system-x86_64 \
    -drive file=disk.img,if=virtio

# VirtIO 網路
qemu-system-x86_64 \
    -netdev tap,id=net0,script=no \
    -device virtio-net-pci,netdev=net0

# VirtIO 顯示卡
qemu-system-x86_64 \
    -vga virtio

# VirtIO 序列控制台
qemu-system-x86_64 \
    -serial mon:stdio \
    -append "console=ttyS0"
```

## QEMU 內部架構

### 二進位翻譯

QEMU 使用動態二進位翻譯技術：

```
翻譯流程：
┌─────────────────────────────────────────────────────────────┐
│                      QEMU 翻譯流程                         │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 讀取 Guest 指令                                        │
│  2. 解析指令結構                                           │
│  3. 查找翻譯區塊快取                                       │
│  4. 若不存在，生成 Host 代碼                               │
│  5. 執行翻譯後的 Host 代碼                                │
│  6. 更新執行統計                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘

優化技術：
- 區塊鏈接（Block Chaining）
- 暫存器配對（Register Pairing）
- 常用指令模式優化
- 直接區塊跳轉優化
```

### TCG 編譯器

Tiny Code Generator (TCG) 是 QEMU 的核心：

```c
// TCG 中間表示範例
// Guest: add r0, r1, r2
// TCG IR:
//   tcg_gen_add_i32(cpu_r0, cpu_r1, cpu_r2)

// 翻譯過程：
// 1. 前端解析 Guest 指令
// 2. 轉換為 TCG IR
// 3. 優化 TCG IR
// 4. 後端生成 Host 指令
```

### 記憶體管理

```c
// QEMU 記憶體虛擬化
// - 軟體 MMU（使用者模式）
// - 硬體 MMU（系統模式 + KVM）
// - 客戶系統虛擬位址 (GVA)
// - 客戶物理位址 (GPA)
// - 主機虛擬位址 (HVA)
// - 主機物理位址 (HPA)
```

## 常用選項參考

### 基本選項

| 選項 | 說明 | 範例 |
|------|------|------|
| -machine | 機器類型 | virt, pc, vexpress-a9 |
| -cpu | CPU 型號 | cortex-a57, host |
| -m | 記憶體大小 | 2G, 4G |
| -smp | CPU 核心數 | 4 |
| -kernel | 核心映像檔 | vmlinuz |
| -initrd | 初始記憶體碟 | initrd.img |
| -append | 核心參數 | "console=ttyS0" |

### 顯示選項

| 選項 | 說明 | 範例 |
|------|------|------|
| -display | 顯示類型 | sdl, gtk, none |
| -vga | 顯示卡 | std, virtio, qxl |
| -nographic | 無圖形 | - |
| -vnc | VNC 伺服器 | :0 |

### 網路選項

| 選項 | 說明 | 範例 |
|------|------|------|
| -net | 網路設定 | user, tap, bridge |
| -netdev | 網路後端 | type=tap,id=net0 |

### 儲存選項

| 選項 | 說明 | 範例 |
|------|------|------|
| -hd* | 硬碟 | -hda disk.img |
| -cdrom | 光碟 | -cdrom os.iso |
| -drive | 驅動器 | file=img,format=qcow2 |

## 實際應用場景

### 嵌入式開發

```bash
# 開發 RISC-V 嵌入式系統
qemu-system-riscv64 \
    -machine virt \
    -m 256M \
    -kernel u-boot \
    -nographic

# 調試核心
qemu-system-riscv64 \
    -machine virt \
    -s -S \
    -kernel vmlinux
# 使用 gdb 連接 localhost:1234
```

### 作業系統教學

```bash
# 執行 MiniOS
qemu-system-i386 \
    -kernel vmlinuz \
    -initrd initrd.gz \
    -append "init=/bin/sh" \
    -nographic

# 測試 Linux 核心
qemu-system-x86_64 \
    -kernel arch/x86/boot/bzImage \
    -drive file=rootfs.img \
    -append "root=/dev/vda"
```

### 軟體測試

```bash
# 測試跨平台軟體
qemu-arm -L /usr/arm-linux-gnueabihf ./program

# 自動化測試
qemu-system-x86_64 \
    -m 1G \
    -snapshot \
    -no-shutdown \
    -pidfile qemu.pid
```

## 效能優化

### 建議配置

```bash
# 高效能配置
qemu-system-x86_64 \
    -enable-kvm \              # 使用 KVM
    -cpu host \                # 使用主機 CPU
    -smp cores=4 \             # 4 核心
    -m 8G \                    # 8GB 記憶體
    -drive file=disk.qcow2,if=virtio \  # VirtIO 儲存
    -netdev tap,id=net0,script=no \      # VirtIO 網路
    -device virtio-net-pci,netdev=net0 \
    -vga virtio                # VirtIO 顯示
```

### 監控效能

```bash
# 顯示效能統計
qemu-system-x86_64 --monitor stdio

# 使用 HMP 命令
info status
info cpus
info mem
info block
info network
```

## 相關概念

- [虛擬機](虛擬機.md) - 虛擬化技術概述
- [RISC-V](RISC-V.md) - RISC-V 架構
- [Docker](Docker.md) - 容器化平台
- [Kubernetes](Kubernetes.md) - 容器編排