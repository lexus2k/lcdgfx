#!/usr/bin/python
# -*- coding: UTF-8 -*-
#    MIT License
#
#    Copyright (c) 2018-2020, Alexey Dynda
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
###################################################################################
# Linux:
#     Fixing pip
#     curl https://bootstrap.pypa.io/get-pip.py | python2
#     sudo pip install freetype-py
# Win32:
#     pip install freetype_py-2.0.0.post6-py2.py3-none-win32.whl
#
# Get fonts
# wget https://ftp.gnu.org/gnu/freefont/freefont-ttf-20120503.zip

import re
import sys
import codecs
from modules import glcdsource
from modules import fontgenerator

def print_help_and_exit():
    sys.stderr.write("Usage: ttf_fonts.py [args] > outputFile\n")
    sys.stderr.write("args:\n")
    sys.stderr.write("      --ttf S        use ttf name as source\n")
    sys.stderr.write("      --glcd S       use glcd file as as source\n")
    sys.stderr.write("      -s <N>         font size (this is not pixels!) \n")
    sys.stderr.write("      -SB <N>        limit size in pixels to value (pixels will be cut)\n")
    sys.stderr.write("      -fh            fixed height\n")
    sys.stderr.write("      -fw            fixed width\n")
    sys.stderr.write("      -g <S> <E>     add chars group to the font\n")
    sys.stderr.write("                     where <S> - first char hex or binary code, or char symbol\n")
    sys.stderr.write("                           <E> - chars count minus 1 (integer), or char symbol\n")
    sys.stderr.write("      -f old         old format 1.7.6 and below\n")
    sys.stderr.write("      -f new         new format 1.7.8 and above\n")
    sys.stderr.write("      -d             Print demo text to console\n")
    sys.stderr.write("      -t text        Use text as demo text\n")
    sys.stderr.write("      --demo-only    Prints demo text to console and exits\n")
    sys.stderr.write("      --output-file  Saves the output to a file directly\n")
    sys.stderr.write("Examples:\n")
    sys.stderr.write("   [convert ttf font to old format]\n")
    sys.stderr.write("      ttf_fonts.py --ttf FreeSans.ttf -s 8 -f old > font.h\n")
    sys.stderr.write("   [convert ttf font to new format with demo text and print to console]\n")
    sys.stderr.write("      ttf_fonts.py --ttf FreeSans.ttf -d -f new\n")
    sys.stderr.write("   [convert GLCD font generated file to new format]\n")
    sys.stderr.write("      ttf_fonts.py --glcd font.c -f new > font.h\n")
    exit(1)

if len(sys.argv) < 2:
    print_help_and_exit()

fsize = 8
fold = False
flimit_bottom = 0
fwidth = False
fheight = False
fname = ""
fgroups = []
TTF = False
GLCD = False
demo_text = False
demo_text_ = "World!q01"
generate_font = True
source = None
output_file = None

# parse args
idx = 1
while idx < len(sys.argv):
    opt = sys.argv[idx]
    if opt == "--ttf":
        TTF = True
        idx += 1
        fname = sys.argv[idx]
    elif opt == "--glcd":
        GLCD = True
        idx += 1
        fname = sys.argv[idx]
    elif opt == "-s":
        idx += 1
        fsize = int(sys.argv[idx])
    elif opt == "-SB":
        idx +=1
        flimit_bottom = int(sys.argv[idx])
    elif opt == "-fh":
        fheight = True
    elif opt == "-fw":
        fwidth = True
    elif opt == "-f":
        idx += 1
        if sys.argv[idx] == "old":
            fold = True
    elif opt == "-g":
        idx += 1
        _start_char = sys.argv[idx]
        if re.search(r'0x\d*', _start_char) is not None:
            code = int(_start_char, 16)
            if sys.version_info < (3, 0):
                _start_char = unichr(code)
            else:
                _start_char = chr(code)
        elif _start_char.isdigit() and len(_start_char) > 1:
            if sys.version_info < (3, 0):
                _start_char = unichr(int(_start_char))
            else:
                _start_char = chr(int(_start_char))
        else:
            if sys.version_info < (3, 0):
                _start_char = _start_char.decode("utf-8")
        idx += 1
        _end_char = sys.argv[idx]
        if _end_char.isdigit():
            char_count = int(_end_char)
            if char_count > 255:
                sys.stderr.write("Number of characters in single block cannot be more than 255\n")
                exit(1)
            if sys.version_info < (3, 0):
                _end_char = unichr(ord(_start_char) + int(_end_char))
            else:
                _end_char = chr(ord(_start_char) + int(_end_char))
        else:
            if sys.version_info < (3, 0):
                _end_char = _end_char.decode("utf-8")
            if ord(_end_char) - ord(_start_char) + 1 > 255:
                sys.stderr.write("Number of characters in single block cannot be more than 255\n")
                exit(1)
        fgroups.append( (_start_char, _end_char ) )
    elif opt == "-d":
        demo_text = True
    elif opt == "-t":
        idx += 1
        demo_text_ = sys.argv[idx]
    elif opt == "--demo-only":
        generate_font = False
        demo_text = True
    elif opt == "--output-file":
        idx += 1
        output_file = sys.argv[idx]
    else:
        sys.stderr.write("Unknown option: {0}".format(opt))
        print_help_and_exit()
    idx += 1

if TTF:
    from modules import ttfsource
    source = ttfsource.TTFSource(fname, fsize)
    if len(fgroups) == 0:
        if sys.version_info < (3, 0):
            fgroups.append((' ', unichr(127)))
        else:
            fgroups.append((' ', chr(127)))
    for g in fgroups:
        source.add_chars(g[0], g[1])

if GLCD:
    source = glcdsource.GLCDSource(fname, fsize, "utf8")

if source is None:
    print_help_and_exit()

if fwidth:
    source.expand_chars_h()
if fheight:
    source.expand_chars_v()
if flimit_bottom > 0:
    source.deflate_chars_bottom( flimit_bottom )

font = fontgenerator.Generator( source )
if fold:
    source.expand_chars()
    demo_array = []
    if demo_text:
        if sys.version_info < (3, 0):
            demo_array = source.getString(demo_text_.decode("utf-8"))
        else:
            demo_array = source.getString(demo_text_)
    if generate_font:
        font.generate_fixed_old(demo_array, output_file)
else:
    demo_array = []
    if demo_text:
        if sys.version_info < (3, 0):
            demo_array = source.getString(demo_text_.decode("utf-8"))
        else:
            demo_array = source.getString(demo_text_)
    if generate_font:
        font.generate_new_format(demo_array, output_file)
