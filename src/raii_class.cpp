// 不可拷贝的文件句柄封装
// 句柄本质上是一个资源所有权。

/* 允许移动
    C++ 的 private 限制是“类级别”的，不是“对象级别”的
    只要在 File 类的成员函数内部，就可以访问 任意 File 对象 的私有成员，不只是 this 这个对象
    private 是类访问控制，不是对象访问控制。File 的成员函数可以访问任何 File 对象的私有成员，所以移动构造函数里可以访问 other.handle_。
    
    std::move 本身不移动资源，它只是把对象转换成右值引用；真正执行资源转移的是移动构造函数或移动赋值函数。

    noexcept 告诉编译器和标准库：这个函数承诺不会抛异常。标准库容器更愿意移动不会抛异常的对象
*/

#include <cstdio>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <cstddef>

class FileHandle {
public:
    explicit FileHandle(const char* path, const char* mode = "w")
        : fhandle_(std::fopen(path, mode)) {
        if (!fhandle_) {
            throw std::runtime_error("Open file failed!");
        }
    }

    ~FileHandle() noexcept {
        if (fhandle_) {
            std::fclose(fhandle_);
        }
    }

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept
        : fhandle_(other.fhandle_) {
        other.fhandle_ = nullptr;
    }

    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            reset();
            fhandle_ = other.fhandle_;
            other.fhandle_ = nullptr;
        }
        return *this;
    }

    void write(const char* data) {
        check_valid();

        if (std::fputs(data, fhandle_) == EOF) {
            throw std::runtime_error("Write file failed");
        }
    }

    bool read(char* buffer, std::size_t size) {
        check_valid();

        if (std::fgets(buffer, static_cast<int>(size), fhandle_) == nullptr) {
            if (std::feof(fhandle_)) {
                return false;
            }
            throw std::runtime_error("Read file failed");
        }

        return true;
    }

    void flush() {
        check_valid();

        if (std::fflush(fhandle_) != 0) {
            throw std::runtime_error("Flush file failed");
        }
    }

    void reset(FILE* newHandle = nullptr) noexcept {
        if (fhandle_ != newHandle) {
            if (fhandle_) {
                std::fclose(fhandle_);
            }
            fhandle_ = newHandle;
        }
    }

    FILE* get() const noexcept {
        return fhandle_;
    }

    FILE* release() noexcept {
        FILE* tmp = fhandle_;
        fhandle_ = nullptr;
        return tmp;
    }

private:
    void check_valid() const {
        if (!fhandle_) {
            throw std::runtime_error("Invalid file handle");
        }
    }

private:
    FILE* fhandle_ = nullptr;
};

int main() {
    try {
        // 1. 写文件：测试构造、write、移动构造、移动赋值
        {
            FileHandle file1("example.txt", "w");

            file1.write("line 1: hello world\n");

            // 移动构造：file2 接管 file1 的 FILE*
            FileHandle file2(std::move(file1));
            file2.write("line 2: after move construct\n");

            // file3 先打开另一个文件
            FileHandle file3("temp.txt", "w");
            file3.write("this is temp file\n");

            // 移动赋值：
            // file3 原本管理的 temp.txt 会被 reset() 关闭
            // 然后 file3 接管 file2 的 FILE*
            file3 = std::move(file2);
            file3.write("line 3: after move assignment\n");

            // 手动 flush，确保写入落盘
            file3.flush();

            // 作用域结束：
            // file3 析构，关闭 example.txt
            // file2 已被移动，内部是 nullptr
            // file1 已被移动，内部是 nullptr
        }

        // 2. 读文件：测试 read
        {
            FileHandle file_read("example.txt", "r");

            char buffer[256];

            std::cout << "read example.txt:" << std::endl;

            while (file_read.read(buffer, sizeof(buffer))) {
                std::cout << buffer;
            }

            std::cout << std::endl;
        }

        // 3. 测试 get：只观察，不转移所有权
        {
            FileHandle file("example.txt", "r");

            FILE* raw = file.get();

            if (raw != nullptr) {
                std::cout << "get() success, raw FILE* is not null" << std::endl;
            }

            // file 仍然拥有 FILE*
            // 离开作用域时 file 会自动 fclose
        }

        // 4. 测试 release：释放所有权，交给外部自己管理
        {
            FileHandle file("example.txt", "r");

            FILE* raw = file.release();

            if (raw != nullptr) {
                std::cout << "release() success, ownership transferred" << std::endl;

                // release 之后，FileHandle 不再负责关闭
                // 所以这里必须手动 fclose，否则泄漏
                std::fclose(raw);
            }

            // file 内部已经是 nullptr
            // 离开作用域时不会再次 fclose
        }

        // 5. 测试 reset：关闭当前文件，重新绑定新的 FILE*
        {
            FileHandle file("example.txt", "r");

            FILE* new_file = std::fopen("temp.txt", "r");
            if (!new_file) {
                throw std::runtime_error("Open temp.txt failed");
            }

            // reset 会先关闭 example.txt
            // 然后接管 temp.txt
            file.reset(new_file);

            char buffer[256];

            std::cout << "read temp.txt after reset:" << std::endl;

            while (file.read(buffer, sizeof(buffer))) {
                std::cout << buffer;
            }

            std::cout << std::endl;

            // 离开作用域时，file 自动关闭 temp.txt
        }

    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}