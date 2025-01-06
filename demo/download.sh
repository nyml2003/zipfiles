#!/bin/bash
VERSION=0.0.16
wget https://github.com/nyml2003/zipfiles/releases/download/$VERSION/zipfiles.tar.gz
mkdir -p zipfiles
tar -xzvf zipfiles.tar.gz -C ./zipfiles
cd zipfiles
sed -i 's/\r$//' start.sh
sed -i 's/\r$//' install.sh
sed -i 's/\r$//' uninstall.sh