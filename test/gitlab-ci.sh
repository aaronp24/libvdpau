#!/bin/sh
set -xe

meson setup -Dprefix=/usr -Ddri2=true -Ddocumentation=true -Dwerror=true build/
ninja -C build/ install test
cp -av build/doc/html public/
