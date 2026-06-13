# Cpp Lab

A small C++17 lab project for learning C++.

这是一个用于 C++ 小实验的仓库，示例包括线程池、智能指针、内存布局等。

快速使用说明（CMake）：

要求：CMake >= 3.16，支持 C++17 编译器。

1) 配置并构建所有默认实验：

```bash
cmake -S . -B build
cmake --build build
```

2) 只构建某个实验目标（例如 `thread_pool` 或 `unique_ptr`）：

```bash
cmake -S . -B build
cmake --build build --target thread_pool
```

3) 禁用所有 `experiments` 子项目（如果只想编译其他目标）：

```bash
cmake -S . -B build -DBUILD_EXPERIMENTS=OFF
cmake --build build
```

4) 运行可执行文件：

构建后，可执行文件位于构建目录下（例如 `build/` 或 `build/experiments/<name>/`），可直接运行：

```bash
# 在构建目录中查找并运行
find build -type f -executable -name thread_pool -print -exec {} \;
```

或者直接指定路径运行：

```bash
./build/experiments/thread_pool/thread_pool
```

5) 添加新的实验步骤：

- 在 `src/` 添加你的实现文件，例如 `src/my_experiment.cpp`。
- 在 `experiments/` 下新建子目录 `my_experiment/` 并添加 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.16)
add_executable(my_experiment ${CMAKE_SOURCE_DIR}/src/my_experiment.cpp)
target_compile_features(my_experiment PUBLIC cxx_std_17)
target_include_directories(my_experiment PRIVATE ${CMAKE_SOURCE_DIR}/src)
```

- 将子目录加入顶层 `CMakeLists.txt`：

```cmake
add_subdirectory(experiments/my_experiment)
```

这样你就可以使用 `cmake --build build --target my_experiment` 来单独构建并运行。

更多信息请参阅仓库源代码目录结构。