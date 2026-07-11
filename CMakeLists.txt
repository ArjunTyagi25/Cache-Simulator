cmake_minimum_required(VERSION 4.3)

project(Cache-Simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} 
	src/main.cpp
	src/memory_subsystem.cpp
	src/memory/memory.cpp 
	src/memory/page.cpp
	src/memory/memory_line.cpp
	src/cache/cache.cpp
	src/cache/cache_line.cpp
)

target_include_directories(${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include/)

target_compile_options(${PROJECT_NAME}
	PRIVATE
	-Wall
	-Wextra
	-Wpedantic
)
