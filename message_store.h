// message_store.h

#ifndef MESSAGE_STORE_H
#define MESSAGE_STORE_H

#include <stdint.h>
#include <time.h>

// Fixed sizes for fields
#define MESSAGE_SIZE 1024
#define SENDER_SIZE 64
#define RECEIVER_SIZE 64
#define CONTENT_SIZE (MESSAGE_SIZE - sizeof(uint64_t) * 2 - SENDER_SIZE - RECEIVER_SIZE - sizeof(uint8_t))

// Forward declaration of Cache structure
struct Cache;

// Message data structure
#pragma pack(push, 1)
typedef struct {
    uint64_t id;                          // 8 bytes
    uint64_t time_sent;                   // 8 bytes
    char sender[SENDER_SIZE];             // 64 bytes
    char receiver[RECEIVER_SIZE];         // 64 bytes
    uint8_t delivered;                    // 1 byte
    char content[CONTENT_SIZE];           // Remaining bytes
} message;
#pragma pack(pop)

// Function declarations
message* create_msg(uint64_t id, const char* sender, const char* receiver, const char* content, uint8_t delivered);
int store_msg(const message* msg, struct Cache* cache); // Modified to accept Cache*
message* retrieve_msg(uint64_t id, struct Cache* cache); // Modified to accept Cache*
void free_msg(message* msg);

#endif // MESSAGE_STORE_H
