cmake_minimum_required(VERSION 3.16)
project(cpp_memory_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(memory_layout src/memory_layout.cpp)
add_executable(stack_heap_raii src/stack_heap_raii.cpp)
add_executable(vector_growth src/vector_growth.cpp)
add_executable(mmap_file src/mmap_file.cpp)
add_executable(shared_memory_writer src/shared_memory_writer.cpp)
add_executable(shared_memory_reader src/shared_memory_reader.cpp)
add_executable(page_faults src/page_faults.cpp)

if (UNIX AND NOT APPLE)
    target_link_libraries(shared_memory_writer PRIVATE rt pthread)
    target_link_libraries(shared_memory_reader PRIVATE rt pthread)
endif()
