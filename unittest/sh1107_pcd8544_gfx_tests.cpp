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

// ====================== SH1107 128x64 (monochrome, 1-bit) ======================

static const int SH1107_W = 128;
static const int SH1107_H = 64;

TEST_GROUP(SH1107_GFX)
{
    DisplaySH1107_128x64_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySH1107_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
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
        return get_mono_pixel(pixels->data(), SH1107_W, x, y);
    }
};

TEST(SH1107_GFX, clear_sets_all_pixels_to_zero)
{
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), SH1107_W, 0, 0, SH1107_W - 1, SH1107_H - 1, 0) );
}

TEST(SH1107_GFX, fill_sets_all_pixels)
{
    display->fill(0xFF);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), SH1107_W, 0, 0, SH1107_W - 1, SH1107_H - 1, 1) );
}

TEST(SH1107_GFX, drawHLine)
{
    display->setColor(0xFFFF);
    display->drawHLine(5, 8, 50);
    capture();
    for ( int x = 5; x <= 50; x++ )
        CHECK_EQUAL( 1, px(x, 8) );
    CHECK_EQUAL( 0, px(4, 8) );
    CHECK_EQUAL( 0, px(51, 8) );
}

TEST(SH1107_GFX, drawVLine)
{
    display->setColor(0xFFFF);
    display->drawVLine(20, 0, 63);
    capture();
    for ( int y = 0; y <= 63; y++ )
        CHECK_EQUAL( 1, px(20, y) );
    CHECK_EQUAL( 0, px(19, 32) );
    CHECK_EQUAL( 0, px(21, 32) );
}

TEST(SH1107_GFX, fillRect)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 8, 30, 23);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), SH1107_W, 10, 8, 30, 23, 1) );
    CHECK_EQUAL( 0, px(9, 16) );
    CHECK_EQUAL( 0, px(31, 16) );
    CHECK_EQUAL( 0, px(20, 7) );
    CHECK_EQUAL( 0, px(20, 24) );
}

TEST(SH1107_GFX, drawRect)
{
    display->setColor(0xFFFF);
    display->drawRect(10, 8, 30, 23);
    capture();
    for ( int x = 10; x <= 30; x++ )
    {
        CHECK_EQUAL( 1, px(x, 8) );
        CHECK_EQUAL( 1, px(x, 23) );
    }
    for ( int y = 8; y <= 23; y++ )
    {
        CHECK_EQUAL( 1, px(10, y) );
        CHECK_EQUAL( 1, px(30, y) );
    }
    CHECK_EQUAL( 0, px(20, 16) );
}

TEST(SH1107_GFX, boundary_pixel_corners)
{
    display->fill(0xFF);
    capture();
    CHECK_EQUAL( 1, px(0, 0) );
    CHECK_EQUAL( 1, px(SH1107_W - 1, 0) );
    CHECK_EQUAL( 1, px(0, SH1107_H - 1) );
    CHECK_EQUAL( 1, px(SH1107_W - 1, SH1107_H - 1) );
}

TEST(SH1107_GFX, drawHLine_single_pixel)
{
    display->setColor(0xFFFF);
    display->drawHLine(50, 32, 50);
    capture();
    CHECK_EQUAL( 1, px(50, 32) );
    CHECK_EQUAL( 0, px(49, 32) );
    CHECK_EQUAL( 0, px(51, 32) );
}

TEST(SH1107_GFX, fillRect_then_clear)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, SH1107_W - 1, SH1107_H - 1);
    display->clear();
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), SH1107_W, 0, 0, SH1107_W - 1, SH1107_H - 1, 0) );
}

// ====================== PCD8544 84x48 (monochrome, 1-bit) ======================

static const int PCD_W = 84;
static const int PCD_H = 48;

TEST_GROUP(PCD8544_GFX)
{
    DisplayPCD8544_84x48_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplayPCD8544_84x48_SPI(-1, {-1, 0, 1, 0, -1, -1});
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
        return get_mono_pixel(pixels->data(), PCD_W, x, y);
    }
};

TEST(PCD8544_GFX, clear_sets_all_pixels_to_zero)
{
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), PCD_W, 0, 0, PCD_W - 1, PCD_H - 1, 0) );
}

TEST(PCD8544_GFX, fill_sets_all_pixels)
{
    display->fill(0xFF);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), PCD_W, 0, 0, PCD_W - 1, PCD_H - 1, 1) );
}

TEST(PCD8544_GFX, drawHLine)
{
    display->setColor(0xFFFF);
    display->drawHLine(5, 16, 50);
    capture();
    for ( int x = 5; x <= 50; x++ )
        CHECK_EQUAL( 1, px(x, 16) );
    CHECK_EQUAL( 0, px(4, 16) );
    CHECK_EQUAL( 0, px(51, 16) );
}

TEST(PCD8544_GFX, drawVLine)
{
    display->setColor(0xFFFF);
    display->drawVLine(42, 0, 47);
    capture();
    for ( int y = 0; y <= 47; y++ )
        CHECK_EQUAL( 1, px(42, y) );
}

TEST(PCD8544_GFX, fillRect)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 8, 30, 23);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), PCD_W, 10, 8, 30, 23, 1) );
    CHECK_EQUAL( 0, px(9, 16) );
    CHECK_EQUAL( 0, px(31, 16) );
}

TEST(PCD8544_GFX, drawRect)
{
    display->setColor(0xFFFF);
    display->drawRect(5, 5, 40, 30);
    capture();
    for ( int x = 5; x <= 40; x++ )
    {
        CHECK_EQUAL( 1, px(x, 5) );
        CHECK_EQUAL( 1, px(x, 30) );
    }
    for ( int y = 5; y <= 30; y++ )
    {
        CHECK_EQUAL( 1, px(5, y) );
        CHECK_EQUAL( 1, px(40, y) );
    }
    CHECK_EQUAL( 0, px(20, 16) );
}

TEST(PCD8544_GFX, boundary_pixel_corners)
{
    display->fill(0xFF);
    capture();
    CHECK_EQUAL( 1, px(0, 0) );
    CHECK_EQUAL( 1, px(PCD_W - 1, 0) );
    CHECK_EQUAL( 1, px(0, PCD_H - 1) );
    CHECK_EQUAL( 1, px(PCD_W - 1, PCD_H - 1) );
}

TEST(PCD8544_GFX, small_display_fillRect_full)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, PCD_W - 1, PCD_H - 1);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), PCD_W, 0, 0, PCD_W - 1, PCD_H - 1, 1) );
}

TEST(PCD8544_GFX, drawHLine_single_pixel)
{
    display->setColor(0xFFFF);
    display->drawHLine(42, 24, 42);
    capture();
    CHECK_EQUAL( 1, px(42, 24) );
    CHECK_EQUAL( 0, px(41, 24) );
    CHECK_EQUAL( 0, px(43, 24) );
}
