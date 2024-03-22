cmake_minimum_required(VERSION 3.5)

project(emu)

# set c++ 20 standard 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
# set optimization level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
# set verbose compilers warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
# set debug logging verbosity
# add_compile_definitions(LOG_DEBUG_LEVEL=5)

# include stb image write headers to the project
include_directories(${CMAKE_SOURCE_DIR}/deps/stb)
# include glm headers to the project
include_directories(${CMAKE_SOURCE_DIR}/deps/glm-0.9.9.8)
# include imgui headers to the project
include_directories(${CMAKE_SOURCE_DIR}/deps/imgui-1.90.3/ ${CMAKE_SOURCE_DIR}/deps/imgui-1.90.3/backends)
# add fmt to the project
include_directories(${CMAKE_SOURCE_DIR}/deps/fmt-10.1.1/include)

# add fmt to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/fmt-10.1.1)
# add sdl2 to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/SDL2-2.28.4)
# add emulator core to the project
add_subdirectory(${CMAKE_SOURCE_DIR}/core)

# include sources
add_executable(${PROJECT_NAME} 

    # compile stb_image_write
    deps/stb/stb_image_write.c

    # compile imgui
    deps/imgui-1.90.3/imgui.cpp
    deps/imgui-1.90.3/imgui_demo.cpp
    deps/imgui-1.90.3/imgui_draw.cpp
    deps/imgui-1.90.3/imgui_tables.cpp
    deps/imgui-1.90.3/imgui_widgets.cpp
    deps/imgui-1.90.3/misc/cpp/imgui_stdlib.cpp
    deps/imgui-1.90.3/backends/imgui_impl_sdl2.cpp
    deps/imgui-1.90.3/backends/imgui_impl_sdlrenderer2.cpp
    
    # compile frontend app
    app/App.cpp
	app/main.cpp
    app/Menu.cpp
    app/Input.cpp)

# link libraries with the project
target_link_libraries(${PROJECT_NAME}
    core;
    fmt::fmt;
    SDL2::SDL2main;
    SDL2::SDL2-static;)
