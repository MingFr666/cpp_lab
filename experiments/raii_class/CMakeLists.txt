cmake_minimum_required(VERSION 3.16)

# RAII class experiment subproject
add_executable(raii_class ${CMAKE_SOURCE_DIR}/src/raii_class.cpp)

target_compile_features(raii_class PUBLIC cxx_std_17)
target_include_directories(raii_class PRIVATE ${CMAKE_SOURCE_DIR}/src)

install(TARGETS raii_class
	RUNTIME DESTINATION ${CPP_LAB_RUNTIME_INSTALL_DIR}
)
