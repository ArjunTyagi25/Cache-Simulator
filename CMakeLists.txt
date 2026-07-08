cmake_minimum_required(VERSION 4.3)

project(Cache-Simulator)

add_executable(cache-simulator 
	lib/main.cpp 
	lib/line.cpp
	lib/cache.cpp)

target_include_directories(cache-simulator PRIVATE
	include)
