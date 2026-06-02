#pragma once
#include <atomic>
#include <cstdint>

constexpr const char* SHM_NAME = "/cpp_memory_lab_shm";
constexpr uint32_t SHM_MAGIC = 0x4D454D31; // MEM1
constexpr uint32_t SLOT_COUNT = 8;

struct alignas(64) SharedHeader {
    uint32_t magic;
    uint32_t slot_count;
    std::atomic<uint64_t> seq;
};

struct alignas(64) OrderSlot {
    std::atomic<uint64_t> version;
    int order_id;
    double price;
    int qty;
    char symbol[32];
};

struct SharedRegion {
    SharedHeader header;
    OrderSlot slots[SLOT_COUNT];
};
