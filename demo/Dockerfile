FROM ubuntu:22.04

RUN mv /etc/apt/sources.list /etc/apt/sources.list.bak 
COPY sources.list /etc/apt/

RUN apt-get update && apt-get install -y \
    git \
    vim \
    tree \
    sudo \
    wget \
    curl \
    lsof \
    fonts-wqy-microhei \
    locales 

RUN locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 && \
    echo "LANGUAGE=zh_CN:zh:en_US:en" >> /etc/default/locale