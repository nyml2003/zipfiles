FROM ubuntu:22.04
LABEL authors="zipfiles"

# 备份并替换 sources.list
RUN mv /etc/apt/sources.list /etc/apt/sources.list.bak 
COPY sources.list /etc/apt/

# 更新并安装必要的软件包
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
    libjsoncpp-dev \
    numactl \
    lsof \
    lsb-release \
    software-properties-common \
    gnupg \
    liblog4cpp5-dev


RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 18 && \
    rm llvm.sh && \
    rm -rf /var/lib/apt/lists/*

# 复制 perf 工具
COPY tools/perf /usr/bin/perf

# 编译并安装 Google Test
RUN cd /usr/src/gtest && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    cp lib/libgtest*.a /usr/local/lib

# 设置本地化
RUN locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    echo "LANGUAGE=zh_CN:zh:en_US:en" >> /etc/default/locale