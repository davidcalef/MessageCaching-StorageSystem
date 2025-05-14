# Message Caching and Storage System

A C-based simulation of hierarchical memory with persistent message storage, in-memory caching, and support for Least Recently Used (LRU) and Random Replacement page eviction policies.

## 📦 Project Overview

This project simulates how operating systems and databases handle memory hierarchy and paging. It features:

- **Persistent message storage** in fixed-size records (`messages.dat`).
- **Indexed retrieval** using a binary index file (`index.dat`).
- **In-memory caching** with support for:
  - **LRU (Least Recently Used)** eviction
  - **Random Replacement** eviction
- **Performance metrics** tracking cache hits, misses, and hit ratios.

The design mirrors real-world systems such as virtual memory managers and CPU cache subsystems.

---

## 🛠 Features

- Fixed-size message structure with ID, timestamp, sender, receiver, delivery flag, and content
- Efficient file-based storage and retrieval using offsets
- Cache implemented via:
  - **Hash table** for O(1) lookup
  - **Doubly linked list** to track usage order (for LRU)
- Flexible testing to compare LRU vs. Random replacement policies
- Metrics reporting:
  - Cache hit count
  - Miss count
  - Hit ratio over simulated message access patterns

---

## 🧪 How It Works

1. **Store Phase**:
   - Creates and stores `N` messages on disk (e.g., 1000), each with a unique ID.
   - Populates `messages.dat` and `index.dat`.

2. **Access Phase**:
   - Initializes cache with user-selected policy (LRU or Random).
   - Randomly accesses messages to simulate workload.
   - Measures cache performance via hit/miss counters.

3. **Eviction**:
   - When the cache exceeds its size limit, a page (message) is evicted using the active policy.

---

### Build Instructions

```bash
gcc -o message_cache main.c cache.c message_store.c
./message_cache

Files:
main.c – Controls testing logic and program flow

cache.c / cache.h – Implements the in-memory cache

message_store.c / message_store.h – Handles file I/O and indexing

Clean Up
After each test run, delete messages.dat and index.dat to reset the environment:
rm messages.dat index.dat

Language: C

Data Structures: Hash Table, Doubly Linked List

File I/O: Binary files with structured offsets

Memory Concepts: Paging, Cache Replacement Policies

This project was developed to reinforce systems-level concepts such as virtual memory, paging, and cache replacement strategies. It's designed for hands-on learning and performance comparison between memory policies.
