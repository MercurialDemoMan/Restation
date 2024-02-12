cmake_minimum_required(VERSION 3.5)

project(PSXEmulator)

# set c++ 17 standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# set optimization level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ggdb")

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

# add stb image write to the project
include_directories(${CMAKE_SOURCE_DIR}/deps/stb)

# include sources
add_executable(emu 
    src/Bus.cpp
    src/CPU.cpp
    src/GPU.cpp
    src/SPU.cpp
	src/main.cpp
    src/MDEC.cpp
    src/Utils.cpp
    src/CDROM.cpp
    src/Timer.cpp
    src/IOPorts.cpp
    src/SerialPort.cpp
    src/DMAChannel.cpp
    src/Peripherals.cpp
    src/DMAChannelSPU.cpp
    src/DMAChannelPIO.cpp
    src/DMAChannelOTC.cpp
    src/DMAChannelGPU.cpp
    src/RamController.cpp
    src/MemController.cpp
    src/DMAController.cpp
    src/CacheController.cpp
    src/CPUDisassembler.cpp
    src/DMAChannelCDROM.cpp
    src/DMAChannelMDECIN.cpp
    src/DMAChannelMDECOUT.cpp
    src/ExceptionController.cpp
    src/InterruptController.cpp
    deps/stb/stb_image_write.c)

# link libraries with the project
target_link_libraries(emu
    SDL2::SDL2main;
    SDL2::SDL2-static;
    fmt::fmt)
