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

// ===========================================================================
// IL9163 rotation tests — exercises s_verticalMode bit 5 (transpose),
// bit 6 (horizontal flip), bit 7 (vertical flip) and IL9163 black display bug
// ===========================================================================

static const int IL_W = 128;
static const int IL_H = 128;

TEST_GROUP(IL9163_ROTATION)
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

    int countAll()
    {
        return rgb16_count_set(pixels->data(), IL_W,
                               0, 0, IL_W - 1, IL_H - 1);
    }
};

// Rotation 0 (default: 0b00000000) — no transpose, no flip
TEST(IL9163_ROTATION, rot0_fillRect)
{
    display->getInterface().setRotation(0);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 1 (0b01100000) — bit 5 (transpose) + bit 6 (hflip)
// This exercises the transposed addressing path in sdl_il9163_data()
TEST(IL9163_ROTATION, rot1_fillRect)
{
    display->getInterface().setRotation(1);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 1 — drawHLine exercises transposed address window (0x2A/0x2B swap)
TEST(IL9163_ROTATION, rot1_drawHLine)
{
    display->getInterface().setRotation(1);
    display->setColor(0xFFFF);
    display->drawHLine(0, 32, 63);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 2 (0b11000000) — bit 7 (vflip) + bit 6 (hflip)
TEST(IL9163_ROTATION, rot2_fillRect)
{
    display->getInterface().setRotation(2);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 2 — drawLine
TEST(IL9163_ROTATION, rot2_drawLine)
{
    display->getInterface().setRotation(2);
    display->setColor(0xFFFF);
    display->drawLine(0, 0, 63, 63);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 3 (0b10100000) — bit 7 (vflip) + bit 5 (transpose)
// This exercises the transposed path with vertical flip
TEST(IL9163_ROTATION, rot3_fillRect)
{
    display->getInterface().setRotation(3);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 3 — full fill and clear verify no corruption
TEST(IL9163_ROTATION, rot3_fill_clear)
{
    display->getInterface().setRotation(3);
    display->fill(0xFF);
    capture();
    int filledCount = countAll();
    CHECK_TRUE( filledCount > 0 );
    display->clear();
    capture();
    CHECK_EQUAL( 0, countAll() );
}

// Rotation 3 — drawVLine exercises transposed vertical addressing
TEST(IL9163_ROTATION, rot3_drawVLine)
{
    display->getInterface().setRotation(3);
    display->setColor(0xFFFF);
    display->drawVLine(32, 0, 63);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// All 4 rotations sequentially — verify no state corruption
TEST(IL9163_ROTATION, all_rotations_sequential)
{
    for ( int r = 0; r < 4; r++ )
    {
        display->getInterface().setRotation(r);
        display->clear();
        display->setColor(0xFFFF);
        display->fillRect(5, 5, 25, 25);
        capture();
        CHECK_TRUE( countAll() > 0 );
    }
}

// ===========================================================================
// ILI9341 rotation tests — exercises s_verticalMode bits and s_horizontalLayout
// ===========================================================================

TEST_GROUP(ILI9341_ROTATION)
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

    int pixLen()
    {
        return (int)pixels->size();
    }

    int countNonZero()
    {
        int count = 0;
        for ( int i = 0; i < pixLen(); i++ )
            if ( (*pixels)[i] != 0 ) count++;
        return count;
    }
};

// Rotation 1 (90° CW, 0b11100000) — bit 5,6,7 all set
TEST(ILI9341_ROTATION, rot1_fillRect)
{
    display->getInterface().setRotation(1);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    CHECK_TRUE( countNonZero() > 0 );
}

// Rotation 2 (180°, 0b01010100) — bits 6,4,2
TEST(ILI9341_ROTATION, rot2_fillRect)
{
    display->getInterface().setRotation(2);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 100, 100);
    capture();
    CHECK_TRUE( countNonZero() > 0 );
}

// Rotation 3 (270°, 0b00100000) — bit 5 only (transpose)
// Key: exercises the s_verticalMode & 0b00100000 path
TEST(ILI9341_ROTATION, rot3_fillRect)
{
    display->getInterface().setRotation(3);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    CHECK_TRUE( countNonZero() > 0 );
}

// Rotation 3 — fill/clear cycle
TEST(ILI9341_ROTATION, rot3_fill_clear)
{
    display->getInterface().setRotation(3);
    display->fill(0xFF);
    capture();
    CHECK_TRUE( countNonZero() > 0 );
    display->clear();
    capture();
    CHECK_EQUAL( 0, countNonZero() );
}

// All rotations sequential
TEST(ILI9341_ROTATION, all_rotations)
{
    for ( int r = 0; r < 4; r++ )
    {
        display->getInterface().setRotation(r);
        display->clear();
        display->setColor(0xFFFF);
        display->fillRect(5, 5, 30, 30);
        capture();
        CHECK_TRUE( countNonZero() > 0 );
    }
}

// ===========================================================================
// SSD1331 rotation exercises s_leftToRight, s_topToBottom, s_verticalMode
// via 0xA0 command. Already partially covered, adding horizontal mode test.
// ===========================================================================

static const int S31_W = 96;
static const int S31_H = 64;

TEST_GROUP(SSD1331_ROTATION_EXTRA)
{
    DisplaySSD1331_96x64x8_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1331_96x64x8_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>( S31_W * S31_H, 0 );
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

    int countAll()
    {
        int c = 0;
        for ( int i = 0; i < S31_W * S31_H; i++ )
            if ( (*pixels)[i] != 0 ) c++;
        return c;
    }
};

// Rotation 0 sets ram_mode = 0b00110010 → verticalMode=0 (horizontal addressing)
// This exercises the horizontal increment path in sdl_ssd1331_data
TEST(SSD1331_ROTATION_EXTRA, rot0_horizontal_mode)
{
    display->getInterface().setRotation(0);
    display->setColor(0xFF);
    display->fillRect(0, 0, 40, 30);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation 2 sets ram_mode = 0b00100000 → verticalMode=0, different direction flags
TEST(SSD1331_ROTATION_EXTRA, rot2_horizontal_mode)
{
    display->getInterface().setRotation(2);
    display->setColor(0xFF);
    display->fillRect(0, 0, 40, 30);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Sequential rotations
TEST(SSD1331_ROTATION_EXTRA, all_rotations)
{
    for ( int r = 0; r < 4; r++ )
    {
        display->getInterface().setRotation(r);
        display->clear();
        display->setColor(0xFF);
        display->fillRect(10, 10, 30, 20);
        capture();
        CHECK_TRUE( countAll() > 0 );
    }
}

// ===========================================================================
// ST7735 display variant exercises the SDL_LCD_ST7735 detection path
// and different command argument lengths in sdl_il9163.c
// ===========================================================================

TEST_GROUP(ST7735_TESTS)
{
    DisplayST7735_128x160x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplayST7735_128x160x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        int w = 128, h = 160;
        pixels = new std::vector<uint16_t>( w * h, 0 );
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
        return rgb16_count_set(pixels->data(), 128,
                               0, 0, 127, 159);
    }
};

TEST(ST7735_TESTS, fill_all)
{
    display->fill(0xFF);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

TEST(ST7735_TESTS, clear_all)
{
    display->fill(0xFF);
    display->clear();
    capture();
    CHECK_EQUAL( 0, countAll() );
}

TEST(ST7735_TESTS, fillRect)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 100, 100);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

TEST(ST7735_TESTS, drawHLine)
{
    display->setColor(0xFFFF);
    display->drawHLine(0, 80, 127);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

TEST(ST7735_TESTS, drawVLine)
{
    display->setColor(0xFFFF);
    display->drawVLine(64, 0, 159);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Rotation exercises ST7735-specific command argument counts
TEST(ST7735_TESTS, rotation_90)
{
    display->getInterface().setRotation(1);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    int c = 0;
    for ( size_t i = 0; i < pixels->size(); i++ )
        if ( (*pixels)[i] != 0 ) c++;
    CHECK_TRUE( c > 0 );
}

TEST(ST7735_TESTS, rotation_180)
{
    display->getInterface().setRotation(2);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    int c = 0;
    for ( size_t i = 0; i < pixels->size(); i++ )
        if ( (*pixels)[i] != 0 ) c++;
    CHECK_TRUE( c > 0 );
}

TEST(ST7735_TESTS, rotation_270)
{
    display->getInterface().setRotation(3);
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    capture();
    int c = 0;
    for ( size_t i = 0; i < pixels->size(); i++ )
        if ( (*pixels)[i] != 0 ) c++;
    CHECK_TRUE( c > 0 );
}

// ===========================================================================
// ST7735 80x160 variant — exercises s_colConnection=26 path in sdl_il9163.c
// ===========================================================================

TEST_GROUP(ST7735_80x160)
{
    DisplayST7735_80x160x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplayST7735_80x160x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( 80 * 160, 0 );
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
        return rgb16_count_set(pixels->data(), 80, 0, 0, 79, 159);
    }
};

TEST(ST7735_80x160, fill_all)
{
    display->fill(0xFF);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

TEST(ST7735_80x160, clear_all)
{
    display->fill(0xFF);
    display->clear();
    capture();
    CHECK_EQUAL( 0, countAll() );
}

TEST(ST7735_80x160, fillRect)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 60, 120);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

TEST(ST7735_80x160, rotation_90)
{
    display->getInterface().setRotation(1);
    display->setColor(0xFFFF);
    display->fillRect(5, 5, 30, 30);
    capture();
    int c = 0;
    for ( size_t i = 0; i < pixels->size(); i++ )
        if ( (*pixels)[i] != 0 ) c++;
    CHECK_TRUE( c > 0 );
}

// ===========================================================================
// ST7789 135x240 variant — exercises s_colConnection=52, s_rowConnection=40
// and TOTAL_COLUMNS=480, TOTAL_ROWS=480 path in sdl_il9163.c
// ===========================================================================

TEST_GROUP(ST7789_TESTS)
{
    DisplayST7789_135x240x16_SPI *display;
    std::vector<uint16_t> *pixels;

    void setup()
    {
        display = new DisplayST7789_135x240x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint16_t>( 135 * 240, 0 );
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
        return rgb16_count_set(pixels->data(), 135, 0, 0, 134, 239);
    }
};

TEST(ST7789_TESTS, fill_all)
{
    display->fill(0xFF);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

TEST(ST7789_TESTS, clear_all)
{
    display->fill(0xFF);
    display->clear();
    capture();
    CHECK_EQUAL( 0, countAll() );
}

TEST(ST7789_TESTS, fillRect)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 100, 200);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

TEST(ST7789_TESTS, drawHLine)
{
    display->setColor(0xFFFF);
    display->drawHLine(0, 120, 134);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

TEST(ST7789_TESTS, rotation_90)
{
    display->getInterface().setRotation(1);
    display->setColor(0xFFFF);
    display->fillRect(5, 5, 50, 50);
    capture();
    int c = 0;
    for ( size_t i = 0; i < pixels->size(); i++ )
        if ( (*pixels)[i] != 0 ) c++;
    CHECK_TRUE( c > 0 );
}

TEST(ST7789_TESTS, rotation_270)
{
    display->getInterface().setRotation(3);
    display->setColor(0xFFFF);
    display->fillRect(5, 5, 50, 50);
    capture();
    int c = 0;
    for ( size_t i = 0; i < pixels->size(); i++ )
        if ( (*pixels)[i] != 0 ) c++;
    CHECK_TRUE( c > 0 );
}

TEST(ST7789_TESTS, all_rotations)
{
    for ( int r = 0; r < 4; r++ )
    {
        display->getInterface().setRotation(r);
        display->clear();
        display->setColor(0xFFFF);
        display->fillRect(5, 5, 30, 30);
        capture();
        int c = 0;
        for ( size_t i = 0; i < pixels->size(); i++ )
            if ( (*pixels)[i] != 0 ) c++;
        CHECK_TRUE( c > 0 );
    }
}
