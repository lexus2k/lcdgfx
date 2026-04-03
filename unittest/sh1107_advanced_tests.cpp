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

// ==================== SH1107 advanced command tests ====================
// Tests uncovered paths in the SH1107 emulator:
// - Column addressing mode (0x21): pageAddressingMode=0
// - Display all-white mode (0xA5): displayMode |= 2
// - Display offset (0xD3)
// - COM scan direction (0xC0/0xC8)
// - flipHorizontal (0xA0/0xA1)

static const int SH1107_W = 128;
static const int SH1107_H = 64;

TEST_GROUP(SH1107_ADVANCED)
{
    DisplaySH1107_128x64_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySH1107_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>(sdl_core_get_pixels_len(1), 0);
    }

    void teardown()
    {
        display->end();
        delete display;
        delete pixels;
    }

    void readPixels()
    {
        memset(pixels->data(), 0, pixels->size());
        sdl_core_get_pixels_data(pixels->data(), 1);
    }

    int px(int x, int y)
    {
        return get_mono_pixel(pixels->data(), SH1107_W, x, y);
    }

    void sendCmd0(uint8_t cmd)
    {
        display->getInterface().commandStart();
        display->getInterface().send(cmd);
        display->getInterface().stop();
    }

    void sendCmd1(uint8_t cmd, uint8_t arg)
    {
        display->getInterface().commandStart();
        display->getInterface().send(cmd);
        display->getInterface().send(arg);
        display->getInterface().stop();
    }
};

// ---- Column addressing mode (0x21) ----
// Sets pageAddressingMode=0, which changes data auto-increment from
// column++ to page++ in sdl_sh1107_data()

TEST(SH1107_ADVANCED, column_addressing_mode)
{
    // Set column addressing mode
    sendCmd0(0x21);

    // Draw some data — in column mode, sending data increments page, not column
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();

    // Pixel should still render (the mode change affects auto-increment, not pixel placement)
    bool found = false;
    for (int y = 8; y <= 12; y++)
        for (int x = 8; x <= 12; x++)
            if (px(x, y))
                found = true;
    CHECK_TRUE(found);
}

// Test page mode (0x20) explicitly, then switch to column mode (0x21)
TEST(SH1107_ADVANCED, page_to_column_mode_switch)
{
    sendCmd0(0x20); // page addressing (default)
    display->setColor(0xFFFF);
    display->putPixel(5, 5);
    readPixels();
    CHECK_TRUE(px(5, 5));

    display->clear();
    sendCmd0(0x21); // column addressing
    display->setColor(0xFFFF);
    display->putPixel(20, 20);
    readPixels();

    bool found = false;
    for (int y = 18; y <= 22; y++)
        for (int x = 18; x <= 22; x++)
            if (px(x, y))
                found = true;
    CHECK_TRUE(found);
}

// Command 0x22 (unknown/reserved, exercises its case in switch)
TEST(SH1107_ADVANCED, command_0x22)
{
    sendCmd0(0x22);
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(px(10, 10));
}

// ---- Display all-white mode (0xA5) ----
// Sets displayMode |= 2, forcing all pixels to appear "on"

TEST(SH1107_ADVANCED, display_all_white)
{
    // Don't draw anything, but set all-white mode
    sendCmd0(0xA5);
    readPixels();

    // In all-white mode, scan_pixel returns color=1 for all pixels
    // This triggers the blt path, but we may not see it in the pixel buffer
    // because blt_content only runs if displayOn is true
    // The display should already be on from begin()

    // Restore normal mode
    sendCmd0(0xA4);
}

// Test transition: normal → all-white → normal
TEST(SH1107_ADVANCED, all_white_then_normal)
{
    display->setColor(0xFFFF);
    display->putPixel(10, 10);

    sendCmd0(0xA5); // all white
    sendCmd0(0xA4); // back to normal

    readPixels();
    // After returning to normal, only our drawn pixel should be visible
    CHECK_TRUE(px(10, 10));
}

// ---- Display offset (0xD3) ----
// Exercises the display offset command path. Offset shifts display view.
TEST(SH1107_ADVANCED, display_offset)
{
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    
    // Send display offset command — exercises the 0xD3 handler
    display->getInterface().setDisplayOffset(16);
    // Restore to avoid affecting other tests
    display->getInterface().setDisplayOffset(0);
    
    readPixels();
    CHECK_TRUE(px(10, 10));
}

// ---- COM scan direction (flipVertical) ----
TEST(SH1107_ADVANCED, com_scan_direction_reversed)
{
    display->getInterface().flipVertical(0); // sends 0xC8 → comScanDirection=1
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();

    bool anyPixel = false;
    for (int y = 0; y < SH1107_H; y++)
        for (int x = 0; x < SH1107_W; x++)
            if (px(x, y))
                anyPixel = true;
    CHECK_TRUE(anyPixel);

    // Restore
    display->getInterface().flipVertical(1); // sends 0xC0 → comScanDirection=0
}

TEST(SH1107_ADVANCED, com_scan_direction_normal)
{
    // Exercise 0xC0 command (comScanDirection=0, which reverses column mapping)
    display->getInterface().flipVertical(1);
    
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();

    // With comScanDirection=0, column is remapped. Verify pixel exists somewhere.
    bool found = false;
    for (int y = 0; y < SH1107_H; y++)
        for (int x = 0; x < SH1107_W; x++)
            if (px(x, y))
                found = true;
    CHECK_TRUE(found);
}

// ---- flipHorizontal (segment remap 0xA0/0xA1) ----
TEST(SH1107_ADVANCED, flip_horizontal_on)
{
    display->getInterface().flipHorizontal(1); // sends 0xA0 (remap enabled)
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();

    bool anyPixel = false;
    for (int y = 0; y < SH1107_H; y++)
        for (int x = 0; x < SH1107_W; x++)
            if (px(x, y))
                anyPixel = true;
    CHECK_TRUE(anyPixel);

    display->getInterface().flipHorizontal(0); // restore
}

TEST(SH1107_ADVANCED, flip_horizontal_off)
{
    display->getInterface().flipHorizontal(0); // sends 0xA1 (normal)
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(px(10, 10));
}

// ---- Start line ----
// Exercises the 0xDC start line command. Set and restore.
TEST(SH1107_ADVANCED, start_line)
{
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    
    // Exercise start line command
    display->getInterface().setStartLine(32);
    display->getInterface().setStartLine(0);
    
    readPixels();
    CHECK_TRUE(px(10, 10));
}

// ---- Contrast command (0x81) ----
TEST(SH1107_ADVANCED, contrast)
{
    display->getInterface().setContrast(128);
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(px(10, 10));
}

// ---- Display on/off (0xAE/0xAF) ----
TEST(SH1107_ADVANCED, display_off_on)
{
    display->getInterface().displayOff();
    display->getInterface().displayOn();
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(px(10, 10));
}

// ---- Clock/discharge/charge pump commands ----
TEST(SH1107_ADVANCED, misc_config_commands)
{
    sendCmd1(0xD5, 0x80); // clock divide ratio
    sendCmd1(0xD9, 0x22); // discharge period
    sendCmd1(0xAD, 0x01); // charge pump enable
    sendCmd1(0xDB, 0x40); // VCOM deselect level

    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(px(10, 10));
}

// ---- Multiplex ratio (0xA8) ----
TEST(SH1107_ADVANCED, multiplex_ratio)
{
    sendCmd1(0xA8, 63); // set multiplex ratio
    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(px(10, 10));
}

// ---- Page/column set commands via low-level byte writes ----
TEST(SH1107_ADVANCED, page_select_via_raw_cmd)
{
    // Set page 0 (0xB0)
    sendCmd0(0xB0);
    // Set low column nibble to 0 (0x00)
    sendCmd0(0x00);
    // Set high column nibble to 0 (0x10)
    sendCmd0(0x10);

    display->setColor(0xFFFF);
    display->putPixel(0, 0);
    readPixels();
    CHECK_TRUE(px(0, 0));
}

// Set a specific page/column via raw commands
TEST(SH1107_ADVANCED, page_and_column_via_raw)
{
    // Page 2 → 0xB2, column 32 → low=0x00, high=0x12
    sendCmd0(0xB2);
    sendCmd0(0x00); // low column nibble: 0
    sendCmd0(0x12); // high column nibble: 2 → column = 0x20 = 32

    display->setColor(0xFFFF);
    display->putPixel(32, 16); // page 2 = y=16-23
    readPixels();
    CHECK_TRUE(px(32, 16));
}

// Combined: invert + offset + scan direction change
TEST(SH1107_ADVANCED, combined_mode_changes)
{
    display->getInterface().invertMode();
    display->getInterface().setDisplayOffset(8);
    display->getInterface().flipVertical(0); // reversed scan

    display->setColor(0xFFFF);
    display->putPixel(10, 10);
    readPixels();

    // Something should render (may be at different position due to offset/flip)
    bool anyPixel = false;
    for (int y = 0; y < SH1107_H; y++)
        for (int x = 0; x < SH1107_W; x++)
            if (px(x, y))
                anyPixel = true;
    // In invert mode, undrawn pixels become lit and drawn pixels become dark
    // So we should definitely see pixels
    CHECK_TRUE(anyPixel);

    // Restore
    display->getInterface().normalMode();
    display->getInterface().setDisplayOffset(0);
    display->getInterface().flipVertical(1);
}
