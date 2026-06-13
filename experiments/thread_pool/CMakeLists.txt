cmake_minimum_required(VERSION 3.16)

# Thread pool experiment subproject
add_executable(thread_pool ${CMAKE_SOURCE_DIR}/src/thread_pool.cpp)

target_compile_features(thread_pool PUBLIC cxx_std_17)
target_include_directories(thread_pool PRIVATE ${CMAKE_SOURCE_DIR}/src)
