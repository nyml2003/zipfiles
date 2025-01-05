#!/bin/bash
VERSION=0.0.10
wget https://github.com/nyml2003/zipfiles/releases/download/$VERSION/zipfiles.tar.gz
tar -xzvf zipfiles.tar.gz
cd app/release
sed -i 's/\r$//' start.sh
sed -i 's/\r$//' install.sh
sed -i 's/\r$//' uninstall.sh