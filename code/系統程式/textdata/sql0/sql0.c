#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define PAGE_SIZE 4096
#define MAX_PAGES 100 // Buffer Pool 記憶體最多暫存 100 頁
#define MAX_KEYS 3    // 為了容易觸發分裂，依然設為 3 (真實資料庫會根據 4KB 動態計算)
#define INVALID_PAGE_NUM 0xFFFFFFFF

// ==========================================
// 1. 資料結構定義
// ==========================================
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

// B+ Tree 節點結構 (將被轉型為 4KB 的 Page)
typedef struct {
    bool is_leaf;
    uint32_t num_keys;
    uint32_t parent_page_num;
    uint32_t next_page_num; // 僅 Leaf node 使用
    uint32_t keys[MAX_KEYS + 1];
    union {
        uint32_t children_page_nums[MAX_KEYS + 2]; // 內部節點：存子節點的 "Page Number"
        Row rows[MAX_KEYS + 1];                    // 葉節點：存實際資料
    } ptrs;
} Node;

// Pager 磁碟分頁管理器
typedef struct {
    FILE* file_descriptor;
    uint32_t file_length;
    uint32_t num_pages;
    void* pages[MAX_PAGES]; // Buffer Pool: 暫存 4KB 分頁的記憶體陣列
} Pager;

// 資料庫與 Meta Page (Page 0)
typedef struct {
    Pager* pager;
    uint32_t root_page_num;
} Table;

// ==========================================
// 2. Pager (Disk I/O & Buffer Pool)
// ==========================================

Pager* pager_open(const char* filename) {
    FILE* fd = fopen(filename, "r+");
    if (fd == NULL) {
        // 如果檔案不存在，建立新檔案
        fd = fopen(filename, "w+");
    }

    fseek(fd, 0, SEEK_END);
    uint32_t file_length = ftell(fd);

    Pager* pager = (Pager*)malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;
    pager->num_pages = (file_length / PAGE_SIZE);

    for (uint32_t i = 0; i < MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

// 核心：向 Pager 請求分頁
void* get_page(Pager* pager, uint32_t page_num) {
    if (page_num >= MAX_PAGES) {
        printf("Error: Page number out of bounds.\n");
        exit(EXIT_FAILURE);
    }

    // 1. Cache Hit: 分頁已在記憶體中，直接回傳
    if (pager->pages[page_num] != NULL) {
        return pager->pages[page_num];
    }

    // 2. Cache Miss: 分配 4KB 記憶體
    void* page = malloc(PAGE_SIZE);
    
    // 3. 如果硬碟裡有這一頁，從硬碟讀取
    uint32_t num_pages_in_file = pager->file_length / PAGE_SIZE;
    if (page_num < num_pages_in_file) {
        fseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
        fread(page, PAGE_SIZE, 1, pager->file_descriptor);
    } else {
        // 全新分頁，初始化為 0
        memset(page, 0, PAGE_SIZE);
    }

    // 4. 放入 Buffer Pool
    pager->pages[page_num] = page;
    if (page_num >= pager->num_pages) {
        pager->num_pages = page_num + 1;
    }
    return page;
}

// 取得全新的分頁編號
uint32_t get_new_page_num(Pager* pager) {
    return pager->num_pages;
}

// 將分頁寫入硬碟
void pager_flush(Pager* pager, uint32_t page_num) {
    if (pager->pages[page_num] == NULL) return;
    fseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
    fwrite(pager->pages[page_num], PAGE_SIZE, 1, pager->file_descriptor);
}

// ==========================================
// 3. 資料庫啟動與關閉
// ==========================================

Table* db_open(const char* filename) {
    Pager* pager = pager_open(filename);
    Table* table = (Table*)malloc(sizeof(Table));
    table->pager = pager;

    if (pager->num_pages == 0) {
        // 新資料庫：初始化 Page 0 (Meta Page)
        void* meta_page = get_page(pager, 0);
        table->root_page_num = INVALID_PAGE_NUM; 
    } else {
        // 舊資料庫：從 Page 0 讀取 root_page_num
        void* meta_page = get_page(pager, 0);
        table->root_page_num = *((uint32_t*)meta_page);
    }
    return table;
}

void db_close(Table* table) {
    Pager* pager = table->pager;
    
    // 儲存 Root Page Num 到 Meta Page (Page 0)
    void* meta_page = get_page(pager, 0);
    *((uint32_t*)meta_page) = table->root_page_num;

    // 將 Buffer Pool 所有快取寫回硬碟 (簡單實作，全部 Flush)
    for (uint32_t i = 0; i < pager->num_pages; i++) {
        if (pager->pages[i] != NULL) {
            pager_flush(pager, i);
            free(pager->pages[i]);
            pager->pages[i] = NULL;
        }
    }

    fclose(pager->file_descriptor);
    free(pager);
    free(table);
}

// ==========================================
// 4. B+ Tree 邏輯 (使用 Pager 與 Page Num)
// ==========================================

// 初始化新節點 (此處 node 實際指向 Pager 給的 4KB 空間)
void initialize_node(Node* node, bool is_leaf) {
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->parent_page_num = INVALID_PAGE_NUM;
    node->next_page_num = INVALID_PAGE_NUM;
}

uint32_t find_leaf_page(Table* table, uint32_t key) {
    if (table->root_page_num == INVALID_PAGE_NUM) return INVALID_PAGE_NUM;
    
    uint32_t curr_page_num = table->root_page_num;
    Node* curr = (Node*)get_page(table->pager, curr_page_num);
    
    while (!curr->is_leaf) {
        int i = 0;
        while (i < curr->num_keys && key >= curr->keys[i]) i++;
        curr_page_num = curr->ptrs.children_page_nums[i];
        curr = (Node*)get_page(table->pager, curr_page_num);
    }
    return curr_page_num;
}

Row* bptree_search(Table* table, uint32_t key) {
    uint32_t leaf_page_num = find_leaf_page(table, key);
    if (leaf_page_num == INVALID_PAGE_NUM) return NULL;

    Node* leaf = (Node*)get_page(table->pager, leaf_page_num);
    for (int i = 0; i < leaf->num_keys; i++) {
        if (leaf->keys[i] == key) return &leaf->ptrs.rows[i];
    }
    return NULL;
}

void insert_into_parent(Table* table, uint32_t left_page_num, uint32_t key, uint32_t right_page_num) {
    Node* left = (Node*)get_page(table->pager, left_page_num);
    Node* right = (Node*)get_page(table->pager, right_page_num);
    uint32_t parent_page_num = left->parent_page_num;

    if (parent_page_num == INVALID_PAGE_NUM) {
        // 建立新的 Root
        uint32_t new_root_num = get_new_page_num(table->pager);
        Node* new_root = (Node*)get_page(table->pager, new_root_num);
        initialize_node(new_root, false);
        
        new_root->keys[0] = key;
        new_root->ptrs.children_page_nums[0] = left_page_num;
        new_root->ptrs.children_page_nums[1] = right_page_num;
        new_root->num_keys = 1;
        
        left->parent_page_num = new_root_num;
        right->parent_page_num = new_root_num;
        table->root_page_num = new_root_num;
        return;
    }

    Node* parent = (Node*)get_page(table->pager, parent_page_num);
    int insert_idx = 0;
    while (insert_idx < parent->num_keys && parent->keys[insert_idx] < key) insert_idx++;

    for (int i = parent->num_keys; i > insert_idx; i--) {
        parent->keys[i] = parent->keys[i - 1];
        parent->ptrs.children_page_nums[i + 1] = parent->ptrs.children_page_nums[i];
    }

    parent->keys[insert_idx] = key;
    parent->ptrs.children_page_nums[insert_idx + 1] = right_page_num;
    parent->num_keys++;
    right->parent_page_num = parent_page_num;

    // Internal Node Split
    if (parent->num_keys > MAX_KEYS) {
        uint32_t new_internal_num = get_new_page_num(table->pager);
        Node* new_internal = (Node*)get_page(table->pager, new_internal_num);
        initialize_node(new_internal, false);

        int split_idx = parent->num_keys / 2;
        uint32_t up_key = parent->keys[split_idx];

        new_internal->num_keys = parent->num_keys - split_idx - 1;
        for (int i = 0; i < new_internal->num_keys; i++) {
            new_internal->keys[i] = parent->keys[split_idx + 1 + i];
            new_internal->ptrs.children_page_nums[i] = parent->ptrs.children_page_nums[split_idx + 1 + i];
            
            // 更新子節點的父節點編號 (需要透過 Pager 拿子節點)
            Node* child = (Node*)get_page(table->pager, new_internal->ptrs.children_page_nums[i]);
            child->parent_page_num = new_internal_num;
        }
        new_internal->ptrs.children_page_nums[new_internal->num_keys] = parent->ptrs.children_page_nums[parent->num_keys];
        Node* last_child = (Node*)get_page(table->pager, new_internal->ptrs.children_page_nums[new_internal->num_keys]);
        last_child->parent_page_num = new_internal_num;

        parent->num_keys = split_idx;
        insert_into_parent(table, parent_page_num, up_key, new_internal_num);
    }
}

void bptree_insert(Table* table, Row* row) {
    uint32_t key = row->id;

    if (table->root_page_num == INVALID_PAGE_NUM) {
        uint32_t root_page_num = get_new_page_num(table->pager); // 應該會是 Page 1 (Page 0 是 Meta)
        Node* root = (Node*)get_page(table->pager, root_page_num);
        initialize_node(root, true);
        
        root->keys[0] = key;
        root->ptrs.rows[0] = *row;
        root->num_keys = 1;
        table->root_page_num = root_page_num;
        return;
    }

    uint32_t leaf_page_num = find_leaf_page(table, key);
    Node* leaf = (Node*)get_page(table->pager, leaf_page_num);
    
    // Check duplicate
    for (int i = 0; i < leaf->num_keys; i++) {
        if (leaf->keys[i] == key) {
            printf("Error: Duplicate primary key %d.\n", key);
            return;
        }
    }

    int insert_idx = 0;
    while (insert_idx < leaf->num_keys && leaf->keys[insert_idx] < key) insert_idx++;

    for (int i = leaf->num_keys; i > insert_idx; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->ptrs.rows[i] = leaf->ptrs.rows[i - 1];
    }
    leaf->keys[insert_idx] = key;
    leaf->ptrs.rows[insert_idx] = *row;
    leaf->num_keys++;

    // Leaf Node Split
    if (leaf->num_keys > MAX_KEYS) {
        uint32_t new_leaf_num = get_new_page_num(table->pager);
        Node* new_leaf = (Node*)get_page(table->pager, new_leaf_num);
        initialize_node(new_leaf, true);
        
        int split_idx = leaf->num_keys / 2;
        new_leaf->num_keys = leaf->num_keys - split_idx;
        for (int i = 0; i < new_leaf->num_keys; i++) {
            new_leaf->keys[i] = leaf->keys[split_idx + i];
            new_leaf->ptrs.rows[i] = leaf->ptrs.rows[split_idx + i];
        }
        leaf->num_keys = split_idx;

        // 維護 Leaf 的 Linked List (存 Page Num)
        new_leaf->next_page_num = leaf->next_page_num;
        leaf->next_page_num = new_leaf_num;

        insert_into_parent(table, leaf_page_num, new_leaf->keys[0], new_leaf_num);
    }
}

// ==========================================
// 5. sql1 前端與執行
// ==========================================

void print_row(Row* row) {
    if (row) printf("(%d, '%s', '%s')\n", row->id, row->username, row->email);
}

void execute_insert(Table* table, char* input) {
    Row row;
    if (sscanf(input, "insert %u %31s %254s", &row.id, row.username, row.email) != 3) {
        printf("Syntax error.\n");
        return;
    }
    bptree_insert(table, &row);
    printf("Executed.\n");
}

void execute_select(Table* table, char* input) {
    uint32_t id;
    if (sscanf(input, "select %u", &id) != 1) return;
    Row* row = bptree_search(table, id);
    if (row) print_row(row);
    else printf("Row not found.\n");
}

void execute_select_all(Table* table) {
    if (table->root_page_num == INVALID_PAGE_NUM) {
        printf("Empty table.\n");
        return;
    }
    
    // 找到最左邊的 Leaf Page
    uint32_t curr_page_num = table->root_page_num;
    Node* curr = (Node*)get_page(table->pager, curr_page_num);
    while (!curr->is_leaf) {
        curr_page_num = curr->ptrs.children_page_nums[0];
        curr = (Node*)get_page(table->pager, curr_page_num);
    }

    int count = 0;
    while (curr_page_num != INVALID_PAGE_NUM) {
        curr = (Node*)get_page(table->pager, curr_page_num);
        for (int i = 0; i < curr->num_keys; i++) {
            print_row(&curr->ptrs.rows[i]);
            count++;
        }
        curr_page_num = curr->next_page_num; // 追蹤 Linked List 讀取硬碟下一頁
    }
    printf("Total rows: %d\n", count);
}

int main() {
    Table* table = db_open("mydb.db");
    char input_buffer[512];

    printf("Welcome to sql1 (Persistent Disk version). \n");

    while (true) {
        printf("sql1> ");
        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) break;
        input_buffer[strcspn(input_buffer, "\n")] = 0;

        if (strcmp(input_buffer, ".exit") == 0) {
            db_close(table);
            printf("Database saved. Bye.\n");
            break;
        } else if (strncmp(input_buffer, "insert", 6) == 0) {
            execute_insert(table, input_buffer);
        } else if (strncmp(input_buffer, "select_all", 10) == 0) {
            execute_select_all(table);
        } else if (strncmp(input_buffer, "select", 6) == 0) {
            execute_select(table, input_buffer);
        }
    }
    return 0;
}