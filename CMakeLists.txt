cmake_minimum_required(VERSION 3.16)
project(cpp_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optionally enable/disable building experiments
option(BUILD_EXPERIMENTS "Build experiment subprojects" ON)

if(BUILD_EXPERIMENTS)
	add_subdirectory(experiments/thread_pool)
	add_subdirectory(experiments/unique_ptr)
endif()


