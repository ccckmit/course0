#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

// macOS 專屬的 Mach-O 標頭檔
#include <mach-o/loader.h>
#include <mach-o/fat.h>
#include <mach/machine.h> // <--- 修正這行的路徑

// --- 16 進位印出函數 (Hex Dump) ---
void hex_dump(const uint8_t *data, size_t size, uint64_t base_addr) {
    for (size_t i = 0; i < size; i += 16) {
        printf("  %04llx: ", (unsigned long long)(base_addr + i));

        for (size_t j = 0; j < 16; j++) {
            if (j == 8) printf(" ");
            if (i + j < size)
                printf("%02x ", data[i + j]);
            else
                printf("   ");
        }
        printf("  ");

        for (size_t j = 0; j < 16; j++) {
            if (i + j < size) {
                unsigned char c = data[i + j];
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "用法: %s <mach-o_binary>\n", argv[0]);
        return 1;
    }

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

    uint8_t *map_start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("mmap 失敗");
        close(fd);
        return 1;
    }
    close(fd);

    uint32_t magic = *(uint32_t *)map_start;

    if (magic == FAT_MAGIC || magic == FAT_CIGAM) {
        fprintf(stderr, "錯誤: 這是一個 Universal (Fat) Binary，包含多種 CPU 架構。\n");
        fprintf(stderr, "為保持程式簡易，本程式僅支援單一架構的 Mach-O 64-bit 檔案。\n");
        munmap(map_start, st.st_size);
        return 1;
    }

    if (magic != MH_MAGIC_64) {
        fprintf(stderr, "錯誤: 不是標準的 Mach-O 64-bit 檔案 (Little-Endian)。\n");
        munmap(map_start, st.st_size);
        return 1;
    }

    struct mach_header_64 *header = (struct mach_header_64 *)map_start;

    printf("\n=== Mach-O 檔案標頭 (Mach-O Header) ===\n");
    printf("  Magic Number: 0x%x\n", header->magic);
    printf("  CPU 類型 (cputype): 0x%x\n", header->cputype);
    printf("  載入指令數量 (ncmds): %d\n", header->ncmds);
    printf("  載入指令總大小 (sizeofcmds): %d (bytes)\n", header->sizeofcmds);

    printf("\n=== 區段標頭 (Section Headers) ===\n");
    printf("  %-16s %-16s %-16s %-8s %-8s\n", "Segment", "Section", "Address", "Offset", "Size");
    
    uint32_t offset = sizeof(struct mach_header_64);
    
    for (uint32_t i = 0; i < header->ncmds; i++) {
        struct load_command *lc = (struct load_command *)(map_start + offset);
        
        if (lc->cmd == LC_SEGMENT_64) {
            struct segment_command_64 *seg = (struct segment_command_64 *)lc;
            struct section_64 *sec = (struct section_64 *)((char *)seg + sizeof(struct segment_command_64));
            
            for (uint32_t j = 0; j < seg->nsects; j++) {
                printf("  %-16.16s %-16.16s %016llx %08llx %08llx\n",
                       sec[j].segname, sec[j].sectname,
                       (unsigned long long)sec[j].addr,
                       (unsigned long long)sec[j].offset,
                       (unsigned long long)sec[j].size);
            }
        }
        offset += lc->cmdsize;
    }

    printf("\n=== 區段內容 Hex Dump ===\n");
    offset = sizeof(struct mach_header_64); 
    
    for (uint32_t i = 0; i < header->ncmds; i++) {
        struct load_command *lc = (struct load_command *)(map_start + offset);
        
        if (lc->cmd == LC_SEGMENT_64) {
            struct segment_command_64 *seg = (struct segment_command_64 *)lc;
            struct section_64 *sec = (struct section_64 *)((char *)seg + sizeof(struct segment_command_64));
            
            for (uint32_t j = 0; j < seg->nsects; j++) {
                if (sec[j].size == 0 || (sec[j].flags & SECTION_TYPE) == S_ZEROFILL) {
                    continue;
                }

                printf("\nContents of (%.16s,%.16s) section:\n", sec[j].segname, sec[j].sectname);
                
                uint8_t *section_data = map_start + sec[j].offset;
                hex_dump(section_data, sec[j].size, sec[j].addr);
            }
        }
        offset += lc->cmdsize;
    }

    munmap(map_start, st.st_size);
    return 0;
}