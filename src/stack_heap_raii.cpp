#include <iostream>
#include <memory>
#include <vector>

struct Resource {
    int id;
    explicit Resource(int id_) : id(id_) {
        std::cout << "Resource(" << id << ") constructed\n";
    }
    ~Resource() {
        std::cout << "Resource(" << id << ") destructed\n";
    }
};

void bad_raw_pointer_example() {
    Resource* p = new Resource(1);
    // If an exception or early return happens here, delete may be skipped.
    delete p;
}

void good_raii_example() {
    auto p = std::make_unique<Resource>(2);
    std::vector<std::unique_ptr<Resource>> v;
    v.push_back(std::make_unique<Resource>(3));
    v.push_back(std::make_unique<Resource>(4));
    // Destructors are called automatically when leaving scope.
}

int main() {
    std::cout << "===== raw pointer =====\n";
    bad_raw_pointer_example();

    std::cout << "\n===== RAII with unique_ptr =====\n";
    good_raii_example();

    std::cout << "\nProgram end.\n";
    return 0;
}
