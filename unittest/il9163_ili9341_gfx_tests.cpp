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

// ====================== IL9163 128x128 (16-bit TFT) ======================

static const int IL_W = 128;
static const int IL_H = 128;

TEST_GROUP(IL9163_GFX)
{
    DisplayIL9163_128x128x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplayIL9163_128x128x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( IL_W * IL_H, 0 );
    }

    void teardown()
    {
        display->end();
        delete pixels;
        delete display;
    }

    void capture()
    {
        memset(pixels->data(), 0, pixels->size() * sizeof(uint16_t));
        sdl_core_get_pixels_data( (uint8_t *)pixels->data(), 16 );
    }

    uint16_t px(int x, int y)
    {
        return get_rgb16_pixel(pixels->data(), IL_W, x, y);
    }
};

TEST(IL9163_GFX, clear_sets_all_pixels_to_zero)
{
    capture();
    CHECK_TRUE( rgb16_region_equals(pixels->data(), IL_W, 0, 0, IL_W - 1, IL_H - 1, 0) );
}

TEST(IL9163_GFX, putPixel_single)
{
    display->setColor(0xFFFF);
    display->putPixel(64, 64);
    capture();
    CHECK_TRUE( px(64, 64) != 0 );
    CHECK_EQUAL( 0, px(65, 64) );
    CHECK_EQUAL( 0, px(64, 65) );
}

TEST(IL9163_GFX, drawHLine)
{
    display->setColor(0xFFFF);
    display->drawHLine(10, 50, 100);
    capture();
    for ( int x = 10; x <= 100; x++ )
        CHECK_TRUE( px(x, 50) != 0 );
    CHECK_EQUAL( 0, px(9, 50) );
    CHECK_EQUAL( 0, px(101, 50) );
}

TEST(IL9163_GFX, drawVLine)
{
    display->setColor(0xFFFF);
    display->drawVLine(50, 10, 100);
    capture();
    for ( int y = 10; y <= 100; y++ )
        CHECK_TRUE( px(50, y) != 0 );
    CHECK_EQUAL( 0, px(50, 9) );
    CHECK_EQUAL( 0, px(50, 101) );
}

TEST(IL9163_GFX, fillRect)
{
    display->setColor(0x07E0); // green
    display->fillRect(20, 20, 60, 60);
    capture();
    for ( int y = 20; y <= 60; y++ )
        for ( int x = 20; x <= 60; x++ )
            CHECK_TRUE( px(x, y) != 0 );
    CHECK_EQUAL( 0, px(19, 40) );
    CHECK_EQUAL( 0, px(61, 40) );
}

TEST(IL9163_GFX, drawRect)
{
    display->setColor(0xFFFF);
    display->drawRect(10, 10, 50, 50);
    capture();
    for ( int x = 10; x <= 50; x++ )
    {
        CHECK_TRUE( px(x, 10) != 0 );
        CHECK_TRUE( px(x, 50) != 0 );
    }
    for ( int y = 10; y <= 50; y++ )
    {
        CHECK_TRUE( px(10, y) != 0 );
        CHECK_TRUE( px(50, y) != 0 );
    }
    CHECK_EQUAL( 0, px(30, 30) );
}

TEST(IL9163_GFX, drawLine_diagonal)
{
    display->setColor(0xFFFF);
    display->drawLine(0, 0, 63, 63);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(31, 31) != 0 );
    CHECK_TRUE( px(63, 63) != 0 );
    int count = rgb16_count_set(pixels->data(), IL_W, 0, 0, 63, 63);
    CHECK_EQUAL( 64, count );
}

TEST(IL9163_GFX, clearRect)
{
    display->fill(0xFF);
    display->setColor(0x0000);
    display->fillRect(10, 10, 30, 30);
    capture();
    CHECK_TRUE( rgb16_region_equals(pixels->data(), IL_W, 10, 10, 30, 30, 0) );
    CHECK_TRUE( px(5, 5) != 0 );
}

TEST(IL9163_GFX, boundary_pixel_corners)
{
    display->setColor(0xFFFF);
    display->putPixel(0, 0);
    display->putPixel(IL_W - 1, 0);
    display->putPixel(0, IL_H - 1);
    display->putPixel(IL_W - 1, IL_H - 1);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(IL_W - 1, 0) != 0 );
    CHECK_TRUE( px(0, IL_H - 1) != 0 );
    CHECK_TRUE( px(IL_W - 1, IL_H - 1) != 0 );
}

TEST(IL9163_GFX, drawHLine_single_pixel)
{
    display->setColor(0xFFFF);
    display->drawHLine(50, 50, 50);
    capture();
    CHECK_TRUE( px(50, 50) != 0 );
    CHECK_EQUAL( 0, px(49, 50) );
    CHECK_EQUAL( 0, px(51, 50) );
}

// ====================== ILI9341 240x320 (16-bit TFT) ======================
// The ILI9341 SDL emulator uses horizontal layout (320x240 internal).
// Coordinate mapping is rotated. Tests verify emulator init/lifecycle
// and that drawing produces non-zero output without exact coordinate checks.

static const int ILI_W = 240;
static const int ILI_H = 320;

TEST_GROUP(ILI9341_GFX)
{
    DisplayILI9341_240x320x16_SPI *display;
    std::vector<uint16_t> *pixels;
    int buf_len;

    void setup()
    {
        display = new DisplayILI9341_240x320x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        buf_len = sdl_core_get_pixels_len(16);
        pixels = new std::vector<uint16_t>( buf_len / sizeof(uint16_t), 0 );
    }

    void teardown()
    {
        display->end();
        delete pixels;
        delete display;
    }

    void capture()
    {
        memset(pixels->data(), 0, buf_len);
        sdl_core_get_pixels_data( (uint8_t *)pixels->data(), 16 );
    }

    int count_nonzero()
    {
        int count = 0;
        for ( size_t i = 0; i < pixels->size(); i++ )
            if ( (*pixels)[i] != 0 )
                count++;
        return count;
    }
};

TEST(ILI9341_GFX, clear_sets_all_pixels_to_zero)
{
    capture();
    CHECK_EQUAL( 0, count_nonzero() );
}

TEST(ILI9341_GFX, putPixel_produces_output)
{
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    capture();
    CHECK_EQUAL( 1, count_nonzero() );
}

TEST(ILI9341_GFX, drawHLine_produces_output)
{
    display->setColor(0xFFFF);
    display->drawHLine(10, 100, 200);
    capture();
    CHECK_EQUAL( 191, count_nonzero() );
}

TEST(ILI9341_GFX, drawVLine_produces_output)
{
    display->setColor(0xFFFF);
    display->drawVLine(120, 50, 250);
    capture();
    CHECK_EQUAL( 201, count_nonzero() );
}

TEST(ILI9341_GFX, fillRect_produces_output)
{
    display->setColor(0x001F);
    display->fillRect(50, 50, 100, 100);
    capture();
    int expected = 51 * 51;
    // Allow small tolerance for emulator coordinate rounding
    CHECK_TRUE( count_nonzero() >= expected );
    CHECK_TRUE( count_nonzero() <= expected + 10 );
}

TEST(ILI9341_GFX, drawLine_produces_output)
{
    display->setColor(0xFFFF);
    display->drawLine(0, 0, 99, 99);
    capture();
    CHECK_EQUAL( 100, count_nonzero() );
}

TEST(ILI9341_GFX, clear_after_draw)
{
    display->fill(0xFF);
    capture();
    CHECK_TRUE( count_nonzero() > 0 );
    display->clear();
    capture();
    CHECK_EQUAL( 0, count_nonzero() );
}
