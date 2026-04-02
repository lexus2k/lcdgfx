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

static const int W = 96;
static const int H = 64;

TEST_GROUP(SSD1331_GFX)
{
    DisplaySSD1331_96x64x8_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1331_96x64x8_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>( sdl_core_get_pixels_len(8), 0 );
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
        sdl_core_get_pixels_data( pixels->data(), 8 );
    }

    uint8_t px(int x, int y)
    {
        return get_rgb8_pixel(pixels->data(), W, x, y);
    }
};

TEST(SSD1331_GFX, clear_sets_all_pixels_to_zero)
{
    capture();
    CHECK_TRUE( rgb8_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 0x00) );
}

TEST(SSD1331_GFX, fill_with_color)
{
    display->fill(0xE0);
    capture();
    CHECK_TRUE( rgb8_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 0xE0) );
}

TEST(SSD1331_GFX, putPixel_with_color)
{
    display->setColor(0xE0); // red in RGB332
    display->putPixel(10, 10);
    display->setColor(0x1C); // green in RGB332
    display->putPixel(20, 20);
    capture();
    CHECK_EQUAL(0xE0, px(10, 10));
    CHECK_EQUAL(0x1C, px(20, 20));
    CHECK_EQUAL(0x00, px(15, 15));
}

TEST(SSD1331_GFX, drawHLine)
{
    display->setColor(0xFF);
    display->drawHLine(10, 16, 50);
    capture();
    for ( int x = 10; x <= 50; x++ )
        CHECK_EQUAL(0xFF, px(x, 16));
    CHECK_EQUAL(0x00, px(9,  16));
    CHECK_EQUAL(0x00, px(51, 16));
    CHECK_EQUAL(0x00, px(30, 15));
    CHECK_EQUAL(0x00, px(30, 17));
}

TEST(SSD1331_GFX, drawVLine)
{
    display->setColor(0xFF);
    display->drawVLine(32, 8, 48);
    capture();
    for ( int y = 8; y <= 48; y++ )
        CHECK_EQUAL(0xFF, px(32, y));
    CHECK_EQUAL(0x00, px(32, 7));
    CHECK_EQUAL(0x00, px(32, 49));
    CHECK_EQUAL(0x00, px(31, 20));
    CHECK_EQUAL(0x00, px(33, 20));
}

TEST(SSD1331_GFX, drawRect_outline)
{
    display->setColor(0x03); // blue
    display->drawRect(10, 10, 50, 40);
    capture();
    CHECK_EQUAL(0x03, px(10, 10));
    CHECK_EQUAL(0x03, px(30, 10));
    CHECK_EQUAL(0x03, px(10, 40));
    CHECK_EQUAL(0x03, px(50, 40));
    CHECK_EQUAL(0x00, px(30, 25));
}

TEST(SSD1331_GFX, fillRect_with_color)
{
    display->setColor(0xFF);
    display->fillRect(10, 10, 40, 30);
    capture();
    CHECK_TRUE( rgb8_region_equals(pixels->data(), W, 10, 10, 40, 30, 0xFF) );
    CHECK_EQUAL(0x00, px(9, 20));
    CHECK_EQUAL(0x00, px(41, 20));
}

TEST(SSD1331_GFX, clearRect_fills_with_zero)
{
    display->setColor(0xFF);
    display->fillRect(0, 0, W - 1, H - 1);
    display->clearRect(20, 20, 50, 40);
    capture();
    CHECK_TRUE( rgb8_region_equals(pixels->data(), W, 20, 20, 50, 40, 0x00) );
    CHECK_EQUAL(0xFF, px(19, 30));
    CHECK_EQUAL(0xFF, px(51, 30));
}

TEST(SSD1331_GFX, clearRect_preserves_drawing_color)
{
    display->setColor(0xE0);
    uint16_t before = display->getColor();
    display->clearRect(10, 10, 50, 50);
    uint16_t after = display->getColor();
    CHECK_EQUAL(before, after);
}

TEST(SSD1331_GFX, setColor_getColor_roundtrip)
{
    display->setColor(0x00);
    CHECK_EQUAL(0x00, display->getColor());
    display->setColor(0xFF);
    CHECK_EQUAL(0xFF, display->getColor());
    display->setColor(0xE0);
    CHECK_EQUAL(0xE0, display->getColor());
}

TEST(SSD1331_GFX, drawLine_diagonal)
{
    display->setColor(0xFF);
    display->drawLine(0, 0, 63, 63);
    capture();
    CHECK_EQUAL(0xFF, px(0, 0));
    CHECK_EQUAL(0xFF, px(32, 32));
    CHECK_EQUAL(0xFF, px(63, 63));
}

TEST(SSD1331_GFX, drawCircle_cardinal_points)
{
    display->setColor(0xFF);
    int cx = 48, cy = 32, r = 10;
    display->drawCircle(cx, cy, r);
    capture();
    CHECK_EQUAL(0xFF, px(cx, cy - r));
    CHECK_EQUAL(0xFF, px(cx, cy + r));
    CHECK_EQUAL(0xFF, px(cx - r, cy));
    CHECK_EQUAL(0xFF, px(cx + r, cy));
    CHECK_EQUAL(0x00, px(cx, cy));
}

TEST(SSD1331_GFX, overlapping_colors)
{
    display->setColor(0xE0); // red
    display->fillRect(10, 10, 50, 40);
    display->setColor(0x1C); // green
    display->fillRect(30, 20, 70, 50);
    capture();
    CHECK_EQUAL(0xE0, px(15, 15));  // red-only region
    CHECK_EQUAL(0x1C, px(60, 45));  // green-only region
    CHECK_EQUAL(0x1C, px(40, 30));  // overlap = green (last draw wins)
}
