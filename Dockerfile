FROM ubuntu:22.04
LABEL authors="zipfiles"

RUN mv /etc/apt/sources.list /etc/apt/sources.list.bak
COPY sources.list /etc/apt/

RUN apt-get update && apt-get install -y \
    cmake \
    git \
    gdb \
    vim \
    libgtest-dev \
    clang \
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
    libjson-glib-dev \
    libjsoncpp-dev 

RUN apt-get install -y \
numactl \
lsof


    
COPY tools/perf /usr/bin/perf

# install clang and all clang tools
RUN apt-get install -y \
    lsb-release \
    software-properties-common \
    gnupg
RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 18

RUN  cd /usr/src/gtest \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make \
    && cp lib/libgtest*.a /usr/local/lib

RUN locale-gen en_US.UTF-8
RUN update-locale LANG=en_US.UTF-8
RUN echo "LANGUAGE=zh_CN:zh:en_US:en" >> /etc/default/locale
