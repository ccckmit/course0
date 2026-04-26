#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "../include/sha.h"

#define HASH_SIZE 65
#define DATA_SIZE 256
#define DIFFICULTY 4

typedef struct Block {
    int index;
    time_t timestamp;
    char data[DATA_SIZE];
    char previous_hash[HASH_SIZE];
    char hash[HASH_SIZE];
    int nonce;
    struct Block* next;
} Block;

Block* genesis_block = NULL;

int is_hash_valid(const char* hash) {
    for (int i = 0; i < DIFFICULTY; i++) {
        if (hash[i] != '0') return 0;
    }
    return 1;
}

char* calculate_hash(int index, time_t timestamp, const char* data, const char* previous_hash, int nonce) {
    char combined[512];
    sprintf(combined, "%d%ld%s%s%d", index, timestamp, data, previous_hash, nonce);
    
    uint8_t digest[SHA256_DIGEST_SIZE];
    sha256((uint8_t*)combined, strlen(combined), digest);
    
    static char hash_str[HASH_SIZE];
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        sprintf(hash_str + (i * 2), "%02x", digest[i]);
    }
    hash_str[64] = '\0';
    
    return hash_str;
}

void mine_block(Block* block) {
    printf("Mining block #%d...", block->index);
    fflush(stdout);
    
    int nonce = 0;
    while (1) {
        char* hash = calculate_hash(block->index, block->timestamp, block->data, block->previous_hash, nonce);
        if (is_hash_valid(hash)) {
            strncpy(block->hash, hash, HASH_SIZE - 1);
            block->hash[HASH_SIZE - 1] = '\0';
            block->nonce = nonce;
            printf(" Done! Nonce: %d\n", nonce);
            break;
        }
        nonce++;
    }
}

Block* create_block(int index, const char* data, const char* previous_hash) {
    Block* block = malloc(sizeof(Block));
    block->index = index;
    block->timestamp = time(NULL);
    strncpy(block->data, data, DATA_SIZE - 1);
    block->data[DATA_SIZE - 1] = '\0';
    strncpy(block->previous_hash, previous_hash, HASH_SIZE - 1);
    block->previous_hash[HASH_SIZE - 1] = '\0';
    block->next = NULL;
    block->nonce = 0;
    
    mine_block(block);
    
    return block;
}

void print_block(Block* block) {
    printf("Block #%d\n", block->index);
    printf("  Timestamp: %ld\n", block->timestamp);
    printf("  Data: %s\n", block->data);
    printf("  Previous Hash: %s\n", block->previous_hash);
    printf("  Hash: %s\n", block->hash);
    printf("  Nonce: %d\n", block->nonce);
    printf("\n");
}

int is_chain_valid() {
    Block* current = genesis_block->next;
    Block* previous = genesis_block;
    
    while (current != NULL) {
        char* calculated_hash = calculate_hash(
            current->index,
            current->timestamp,
            current->data,
            current->previous_hash,
            current->nonce
        );
        
        if (strcmp(calculated_hash, current->hash) != 0) {
            return 0;
        }
        
        if (!is_hash_valid(current->hash)) {
            return 0;
        }
        
        if (strcmp(current->previous_hash, previous->hash) != 0) {
            return 0;
        }
        
        previous = current;
        current = current->next;
    }
    
    return 1;
}

void print_chain() {
    Block* current = genesis_block;
    while (current != NULL) {
        print_block(current);
        current = current->next;
    }
}

void free_chain() {
    Block* current = genesis_block;
    while (current != NULL) {
        Block* next = current->next;
        free(current);
        current = next;
    }
}

int main() {
    printf("=== Simple Blockchain Demo ===\n\n");
    
    genesis_block = create_block(0, "Genesis Block", "0000000000000000000000000000000000000000000000000000000000000000");
    
    Block* block1 = create_block(1, "Alice sends 10 coins to Bob", genesis_block->hash);
    genesis_block->next = block1;
    
    Block* block2 = create_block(2, "Bob sends 5 coins to Charlie", block1->hash);
    block1->next = block2;
    
    Block* block3 = create_block(3, "Charlie sends 3 coins to Dave", block2->hash);
    block2->next = block3;
    
    print_chain();
    
    printf("=== Chain Validation ===\n");
    if (is_chain_valid()) {
        printf("Chain is VALID!\n");
    } else {
        printf("Chain is INVALID!\n");
    }
    
    free_chain();
    
    return 0;
}