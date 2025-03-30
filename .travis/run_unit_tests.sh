#!/bin/sh

if [ "$1" = "clang" ]; then
    CPPFLAGS="-I/usr/include/SDL2" CC=clang CXX=clang++ LD=clang++ make ARCH=linux SDL_EMULATION=y check
else
    make ARCH=linux EXTRA_CPPFLAGS="--coverage" SDL_EMULATION=y check
fi
