#!/usr/bin/env bash

cd build

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
ldconfig -p | grep SDL

wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy*.AppImage

./linuxdeploy-x86_64.AppImage --appimage-extract-and-run --appdir AppDir --output appimage

