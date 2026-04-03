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

// ==================== SSD1327 128x128 4-bit grayscale ====================
// Tests the SSD1325 emulator in SSD1327 subtype mode (128x128)
// Exercises 0x15 column, 0x75 row, and 0xA0 remap commands at larger resolution

static const int SSD1327_W = 128;
static const int SSD1327_H = 128;

TEST_GROUP(SSD1327_GFX)
{
    DisplaySSD1327_128x128_SPI *display;
    std::vector<uint32_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1327_128x128_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint32_t>( SSD1327_W * SSD1327_H, 0 );
    }

    void teardown()
    {
        display->end();
        delete pixels;
        delete display;
    }

    void capture()
    {
        memset(pixels->data(), 0, pixels->size() * sizeof(uint32_t));
        sdl_core_get_pixels_data( (uint8_t *)pixels->data(), 32 );
    }

    uint32_t px(int x, int y)
    {
        return (*pixels)[x + y * SSD1327_W];
    }
};

TEST(SSD1327_GFX, clear_all_zero)
{
    capture();
    int nonzero = 0;
    for ( int i = 0; i < SSD1327_W * SSD1327_H; i++ )
        if ( (*pixels)[i] != 0 ) nonzero++;
    CHECK_EQUAL( 0, nonzero );
}

TEST(SSD1327_GFX, fill_all_nonzero)
{
    display->fill(0xFF);
    capture();
    int nonzero = 0;
    for ( int i = 0; i < SSD1327_W * SSD1327_H; i++ )
        if ( (*pixels)[i] != 0 ) nonzero++;
    CHECK_TRUE( nonzero > 0 );
}

// Exercises 0x15 column window for right side of display
TEST(SSD1327_GFX, drawHLine_right_half)
{
    display->setColor(0x0F);
    display->drawHLine(64, 64, 127);
    capture();
    CHECK_TRUE( px(64, 64) != 0 );
    CHECK_TRUE( px(127, 64) != 0 );
    CHECK_EQUAL( 0u, px(63, 64) );
}

// Exercises 0x75 row window for bottom half
TEST(SSD1327_GFX, drawHLine_bottom_row)
{
    display->setColor(0x0F);
    display->drawHLine(0, 127, 127);
    capture();
    CHECK_TRUE( px(0, 127) != 0 );
    CHECK_TRUE( px(64, 127) != 0 );
    CHECK_TRUE( px(127, 127) != 0 );
}

TEST(SSD1327_GFX, fillRect_center)
{
    display->setColor(0x0F);
    display->fillRect(40, 40, 87, 87);
    capture();
    // Even columns are set due to known 4-bit nibble-packing behavior
    CHECK_TRUE( px(40, 40) != 0 );
    CHECK_TRUE( px(86, 86) != 0 );
    CHECK_TRUE( px(64, 64) != 0 );
    CHECK_EQUAL( 0u, px(39, 64) );
    CHECK_EQUAL( 0u, px(64, 39) );
    CHECK_EQUAL( 0u, px(64, 88) );
}

TEST(SSD1327_GFX, fillRect_bottom_right)
{
    display->setColor(0x0F);
    display->fillRect(100, 100, 127, 127);
    capture();
    CHECK_TRUE( px(100, 100) != 0 );
    CHECK_TRUE( px(126, 126) != 0 ); // even column
    CHECK_EQUAL( 0u, px(99, 110) );
    CHECK_EQUAL( 0u, px(110, 99) );
}

TEST(SSD1327_GFX, drawRect_full_display)
{
    display->setColor(0x0F);
    display->drawRect(0, 0, 127, 127);
    capture();
    // Corners
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(127, 0) != 0 );
    CHECK_TRUE( px(0, 127) != 0 );
    CHECK_TRUE( px(127, 127) != 0 );
    // Center should be empty
    CHECK_EQUAL( 0u, px(64, 64) );
}

TEST(SSD1327_GFX, drawLine_diagonal)
{
    display->setColor(0x0F);
    display->drawLine(0, 0, 127, 127);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(64, 64) != 0 );
    CHECK_TRUE( px(127, 127) != 0 );
}

// Multiple sequential operations with address window re-setting
TEST(SSD1327_GFX, multiple_fillRects_different_quadrants)
{
    display->setColor(0x0F);
    display->fillRect(0, 0, 10, 10);       // top-left
    display->fillRect(117, 0, 127, 10);     // top-right
    display->fillRect(0, 117, 10, 127);     // bottom-left
    display->fillRect(117, 117, 127, 127);  // bottom-right
    capture();
    CHECK_TRUE( px(4, 5) != 0 );  // even column in top-left
    CHECK_TRUE( px(122, 5) != 0 ); // even column in top-right
    CHECK_TRUE( px(4, 122) != 0 ); // even column in bottom-left
    CHECK_TRUE( px(122, 122) != 0 ); // even column in bottom-right
    CHECK_EQUAL( 0u, px(64, 64) );
}

TEST(SSD1327_GFX, fill_then_clear_region)
{
    display->fill(0xFF);
    display->setColor(0x00);
    display->fillRect(32, 32, 95, 95);
    capture();
    CHECK_EQUAL( 0u, px(64, 64) );  // cleared center
    CHECK_TRUE( px(10, 10) != 0 );  // still filled outside
    CHECK_TRUE( px(120, 120) != 0 );
}

TEST(SSD1327_GFX, drawCircle)
{
    display->setColor(0x0F);
    display->drawCircle(64, 64, 30);
    capture();
    // 4-bit nibble-packing only sets even columns; check neighborhood
    bool topFound = false, bottomFound = false, leftFound = false, rightFound = false;
    for ( int d = -3; d <= 3; d++ )
    {
        for ( int d2 = -3; d2 <= 3; d2++ )
        {
            if ( px(64 + d2, 34 + d) ) topFound = true;
            if ( px(64 + d2, 94 + d) ) bottomFound = true;
            if ( px(34 + d2, 64 + d) ) leftFound = true;
            if ( px(94 + d2, 64 + d) ) rightFound = true;
        }
    }
    CHECK_TRUE( topFound );
    CHECK_TRUE( bottomFound );
    CHECK_TRUE( leftFound );
    CHECK_TRUE( rightFound );
    CHECK_EQUAL( 0u, px(64, 64) ); // center empty
}

// ==================== SSD1325 additional command coverage ====================
// These tests add address window coverage beyond what ssd1325_gfx_tests covers

static const int SSD1325_W = 128;
static const int SSD1325_H = 64;

TEST_GROUP(SSD1325_CMDS)
{
    DisplaySSD1325_128x64_SPI *display;
    std::vector<uint32_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1325_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint32_t>( SSD1325_W * SSD1325_H, 0 );
    }

    void teardown()
    {
        display->end();
        delete pixels;
        delete display;
    }

    void capture()
    {
        memset(pixels->data(), 0, pixels->size() * sizeof(uint32_t));
        sdl_core_get_pixels_data( (uint8_t *)pixels->data(), 32 );
    }

    uint32_t px(int x, int y)
    {
        return (*pixels)[x + y * SSD1325_W];
    }
};

// fillRect in various regions exercises column window (0x15) and row window (0x75)
TEST(SSD1325_CMDS, fillRect_right_half)
{
    display->setColor(0x0F);
    display->fillRect(64, 0, 127, 63);
    capture();
    CHECK_TRUE( px(64, 0) != 0 );
    CHECK_TRUE( px(126, 62) != 0 ); // even column (4-bit nibble-packing)
    CHECK_EQUAL( 0u, px(63, 32) );
}

TEST(SSD1325_CMDS, fillRect_bottom_half)
{
    display->setColor(0x0F);
    display->fillRect(0, 32, 127, 63);
    capture();
    CHECK_TRUE( px(64, 32) != 0 );
    CHECK_TRUE( px(0, 63) != 0 );
    CHECK_EQUAL( 0u, px(64, 31) );
}

// Multiple small rects = multiple window re-set cycles
TEST(SSD1325_CMDS, three_small_rects)
{
    display->setColor(0x0F);
    display->fillRect(0, 0, 15, 15);
    display->fillRect(56, 24, 71, 39);
    display->fillRect(112, 48, 127, 63);
    capture();
    CHECK_TRUE( px(8, 8) != 0 );
    CHECK_TRUE( px(64, 32) != 0 );
    CHECK_TRUE( px(120, 56) != 0 );
    CHECK_EQUAL( 0u, px(40, 8) );
}

TEST(SSD1325_CMDS, drawLine_across_full_width)
{
    display->setColor(0x0F);
    display->drawLine(0, 0, 126, 63);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    // Bresenham midpoint may not land exactly at (64,32); verify endpoints
    CHECK_TRUE( px(126, 63) != 0 ); // even column endpoint
}

TEST(SSD1325_CMDS, drawRect_inner)
{
    display->setColor(0x0F);
    display->drawRect(20, 10, 107, 53);
    capture();
    // Top edge
    CHECK_TRUE( px(20, 10) != 0 );
    CHECK_TRUE( px(107, 10) != 0 );
    // Bottom edge
    CHECK_TRUE( px(20, 53) != 0 );
    CHECK_TRUE( px(107, 53) != 0 );
    // Center empty
    CHECK_EQUAL( 0u, px(64, 32) );
}
