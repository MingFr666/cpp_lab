#include "shared_memory_common.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>

bool read_slot_consistent(const OrderSlot& slot, OrderSlot& out) {
    uint64_t v1 = slot.version.load(std::memory_order_acquire);
    if (v1 & 1ULL) return false; // writer in progress

    out.order_id = slot.order_id;
    out.price = slot.price;
    out.qty = slot.qty;
    std::memcpy(out.symbol, slot.symbol, sizeof(out.symbol));

    uint64_t v2 = slot.version.load(std::memory_order_acquire);
    return v1 == v2 && !(v2 & 1ULL);
}

int main() {
    int fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (fd == -1) { perror("shm_open: run writer first"); return 1; }

    void* addr = mmap(nullptr, sizeof(SharedRegion), PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    auto* region = static_cast<const SharedRegion*>(addr);
    if (region->header.magic != SHM_MAGIC) {
        std::cerr << "bad magic, shared memory not initialized\n";
        munmap(addr, sizeof(SharedRegion));
        close(fd);
        return 1;
    }

    for (int round = 0; round < 40; ++round) {
        uint64_t seq = region->header.seq.load(std::memory_order_acquire);
        std::cout << "\nseq=" << seq << '\n';
        for (uint32_t i = 0; i < SLOT_COUNT; ++i) {
            OrderSlot snapshot{};
            if (read_slot_consistent(region->slots[i], snapshot) && snapshot.order_id != 0) {
                std::cout << "slot=" << i
                          << " order_id=" << snapshot.order_id
                          << " symbol=" << snapshot.symbol
                          << " price=" << snapshot.price
                          << " qty=" << snapshot.qty << '\n';
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    munmap(addr, sizeof(SharedRegion));
    close(fd);
    return 0;
}
