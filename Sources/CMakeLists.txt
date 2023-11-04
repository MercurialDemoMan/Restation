cmake_minimum_required(VERSION 3.5)

project(PSXEmulator)

# set c++ 17 standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# set optimization level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")

# set verbose compilers warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# add glm to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/glm)

# set current SDL2 path to our local project folder
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/deps/SDL2)

# find the sdl package
find_package(SDL2 REQUIRED)

# add the sdl2 include directory to the project
include_directories(${SDL2_INCLUDE_DIRS})

# include sources
add_executable(emu 
	src/main.cpp
    src/CPU.cpp
    src/GPU.cpp
    src/SPU.cpp
    src/MDEC.cpp
    src/CDROM.cpp
    src/Timer.cpp
    src/Peripherals.cpp
    src/DMAController.cpp
    src/InterruptController.cpp
    src/Bus.cpp)

# link executable with the project
target_link_libraries(emu
    ${SDL2_LIBRARIES})
