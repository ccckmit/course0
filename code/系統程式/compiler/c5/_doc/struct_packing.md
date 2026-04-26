# Struct Packing 與二進位檔案寫入原理

在開發編譯器後端或是像 C5 這樣需要輸出二進位目標檔（如 ELF 檔）的程式時，一個常見且核心的問題是：**「為什麼不能直接把裝好資料的 `struct` 透過一次 `write()` 丟進檔案裡？」**

這份文件將解釋其背後的硬體與編譯器設計原理，以及標準 C 語言 (GCC) 與我們 C5 編譯器的處理作法差異。

---

## 1. 什麼是 Struct Packing (結構體對齊) 與 Padding (填充)？

CPU 為了存取記憶體的最大化效率，通常會要求變數放置在特定記憶體位址的倍數上（這被稱為 **Data Alignment**）。例如：4 bytes 的 `int` 最好放在 4 的倍數位址上，8 bytes 的 `long` 必須落在 8 的倍數。

當你在 `struct` 內混搭了大小不同的型別，為了服膺這種硬體對齊規範，編譯器（包含 GCC 以及 C5）會自動在變數成員之間「偷塞空白的記憶體位元組」，這被稱為 **Padding**。

### 範例：隱形的 Padding
如果我們定義了一個簡化的 ELF Header 結構：
```c
struct Elf_Header {
    char magic[4]; // 佔據 4 bytes
    // --- 【隱形陷阱】編譯器會在這裡自動安插 4 bytes 的 Padding 空白 ---
    // --- 為了讓下面的 entry 能夠落在 8 bytes 的整數倍位址上 ---
    long entry;    // 佔據 8 bytes
};
```
如果你把這個 `struct Elf_Header` 直接傳給 `write()` 函數寫入硬碟，**那些毫無意義的 4 bytes Padding 也會一併被寫進檔案裡！**

二進位檔案格式（如 ELF）的規格對每一個 Byte 在什麼位址都有極死板的嚴格規定。那些非預期的 Padding 會直接導致最終輸出的檔案格式偏移，使得不論是 OS 作業系統還是 Linker，在讀取這個檔案時都會發生「格式毀損」甚至當機。這就是所謂的 **Struct Packing 議題**。

---

## 2. C5 (與 C4) 的解法：手動位移控制

在 C5 或其前身 C4 的微型編譯器中，受限於語法的精簡，我們沒有實作更高階的編譯器對齊控制語法。任何一個宣告出來的 struct，都有潛在且不可控的 Padding 問題。

因此，如果把 struct 直接送給 `write` 寫入 ELF，檔案絕對會壞掉。

為了避開這個地雷，我們在程式碼中選擇了**最安全但比較手工的作法**：**寫死絕對位移 (Absolute offset) 與大小控制。**
透過我們自訂的輔助函數：
* `w16(buf, offset, value)`
* `w32(buf, offset, value)`
* `w64(buf, offset, value)`

我們是操作一塊巨大的連續 `char` 陣列（記憶體 Buffer），精準計算並算準了每一個變數應該要在檔案格式的第幾個 Byte，然後一個坑一個坑地填入資料。這種作法徹底繞過了 C 語法中 struct 造成的 Padding 變數！

---

## 3. 標準 GCC 的解法：打包屬性 (Packed Attribute)

那如果我們是用正規的 GCC 開發這套工具鏈，難道也只能痛苦地手算位移嗎？

答案是：**不用！GCC 可以直接把整個 struct 寫入檔案。**

為了解決系統層級的設計師（例如網路封包設計、或檔案格式解析者）需要「所見即所得」的記憶體排列，GCC 以及許多主流編譯器提供了一個強大的外掛語法：`__attribute__((packed))`。

如果你在結構體宣告時加上這個魔法標記，就是在對編譯器下達強制令：**「不准塞任何 Padding 空白，無視 CPU 對齊效率，給我強制擠緊排列在一起！」**

```c
struct Elf64_Sym {
  long st_name;
  char st_info;
  char st_other;
  short st_shndx;
  long st_value;
  long st_size;
} __attribute__((packed)); // <--- 關鍵：強制緊密排列！
```

當你使用了這種宣告後，這個 `struct` 在記憶體裡面的二進位排列，就等於是這份 ELF 的官方規格長相。在這種前提下，我們就可以非常優雅且安全地撰寫：

```c
struct Elf64_Sym sym;
/* ... 填寫 sym 內的內容 ... */

// GCC 可以一行搞定，毫不猶豫地安全寫入：
write(fd, &sym, sizeof(struct Elf64_Sym));
```

## 結語

* **如果開發環境沒有強大控制（如 C5）**：用底層陣列精算位移寫出（如 `w16`/`w64`）是最萬無一失的方案。
* **如果開發環境是現代編譯器（如 GCC）**：善用 `__attribute__((packed))`，可以讓後端輸出邏輯變得極致優雅與易讀。
