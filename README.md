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

## 🚀 Getting Started

### Build Instructions

```bash
gcc -o message_cache main.c cache.c message_store.c
./message_cache
