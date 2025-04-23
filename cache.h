// cache.h

#ifndef CACHE_H
#define CACHE_H

#include "message_store.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CACHE_CAPACITY 100 // Adjustable

// Replacement policy enum
typedef enum {
    REPLACEMENT_POLICY_RANDOM,
    REPLACEMENT_POLICY_LRU
} ReplacementPolicy;

// Cache node structure
typedef struct CacheNode {
    uint64_t id;
    message* msg;
    struct CacheNode* prev;
    struct CacheNode* next;
    struct CacheNode* hash_next; // For hash table collision handling
} CacheNode;

// Cache structure
typedef struct Cache {
    CacheNode* head; // Most recently used
    CacheNode* tail; // Least recently used
    int size;
    CacheNode* hash_table[CACHE_CAPACITY]; // Simple hash table
    ReplacementPolicy policy; // Replacement policy
    int hit_count; // For metrics
    int miss_count; // For metrics
} Cache;

// Function declarations
void init_cache(Cache* cache, ReplacementPolicy policy);
void free_cache(Cache* cache);
message* get_from_cache(Cache* cache, uint64_t id);
void add_to_cache(Cache* cache, message* msg);
void remove_from_cache(Cache* cache, uint64_t id);
void set_replacement_policy(Cache* cache, ReplacementPolicy policy);

#endif // CACHE_H
