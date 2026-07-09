cmake_minimum_required(VERSION 4.3)

project(Cache-Simulator)

add_executable(cache-simulator 
	src/main.cpp
	src/main_memory.cpp 
	src/line.cpp
	src/page.cpp
	src/cache.cpp)

target_include_directories(cache-simulator PRIVATE
	include)
