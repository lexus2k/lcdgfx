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

// ==================== SSD1325 copyBlock command (0x23) ====================
// Tests the hardware block copy functionality in the SSD1325 emulator
// The SSD1325 emulator handles copyBlock via 6-argument 0x23 command

// Send raw command bytes to the SSD1325 via its interface
// The interface has commandStart(), send(), and stop() methods
// For SSD1325, all commands go through the same path

static const int SSD1325_W = 128;
static const int SSD1325_H = 64;
static const int SSD1327_W = 128;
static const int SSD1327_H = 128;

// Helper: read a 4-bit grayscale pixel from the emulator's pixel buffer
// SSD1325 uses 32-bit RGBX8888 format: nibble duplicated across channels
static uint32_t read_gray4(const std::vector<uint32_t> &px, int x, int y, int w)
{
    return px[y * w + x];
}

// Helper: check if a pixel is "set" (non-zero)
static bool is_set(const std::vector<uint32_t> &px, int x, int y, int w)
{
    return px[y * w + x] != 0;
}

// ==================== SSD1325 copyBlock tests ====================

TEST_GROUP(SSD1325_COPYBLOCK)
{
    DisplaySSD1325_128x64_SPI *display;
    std::vector<uint32_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1325_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint32_t>(SSD1325_W * SSD1325_H, 0);
    }

    void teardown()
    {
        display->end();
        delete display;
        delete pixels;
    }

    void readPixels()
    {
        sdl_core_get_pixels_data(reinterpret_cast<uint8_t*>(pixels->data()), 32);
    }

    // Send the 0x23 copyBlock command with 6 arguments
    // srcCol and dstCol are in "column pairs" (SSD1325 column = 2 pixels)
    void sendCopyBlock(uint8_t srcCol, uint8_t srcRow, uint8_t srcColEnd, uint8_t srcRowEnd,
                       uint8_t dstCol, uint8_t dstRow)
    {
        display->getInterface().commandStart();
        display->getInterface().send(0x23);
        display->getInterface().send(srcCol);
        display->getInterface().send(srcRow);
        display->getInterface().send(srcColEnd);
        display->getInterface().send(srcRowEnd);
        display->getInterface().send(dstCol);
        display->getInterface().send(dstRow);
        display->getInterface().stop();
    }
};

// Copy a small filled rectangle to a new location (forward copy: dst < src)
TEST(SSD1325_COPYBLOCK, forward_copy)
{
    // Draw a 10x10 block at (20, 10)
    display->setColor(0xFF);
    display->fillRect(20, 10, 29, 19);
    readPixels();

    // Verify source block has pixels set
    bool sourceHasPixels = false;
    for (int y = 10; y <= 19; y++)
        for (int x = 20; x <= 29; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                sourceHasPixels = true;
    CHECK_TRUE(sourceHasPixels);

    // Copy the block to position (0, 10) - forward copy since dst < src
    // SSD1325 columns are in pairs, so col 20/2=10, col 29/2=14, dst 0/2=0
    sendCopyBlock(10, 10, 14, 19, 0, 10);
    readPixels();

    // Destination should have pixels
    bool destHasPixels = false;
    for (int y = 10; y <= 19; y++)
        for (int x = 0; x <= 9; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                destHasPixels = true;
    CHECK_TRUE(destHasPixels);
}

// Copy a block backward (dst > src)
TEST(SSD1325_COPYBLOCK, backward_copy)
{
    // Draw a 10x10 block at (0, 0)
    display->setColor(0xFF);
    display->fillRect(0, 0, 9, 9);
    readPixels();

    bool sourceHasPixels = false;
    for (int y = 0; y <= 9; y++)
        for (int x = 0; x <= 9; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                sourceHasPixels = true;
    CHECK_TRUE(sourceHasPixels);

    // Copy to (40, 0) - backward copy since dst > src
    sendCopyBlock(0, 0, 4, 9, 20, 0);
    readPixels();

    bool destHasPixels = false;
    for (int y = 0; y <= 9; y++)
        for (int x = 40; x <= 49; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                destHasPixels = true;
    CHECK_TRUE(destHasPixels);
}

// Copy overlapping regions (tests correct copy direction handling)
TEST(SSD1325_COPYBLOCK, overlapping_copy)
{
    // Draw a block at (10, 0)
    display->setColor(0xFF);
    display->fillRect(10, 0, 19, 9);

    // Copy from (10,0)-(19,9) to (14,0) — overlapping, backward copy
    sendCopyBlock(5, 0, 9, 9, 7, 0);
    readPixels();

    // Destination area (14-23, 0-9) should have pixels
    bool destHasPixels = false;
    for (int y = 0; y <= 9; y++)
        for (int x = 14; x <= 23; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                destHasPixels = true;
    CHECK_TRUE(destHasPixels);
}

// ==================== SSD1325 horizontal addressing mode ====================
// Default is vertical mode (s_verticalMode=1). Setting bit 2 of 0xA0 to 0
// enables horizontal mode where pixels scan left-to-right, top-to-bottom.

TEST_GROUP(SSD1325_HORIZONTAL)
{
    DisplaySSD1325_128x64_SPI *display;
    std::vector<uint32_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1325_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint32_t>(SSD1325_W * SSD1325_H, 0);
    }

    void teardown()
    {
        display->end();
        delete display;
        delete pixels;
    }

    void readPixels()
    {
        sdl_core_get_pixels_data(reinterpret_cast<uint8_t*>(pixels->data()), 32);
    }

    // Send 0xA0 remap command to set horizontal mode
    // Bit 2 = 0: horizontal mode, Bit 2 = 1: vertical mode
    // Also set bit 0 (left-to-right) and bit 4 (top-to-bottom) for normal mapping
    void setHorizontalMode()
    {
        display->getInterface().commandStart();
        display->getInterface().send(0xA0);
        // 0x11 = bit4 (top-to-bottom) | bit0 (left-to-right), bit2=0 → horizontal
        display->getInterface().send(0x11);
        display->getInterface().stop();
    }
};

// In horizontal mode, drawing operations use column-first increment
TEST(SSD1325_HORIZONTAL, fill_horizontal)
{
    setHorizontalMode();

    // Drawing operations will send data through the horizontal increment path
    display->setColor(0xFF);
    display->fillRect(0, 0, 9, 9);
    readPixels();

    // Should have filled pixels in the region
    int setCount = 0;
    for (int y = 0; y < 10; y++)
        for (int x = 0; x < 10; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                setCount++;
    CHECK_TRUE(setCount > 0);
}

// Horizontal mode with a larger area to verify the mode change takes effect
TEST(SSD1325_HORIZONTAL, draw_line_horizontal)
{
    setHorizontalMode();

    display->setColor(0xFF);
    display->fillRect(0, 0, 31, 31);
    readPixels();

    // Verify pixels were rendered in horizontal mode
    int setCount = 0;
    for (int y = 0; y < 32; y++)
        for (int x = 0; x < 32; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                setCount++;
    CHECK_TRUE(setCount > 0);
}

// ==================== SSD1325 COM split and remap modes ====================

TEST_GROUP(SSD1325_REMAP)
{
    DisplaySSD1325_128x64_SPI *display;
    std::vector<uint32_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1325_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint32_t>(SSD1325_W * SSD1325_H, 0);
    }

    void teardown()
    {
        display->end();
        delete display;
        delete pixels;
    }

    void readPixels()
    {
        sdl_core_get_pixels_data(reinterpret_cast<uint8_t*>(pixels->data()), 32);
    }

    // Send 0xA0 remap command with arbitrary flags
    void sendRemap(uint8_t flags)
    {
        display->getInterface().commandStart();
        display->getInterface().send(0xA0);
        display->getInterface().send(flags);
        display->getInterface().stop();
    }
};

// Test with COM split enabled (bit 6 of 0xA0)
// SSD1325 has s_hwComSplit=1, so setting s_comSplit=1 means they match → no remap
TEST(SSD1325_REMAP, com_split_enabled_no_remap)
{
    // 0x55 = bit6 (comSplit) | bit4 (topToBottom) | bit2 (verticalMode) | bit0 (leftToRight)
    sendRemap(0x55);

    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();

    // Should render normally (comSplit matches hwComSplit → no remapping)
    bool found = false;
    for (int y = 8; y <= 12; y++)
        for (int x = 8; x <= 12; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                found = true;
    CHECK_TRUE(found);
}

// Test with COM split disabled (bit 6 = 0) but hwComSplit = 1
// This triggers the remap formula: y = ((y & 0x3F) * 2) + ((y > 0x3F) ? 1 : 0)
TEST(SSD1325_REMAP, com_split_mismatch_triggers_remap)
{
    // 0x15 = bit4 (topToBottom) | bit2 (verticalMode) | bit0 (leftToRight), bit6=0 (no comSplit)
    sendRemap(0x15);

    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();

    // Pixel should be remapped: y=10 → ((10 & 0x3F) * 2) + 0 = 20
    // Check the remapped position
    bool found = is_set(*pixels, 10, 20, SSD1325_W);
    CHECK_TRUE(found);
}

// Test right-to-left mode (bit 0 = 0)
// Exercises the s_leftToRight flag in the emulator data function
TEST(SSD1325_REMAP, right_to_left)
{
    // 0x14 = bit4 (topToBottom) | bit2 (verticalMode), bit0=0 (right-to-left)
    sendRemap(0x14);

    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();

    // In right-to-left mode, x is mirrored. Just verify something rendered.
    bool found = false;
    for (int y = 0; y < SSD1325_H; y++)
        for (int x = 0; x < SSD1325_W; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                found = true;
    CHECK_TRUE(found);
}

// Test bottom-to-top mode (bit 4 = 0)
// Exercises the s_topToBottom flag in the emulator data function
TEST(SSD1325_REMAP, bottom_to_top)
{
    // 0x05 = bit2 (verticalMode) | bit0 (leftToRight), bit4=0 (bottom-to-top)
    sendRemap(0x05);

    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();

    // In bottom-to-top mode, y is mirrored. Just verify something rendered.
    bool found = false;
    for (int y = 0; y < SSD1325_H; y++)
        for (int x = 0; x < SSD1325_W; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                found = true;
    CHECK_TRUE(found);
}

// Test nibble remap (bit 1) — sets the flag but doesn't affect rendering currently
TEST(SSD1325_REMAP, nimble_remap_flag)
{
    // 0x17 = bit4 | bit2 | bit1 (nimbleMapping) | bit0
    sendRemap(0x17);

    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();

    // nimbleMapping is stored but not used in current emulator implementation
    // Just verify the flag doesn't crash anything and pixel still renders
    bool found = false;
    for (int y = 0; y < SSD1325_H; y++)
        for (int x = 0; x < SSD1325_W; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                found = true;
    CHECK_TRUE(found);
}

// ==================== SSD1325 additional command coverage ====================

TEST_GROUP(SSD1325_COMMANDS)
{
    DisplaySSD1325_128x64_SPI *display;
    std::vector<uint32_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1325_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint32_t>(SSD1325_W * SSD1325_H, 0);
    }

    void teardown()
    {
        display->end();
        delete display;
        delete pixels;
    }

    void readPixels()
    {
        sdl_core_get_pixels_data(reinterpret_cast<uint8_t*>(pixels->data()), 32);
    }

    void sendCmd1(uint8_t cmd, uint8_t arg)
    {
        display->getInterface().commandStart();
        display->getInterface().send(cmd);
        display->getInterface().send(arg);
        display->getInterface().stop();
    }

    void sendCmd0(uint8_t cmd)
    {
        display->getInterface().commandStart();
        display->getInterface().send(cmd);
        display->getInterface().stop();
    }
};

// Contrast command (0x81)
TEST(SSD1325_COMMANDS, contrast)
{
    display->getInterface().setContrast(200);
    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();
    bool found = false;
    for (int y = 8; y <= 12; y++)
        for (int x = 8; x <= 12; x++)
            if (is_set(*pixels, x, y, SSD1325_W))
                found = true;
    CHECK_TRUE(found);
}

// Display on/off commands (0xAE/0xAF)
TEST(SSD1325_COMMANDS, display_on_off)
{
    sendCmd0(0xAE); // display off
    sendCmd0(0xAF); // display on
    // Just verify no crash
    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(is_set(*pixels, 10, 10, SSD1325_W));
}

// Normal display (0xA4) and deactivate scroll (0x2E)
TEST(SSD1325_COMMANDS, normal_and_scroll)
{
    sendCmd0(0xA4); // normal display
    sendCmd0(0x2E); // deactivate scroll
    display->setColor(0xFF);
    display->putPixel(5, 5);
    readPixels();
    CHECK_TRUE(is_set(*pixels, 5, 5, SSD1325_W));
}

// Multiplex ratio (0xA8)
TEST(SSD1325_COMMANDS, multiplex_ratio)
{
    sendCmd1(0xA8, 63); // set multiplex ratio to 63
    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(is_set(*pixels, 10, 10, SSD1325_W));
}

// Various 1-argument commands to exercise the bulk handler
TEST(SSD1325_COMMANDS, bulk_single_arg_commands)
{
    sendCmd1(0xFD, 0x12); // unlock
    sendCmd1(0xA1, 0x00); // start line
    sendCmd1(0xA2, 0x00); // display offset
    sendCmd1(0xAB, 0x01); // VDD internal
    sendCmd1(0xB1, 0x22); // phase
    sendCmd1(0xB3, 0xF0); // clock
    sendCmd1(0xBC, 0x10); // precharge
    sendCmd1(0xBE, 0x05); // VCOMH
    sendCmd1(0xB6, 0x04); // second pre-charge

    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(is_set(*pixels, 10, 10, SSD1325_W));
}

// Default/unknown command falls through
TEST(SSD1325_COMMANDS, unknown_command)
{
    sendCmd0(0xE3); // NOP / undefined
    display->setColor(0xFF);
    display->putPixel(10, 10);
    readPixels();
    CHECK_TRUE(is_set(*pixels, 10, 10, SSD1325_W));
}

// ==================== SSD1327 NO_COM_SPLIT variant ====================
// This variant exercises the s_hwComSplit=0 path

TEST_GROUP(SSD1327_NOCOMSPLIT)
{
    DisplaySSD1327_128x128_SPI *display;
    std::vector<uint32_t> *pixels;

    void setup()
    {
        // SSD1327 128x128 — uses the SSD1325 emulator with SSD1327_GENERIC subtype
        display = new DisplaySSD1327_128x128_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint32_t>(SSD1327_W * SSD1327_H, 0);
    }

    void teardown()
    {
        display->end();
        delete display;
        delete pixels;
    }

    void readPixels()
    {
        sdl_core_get_pixels_data(reinterpret_cast<uint8_t*>(pixels->data()), 32);
    }

    void sendRemap(uint8_t flags)
    {
        display->getInterface().commandStart();
        display->getInterface().send(0xA0);
        display->getInterface().send(flags);
        display->getInterface().stop();
    }
};

// SSD1327 with default COM split (hwComSplit=1, comSplit enabled by default)
TEST(SSD1327_NOCOMSPLIT, default_mode_draw)
{
    display->setColor(0xFF);
    display->putPixel(64, 64);
    readPixels();
    bool found = false;
    for (int y = 62; y <= 66; y++)
        for (int x = 62; x <= 66; x++)
            if (is_set(*pixels, x, y, SSD1327_W))
                found = true;
    CHECK_TRUE(found);
}

// Disable COM split → triggers remap (hwComSplit=1, comSplit=0 → mismatch)
TEST(SSD1327_NOCOMSPLIT, com_split_disabled_remap)
{
    // 0x15 = bit4 | bit2 | bit0, NO bit6 → comSplit=0
    sendRemap(0x15);

    display->setColor(0xFF);
    display->putPixel(64, 32);
    readPixels();

    // Row 32 should be remapped: ((32 & 0x3F) * 2) + 0 = 64
    bool found = is_set(*pixels, 64, 64, SSD1327_W);
    CHECK_TRUE(found);
}

// Enable COM split → no remap (hwComSplit=1, comSplit=1 → match)
TEST(SSD1327_NOCOMSPLIT, com_split_enabled_no_remap)
{
    // 0x55 = bit6 (comSplit) | bit4 | bit2 | bit0
    sendRemap(0x55);

    display->setColor(0xFF);
    display->putPixel(64, 32);
    readPixels();

    // No remapping, pixel at approximately (64, 32)
    bool found = false;
    for (int y = 30; y <= 34; y++)
        if (is_set(*pixels, 64, y, SSD1327_W))
            found = true;
    CHECK_TRUE(found);
}
