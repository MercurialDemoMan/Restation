cmake_minimum_required(VERSION 3.5)

project(PSXEmulator)

# set c++ 17 standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# set optimization level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")

# set verbose compilers warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# set debug logging verbosity
add_compile_definitions(LOG_DEBUG_LEVEL=5)

# add fmt to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/fmt-10.1.1)

# add glm to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/glm)

# add sdl2 to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/SDL2-2.28.4)

# include sources
add_executable(emu 
	src/main.cpp
    src/CPU.cpp
    src/GPU.cpp
    src/SPU.cpp
    src/MDEC.cpp
    src/CDROM.cpp
    src/Timer.cpp
    src/RamController.cpp
    src/MemController.cpp
    src/Peripherals.cpp
    src/DMAController.cpp
    src/CacheController.cpp
    src/CPUDisassembler.cpp
    src/ExceptionController.cpp
    src/InterruptController.cpp
    src/Bus.cpp)

# link libraries with the project
target_link_libraries(emu
    SDL2::SDL2main;
    SDL2::SDL2-static;
    fmt::fmt)
