#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>

int g_init = 123;                  // .data
int g_uninit;                      // .bss
const int g_const = 456;           // read-only data, usually .rodata
static int g_static_init = 789;    // .data, internal linkage

struct Empty {};

struct Plain {
    char c;
    int i;
    double d;
};

struct WithVirtual {
    virtual void f() {}
    int x = 1;
};

void print_addr(const char* name, const void* p) {
    std::cout << std::left << std::setw(24) << name << " address = " << p << '\n';
}

void stack_func() {
    int local = 10;
    print_addr("stack local", &local);
}

int main() {
    int stack_var = 1;
    int* heap_var = new int(2);
    static int local_static = 3;
    const char* literal = "hello literal";
    std::string s = "hello string";

    std::cout << "===== sizes =====\n";
    std::cout << "sizeof(char)        = " << sizeof(char) << '\n';
    std::cout << "sizeof(int)         = " << sizeof(int) << '\n';
    std::cout << "sizeof(double)      = " << sizeof(double) << '\n';
    std::cout << "sizeof(void*)       = " << sizeof(void*) << " bytes\n";
    std::cout << "sizeof(Empty)       = " << sizeof(Empty) << " byte\n";
    std::cout << "sizeof(Plain)       = " << sizeof(Plain) << " bytes\n";
    std::cout << "sizeof(WithVirtual) = " << sizeof(WithVirtual) << " bytes\n";

    std::cout << "\n===== addresses =====\n";
    print_addr("function main", reinterpret_cast<void*>(&main));
    print_addr("global init", &g_init);
    print_addr("global uninit", &g_uninit);
    print_addr("global const", &g_const);
    print_addr("global static init", &g_static_init);
    print_addr("local static", &local_static);
    print_addr("stack var", &stack_var);
    stack_func();
    print_addr("heap int", heap_var);
    print_addr("string object stack", &s);
    print_addr("string buffer", s.data());
    print_addr("string literal", literal);

    std::cout << "\nTip: run `cat /proc/" << getpid() << "/maps` in another terminal on Linux to inspect process VMAs.\n";

    delete heap_var;
    return 0;
}
