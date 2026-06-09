#include <iostream>
#include <unistd.h>

static int arr[100000000]; // 约 381 MB

int main() {
    std::cout << "pid = " << getpid() << "\n";
    std::cout << "&arr[0] = " << &arr[0] << "\n";

    std::cout << "启动后，未访问 arr，查看 VmSize/VmRSS\n";
    std::cin.get();

    arr[0] = 1;

    std::cout << "只写 arr[0]，再查看 VmSize/VmRSS\n";
    std::cin.get();

    for (int i = 0; i < 100000000; ++i) {
        arr[i] = i;
    }

    std::cout << "写完整个 arr，再查看 VmSize/VmRSS\n";
    std::cin.get();
}