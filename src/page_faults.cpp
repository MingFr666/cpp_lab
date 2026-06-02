#include <sys/resource.h>
#include <unistd.h>
#include <iostream>
#include <vector>

void print_usage(const char* tag) {
    rusage ru{};
    getrusage(RUSAGE_SELF, &ru);
    std::cout << tag
              << " minor_faults=" << ru.ru_minflt
              << " major_faults=" << ru.ru_majflt << '\n';
}

int main() {
    constexpr size_t MB = 1024 * 1024;
    constexpr size_t N = 256 * MB;
    constexpr size_t PAGE = 4096;

    print_usage("before alloc");
    std::vector<char> buf(N);
    print_usage("after vector construction");

    for (size_t i = 0; i < N; i += PAGE) {
        buf[i] = 1; // touch each page
    }
    print_usage("after touching pages");

    std::cout << "pid=" << getpid() << " size=" << N / MB << "MB\n";
    return 0;
}
