cmake_minimum_required(VERSION 3.16)

# spin_lock experiment subproject
add_executable(spin_lock ${CMAKE_SOURCE_DIR}/src/spin_lock.cpp)

target_compile_features(spin_lock PUBLIC cxx_std_17)
target_include_directories(spin_lock PRIVATE ${CMAKE_SOURCE_DIR}/src)

install(TARGETS spin_lock
	RUNTIME DESTINATION ${CPP_LAB_RUNTIME_INSTALL_DIR}
)
