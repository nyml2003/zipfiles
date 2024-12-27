#!/bin/bash

# 定义一个函数来检查包是否已经安装
check_and_install() {
    dpkg -s "$1" &>/dev/null
    if [ $? -eq 0 ]; then
        echo "$1 is already installed."
    else
        echo "Installing $1..."
        apt-get install -y "$1"
    fi
}

# 更新包列表
apt-get update

# 检查并安装运行时版本的库
check_and_install libgtk-3-0
check_and_install libwebkit2gtk-4.1-0
check_and_install libjsoncpp25
check_and_install liblog4cpp5v5
check_and_install libcrypto++8

# 复制到默认路径
default_path="/usr/local/zipfiles/.zip"
mkdir -p $default_path
echo "Copying files to $default_path"
# 复制脚本所在目录，而不是当前目录, 复制一个，echo一句话
for file in $(ls $(dirname $0)); do
    cp -r $(dirname $0)/$file $default_path
    echo "Copying $file"
done

echo "All required packages and locales are installed or already present."