cmake_minimum_required(VERSION 2.6)

# Project name
project(rew_to_json)

# Paths
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Include directories
include_directories("${PROJECT_SOURCE_DIR}")

# Find all source files
file(GLOB files ${PROJECT_SOURCE_DIR}/*.cpp)

# Static build for Windows (disable for now)
if (WIN32)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	set(BUILD_SHARED_LIBS OFF)
	set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

	# Not needed since we're not searching for dependencies
	#set(CMAKE_MODULE_PATH "/usr/i686-w64-mingw32/" ${CMAKE_MODULE_PATH})
endif (WIN32)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic-errors -O3")

# Executable
add_executable(${PROJECT_NAME} ${files})