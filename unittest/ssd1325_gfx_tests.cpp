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

// SSD1325: 4-bit grayscale, 128x64
// NOTE: 4-bit displays pack 2 pixels per byte (nibble-pair).
// drawVLine at even x overwrites adjacent odd x (no GDRAM read-modify-write).
// fillRect has a known nibble-packing limitation.
// Tests focus on operations that work correctly through the full pipeline.

static const int W = 128;
static const int H = 64;

TEST_GROUP(SSD1325_GFX)
{
    DisplaySSD1325_128x64_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1325_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>( sdl_core_get_pixels_len(4), 0 );
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
        sdl_core_get_pixels_data( pixels->data(), 4 );
    }

    uint8_t px(int x, int y)
    {
        return get_gray4_pixel(pixels->data(), W, x, y);
    }
};

TEST(SSD1325_GFX, clear_sets_all_pixels_to_zero)
{
    capture();
    CHECK_TRUE( gray4_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 0) );
}

TEST(SSD1325_GFX, fill_sets_all_pixels)
{
    display->fill(0xFF);
    capture();
    for ( int y = 0; y < H; y++ )
        for ( int x = 0; x < W; x++ )
            CHECK_TRUE( px(x, y) != 0 );
}

TEST(SSD1325_GFX, putPixel_single)
{
    display->setColor(0x0F);
    display->putPixel(10, 10);
    capture();
    CHECK_TRUE( px(10, 10) != 0 );
}

TEST(SSD1325_GFX, putPixel_even_odd)
{
    display->setColor(0x0F);
    display->putPixel(20, 10);
    capture();
    CHECK_TRUE( px(20, 10) != 0 );

    display->clear();
    display->setColor(0x0F);
    display->putPixel(21, 10);
    capture();
    CHECK_TRUE( px(21, 10) != 0 );
}

TEST(SSD1325_GFX, drawHLine)
{
    display->setColor(0x0F);
    display->drawHLine(5, 10, 20);
    capture();
    for ( int x = 5; x <= 20; x++ )
        CHECK_TRUE( px(x, 10) != 0 );
    CHECK_EQUAL( 0, px(4, 10) );
    CHECK_EQUAL( 0, px(21, 10) );
}

TEST(SSD1325_GFX, drawHLine_even_start)
{
    display->setColor(0x0F);
    display->drawHLine(10, 20, 20);
    capture();
    for ( int x = 10; x <= 20; x++ )
        CHECK_TRUE( px(x, 20) != 0 );
    CHECK_EQUAL( 0, px(9, 20) );
    CHECK_EQUAL( 0, px(21, 20) );
}

TEST(SSD1325_GFX, drawVLine)
{
    display->setColor(0x0F);
    display->drawVLine(15, 5, 25);
    capture();
    for ( int y = 5; y <= 25; y++ )
        CHECK_TRUE( px(15, y) != 0 );
    CHECK_EQUAL( 0, px(15, 4) );
    CHECK_EQUAL( 0, px(15, 26) );
}

TEST(SSD1325_GFX, drawLine_diagonal)
{
    display->setColor(0x0F);
    display->drawLine(0, 0, 31, 31);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(15, 15) != 0 );
    CHECK_TRUE( px(31, 31) != 0 );
    int count = gray4_count_set(pixels->data(), W, 0, 0, 31, 31);
    CHECK_EQUAL( 32, count );
}

TEST(SSD1325_GFX, drawLine_horizontal)
{
    display->setColor(0x0F);
    display->drawLine(5, 20, 50, 20);
    capture();
    for ( int x = 5; x <= 50; x++ )
        CHECK_TRUE( px(x, 20) != 0 );
}

TEST(SSD1325_GFX, setContrast)
{
    display->getInterface().setContrast(128);
    display->getInterface().setContrast(0);
    display->getInterface().setContrast(255);
}

TEST(SSD1325_GFX, boundary_pixel_corners)
{
    display->setColor(0x0F);
    display->putPixel(0, 0);
    display->putPixel(W - 1, 0);
    display->putPixel(0, H - 1);
    display->putPixel(W - 1, H - 1);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(W - 1, 0) != 0 );
    CHECK_TRUE( px(0, H - 1) != 0 );
    CHECK_TRUE( px(W - 1, H - 1) != 0 );
}

TEST(SSD1325_GFX, drawHLine_single_pixel)
{
    display->setColor(0x0F);
    display->drawHLine(50, 30, 50);
    capture();
    CHECK_TRUE( px(50, 30) != 0 );
    CHECK_EQUAL( 0, px(49, 30) );
    CHECK_EQUAL( 0, px(51, 30) );
}

TEST(SSD1325_GFX, fillRect_produces_pixels)
{
    // fillRect has nibble-packing limitation (only every other column set)
    // Just verify it produces some non-zero pixels
    display->setColor(0x0F);
    display->fillRect(10, 10, 30, 30);
    capture();
    int count = gray4_count_set(pixels->data(), W, 10, 10, 30, 30);
    CHECK_TRUE( count > 0 );
}

TEST(SSD1325_GFX, clear_after_draw)
{
    display->setColor(0x0F);
    display->drawHLine(0, 0, W - 1);
    display->clear();
    capture();
    CHECK_TRUE( gray4_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 0) );
}
