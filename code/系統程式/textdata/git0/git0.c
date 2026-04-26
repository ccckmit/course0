#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define GIT0_DIR ".git0"
#define OBJECTS_DIR GIT0_DIR "/objects"
#define INDEX_FILE GIT0_DIR "/index"
#define COMMITS_FILE GIT0_DIR "/commits"
#define HASH_LEN 8
#define FNV1A_OFFSET 0x811C9DC5UL
#define FNV1A_PRIME 0x01000193UL

// --- 修復 1：在這裡先宣告函數原型 ---
char *strrstr(const char *haystack, const char *needle);

// 輔助函數：檢查文件是否存在
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// 輔助函數：檢查目錄是否存在
int dir_exists(const char *dirname) {
    struct stat buffer;
    if (stat(dirname, &buffer) != 0) return 0;
    return S_ISDIR(buffer.st_mode);
}

// 輔助函數：讀取文件全部內容
char *read_file(const char *filename, size_t *len) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    *len = ftell(fp);
    rewind(fp);
    char *data = malloc(*len + 1);
    if (!data) { fclose(fp); return NULL; }
    fread(data, 1, *len, fp);
    data[*len] = '\0';
    fclose(fp);
    return data;
}

// 輔助函數：寫入文件
int write_file(const char *filename, const char *data, size_t len) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return -1;
    fwrite(data, 1, len, fp);
    fclose(fp);
    return 0;
}

// FNV-1a 哈希函數
unsigned int fnv1a_hash(const char *data, size_t len) {
    unsigned int hash = FNV1A_OFFSET;
    for (size_t i = 0; i < len; i++) {
        hash ^= (unsigned char)data[i];
        hash *= FNV1A_PRIME;
    }
    return hash;
}

// 將哈希值轉為 8 位十六進制字符串
void hash_to_hex(unsigned int hash, char *hex_str) {
    sprintf(hex_str, "%08x", hash);
}

// 初始化倉庫
int init_git0() {
    if (dir_exists(GIT0_DIR)) {
        printf("git0 repository already exists.\n");
        return -1;
    }
    mkdir(GIT0_DIR, 0755);
    mkdir(OBJECTS_DIR, 0755);
    fclose(fopen(INDEX_FILE, "w"));
    fclose(fopen(COMMITS_FILE, "w"));
    printf("Initialized empty git0 repository in %s/\n", GIT0_DIR);
    return 0;
}

// 添加文件到暫存區
int add_file(const char *filename) {
    if (!file_exists(filename)) { printf("File not found: %s\n", filename); return -1; }
    if (!dir_exists(GIT0_DIR)) { printf("Not a git0 repo. Run 'init' first.\n"); return -1; }

    size_t len;
    char *content = read_file(filename, &len);
    unsigned int hash = fnv1a_hash(content, len);
    char hash_hex[HASH_LEN + 1];
    hash_to_hex(hash, hash_hex);

    char obj_path[256];
    snprintf(obj_path, sizeof(obj_path), "%s/%s", OBJECTS_DIR, hash_hex);
    if (!file_exists(obj_path)) write_file(obj_path, content, len);
    free(content);

    size_t index_len;
    char *index_content = read_file(INDEX_FILE, &index_len);
    char new_index[4096] = {0};
    int found = 0;

    if (index_content) {
        char *line = strtok(index_content, "\n");
        while (line) {
            char f[256], h[HASH_LEN + 1];
            sscanf(line, "%255s %8s", f, h);
            if (strcmp(f, filename) == 0) {
                sprintf(new_index + strlen(new_index), "%s %s\n", filename, hash_hex);
                found = 1;
            } else {
                sprintf(new_index + strlen(new_index), "%s\n", line);
            }
            line = strtok(NULL, "\n");
        }
        free(index_content);
    }
    if (!found) sprintf(new_index + strlen(new_index), "%s %s\n", filename, hash_hex);
    write_file(INDEX_FILE, new_index, strlen(new_index));

    printf("Added %s to index.\n", filename);
    return 0;
}

// 提交變更
int commit_changes(const char *message) {
    if (!dir_exists(GIT0_DIR)) { printf("Not a git0 repo. Run 'init' first.\n"); return -1; }

    size_t index_len;
    char *index_content = read_file(INDEX_FILE, &index_len);
    if (!index_content || index_len == 0) {
        printf("No changes to commit. Use 'add' first.\n");
        if (index_content) free(index_content);
        return -1;
    }

    char parent_id[HASH_LEN + 1] = "none";
    size_t commits_len;
    char *commits_content = read_file(COMMITS_FILE, &commits_len);
    if (commits_content && commits_len > 0) {
        // --- 修復 2：這裡現在可以正常調用了 ---
        char *last = strrstr(commits_content, "commit ");
        if (last) sscanf(last, "commit %8s", parent_id);
        free(commits_content);
    }

    srand(time(NULL));
    unsigned int commit_hash = (unsigned int)time(NULL) ^ (rand() << 16);
    char commit_id[HASH_LEN + 1];
    hash_to_hex(commit_hash, commit_id);

    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    char commit_content[8192];
    sprintf(commit_content,
        "commit %s\nparent %s\ndate %s\nmessage %s\nfiles\n%s\n",
        commit_id, parent_id, timestamp, message, index_content);
    free(index_content);

    FILE *fp = fopen(COMMITS_FILE, "a");
    fputs(commit_content, fp);
    fclose(fp);

    printf("Committed as %s\n", commit_id);
    return 0;
}

// 輔助：查找字符串最後一次出現的位置 (實現部分移到後面不要緊，因為前面已經宣告過了)
char *strrstr(const char *haystack, const char *needle) {
    char *last = NULL, *cur = strstr(haystack, needle);
    while (cur) { last = cur; cur = strstr(cur + 1, needle); }
    return last;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: git0 <command> [args]\n");
        printf("  init          Initialize repo\n");
        printf("  add <file>    Add file to index\n");
        printf("  commit <msg>  Commit changes\n");
        return 0;
    }
    if (strcmp(argv[1], "init") == 0) return init_git0();
    if (strcmp(argv[1], "add") == 0) return argc >=3 ? add_file(argv[2]) : (printf("Usage: add <file>\n"), -1);
    if (strcmp(argv[1], "commit") == 0) return argc >=3 ? commit_changes(argv[2]) : (printf("Usage: commit <msg>\n"), -1);
    printf("Unknown command: %s\n", argv[1]);
    return -1;
}