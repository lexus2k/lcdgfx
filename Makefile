#    MIT License
#
#    Copyright (c) 2019-2022, Alexey Dynda
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

.PHONY: docs library help check cppcheck format autogenerate

ARCH ?= linux
SDL_EMULATION ?= n
CPPFLAGS += -I./src -I./tools/sdl $(EXTRA_CPPFLAGS)
ifeq ($(SDL_EMULATION),y)
    CPPFLAGS += -DSDL_EMULATION
endif
CXXFLAGS += -std=c++11
BLD ?= $(shell pwd)/bld

help:
	@echo "============== LCDGFX library ================"
	@echo "make docs          generates documentation"
	@echo "make help          prints this help"
	@echo "make library       build library"
	@echo "make ssd1306_sdl   build SDL emulation library"
	@echo "make cppcheck      run cppcheck tests"
	@echo "make check SDL_EMULATION=y       run unit tests"
	@echo "make format        perform style formatting"
	@echo ""
	@echo "to build examples use scripts in tools subdir"
	@echo "ARCH=<arch>        specify architecture: avr, linux, esp32"

docs:
	@cd src && doxygen doxygen.cfg 1>/dev/null
	@echo "[DONE] check docs folder"

library:
	mkdir -p $(BLD)
	make -C ./src/ -f Makefile.$(ARCH) SDL_EMULATION=$(SDL_EMULATION) EXTRA_CPPFLAGS="$(EXTRA_CPPFLAGS)" BLD=$(BLD)

ssd1306_sdl:
	mkdir -p $(BLD)
	$(MAKE) -C ./tools/sdl -f Makefile.$(ARCH) SDL_EMULATION=$(SDL_EMULATION) EXTRA_CPPFLAGS="$(EXTRA_CPPFLAGS)" BLD=$(BLD)

include Makefile.cpputest

cppcheck:
	@cppcheck --force \
	    --enable=warning,style,performance,portability \
	    --suppress=ctuOneDefinitionRuleViolation \
            --suppress=unusedStructMember \
	    --suppress=information -q \
	    --error-exitcode=1 \
	    -I src \
	    src
	@echo "[DONE]"


clean:
	make -C ./src/ -f Makefile.$(ARCH) clean
	make -C ./tools/sdl -f Makefile.$(ARCH) clean
	rm -rf gmon.out

format:
	@tools/style_format.sh
	@echo "[DONE] Code style formatting"

autogenerate:
	@cd tools && ./lcd_code_generator.py -c all
	@tools/style_format.sh
	@echo "[DONE] Code generation"

coverage:
	$(MAKE) ARCH=linux SDL_EMULATION=y EXTRA_CPPFLAGS="--coverage" check
	# COVERALLS_REPO_TOKEN=4Ia7t9YDo22zQcFEnMYLh1tiWCRlBJlhk coveralls -b . \
	#    --exclude docs --exclude extra --exclude unittest --exclude bld --exclude tools --exclude examples \
	#    --gcov-options '\-lp' --dryrun
	lcov --base-directory src --directory src -c -o lcov.info
	lcov --remove lcov.info "*unittest*" -o lcov.info # remove output for external libraries
	lcov --remove lcov.info "/usr/include*" -o lcov.info # remove output for external libraries
	# -rm -rf test_coverage
	# genhtml -o test_coverage -t "tinyproto test coverage" --num-spaces 4 lcov.info
