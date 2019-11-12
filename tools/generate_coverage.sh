#!/bin/sh

make EXTRA_CPPFLAGS="--coverage" SDL_EMULATION=y check
lcov -t "lcdgfx" -o ./bld/lcdgfx.info -c -d ./src
genhtml -o ./bld/report ./bld/lcdgfx.info
