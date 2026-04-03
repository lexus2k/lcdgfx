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

// ==================== SH1107 command/addressing path tests ====================
// Exercises page-byte addressing (0xB0-0xBF), column nibble commands (0x00-0x0F, 0x10-0x1F)
// and display state commands (0xAF, 0xAE, 0xA8 MUX ratio, 0xD3 offset)

static const int SH1107_W = 128;
static const int SH1107_H = 64;

TEST_GROUP(SH1107_CMDS)
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

// Drawing at different page boundaries exercises page selection (0xB0+)
TEST(SH1107_CMDS, fillRect_page_0)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 127, 7);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), SH1107_W, 0, 0, 127, 7, 1) );
    CHECK_EQUAL( 0, px(64, 8) );
}

TEST(SH1107_CMDS, fillRect_page_7)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 56, 127, 63);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), SH1107_W, 0, 56, 127, 63, 1) );
    CHECK_EQUAL( 0, px(64, 55) );
}

// Cross-page fill exercises page transitions during data write
TEST(SH1107_CMDS, fillRect_cross_page)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 5, 50, 20);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), SH1107_W, 10, 5, 50, 20, 1) );
    CHECK_EQUAL( 0, px(9, 12) );
    CHECK_EQUAL( 0, px(51, 12) );
    CHECK_EQUAL( 0, px(30, 4) );
    CHECK_EQUAL( 0, px(30, 21) );
}

// Multiple draws in different pages exercises column address reset
TEST(SH1107_CMDS, draws_on_every_page)
{
    display->setColor(0xFFFF);
    for ( int page = 0; page < 8; page++ )
    {
        int y = page * 8;
        display->fillRect(page * 15, y, page * 15 + 10, y + 7);
    }
    capture();
    for ( int page = 0; page < 8; page++ )
    {
        int y = page * 8 + 3;
        CHECK_TRUE( px(page * 15 + 5, y) != 0 );
    }
}

// drawHLine at non-page-aligned y exercises partial page byte masking
TEST(SH1107_CMDS, drawHLine_non_aligned)
{
    display->setColor(0xFFFF);
    display->drawHLine(10, 13, 110);
    capture();
    for ( int x = 10; x <= 110; x++ )
        CHECK_EQUAL( 1, px(x, 13) );
    CHECK_EQUAL( 0, px(9, 13) );
    CHECK_EQUAL( 0, px(111, 13) );
    CHECK_EQUAL( 0, px(60, 12) );
    CHECK_EQUAL( 0, px(60, 14) );
}

// drawVLine spanning all pages exercises full column data write
TEST(SH1107_CMDS, drawVLine_full_height)
{
    display->setColor(0xFFFF);
    display->drawVLine(64, 0, 63);
    capture();
    for ( int y = 0; y <= 63; y++ )
        CHECK_EQUAL( 1, px(64, y) );
}

TEST(SH1107_CMDS, drawVLine_partial_pages)
{
    display->setColor(0xFFFF);
    display->drawVLine(30, 3, 28);
    capture();
    for ( int y = 3; y <= 28; y++ )
        CHECK_EQUAL( 1, px(30, y) );
    CHECK_EQUAL( 0, px(30, 2) );
    CHECK_EQUAL( 0, px(30, 29) );
}

// drawLine exercises putPixel path with page-byte writes
TEST(SH1107_CMDS, drawLine_45deg)
{
    display->setColor(0xFFFF);
    display->drawLine(0, 0, 63, 63);
    capture();
    CHECK_EQUAL( 1, px(0, 0) );
    CHECK_EQUAL( 1, px(32, 32) );
    CHECK_EQUAL( 1, px(63, 63) );
}

// Draw rectangle spanning multiple pages
TEST(SH1107_CMDS, drawRect_multi_page)
{
    display->setColor(0xFFFF);
    display->drawRect(5, 5, 122, 58);
    capture();
    // Horizontal edges
    CHECK_EQUAL( 1, px(5, 5) );
    CHECK_EQUAL( 1, px(64, 5) );
    CHECK_EQUAL( 1, px(122, 5) );
    CHECK_EQUAL( 1, px(5, 58) );
    CHECK_EQUAL( 1, px(122, 58) );
    // Vertical edges
    CHECK_EQUAL( 1, px(5, 32) );
    CHECK_EQUAL( 1, px(122, 32) );
    CHECK_EQUAL( 0, px(64, 32) ); // center empty
}

// Fill then clear center
TEST(SH1107_CMDS, fill_clear_center)
{
    display->fill(0xFF);
    display->setColor(0x0000);
    display->fillRect(32, 16, 95, 47);
    capture();
    CHECK_EQUAL( 0, px(64, 32) );
    CHECK_EQUAL( 1, px(10, 10) );
    CHECK_EQUAL( 1, px(110, 56) );
}

// ==================== SSD1306 additional address path coverage ====================

static const int SSD1306_W = 128;
static const int SSD1306_H = 64;

TEST_GROUP(SSD1306_CMDS)
{
    DisplaySSD1306_128x64_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1306_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
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
        return get_mono_pixel(pixels->data(), SSD1306_W, x, y);
    }
};

// Exercises 0x21/0x22 column/page address window commands
TEST(SSD1306_CMDS, fillRect_bottom_right)
{
    display->setColor(0xFFFF);
    display->fillRect(64, 32, 127, 63);
    capture();
    CHECK_TRUE( mono_region_equals(pixels->data(), SSD1306_W, 64, 32, 127, 63, 1) );
    CHECK_EQUAL( 0, px(63, 48) );
    CHECK_EQUAL( 0, px(96, 31) );
}

// Multiple small rects exercise address window re-setting
TEST(SSD1306_CMDS, multiple_small_fills)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 10, 10);
    display->fillRect(60, 25, 70, 35);
    display->fillRect(117, 53, 127, 63);
    capture();
    CHECK_EQUAL( 1, px(5, 5) );
    CHECK_EQUAL( 1, px(65, 30) );
    CHECK_EQUAL( 1, px(122, 58) );
    CHECK_EQUAL( 0, px(40, 20) );
}

// Wide drawHLine exercises column address window
TEST(SSD1306_CMDS, drawHLine_full_width)
{
    display->setColor(0xFFFF);
    display->drawHLine(0, 32, 127);
    capture();
    for ( int x = 0; x <= 127; x++ )
        CHECK_EQUAL( 1, px(x, 32) );
    CHECK_EQUAL( 0, px(64, 31) );
    CHECK_EQUAL( 0, px(64, 33) );
}

// fill + partial clear exercises zero-color data writes
TEST(SSD1306_CMDS, fill_and_clear_band)
{
    display->fill(0xFF);
    display->setColor(0x0000);
    display->fillRect(0, 16, 127, 47);
    capture();
    CHECK_EQUAL( 0, px(64, 32) );
    CHECK_EQUAL( 1, px(64, 10) );
    CHECK_EQUAL( 1, px(64, 55) );
}

// Circle exercises all quadrants of pixel placement
TEST(SSD1306_CMDS, drawCircle_large)
{
    display->setColor(0xFFFF);
    display->drawCircle(64, 32, 20);
    capture();
    CHECK_EQUAL( 1, px(64, 12) );  // top
    CHECK_EQUAL( 1, px(64, 52) );  // bottom
    // Count set pixels in left and right arc regions
    int leftCount = 0, rightCount = 0;
    for ( int y = 27; y <= 37; y++ )
    {
        for ( int x = 41; x <= 48; x++ ) if ( px(x, y) ) leftCount++;
        for ( int x = 80; x <= 87; x++ ) if ( px(x, y) ) rightCount++;
    }
    CHECK_TRUE( leftCount > 0 );
    CHECK_TRUE( rightCount > 0 );
    CHECK_EQUAL( 0, px(64, 32) );  // center
}
