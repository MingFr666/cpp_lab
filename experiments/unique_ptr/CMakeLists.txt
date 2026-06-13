cmake_minimum_required(VERSION 3.16)

# UniquePtr experiment subproject
add_executable(unique_ptr ${CMAKE_SOURCE_DIR}/src/UniquePtr.cpp)

target_compile_features(unique_ptr PUBLIC cxx_std_17)
target_include_directories(unique_ptr PRIVATE ${CMAKE_SOURCE_DIR}/src)
