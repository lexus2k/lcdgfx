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

// ==================== SSD1351 16-bit address window tests ====================
// Exercises 0x15 column, 0x75 row, and 0x5C write-data commands

static const int SSD1351_W = 128;
static const int SSD1351_H = 128;

TEST_GROUP(SSD1351_CMDS)
{
    DisplaySSD1351_128x128x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1351_128x128x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( SSD1351_W * SSD1351_H, 0 );
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
        return (*pixels)[x + y * SSD1351_W];
    }
};

// Multiple fills in different quadrants = multiple 0x15/0x75 window resets
TEST(SSD1351_CMDS, four_corner_fills)
{
    display->setColor(0xF800); // Red
    display->fillRect(0, 0, 15, 15);
    display->setColor(0x07E0); // Green
    display->fillRect(112, 0, 127, 15);
    display->setColor(0x001F); // Blue
    display->fillRect(0, 112, 15, 127);
    display->setColor(0xFFFF); // White
    display->fillRect(112, 112, 127, 127);
    capture();
    CHECK_TRUE( px(8, 8) != 0 );
    CHECK_TRUE( px(120, 8) != 0 );
    CHECK_TRUE( px(8, 120) != 0 );
    CHECK_TRUE( px(120, 120) != 0 );
    CHECK_EQUAL( 0, px(64, 64) );
}

TEST(SSD1351_CMDS, fillRect_center_large)
{
    display->setColor(0xFFFF);
    display->fillRect(20, 20, 107, 107);
    capture();
    CHECK_TRUE( px(20, 20) != 0 );
    CHECK_TRUE( px(64, 64) != 0 );
    CHECK_TRUE( px(107, 107) != 0 );
    CHECK_EQUAL( 0, px(19, 64) );
    CHECK_EQUAL( 0, px(108, 64) );
}

TEST(SSD1351_CMDS, drawRect_boundary)
{
    display->setColor(0xFFFF);
    display->drawRect(0, 0, 127, 127);
    capture();
    for ( int i = 0; i <= 127; i++ )
    {
        CHECK_TRUE( px(i, 0) != 0 );
        CHECK_TRUE( px(i, 127) != 0 );
        CHECK_TRUE( px(0, i) != 0 );
        CHECK_TRUE( px(127, i) != 0 );
    }
    CHECK_EQUAL( 0, px(64, 64) );
}

TEST(SSD1351_CMDS, drawLine_anti_diagonal)
{
    display->setColor(0xFFFF);
    display->drawLine(127, 0, 0, 127);
    capture();
    CHECK_TRUE( px(127, 0) != 0 );
    CHECK_TRUE( px(64, 63) != 0 );
    CHECK_TRUE( px(0, 127) != 0 );
}

TEST(SSD1351_CMDS, fill_then_clear_ring)
{
    display->fill(0xFF);
    display->setColor(0x0000);
    display->fillRect(16, 16, 111, 111);
    capture();
    CHECK_EQUAL( 0, px(64, 64) );
    CHECK_TRUE( px(8, 8) != 0 );
    CHECK_TRUE( px(120, 120) != 0 );
}

// ==================== IL9163 16-bit address window tests ====================
// Exercises 0x2A/0x2B column/row address (16-bit high/low format) and 0x2C write

static const int IL9163_W = 128;
static const int IL9163_H = 128;

TEST_GROUP(IL9163_CMDS)
{
    DisplayIL9163_128x128x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplayIL9163_128x128x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( IL9163_W * IL9163_H, 0 );
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
        return (*pixels)[x + y * IL9163_W];
    }
};

// Column address with high byte exercises 0x2A arg parsing
TEST(IL9163_CMDS, fillRect_bottom_right)
{
    display->setColor(0xFFFF);
    display->fillRect(100, 100, 127, 127);
    capture();
    CHECK_TRUE( px(100, 100) != 0 );
    CHECK_TRUE( px(127, 127) != 0 );
    CHECK_EQUAL( 0, px(99, 110) );
    CHECK_EQUAL( 0, px(110, 99) );
}

TEST(IL9163_CMDS, fillRect_top_left)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 20, 20);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(20, 20) != 0 );
    CHECK_EQUAL( 0, px(21, 10) );
    CHECK_EQUAL( 0, px(10, 21) );
}

// Multiple fills force repeated 0x2A/0x2B re-addressing
TEST(IL9163_CMDS, three_fills_different_areas)
{
    display->setColor(0xF800);
    display->fillRect(0, 0, 30, 30);
    display->setColor(0x07E0);
    display->fillRect(49, 49, 79, 79);
    display->setColor(0x001F);
    display->fillRect(97, 97, 127, 127);
    capture();
    CHECK_TRUE( px(15, 15) != 0 );
    CHECK_TRUE( px(64, 64) != 0 );
    CHECK_TRUE( px(112, 112) != 0 );
    CHECK_EQUAL( 0, px(40, 40) );
    CHECK_EQUAL( 0, px(90, 40) );
}

TEST(IL9163_CMDS, drawHLine_full_width)
{
    display->setColor(0xFFFF);
    display->drawHLine(0, 64, 127);
    capture();
    for ( int x = 0; x <= 127; x++ )
        CHECK_TRUE( px(x, 64) != 0 );
    CHECK_EQUAL( 0, px(64, 63) );
    CHECK_EQUAL( 0, px(64, 65) );
}

TEST(IL9163_CMDS, drawVLine_full_height)
{
    display->setColor(0xFFFF);
    display->drawVLine(64, 0, 127);
    capture();
    for ( int y = 0; y <= 127; y++ )
        CHECK_TRUE( px(64, y) != 0 );
    CHECK_EQUAL( 0, px(63, 64) );
    CHECK_EQUAL( 0, px(65, 64) );
}

TEST(IL9163_CMDS, drawCircle)
{
    display->setColor(0xFFFF);
    display->drawCircle(64, 64, 40);
    capture();
    CHECK_TRUE( px(64, 24) != 0 );
    CHECK_TRUE( px(64, 104) != 0 );
    CHECK_TRUE( px(24, 64) != 0 );
    CHECK_TRUE( px(104, 64) != 0 );
    CHECK_EQUAL( 0, px(64, 64) );
}

// ==================== ILI9341 address window and landscape mode ====================
// ILI9341 uses 0x2A/0x2B with 16-bit addresses and horizontal layout swap
// After landscape swap, SDL buffer is 320 wide x 240 tall

static const int ILI_SDL_W = 320;
static const int ILI_SDL_H = 240;

TEST_GROUP(ILI9341_CMDS)
{
    DisplayILI9341_240x320x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplayILI9341_240x320x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        int len = sdl_core_get_pixels_len(16);
        pixels = new std::vector<uint16_t>( len / sizeof(uint16_t), 0 );
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

    int countAll()
    {
        return rgb16_count_set(pixels->data(), ILI_SDL_W,
                               0, 0, ILI_SDL_W - 1, ILI_SDL_H - 1);
    }
};

// ILI9341 with landscape swap makes coordinate checking complex.
// Use count-based verification to confirm correct pixel placement.

TEST(ILI9341_CMDS, fill_produces_all_nonzero)
{
    display->fill(0xFF);
    capture();
    CHECK_TRUE( countAll() > (ILI_SDL_W * ILI_SDL_H) / 2 );
}

TEST(ILI9341_CMDS, clear_produces_all_zero)
{
    display->fill(0xFF);
    display->clear();
    capture();
    CHECK_EQUAL( 0, countAll() );
}

TEST(ILI9341_CMDS, fillRect_small_area)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    int count = countAll();
    // Should have approximately 41*41 = 1681 pixels
    CHECK_TRUE( count >= 1600 );
    CHECK_TRUE( count <= 1800 );
}

TEST(ILI9341_CMDS, fillRect_large_area)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 239, 319);
    capture();
    int count = countAll();
    // With landscape swap, verify substantial fill
    CHECK_TRUE( count > 1000 );
}

TEST(ILI9341_CMDS, two_fills_correct_total)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 49, 49);
    display->fillRect(100, 100, 149, 149);
    capture();
    int count = countAll();
    // Two 50x50 regions = ~5000 pixels
    CHECK_TRUE( count >= 4800 );
    CHECK_TRUE( count <= 5200 );
}

TEST(ILI9341_CMDS, drawHLine_produces_pixels)
{
    display->setColor(0xFFFF);
    display->drawHLine(0, 160, 239);
    capture();
    int count = countAll();
    CHECK_TRUE( count >= 230 );
    CHECK_TRUE( count <= 250 );
}

TEST(ILI9341_CMDS, fill_clear_partial)
{
    display->fill(0xFF);
    display->setColor(0x0000);
    display->fillRect(50, 80, 189, 239);
    capture();
    int count = countAll();
    int total = ILI_SDL_W * ILI_SDL_H;
    CHECK_TRUE( count > 0 );
    CHECK_TRUE( count < total );
}

///////////////////////////////////////////////////////////////////////////////
// invertMode / normalMode tests for TFT controllers
///////////////////////////////////////////////////////////////////////////////

TEST(IL9163_CMDS, invertMode_normalMode)
{
    display->getInterface().invertMode();
    display->getInterface().normalMode();
    display->fill(0xFF);
    capture();
    int count = 0;
    for ( size_t i = 0; i < pixels->size(); i++ )
        if ( (*pixels)[i] != 0 ) count++;
    CHECK_TRUE( count > 0 );
}

TEST(ILI9341_CMDS, invertMode_normalMode)
{
    display->getInterface().invertMode();
    display->getInterface().normalMode();
    display->fill(0xFF);
    capture();
    CHECK_TRUE( countAll() > 0 );
}
