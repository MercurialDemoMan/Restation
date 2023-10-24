cmake_minimum_required(VERSION 3.0)

project(PSXEmulator)

# set c++ 17 standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# set optimization level
set(CMAKE_CXX_FLAGS "$CMAKE_CXX_FLAGS" -O0 -ggdb)

# set verbose compilers warnings
set(CMAKE_CXX_FLAGS "$CMAKE_CXX_FLAGS" -Wall -Wextra)

add_executable(emu 
	main.cpp)

