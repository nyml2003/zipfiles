FROM ubuntu:22.04
LABEL authors="zipfiles"

RUN mv /etc/apt/sources.list /etc/apt/sources.list.bak
COPY sources.list /etc/apt/

RUN apt update && apt install -y \
    gcc \
    cmake \
    g++ \
    git \
    gdb \
    vim \
    libgtest-dev \
    clang-format \
    tree \
    sudo \
    wget \
    curl \
    pkg-config \
    libgtk-3-dev \
    libwebkit2gtk-4.1-dev \
    fonts-wqy-microhei \
    locales \
    valgrind \
    libjson-glib-dev


RUN  cd /usr/src/gtest \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make \
    && cp lib/libgtest*.a /usr/local/lib

RUN locale-gen en_US.UTF-8
RUN update-locale LANG=en_US.UTF-8
RUN echo "LANGUAGE=zh_CN:zh:en_US:en" >> /etc/default/locale
