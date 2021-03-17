# -*- coding: UTF-8 -*-
#    MIT License
#
#    Copyright (c) 2018-2021, Alexey Dynda
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
# Fixing pip
# curl https://bootstrap.pypa.io/get-pip.py | python2
# sudo pip install freetype-py
#
# Get fonts
# wget https://ftp.gnu.org/gnu/freefont/freefont-ttf-20120503.zip

from __future__ import print_function
import sys
import io

# Tricky way to use UTF-8 on Python2
if sys.version_info < (3, 0):
    reload(sys)
    sys.setdefaultencoding("utf-8")

def valid_char(char):
    '''
       Removes non-printable chars according to https://www.ascii-code.com/
    '''
    code = ord(char)
    if code < 32:
        return '.'
    if code == 129 or \
       code == 141 or \
       code == 143 or \
       code == 144 or \
       code == 157 or \
       code == 160 or \
       code == 173:
        return '.'
    return char

class Generator:
    source = None

    def __init__(self, source):
        self.source = source

    def generate_fixed_old(self, demo_array, output_file):
        self.source.expand_chars()
        output_string = demo_array + [
            "extern const uint8_t {0}[] PROGMEM;".format(self.source.name),
            "const uint8_t {0}[] PROGMEM =".format(self.source.name),
            "{",
            "#ifdef CONFIG_SSD1306_UNICODE_ENABLE",
            "//  type|width|height|first char",
            "    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (1, self.source.width, self.source.height, self.source.first_char),
            "//  unicode(2B)|count",
            ("    0x%02X, 0x%02X, 0x%02X, // unicode record" % \
                 ((self.source.first_char >> 8) & 0xFF, self.source.first_char & 0xFF, \
                  len(self.source.get_group_chars()) & 0xFF)),
            "#else",
            "//  type|width|height|first char",
            "    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (0, self.source.width, self.source.height, self.source.first_char),
            "#endif"
        ]

        char_code = self.source.first_char
        for char in self.source.get_group_chars():
            full_string = "    "
            for row in range(self.source.rows()):
                for x in range(self.source.width):
                    data = 0
                    for i in range(8):
                        y = row * 8 + i
                        if y >= self.source.height:
                            break
                        data |= (self.source.charBitmap(char)[y][x] << i)
                    full_string += "0x%02X, " % data
            if sys.version_info < (3, 0):
                full_string += "// char '%s' (0x%04X/%d)" % (valid_char(char).encode("utf-8"), char_code, char_code)
            else:
                full_string += "// char '%s' (0x%04X/%d)" % (valid_char(char), char_code, char_code)
            char_code = char_code + 1

            output_string.append(full_string)

        output_string.append("#ifdef CONFIG_SSD1306_UNICODE_ENABLE")
        output_string.append("    0x00, 0x00, 0x00, // end of unicode tables")
        output_string.append("#endif")
        output_string.append("};")

        if output_file:
            try:
                with io.open(output_file, "w", encoding='utf-8') as fOut:
                    out_str = None
                    if sys.version_info < (3, 0):
                        out_str = "\n".join(output_string).decode("utf-8")
                    else:
                        out_str = "\n".join(output_string)
                    fOut.write(out_str)

                print("The file %s was created sucesfully" % (output_file))
            except Exception as e:
                print("There was an error creating the output file: %r" % (e))
                exit(1)

        else:
            if sys.version_info < (3, 0):
                print("\n".join(output_string).decode("utf-8"))
            else:
                sys.stdout.buffer.write("\n".join(output_string).encode("utf-8"))

    def generate_new_format(self, demo_array, output_file):
        total_size = 4
        self.source.expand_chars_top()
        output_string = demo_array + [
            "extern const uint8_t %s[] PROGMEM;" % ("free_" + self.source.name),
            "const uint8_t %s[] PROGMEM =" % ("free_" + self.source.name),
            "{",
            "//  type|width|height|first char",
            "    0x%02X, 0x%02X, 0x%02X, 0x%02X," % (2, self.source.width, self.source.height, 0x00)
        ]
        for group in range(self.source.groups_count()):
            chars = self.source.get_group_chars(group)
            total_size += 3
            if sys.version_info < (3, 0):
                output_string.append("// GROUP first '%s' total %d chars" % (valid_char(chars[0]).encode("utf-8"), len(chars)))
            else:
                chars  = list(chars)
                output_string.append("// GROUP first '%s' total %d chars" % (valid_char(chars[0]), len(chars)))
            output_string.append("//  unicode(LSB,MSB)|count")
            output_string.append("    0x%02X, 0x%02X, 0x%02X, // unicode record" % \
                 ((ord(chars[0]) >> 8) & 0xFF, ord(chars[0]) & 0xFF, len(chars) & 0xFF))
            # jump table
            offset = 0
            heights = []
            sizes = []
            for char in chars:
                bitmap = self.source.charBitmap(char)
                full_string = "    "
                width = len(bitmap[0])
                height = len(bitmap)
                while (height > 0) and (sum(bitmap[height -1]) == 0):
                    height -= 1
                heights.append( height )
                size = int((height + 7) / 8) * width
                sizes.append( size )
                total_size += 4
                full_string += "0x%02X, 0x%02X, 0x%02X, 0x%02X," % (offset >> 8, offset & 0xFF, width, height)
                if sys.version_info < (3, 0):
                    full_string += "// char '%s' (0x%04X/%d)" % (valid_char(char).encode("utf-8"), ord(char), ord(char))
                else:
                    full_string += "// char '%s' (0x%04X/%d)" % (valid_char(char), ord(char), ord(char))
                offset += size
                output_string.append(full_string)
            total_size += 2
            output_string.append("    0x%02X, 0x%02X," % (offset >> 8, offset & 0xFF))
            # char data
            for index in range(len(chars)):
                full_string = ""
                char = chars[index]
                bitmap = self.source.charBitmap(char)
                full_string += "    "
                size = 0
                for row in range(int((heights[index] + 7) / 8)):
                    for x in range(len(bitmap[0])):
                        data = 0
                        for i in range(8):
                            y = row * 8 + i
                            if y >= len(bitmap):
                                break
                            data |= (self.source.charBitmap(char)[y][x] << i)
                        total_size += 1
                        size += 1
                        full_string += "0x%02X, " % data
                if sys.version_info < (3, 0):
                    full_string += "// char '%s' (0x%04X/%d)" % (valid_char(char).encode("utf-8"), ord(char), ord(char))
                else:
                    full_string += "// char '%s' (0x%04X/%d)" % (valid_char(char), ord(char), ord(char))
                if size != sizes[index]:
                    print("ERROR!!!!")
                    exit(1)

                output_string.append(full_string)
        total_size += 3
        output_string.append("    0x00, 0x00, 0x00, // end of unicode tables")
        output_string.append("    // FONT REQUIRES %d BYTES" % (total_size))
        output_string.append("};")
        if output_file:
            try:
                with io.open(output_file, "w", encoding='utf-8') as fOut:
                    out_str = None
                    if sys.version_info < (3, 0):
                        out_str = "\n".join(output_string).decode("utf-8")
                    else:
                        out_str = "\n".join(output_string)
                    fOut.write(out_str)

                print("The file %s was created sucesfully" % (output_file))
            except Exception as e:
                print("There was an error creating the output file: %r" % (e))
                exit(1)

        else:
            if sys.version_info < (3, 0):
                print("\n".join(output_string).decode("utf-8"))
            else:
                sys.stdout.buffer.write("\n".join(output_string).encode("utf-8"))
