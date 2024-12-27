#!/bin/bash
default_path="/usr/local/zipfiles/.zip"
echo "Starting the application"
nohup $default_path/server &
$default_path/client