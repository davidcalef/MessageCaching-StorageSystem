//https://docs.google.com/spreadsheets/d/16t_XwdNiiBt2Ph87RkHKbPDmvHkRYLmvKLTZj2_wxqE/edit?usp=sharing
// main.c

#include "message_store.h"
#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_MESSAGES 1000
#define TOTAL_ACCESSES 1000

void run_cache_test(ReplacementPolicy policy) {
    Cache cache;
    init_cache(&cache, policy);

    // Reset metrics
    cache.hit_count = 0;
    cache.miss_count = 0;

    // Perform random accesses
    printf("Performing random accesses with %s policy...\n", policy == REPLACEMENT_POLICY_LRU ? "LRU" : "Random Replacement");
    for (int i = 0; i < TOTAL_ACCESSES; i++) {
        uint64_t random_id = (rand() % TOTAL_MESSAGES) + 1;
        message* retrieved_msg = retrieve_msg(random_id, &cache);
        if (retrieved_msg != NULL) {
            // Optionally process the retrieved message
            free_msg(retrieved_msg);
        }
    }

    // Display results
    printf("Replacement Policy: %s\n", policy == REPLACEMENT_POLICY_LRU ? "LRU" : "Random Replacement");
    printf("Cache Hits: %d\n", cache.hit_count);
    printf("Cache Misses: %d\n", cache.miss_count);
    printf("Cache Hit Ratio: %.2f%%\n\n", (cache.hit_count * 100.0) / (cache.hit_count + cache.miss_count));

    free_cache(&cache);
}

int main() {
    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Store messages
    printf("Storing messages...\n");
    for (uint64_t i = 1; i <= TOTAL_MESSAGES; i++) {
        char content[100];
        snprintf(content, sizeof(content), "Message content for message ID %llu", (unsigned long long)i);
        message* msg = create_msg(i, "Sender", "Receiver", content, 0);
        if (msg == NULL) {
            fprintf(stderr, "Failed to create message ID %llu.\n", (unsigned long long)i);
            continue;
        }

        // Store without cache, as we will test cache separately
        if (store_msg(msg, NULL) != 0) {
            fprintf(stderr, "Failed to store message ID %llu.\n", (unsigned long long)i);
        }

        free_msg(msg);
    }

    // Run tests for each replacement policy
    run_cache_test(REPLACEMENT_POLICY_LRU);
    run_cache_test(REPLACEMENT_POLICY_RANDOM);

    return 0;
}
