#!/bin/sh
set -xe

meson setup build/
meson configure -Dprefix=/usr -Ddri2=true -Ddocumentation=true build/
ninja -C build/ install test
cp -av build/doc/html public/
