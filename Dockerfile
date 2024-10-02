FROM ubuntu:22.04
LABEL authors="zipfiles"

RUN mv /etc/apt/sources.list /etc/apt/sources.list.bak
COPY sources.list /etc/apt/

RUN apt-get update && apt-get install -y \
    gcc \
    cmake \
    g++ \
    git \
    gdb \
    vim \
    libgtest-dev \
    wget

# install perf
RUN apt-get install -y \
    bc \
    build-essential \
    flex \
    bison \
    dwarves \
    libssl-dev \
    libelf-dev
RUN apt-get install -y \
    binutils-dev \
    debuginfod \
    libaio-dev \
    libbabeltrace-dev \
    libcap-dev \
    libdw-dev \
    libdwarf-dev \
    libelf-dev \
    libiberty-dev \
    liblzma-dev \
    libnuma-dev \
    libperl-dev \
    libpfm4-dev \
    libslang2-dev \
    libssl-dev \
    libtraceevent-dev \
    libunwind-dev \
    libzstd-dev \
    libzstd1 \
    python-setuptools \
    python3 \
    python3-dev \
    systemtap-sdt-dev \
    zlib1g-dev
COPY tools/perf /usr/bin/perf

# install clang and all clang tools
RUN apt-get install -y \
    lsb-release \
    software-properties-common \
    gnupg
RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh all 18

RUN cd /usr/src/gtest \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make \
    && cp lib/libgtest*.a /usr/local/lib
