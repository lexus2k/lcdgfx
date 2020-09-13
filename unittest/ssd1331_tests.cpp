/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <CppUTest/TestHarness.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "lcdgfx.h"
#include "sdl_core.h"
#include "utils/utils.h"
#include "ssd1331_data.h"


TEST_GROUP(SSD1331)
{
    void setup()
    {
        // ...
    }

    void teardown()
    {
        // ...
    }
};

TEST(SSD1331, rgb8_test)
{
    DisplaySSD1331_96x64x8_SPI display(-1,{-1, 0, 1, 0, -1, -1});
    display.begin();
    display.clear();
    display.setFixedFont(ssd1306xled_font6x8);
    display.printFixed (0,  8, "Line 1. Normal", STYLE_NORMAL);
    display.printFixed (0, 16, "Line 2. Bold", STYLE_BOLD);
    display.printFixed (0, 24, "Line 3. Italic", STYLE_ITALIC);
    display.setFreeFont(free_calibri11x12);
    display.printFixed (0, 40, "Line 4. Free Calibri", STYLE_NORMAL);

    std::vector<uint8_t> pixels( sdl_core_get_pixels_len( 8 ), 0 );
    sdl_core_get_pixels_data( pixels.data(), 8 );
//    print_buffer_data( pixels.data(), sdl_core_get_pixels_len( 8 ), 8, 96 );
//    print_screen_content( pixels.data(), sdl_core_get_pixels_len( 8 ), 8, 96 );

    CHECK_EQUAL( sizeof(rgb8_test_data), pixels.size() );
    MEMCMP_EQUAL( rgb8_test_data, pixels.data(), sizeof(rgb8_test_data));

    display.end();
}

