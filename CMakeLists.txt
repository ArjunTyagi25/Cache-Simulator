cmake_minimum_required(VERSION 4.3)

project(Cache-Simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} 
	src/main.cpp
	src/memory.cpp 
	src/line.cpp
	src/page.cpp
	src/cache.cpp
)

target_include_directories(${PROJECT_NAME} 
	PRIVATE
	include)

target_compile_options(${PROJECT_NAME}
	PRIVATE
	-Wall
	-Wextra
	-Wpedantic
)
