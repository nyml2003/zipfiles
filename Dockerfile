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
    tree


RUN  cd /usr/src/gtest \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make \
    && cp lib/libgtest*.a /usr/local/lib
