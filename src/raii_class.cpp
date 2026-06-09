// 不可拷贝的文件句柄封装
//句柄本质上是一个 资源所有权。
#include <cstdio>
#include <stdexcept>


class File {
private:
    FILE* handle_;
public:
    explicit File(const char* filename): handle_(std::fopen(filename, "w")) {
        if (!handle_) {
            throw std::runtime_error("Failed to open file");
        }
    }
    ~File() {
        if (handle_) {
            std::fclose(handle_);
        }
    }
    // 禁止拷贝
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    // 允许移动
    // C++ 的 private 限制是“类级别”的，不是“对象级别”的
    // 只要在 File 类的成员函数内部，就可以访问 任意 File 对象 的私有成员，不只是 this 这个对象
    // private 是类访问控制，不是对象访问控制。File 的成员函数可以访问任何 File 对象的私有成员，所以移动构造函数里可以访问 other.handle_。
    
    //std::move 是触发移动赋值的“语法开关”；
    // 移动赋值函数才是真正执行资源转移的代码。
    File(File&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr; // 转移所有权后置空原句柄
    }
    File& operator=(File&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                std::fclose(handle_); // 释放当前资源
            }
            handle_ = other.handle_; // 转移所有权
            other.handle_ = nullptr; // 转移后置空原句柄
        }
        return *this;
    }

    void write(const char* data) {
        if (!handle_) {
            throw std::runtime_error("File not open");
        }
        std::fputs(data, handle_);
    }
};

int main() {
    File file1("example.txt");
    file1.write("Hello, World!\n");

    // File file2 = file1; // 编译错误：拷贝构造被删除
    File file3 = std::move(file1); // 移动构造
    file3.write("This is a moved file handle.\n");

    File file4(std::move(file3)); // 再次移动
    file4.write("This is another moved file handle.\n");

    return 0;
}