FROM ubuntu:jammy

RUN apt -y update
RUN apt -y install gcc g++ clang
RUN apt -y install make cmake
RUN apt -y install vim bash
RUN apt -y install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libglew-dev
RUN apt -y install mingw-w64
RUN echo 1 | update-alternatives --config i686-w64-mingw32-gcc
RUN echo 1 | update-alternatives --config i686-w64-mingw32-g++
RUN echo 1 | update-alternatives --config x86_64-w64-mingw32-gcc
RUN echo 1 | update-alternatives --config x86_64-w64-mingw32-g++
WORKDIR /app
