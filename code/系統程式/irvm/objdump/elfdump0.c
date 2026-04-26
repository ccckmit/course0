#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <elf.h>

// --- 16 進位印出函數 (Hex Dump) ---
void hex_dump(const uint8_t *data, size_t size, uint64_t base_addr) {
    for (size_t i = 0; i < size; i += 16) {
        // 印出記憶體位址 (強制轉型並用 %llx 解決 Mac 跨平台警告)
        printf("  %04llx: ", (unsigned long long)(base_addr + i));

        // 印出 16 進位數值
        for (size_t j = 0; j < 16; j++) {
            if (j == 8) printf(" "); // 中間加個空格提升可讀性
            if (i + j < size)
                printf("%02x ", data[i + j]);
            else
                printf("   ");
        }
        printf("  ");

        // 印出可讀的 ASCII 字元
        for (size_t j = 0; j < 16; j++) {
            if (i + j < size) {
                unsigned char c = data[i + j];
                // 過濾不可見字元，轉成 '.'
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "用法: %s <elf_binary>\n", argv[0]);
        return 1;
    }

    // 1. 開啟檔案並取得大小
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("無法開啟檔案");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("無法取得檔案資訊");
        close(fd);
        return 1;
    }

    // 2. 將整個檔案映射 (Map) 到記憶體中
    uint8_t *map_start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("mmap 失敗");
        close(fd);
        return 1;
    }
    close(fd);

    // 3. 解析 ELF Header
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)map_start;

    // 檢查 Magic Number: 0x7f 'E' 'L' 'F'
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "錯誤: 此檔案不是標準的 ELF 格式。\n");
        munmap(map_start, st.st_size);
        return 1;
    }

    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64) {
        fprintf(stderr, "提示: 為了保持程式簡易，目前僅支援 64-bit ELF 解析。\n");
        munmap(map_start, st.st_size);
        return 1;
    }

    printf("\n=== ELF 檔案標頭 (ELF Header) ===\n");
    // 改用 %llx 與 %llu，並加上強制轉型
    printf("  進入點位址 (Entry point): 0x%llx\n", (unsigned long long)ehdr->e_entry);
    printf("  區段標頭偏移量 (Start of section headers): %llu (bytes)\n", (unsigned long long)ehdr->e_shoff);
    printf("  區段數量 (Number of section headers): %d\n", ehdr->e_shnum);

    // 4. 定位 Section Headers 與字串表 (String Table)
    Elf64_Shdr *shdrs = (Elf64_Shdr *)(map_start + ehdr->e_shoff);
    Elf64_Shdr *shstrtab_hdr = &shdrs[ehdr->e_shstrndx];
    char *shstrtab = (char *)(map_start + shstrtab_hdr->sh_offset);

    printf("\n=== 區段標頭 (Section Headers) ===\n");
    printf("  [Nr] %-18s %-16s %-8s %-8s\n", "Name", "Address", "Offset", "Size");
    for (int i = 0; i < ehdr->e_shnum; i++) {
        char *name = shstrtab + shdrs[i].sh_name;
        if (strlen(name) == 0) continue; 

        // 改用 %llx，並加上強制轉型
        printf("  [%2d] %-18s %016llx %08llx %08llx\n", 
               i, name, 
               (unsigned long long)shdrs[i].sh_addr, 
               (unsigned long long)shdrs[i].sh_offset, 
               (unsigned long long)shdrs[i].sh_size);
    }

    printf("\n=== 區段內容 Hex Dump ===\n");
    for (int i = 0; i < ehdr->e_shnum; i++) {
        char *name = shstrtab + shdrs[i].sh_name;
        
        if (shdrs[i].sh_size == 0 || shdrs[i].sh_type == SHT_NOBITS || strlen(name) == 0) {
            continue;
        }

        printf("\nContents of section %s:\n", name);
        
        uint8_t *section_data = map_start + shdrs[i].sh_offset;
        uint64_t display_addr = (shdrs[i].sh_addr != 0) ? shdrs[i].sh_addr : shdrs[i].sh_offset;
        
        hex_dump(section_data, shdrs[i].sh_size, display_addr);
    }

    munmap(map_start, st.st_size);
    return 0;
}
