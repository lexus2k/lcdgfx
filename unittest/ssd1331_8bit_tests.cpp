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

// ==================== SSD1331 8-bit mode address/window tests ====================

static const int SSD1331_W = 96;
static const int SSD1331_H = 64;

TEST_GROUP(SSD1331_8BIT)
{
    DisplaySSD1331_96x64x8_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1331_96x64x8_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>( SSD1331_W * SSD1331_H, 0 );
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
        return (*pixels)[x + y * SSD1331_W];
    }
};

// Test drawing in different regions exercises 0x15/0x75 column/row window commands
TEST(SSD1331_8BIT, fillRect_bottom_right_corner)
{
    display->setColor(0xE0); // Red in RGB332
    display->fillRect(80, 50, 95, 63);
    capture();
    CHECK_TRUE( px(80, 50) != 0 );
    CHECK_TRUE( px(95, 63) != 0 );
    CHECK_EQUAL( 0, px(79, 55) );
    CHECK_EQUAL( 0, px(80, 49) );
}

TEST(SSD1331_8BIT, fillRect_top_left_corner)
{
    display->setColor(0x1C); // Green in RGB332
    display->fillRect(0, 0, 15, 15);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(15, 15) != 0 );
    CHECK_EQUAL( 0, px(16, 8) );
    CHECK_EQUAL( 0, px(8, 16) );
}

TEST(SSD1331_8BIT, drawHLine_at_bottom_edge)
{
    display->setColor(0xFF);
    display->drawHLine(0, 63, 95);
    capture();
    for ( int x = 0; x <= 95; x++ )
        CHECK_TRUE( px(x, 63) != 0 );
    CHECK_EQUAL( 0, px(50, 62) );
}

TEST(SSD1331_8BIT, drawVLine_at_right_edge)
{
    display->setColor(0xFF);
    display->drawVLine(95, 0, 63);
    capture();
    for ( int y = 0; y <= 63; y++ )
        CHECK_TRUE( px(95, y) != 0 );
    CHECK_EQUAL( 0, px(94, 32) );
}

// Multiple sequential draws exercise address window re-setting
TEST(SSD1331_8BIT, multiple_rects_different_regions)
{
    display->setColor(0xE0);
    display->fillRect(0, 0, 10, 10);
    display->setColor(0x1C);
    display->fillRect(50, 30, 60, 40);
    display->setColor(0x03);
    display->fillRect(80, 50, 90, 60);
    capture();
    CHECK_TRUE( px(5, 5) != 0 );
    CHECK_TRUE( px(55, 35) != 0 );
    CHECK_TRUE( px(85, 55) != 0 );
    CHECK_EQUAL( 0, px(30, 25) );
}

TEST(SSD1331_8BIT, drawLine_steep_vertical)
{
    display->setColor(0xFF);
    display->drawLine(48, 0, 48, 63);
    capture();
    for ( int y = 0; y <= 63; y++ )
        CHECK_TRUE( px(48, y) != 0 );
}

TEST(SSD1331_8BIT, drawLine_shallow_horizontal)
{
    display->setColor(0xFF);
    display->drawLine(0, 32, 95, 32);
    capture();
    for ( int x = 0; x <= 95; x++ )
        CHECK_TRUE( px(x, 32) != 0 );
}

TEST(SSD1331_8BIT, drawCircle_centered)
{
    display->setColor(0xFF);
    display->drawCircle(48, 32, 20);
    capture();
    // Cardinal points of circle
    CHECK_TRUE( px(48, 12) != 0 ); // top
    CHECK_TRUE( px(48, 52) != 0 ); // bottom
    CHECK_TRUE( px(28, 32) != 0 ); // left
    CHECK_TRUE( px(68, 32) != 0 ); // right
    CHECK_EQUAL( 0, px(48, 32) );  // center empty
}

// Fill then partial clear exercises color=0 path through data handler
TEST(SSD1331_8BIT, fill_then_partial_clear)
{
    display->fill(0xFF);
    display->setColor(0x00);
    display->fillRect(10, 10, 20, 20);
    capture();
    CHECK_EQUAL( 0, px(15, 15) );
    CHECK_TRUE( px(5, 5) != 0 );
    CHECK_TRUE( px(25, 25) != 0 );
}

TEST(SSD1331_8BIT, single_pixel_draw)
{
    display->setColor(0xFF);
    display->putPixel(50, 32);
    capture();
    CHECK_TRUE( px(50, 32) != 0 );
    CHECK_EQUAL( 0, px(49, 32) );
    CHECK_EQUAL( 0, px(51, 32) );
    CHECK_EQUAL( 0, px(50, 31) );
    CHECK_EQUAL( 0, px(50, 33) );
}

// Full-screen fill exercises max address range
TEST(SSD1331_8BIT, fill_all_nonzero)
{
    display->fill(0xFF);
    capture();
    int count = 0;
    for ( int i = 0; i < SSD1331_W * SSD1331_H; i++ )
        if ( (*pixels)[i] != 0 ) count++;
    CHECK_EQUAL( SSD1331_W * SSD1331_H, count );
}

// drawRect exercises both horizontal and vertical line address window patterns
TEST(SSD1331_8BIT, drawRect_at_edges)
{
    display->setColor(0xFF);
    display->drawRect(0, 0, 95, 63);
    capture();
    // Top and bottom edges
    for ( int x = 0; x <= 95; x++ )
    {
        CHECK_TRUE( px(x, 0) != 0 );
        CHECK_TRUE( px(x, 63) != 0 );
    }
    // Left and right edges
    for ( int y = 0; y <= 63; y++ )
    {
        CHECK_TRUE( px(0, y) != 0 );
        CHECK_TRUE( px(95, y) != 0 );
    }
    // Center empty
    CHECK_EQUAL( 0, px(48, 32) );
}
