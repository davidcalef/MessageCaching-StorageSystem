// cache.c

#include "cache.h"
#include <stdio.h>
#include <time.h>

// Hash function for the cache
static int hash(uint64_t id) {
    return id % CACHE_CAPACITY;
}

// Initialize the cache
void init_cache(Cache* cache, ReplacementPolicy policy) {
    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
    memset(cache->hash_table, 0, sizeof(cache->hash_table));
    cache->policy = policy;
    cache->hit_count = 0;
    cache->miss_count = 0;
}

// Free the cache
void free_cache(Cache* cache) {
    CacheNode* current = cache->head;
    while (current != NULL) {
        CacheNode* next = current->next;
        free_msg(current->msg);
        free(current);
        current = next;
    }
    cache->head = NULL;
    cache->tail = NULL;
    cache->size = 0;
    memset(cache->hash_table, 0, sizeof(cache->hash_table));
}

// Move a node to the front (most recently used)
static void move_to_front(Cache* cache, CacheNode* node) {
    if (node == cache->head) {
        return; // Already at the front
    }

    // Detach the node
    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }

    if (node == cache->tail) {
        cache->tail = node->prev;
    }

    // Move to front
    node->prev = NULL;
    node->next = cache->head;
    if (cache->head) {
        cache->head->prev = node;
    }
    cache->head = node;

    if (cache->tail == NULL) {
        cache->tail = node;
    }
}

// Get a message from the cache by ID
message* get_from_cache(Cache* cache, uint64_t id) {
    int index = hash(id);
    CacheNode* node = cache->hash_table[index];

    while (node != NULL) {
        if (node->id == id) {
            // Cache hit
            cache->hit_count++;
            if (cache->policy == REPLACEMENT_POLICY_LRU) {
                // Move the node to the front (LRU policy)
                move_to_front(cache, node);
            }
            return node->msg;
        }
        node = node->hash_next;
    }
    // Cache miss
    cache->miss_count++;
    return NULL; // Not found in cache
}

// Remove a node from the cache
void remove_from_cache(Cache* cache, uint64_t id) {
    int index = hash(id);
    CacheNode* node = cache->hash_table[index];
    CacheNode* prev_node = NULL;

    while (node != NULL) {
        if (node->id == id) {
            // Remove from hash table
            if (prev_node == NULL) {
                cache->hash_table[index] = node->hash_next;
            } else {
                prev_node->hash_next = node->hash_next;
            }

            // Remove from linked list
            if (node->prev) {
                node->prev->next = node->next;
            } else {
                cache->head = node->next;
            }
            if (node->next) {
                node->next->prev = node->prev;
            } else {
                cache->tail = node->prev;
            }

            // Free resources
            free_msg(node->msg);
            free(node);
            cache->size--;

            return;
        }
        prev_node = node;
        node = node->hash_next;
    }
}

// Remove a random node from the cache
static void remove_random(Cache* cache) {
    if (cache->size == 0) {
        return;
    }

    // Generate a random index within the cache size
    int random_index = rand() % cache->size;
    CacheNode* node = cache->head;
    for (int i = 0; i < random_index && node != NULL; i++) {
        node = node->next;
    }

    if (node != NULL) {
        remove_from_cache(cache, node->id);
    }
}

// Add a message to the cache
void add_to_cache(Cache* cache, message* msg) {
    int index = hash(msg->id);
    CacheNode* existing_node = cache->hash_table[index];

    // Check if the message is already in the cache
    while (existing_node != NULL) {
        if (existing_node->id == msg->id) {
            // Update the message
            memcpy(existing_node->msg, msg, sizeof(message));
            if (cache->policy == REPLACEMENT_POLICY_LRU) {
                // Move to front
                move_to_front(cache, existing_node);
            }
            free_msg(msg); // Free the new message since we updated the existing one
            return;
        }
        existing_node = existing_node->hash_next;
    }

    // Create a new cache node
    CacheNode* new_node = (CacheNode*)malloc(sizeof(CacheNode));
    if (new_node == NULL) {
        perror("Failed to allocate memory for cache node");
        free_msg(msg);
        return;
    }
    new_node->id = msg->id;
    new_node->msg = msg;
    new_node->prev = NULL;
    new_node->next = NULL;
    new_node->hash_next = NULL;

    // Add to hash table
    new_node->hash_next = cache->hash_table[index];
    cache->hash_table[index] = new_node;

    // Add to front of the linked list
    new_node->prev = NULL;
    new_node->next = cache->head;
    if (cache->head) {
        cache->head->prev = new_node;
    }
    cache->head = new_node;
    if (cache->tail == NULL) {
        cache->tail = new_node;
    }

    cache->size++;

    // Check if cache is over capacity
    if (cache->size > CACHE_CAPACITY) {
        if (cache->policy == REPLACEMENT_POLICY_LRU) {
            // Remove least recently used (tail)
            CacheNode* lru = cache->tail;
            if (lru != NULL) {
                remove_from_cache(cache, lru->id);
            }
        } else if (cache->policy == REPLACEMENT_POLICY_RANDOM) {
            remove_random(cache);
        }
    }
}

// Set the replacement policy
void set_replacement_policy(Cache* cache, ReplacementPolicy policy) {
    cache->policy = policy;
}
