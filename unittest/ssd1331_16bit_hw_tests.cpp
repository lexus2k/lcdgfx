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

// ==================== SSD1331 16-bit mode tests ====================
// Exercises the 16-bit data path (s_16bitmode=1), hardware drawLine (0x21),
// and copyBlock (0x23) commands in the SSD1331 emulator.

static const int SSD1331_W = 96;
static const int SSD1331_H = 64;

TEST_GROUP(SSD1331_16BIT)
{
    DisplaySSD1331_96x64x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1331_96x64x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( SSD1331_W * SSD1331_H, 0 );
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
        return (*pixels)[x + y * SSD1331_W];
    }
};

// Basic 16-bit fill triggers s_16bitmode data path (two bytes per pixel)
TEST(SSD1331_16BIT, fill_all)
{
    display->fill(0xFF);
    capture();
    int count = rgb16_count_set(pixels->data(), SSD1331_W,
                                0, 0, SSD1331_W - 1, SSD1331_H - 1);
    CHECK_TRUE( count > (SSD1331_W * SSD1331_H) / 2 );
}

TEST(SSD1331_16BIT, clear_all)
{
    display->fill(0xFF);
    display->clear();
    capture();
    int count = rgb16_count_set(pixels->data(), SSD1331_W,
                                0, 0, SSD1331_W - 1, SSD1331_H - 1);
    CHECK_EQUAL( 0, count );
}

TEST(SSD1331_16BIT, fillRect_red)
{
    display->setColor(0xF800); // Red in RGB565
    display->fillRect(10, 10, 50, 40);
    capture();
    CHECK_TRUE( px(10, 10) != 0 );
    CHECK_TRUE( px(30, 25) != 0 );
    CHECK_TRUE( px(50, 40) != 0 );
    CHECK_EQUAL( 0, px(9, 25) );
    CHECK_EQUAL( 0, px(51, 25) );
}

TEST(SSD1331_16BIT, fillRect_green)
{
    display->setColor(0x07E0); // Green
    display->fillRect(0, 0, 95, 63);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(95, 63) != 0 );
    CHECK_TRUE( px(48, 32) != 0 );
}

TEST(SSD1331_16BIT, fillRect_blue)
{
    display->setColor(0x001F); // Blue
    display->fillRect(60, 40, 90, 60);
    capture();
    CHECK_TRUE( px(60, 40) != 0 );
    CHECK_TRUE( px(90, 60) != 0 );
    CHECK_EQUAL( 0, px(59, 50) );
}

TEST(SSD1331_16BIT, drawHLine)
{
    display->setColor(0xFFFF);
    display->drawHLine(0, 32, 95);
    capture();
    for ( int x = 0; x <= 95; x++ )
        CHECK_TRUE( px(x, 32) != 0 );
    CHECK_EQUAL( 0, px(48, 31) );
    CHECK_EQUAL( 0, px(48, 33) );
}

TEST(SSD1331_16BIT, drawVLine)
{
    display->setColor(0xFFFF);
    display->drawVLine(48, 0, 63);
    capture();
    for ( int y = 0; y <= 63; y++ )
        CHECK_TRUE( px(48, y) != 0 );
    CHECK_EQUAL( 0, px(47, 32) );
    CHECK_EQUAL( 0, px(49, 32) );
}

TEST(SSD1331_16BIT, drawLine_diagonal)
{
    display->setColor(0xFFFF);
    display->drawLine(0, 0, 63, 63);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(32, 32) != 0 );
    CHECK_TRUE( px(63, 63) != 0 );
}

TEST(SSD1331_16BIT, drawRect)
{
    display->setColor(0xFFFF);
    display->drawRect(5, 5, 90, 58);
    capture();
    CHECK_TRUE( px(5, 5) != 0 );
    CHECK_TRUE( px(90, 58) != 0 );
    CHECK_TRUE( px(5, 58) != 0 );
    CHECK_TRUE( px(90, 5) != 0 );
    CHECK_EQUAL( 0, px(48, 32) );
}

// Multiple overlapping fills to test address window re-setting in 16-bit mode
TEST(SSD1331_16BIT, overlapping_fills)
{
    display->setColor(0xF800); // Red
    display->fillRect(0, 0, 47, 31);
    display->setColor(0x07E0); // Green
    display->fillRect(24, 16, 71, 47);
    capture();
    // Overlap region should be green (last drawn)
    CHECK_TRUE( px(30, 20) != 0 );
    // Red-only region
    CHECK_TRUE( px(10, 10) != 0 );
    // Green-only region
    CHECK_TRUE( px(60, 40) != 0 );
    // Outside both
    CHECK_EQUAL( 0, px(80, 55) );
}

// Fill then putPixel tests the 16-bit byte-buffering (firstByte/dataFirst)
TEST(SSD1331_16BIT, putPixel_single)
{
    display->setColor(0xFFFF);
    display->putPixel(48, 32);
    capture();
    CHECK_TRUE( px(48, 32) != 0 );
    CHECK_EQUAL( 0, px(47, 32) );
    CHECK_EQUAL( 0, px(49, 32) );
}

// ==================== SSD1331 hardware drawLine (0x21 command) ====================
// Uses getInterface().drawLine() to send the hardware line draw command

TEST_GROUP(SSD1331_HW_LINE)
{
    DisplaySSD1331_96x64x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1331_96x64x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( SSD1331_W * SSD1331_H, 0 );
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
        return (*pixels)[x + y * SSD1331_W];
    }
};

// Hardware horizontal line
TEST(SSD1331_HW_LINE, horizontal)
{
    display->getInterface().drawLine(0, 32, 95, 32, 0xFFFF);
    capture();
    CHECK_TRUE( px(0, 32) != 0 );
    CHECK_TRUE( px(48, 32) != 0 );
    // Endpoint is exclusive in the emulator's drawLine
    CHECK_EQUAL( 0, px(48, 31) );
    CHECK_EQUAL( 0, px(48, 33) );
}

// Hardware vertical line
TEST(SSD1331_HW_LINE, vertical)
{
    display->getInterface().drawLine(48, 0, 48, 63, 0xFFFF);
    capture();
    CHECK_TRUE( px(48, 0) != 0 );
    CHECK_TRUE( px(48, 32) != 0 );
    CHECK_EQUAL( 0, px(47, 32) );
    CHECK_EQUAL( 0, px(49, 32) );
}

// Hardware diagonal line (dx > dy)
TEST(SSD1331_HW_LINE, diagonal_shallow)
{
    display->getInterface().drawLine(0, 0, 90, 30, 0xF800);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(45, 15) != 0 );
}

// Hardware diagonal line (dy > dx)
TEST(SSD1331_HW_LINE, diagonal_steep)
{
    display->getInterface().drawLine(10, 0, 30, 63, 0x07E0);
    capture();
    CHECK_TRUE( px(10, 0) != 0 );
    // Midpoint may not be exactly at (20,31) due to integer line drawing
    bool midFound = false;
    for ( int x = 18; x <= 22; x++ )
        for ( int y = 28; y <= 34; y++ )
            if ( px(x, y) ) midFound = true;
    CHECK_TRUE( midFound );
}

// Hardware line with different colors (exercises 16-bit color assembly in emulator)
TEST(SSD1331_HW_LINE, color_red)
{
    display->getInterface().drawLine(0, 0, 95, 0, 0xF800);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(48, 0) != 0 );
}

TEST(SSD1331_HW_LINE, color_blue)
{
    display->getInterface().drawLine(0, 63, 95, 63, 0x001F);
    capture();
    CHECK_TRUE( px(0, 63) != 0 );
    CHECK_TRUE( px(48, 63) != 0 );
}

// Multiple hardware lines in different directions
TEST(SSD1331_HW_LINE, cross_pattern)
{
    display->getInterface().drawLine(0, 32, 95, 32, 0xFFFF);
    display->getInterface().drawLine(48, 0, 48, 63, 0xFFFF);
    capture();
    CHECK_TRUE( px(0, 32) != 0 );
    CHECK_TRUE( px(94, 32) != 0 ); // endpoint is exclusive in emulator
    CHECK_TRUE( px(48, 0) != 0 );
    CHECK_TRUE( px(48, 62) != 0 );
    CHECK_TRUE( px(48, 32) != 0 ); // intersection
}

// ==================== SSD1331 hardware copyBlock (0x23 command) ====================

TEST_GROUP(SSD1331_HW_COPY)
{
    DisplaySSD1331_96x64x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1331_96x64x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( SSD1331_W * SSD1331_H, 0 );
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
        return (*pixels)[x + y * SSD1331_W];
    }
};

// Copy a filled rectangle to a new position
TEST(SSD1331_HW_COPY, copy_right)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 19, 19);
    // Copy block from (0,0)-(19,19) to (40,0)
    display->getInterface().copyBlock(0, 0, 19, 19, 40, 0);
    capture();
    // Original should still be there
    CHECK_TRUE( px(10, 10) != 0 );
    // Copy destination should also be set
    CHECK_TRUE( px(50, 10) != 0 );
}

// Copy block downward
TEST(SSD1331_HW_COPY, copy_down)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 5, 30, 15);
    display->getInterface().copyBlock(10, 5, 30, 15, 10, 40);
    capture();
    CHECK_TRUE( px(20, 10) != 0 );  // original
    CHECK_TRUE( px(20, 45) != 0 );  // copy
}

// Copy overlapping blocks
TEST(SSD1331_HW_COPY, copy_overlap)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 29, 29);
    // Copy with overlap: destination overlaps source
    display->getInterface().copyBlock(0, 0, 29, 29, 15, 15);
    capture();
    // Both original and destination regions should have content
    CHECK_TRUE( px(5, 5) != 0 );
    CHECK_TRUE( px(25, 25) != 0 );
}

// ==================== SSD1331 setRotation exercises 0xA0 remap ====================
// Different rotation values set different flags (s_leftToRight, s_topToBottom, s_verticalMode)

TEST_GROUP(SSD1331_ROTATION)
{
    DisplaySSD1331_96x64x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1331_96x64x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( SSD1331_W * SSD1331_H, 0 );
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
        return rgb16_count_set(pixels->data(), SSD1331_W,
                               0, 0, SSD1331_W - 1, SSD1331_H - 1);
    }
};

// Rotation 0 (default) - leftToRight=1, topToBottom=1, verticalMode=0
TEST(SSD1331_ROTATION, rot0_fill)
{
    display->getInterface().setRotation(0);
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 20, 20);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 1 (90 CW) - exercises different remap flags
TEST(SSD1331_ROTATION, rot1_fill)
{
    display->getInterface().setRotation(1);
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 20, 20);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 2 (180) - reverses leftToRight and topToBottom
TEST(SSD1331_ROTATION, rot2_fill)
{
    display->getInterface().setRotation(2);
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 20, 20);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 3 (270) - yet another remap config
TEST(SSD1331_ROTATION, rot3_fill)
{
    display->getInterface().setRotation(3);
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 20, 20);
    capture();
    CHECK_TRUE( countAll() > 0 );
}
