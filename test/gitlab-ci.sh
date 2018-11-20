#!/bin/sh
set -x

meson setup build/
meson configure -Dprefix=/usr -Ddri2=true build/
ninja -C build/ install test
