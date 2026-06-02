#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

int main() {
    const char* path = "mmap_demo.dat";
    const size_t size = 4096;

    int fd = open(path, O_RDWR | O_CREAT, 0644);
    if (fd == -1) { perror("open"); return 1; }
    if (ftruncate(fd, size) == -1) { perror("ftruncate"); close(fd); return 1; }

    void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    char* p = static_cast<char*>(addr);
    std::strcpy(p, "hello from mmap file");
    msync(addr, size, MS_SYNC);

    std::cout << "mapped address: " << addr << '\n';
    std::cout << "content: " << p << '\n';
    std::cout << "file: " << path << '\n';

    munmap(addr, size);
    close(fd);
    return 0;
}
