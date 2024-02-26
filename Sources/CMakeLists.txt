cmake_minimum_required(VERSION 3.5)

project(PSXEmulator)

# set c++ 20 standard 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# set optimization level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ggdb")

# set verbose compilers warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# set debug logging verbosity
# add_compile_definitions(LOG_DEBUG_LEVEL=5)

# add fmt to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/fmt-10.1.1)

# add glm to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/glm-0.9.9.8)

# add sdl2 to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/SDL2-2.28.4)

# add stb image write to the project
include_directories(${CMAKE_SOURCE_DIR}/deps/stb)

# add imgui to the project
include_directories(${CMAKE_SOURCE_DIR}/deps/imgui-1.90.3/ ${CMAKE_SOURCE_DIR}/deps/imgui-1.90.3/backends)

# include sources
add_executable(emu 

    # compile stb_image_write
    deps/stb/stb_image_write.c

    # compile imgui
    deps/imgui-1.90.3/imgui_demo.cpp
    deps/imgui-1.90.3/imgui_draw.cpp
    deps/imgui-1.90.3/imgui_tables.cpp
    deps/imgui-1.90.3/imgui_widgets.cpp
    deps/imgui-1.90.3/imgui.cpp
    deps/imgui-1.90.3/backends/imgui_impl_sdl2.cpp
    deps/imgui-1.90.3/backends/imgui_impl_sdlrenderer2.cpp

    # compile core
    core/Bus.cpp
    core/CPU.cpp
    core/GPU.cpp
    core/SPU.cpp
    core/GTE.cpp
    core/MDEC.cpp
    core/Disc.cpp
    core/Utils.cpp
    core/CDROM.cpp
    core/Timer.cpp
    core/IOPorts.cpp
    core/SerialPort.cpp
    core/DMAChannel.cpp
    core/Peripherals.cpp
    core/DMAChannelSPU.cpp
    core/DMAChannelPIO.cpp
    core/DMAChannelOTC.cpp
    core/DMAChannelGPU.cpp
    core/RamController.cpp
    core/MemController.cpp
    core/DMAController.cpp
    core/CacheController.cpp
    core/CPUDisassembler.cpp
    core/DMAChannelCDROM.cpp
    core/DMAChannelMDECIN.cpp
    core/DMAChannelMDECOUT.cpp
    core/ExceptionController.cpp
    core/InterruptController.cpp
    core/PeripheralsDigitalController.cpp
    core/PeripheralsMemoryCardController.cpp
    
    # compile frontend app
    app/App.cpp
	app/main.cpp
    app/Menu.cpp
    app/Input.cpp)

# link libraries with the project
target_link_libraries(emu
    SDL2::SDL2main;
    SDL2::SDL2-static;
    fmt::fmt)
