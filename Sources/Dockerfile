FROM archlinux:base-devel

RUN pacman -Syu --noconfirm
RUN pacman -S "gcc>=12.1" --noconfirm
RUN pacman -S "make" --noconfirm
RUN pacman -S "cmake>=3.20.0" --noconfirm
RUN pacman -S "vim" --noconfirm
RUN pacman -S "bash" --noconfirm
RUN pacman -S "mingw-w64" --noconfirm

WORKDIR /app
