cmake_minimum_required(VERSION 3.16)
project(cpp_lab LANGUAGES CXX)

# For single-config generators (e.g. Unix Makefiles), default to Debug for source-level debugging.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

# Default install prefix to project-relative ./install unless user overrides it.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif()

# Runtime install directory for executables; can be overridden by -DCPP_LAB_RUNTIME_INSTALL_DIR=...
set(CPP_LAB_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}" CACHE STRING "Runtime install directory for cpp_lab executables")

# Optionally enable/disable building experiments
option(BUILD_EXPERIMENTS "Build experiment subprojects" ON)

if(BUILD_EXPERIMENTS)
	add_subdirectory(experiments/thread_pool)
	add_subdirectory(experiments/unique_ptr)
	add_subdirectory(experiments/raii_class)
	add_subdirectory(experiments/share_ptr)
	add_subdirectory(experiments/spin_lock)
endif()


