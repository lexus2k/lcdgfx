#    MIT License
#
#    Copyright (c) 2019, Alexey Dynda
#
#    Permission is hereby granted, free of charge, to any person obtaining a copy
#    of this software and associated documentation files (the "Software"), to deal
#    in the Software without restriction, including without limitation the rights
#    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the Software is
#    furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in all
#    copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#    SOFTWARE.
#
#################################################################

default: library

.PHONY: docs library help check cppcheck

ARCH ?= linux
SDL_EMULATION ?= n
CPPFLAGS += -I./src -I./tools/sdl $(EXTRA_CPPFLAGS)
ifeq ($(SDL_EMULATION),y)
    CPPFLAGS += -DSDL_EMULATION
endif
CPPFLAGS += -std=c++11
BLD ?= $(shell pwd)/bld

help:
	@echo "============== LCDGFX library ================"
	@echo "make docs          generates documentation"
	@echo "make help          prints this help"
	@echo "make cppcheck      run cppcheck tests"
	@echo ""
	@echo "to build examples use scripts in tools subdir"
	@echo "ARCH=<arch>        specify architecture: avr, linux, esp32"

docs:
	@cd src && doxygen doxygen.cfg 1>/dev/null
	@echo "[DONE] check docs folder"

library:
	make -C ./src/ -f Makefile.$(ARCH) SDL_EMULATION=$(SDL_EMULATION) EXTRA_CPPFLAGS="$(EXTRA_CPPFLAGS)" BLD=$(BLD)

ssd1306_sdl:
	$(MAKE) -C ./tools/sdl -f Makefile.linux SDL_EMULATION=$(SDL_EMULATION) EXTRA_CPPFLAGS="$(EXTRA_CPPFLAGS)" BLD=$(BLD)

include Makefile.cpputest

cppcheck:
	@cppcheck --force \
	    --enable=warning,style,performance,portability \
	    --suppress=information \
	    -q -I ./src \
	    --error-exitcode=1 .
	@echo "[DONE]"


clean:
	make -C ./src/ -f Makefile.$(ARCH) clean
	make -C ./tools/sdl -f Makefile.linux clean
	rm -rf gmon.out
