// message_store.c

#include "message_store.h"
#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_FILE "messages.dat"
#define INDEX_FILE "index.dat"

// Index entry structure
#pragma pack(push, 1)
typedef struct {
    uint64_t id;
    uint64_t offset;
} index_entry;
#pragma pack(pop)

// Create a new message
message* create_msg(uint64_t id, const char* sender, const char* receiver, const char* content, uint8_t delivered) {
    // Allocate memory for the message
    message* msg = (message*)malloc(sizeof(message));
    if (msg == NULL) {
        perror("Failed to allocate memory for message");
        return NULL;
    }

    // Set the id
    msg->id = id;

    // Set time_sent to current time
    msg->time_sent = (uint64_t)time(NULL);

    // Copy sender
    strncpy(msg->sender, sender, SENDER_SIZE - 1);
    msg->sender[SENDER_SIZE - 1] = '\0'; // Ensure null-termination

    // Copy receiver
    strncpy(msg->receiver, receiver, RECEIVER_SIZE - 1);
    msg->receiver[RECEIVER_SIZE - 1] = '\0';

    // Copy content
    strncpy(msg->content, content, CONTENT_SIZE - 1);
    msg->content[CONTENT_SIZE - 1] = '\0';

    // Set delivered flag
    msg->delivered = delivered;

    return msg;
}

// Store the message on disk
int store_msg(const message* msg, Cache* cache) {
    if (msg == NULL) {
        fprintf(stderr, "Null message pointer provided to store_msg.\n");
        return -1;
    }

    // If cache is provided, add the message to the cache
    if (cache != NULL) {
        message* msg_copy = (message*)malloc(sizeof(message));
        if (msg_copy == NULL) {
            perror("Failed to allocate memory for message copy");
            return -1;
        }
        memcpy(msg_copy, msg, sizeof(message));
        add_to_cache(cache, msg_copy);
    }

    FILE* msg_file = fopen(MESSAGE_FILE, "ab"); // Open in append binary mode
    if (msg_file == NULL) {
        perror("Failed to open message file");
        return -1;
    }

    if (fseek(msg_file, 0, SEEK_END) != 0) {
        perror("Failed to seek to end of message file");
        fclose(msg_file);
        return -1;
    }

    uint64_t offset = (uint64_t)ftell(msg_file);
    if (offset == (uint64_t)-1) {
        perror("Failed to get current file offset");
        fclose(msg_file);
        return -1;
    }

    size_t written = fwrite(msg, sizeof(message), 1, msg_file);
    if (written != 1) {
        perror("Failed to write message to file");
        fclose(msg_file);
        return -1;
    }

    fclose(msg_file);

    // Update the index file
    FILE* idx_file = fopen(INDEX_FILE, "ab"); // Open in append binary mode
    if (idx_file == NULL) {
        perror("Failed to open index file");
        return -1;
    }

    index_entry entry;
    entry.id = msg->id;
    entry.offset = offset;

    written = fwrite(&entry, sizeof(index_entry), 1, idx_file);
    if (written != 1) {
        perror("Failed to write index entry to file");
        fclose(idx_file);
        return -1;
    }

    fclose(idx_file);

    return 0;
}

// Retrieve a message by its ID
message* retrieve_msg(uint64_t id, Cache* cache) {
    // First, try to get the message from the cache
    if (cache != NULL) {
        message* cached_msg = get_from_cache(cache, id);
        if (cached_msg != NULL) {
            // Return a copy of the message
            message* msg_copy = (message*)malloc(sizeof(message));
            if (msg_copy == NULL) {
                perror("Failed to allocate memory for message copy");
                return NULL;
            }
            memcpy(msg_copy, cached_msg, sizeof(message));
            return msg_copy;
        }
    }

    // Message not found in cache, retrieve from disk
    FILE* idx_file = fopen(INDEX_FILE, "rb");
    if (idx_file == NULL) {
        perror("Failed to open index file");
        return NULL;
    }

    index_entry entry;
    int found = 0;

    while (fread(&entry, sizeof(index_entry), 1, idx_file) == 1) {
        if (entry.id == id) {
            found = 1;
            break;
        }
    }

    fclose(idx_file);

    if (!found) {
        fprintf(stderr, "Message with ID %llu not found.\n", (unsigned long long)id);
        return NULL;
    }

    FILE* msg_file = fopen(MESSAGE_FILE, "rb");
    if (msg_file == NULL) {
        perror("Failed to open message file");
        return NULL;
    }

    if (fseek(msg_file, (long)entry.offset, SEEK_SET) != 0) {
        perror("Failed to seek to message offset");
        fclose(msg_file);
        return NULL;
    }

    message* msg = (message*)malloc(sizeof(message));
    if (msg == NULL) {
        perror("Failed to allocate memory for message");
        fclose(msg_file);
        return NULL;
    }

    if (fread(msg, sizeof(message), 1, msg_file) != 1) {
        perror("Failed to read message from file");
        free(msg);
        fclose(msg_file);
        return NULL;
    }

    fclose(msg_file);

    // Add the retrieved message to the cache
    if (cache != NULL) {
        message* msg_copy = (message*)malloc(sizeof(message));
        if (msg_copy == NULL) {
            perror("Failed to allocate memory for message copy");
            free(msg);
            return NULL;
        }
        memcpy(msg_copy, msg, sizeof(message));
        add_to_cache(cache, msg_copy);
    }

    return msg;
}

// Free a message
void free_msg(message* msg) {
    if (msg != NULL) {
        free(msg);
    }
}
