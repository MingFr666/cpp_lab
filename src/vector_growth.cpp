#include <iostream>
#include <vector>
#include <iomanip>

int main() {
    std::vector<int> v;
    const int* last_data = nullptr;

    std::cout << std::left << std::setw(8) << "i"
              << std::setw(10) << "size"
              << std::setw(12) << "capacity"
              << "data pointer\n";

    for (int i = 0; i < 40; ++i) {
        v.push_back(i);
        if (v.data() != last_data) {
            std::cout << std::left << std::setw(8) << i
                      << std::setw(10) << v.size()
                      << std::setw(12) << v.capacity()
                      << static_cast<const void*>(v.data()) << "  <-- reallocated\n";
            last_data = v.data();
        }
    }

    std::cout << "\nObservation: vector grows by allocating a new continuous buffer, moving/copying old elements, then freeing old buffer.\n";
    return 0;
}
