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
    liblog4cpp5-dev \
    libcrypto++-dev 

RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 18 && \
    apt-get update && apt-get install -y clang-18 clang-tools-18 clangd-18 clang-format-18 clang-tidy-18 lldb-18 lld-18 libc++-18-dev libc++abi-18-dev libclang-18-dev libclang-common-18-dev libclang-cpp18 libclang1-18 liblldb-18-dev libomp-18-dev && \
    rm llvm.sh && \
    rm -rf /var/lib/apt/lists/*

# 把 clang18 设置为默认的编译器
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100 && \
    update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100 && \
    update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100 && \
    update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-18 100 && \
    update-alternatives --install /usr/bin/lldb lldb /usr/bin/lldb-18 100 && \
    update-alternatives --install /usr/bin/lld lld /usr/bin/lld-18 100 


# 复制 perf 工具
COPY tools/perf /usr/bin/perf

# 编译并安装 Google Test
RUN cd /usr/src/gtest && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang .. && \
    make && \
    cp lib/libgtest*.a /usr/local/lib

# 设置本地化
RUN locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    echo "LANGUAGE=zh_CN:zh:en_US:en" >> /etc/default/locale
