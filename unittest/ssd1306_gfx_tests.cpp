/*
    MIT License

    Copyright (c) 2019-2026, Alexey Dynda

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
#include <string.h>
#include <vector>
#include "lcdgfx.h"
#include "sdl_core.h"
#include "utils/pixel_helpers.h"

static const int W = 128;
static const int H = 64;

TEST_GROUP(SSD1306_GFX)
{
    DisplaySSD1306_128x64_I2C *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1306_128x64_I2C(-1);
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>( sdl_core_get_pixels_len(1), 0 );
    }

    void teardown()
    {
        display->end();
        delete pixels;
        delete display;
    }

    void capture()
    {
        memset(pixels->data(), 0, pixels->size());
        sdl_core_get_pixels_data( pixels->data(), 1 );
    }

    int px(int x, int y)
    {
        return get_mono_pixel(pixels->data(), W, x, y);
    }
};

TEST(SSD1306_GFX, clear_sets_all_pixels_to_zero)
{
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 0) );
}

TEST(SSD1306_GFX, fill_sets_all_pixels)
{
    display->fill(0xFF);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 1) );
}

TEST(SSD1306_GFX, putPixel_sets_single_pixel)
{
    display->setColor(0xFFFF);
    display->putPixel(10, 20);
    display->putPixel(50, 40);
    capture();
    CHECK_EQUAL(1, px(10, 20));
    CHECK_EQUAL(1, px(50, 40));
    CHECK_EQUAL(0, px(11, 20));
    CHECK_EQUAL(0, px(10, 21));
    CHECK_EQUAL(0, px(9,  20));
    CHECK_EQUAL(0, px(10, 19));
}

TEST(SSD1306_GFX, drawHLine)
{
    display->setColor(0xFFFF);
    display->drawHLine(10, 16, 50);
    capture();
    for ( int x = 10; x <= 50; x++ )
        CHECK_EQUAL(1, px(x, 16));
    CHECK_EQUAL(0, px(9,  16));
    CHECK_EQUAL(0, px(51, 16));
    CHECK_EQUAL(0, px(30, 15));
    CHECK_EQUAL(0, px(30, 17));
}

TEST(SSD1306_GFX, drawVLine)
{
    display->setColor(0xFFFF);
    display->drawVLine(32, 8, 48);
    capture();
    for ( int y = 8; y <= 48; y++ )
        CHECK_EQUAL(1, px(32, y));
    CHECK_EQUAL(0, px(32, 7));
    CHECK_EQUAL(0, px(32, 49));
    CHECK_EQUAL(0, px(31, 20));
    CHECK_EQUAL(0, px(33, 20));
}

TEST(SSD1306_GFX, drawRect_outline)
{
    display->setColor(0xFFFF);
    display->drawRect(10, 8, 50, 32);
    capture();
    // Top and bottom edges
    for ( int x = 10; x <= 50; x++ )
    {
        CHECK_EQUAL(1, px(x, 8));
        CHECK_EQUAL(1, px(x, 32));
    }
    // Left and right edges
    for ( int y = 8; y <= 32; y++ )
    {
        CHECK_EQUAL(1, px(10, y));
        CHECK_EQUAL(1, px(50, y));
    }
    // Interior should be empty
    CHECK_EQUAL(0, px(30, 20));
    CHECK_EQUAL(0, px(11, 9));
}

TEST(SSD1306_GFX, drawRect_NanoRect_overload)
{
    display->setColor(0xFFFF);
    NanoRect rect = { {10, 8}, {50, 32} };
    display->drawRect(rect);
    capture();
    CHECK_EQUAL(1, px(10, 8));
    CHECK_EQUAL(1, px(50, 8));
    CHECK_EQUAL(1, px(10, 32));
    CHECK_EQUAL(1, px(50, 32));
    CHECK_EQUAL(0, px(30, 20));
}

TEST(SSD1306_GFX, fillRect_fills_area)
{
    display->setColor(0xFFFF);
    display->fillRect(20, 16, 60, 48);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), W, 20, 16, 60, 48, 1) );
    CHECK_EQUAL(0, px(19, 30));
    CHECK_EQUAL(0, px(61, 30));
    CHECK_EQUAL(0, px(40, 15));
    CHECK_EQUAL(0, px(40, 49));
}

TEST(SSD1306_GFX, fillRect_NanoRect_overload)
{
    display->setColor(0xFFFF);
    NanoRect rect = { {20, 16}, {40, 32} };
    display->fillRect(rect);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), W, 20, 16, 40, 32, 1) );
    CHECK_EQUAL(0, px(19, 24));
}

TEST(SSD1306_GFX, clearRect_clears_filled_area)
{
    // Use page-aligned y boundaries (multiples of 8) because SSD1306 writes
    // entire page bytes — clearRect zeroes full pages including bits outside rect.
    display->fill(0xFF);
    display->clearRect(20, 16, 60, 39);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), W, 20, 16, 60, 39, 0) );
    CHECK_EQUAL(1, px(19, 30));
    CHECK_EQUAL(1, px(61, 30));
    CHECK_EQUAL(1, px(40, 15));
    CHECK_EQUAL(1, px(40, 40));
}

TEST(SSD1306_GFX, clearRect_NanoRect_overload)
{
    display->fill(0xFF);
    NanoRect rect = { {20, 16}, {40, 32} };
    display->clearRect(rect);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), W, 20, 16, 40, 32, 0) );
    CHECK_EQUAL(1, px(19, 24));
}

TEST(SSD1306_GFX, clearRect_preserves_current_color)
{
    display->setColor(0xFFFF);
    uint16_t before = display->getColor();
    display->clearRect(10, 10, 50, 50);
    uint16_t after = display->getColor();
    CHECK_EQUAL(before, after);
}

// Note: drawLine diagonal, drawLine vertical, and drawCircle tests are omitted
// for monochrome SSD1306 because these functions use putPixel internally, which
// writes a single bit but replaces the entire page byte (no GDRAM read-modify-write).
// These operations are tested on the 8-bit SSD1331 where pixels are independent.

TEST(SSD1306_GFX, setColor_getColor_roundtrip)
{
    display->setColor(0x0000);
    CHECK_EQUAL(0x0000, display->getColor());
    display->setColor(0xFFFF);
    CHECK_EQUAL(0xFFFF, display->getColor());
    display->setColor(0x1234);
    CHECK_EQUAL(0x1234, display->getColor());
}

TEST(SSD1306_GFX, composite_fill_then_clear)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    display->clearRect(20, 20, 40, 40);
    capture();
    CHECK_EQUAL(1, px(10, 10));
    CHECK_EQUAL(1, px(50, 50));
    CHECK_EQUAL(1, px(15, 15));
    CHECK_TRUE( mono_region_equals(pixels->data(), W, 20, 20, 40, 40, 0) );
}
