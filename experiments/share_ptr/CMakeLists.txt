cmake_minimum_required(VERSION 3.16)

add_executable(share_ptr ${CMAKE_SOURCE_DIR}/src/share_ptr.cpp)

target_compile_features(share_ptr PUBLIC cxx_std_17)
target_include_directories(share_ptr PRIVATE ${CMAKE_SOURCE_DIR}/src)

install(TARGETS share_ptr
	RUNTIME DESTINATION ${CPP_LAB_RUNTIME_INSTALL_DIR}
)
