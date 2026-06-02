#include "shared_memory_common.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) { perror("shm_open"); return 1; }

    if (ftruncate(fd, sizeof(SharedRegion)) == -1) { perror("ftruncate"); close(fd); return 1; }

    void* addr = mmap(nullptr, sizeof(SharedRegion), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    auto* region = static_cast<SharedRegion*>(addr);
    region->header.magic = SHM_MAGIC;
    region->header.slot_count = SLOT_COUNT;
    region->header.seq.store(0, std::memory_order_relaxed);

    for (uint64_t i = 0; i < 100; ++i) {
        uint32_t idx = i % SLOT_COUNT;
        auto& slot = region->slots[idx];

        uint64_t v = slot.version.load(std::memory_order_relaxed);
        slot.version.store(v + 1, std::memory_order_release); // odd: writing

        slot.order_id = static_cast<int>(100000 + i);
        slot.price = 10.0 + static_cast<double>(i) * 0.01;
        slot.qty = 100 + static_cast<int>(i);
        std::snprintf(slot.symbol, sizeof(slot.symbol), "600%03llu.SH", static_cast<unsigned long long>(i % 1000));

        slot.version.store(v + 2, std::memory_order_release); // even: stable
        region->header.seq.fetch_add(1, std::memory_order_release);

        std::cout << "write slot=" << idx << " order_id=" << slot.order_id
                  << " symbol=" << slot.symbol << " price=" << slot.price << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    munmap(addr, sizeof(SharedRegion));
    close(fd);
    std::cout << "Done. To remove shm: shm_unlink in code, or `rm /dev/shm/cpp_memory_lab_shm` on Linux.\n";
    return 0;
}
