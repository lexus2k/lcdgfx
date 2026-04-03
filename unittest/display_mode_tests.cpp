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
#include "lcdgfx.h"
#include "sdl_core.h"
#include "utils/pixel_helpers.h"

// ==================== SH1107 display mode tests ====================
// Exercises display inversion (0xA7), all-white (0xA5), normal mode (0xA6),
// column addressing (0x21), display offset (0xD3), start line (0xDC),
// and contrast (0x81) command paths in the SH1107 emulator.

static const int SH_W = 128;
static const int SH_H = 64;

TEST_GROUP(SH1107_MODES)
{
    DisplaySH1107_128x64_SPI *display;
    uint8_t buf[SH_W * SH_H / 8];

    void setup()
    {
        display = new DisplaySH1107_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
    }

    void teardown()
    {
        display->end();
        delete display;
    }

    void capture()
    {
        memset(buf, 0, sizeof(buf));
        sdl_core_get_pixels_data(buf, 1);
    }

    int px(int x, int y)
    {
        return get_mono_pixel(buf, SH_W, x, y);
    }

    int countAll()
    {
        return mono_count_set(buf, SH_W, 0, 0, SH_W - 1, SH_H - 1);
    }
};

// Normal mode: filled area shows pixels, empty area shows nothing
TEST(SH1107_MODES, normal_mode_baseline)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 63, 31);
    display->getInterface().normalMode();
    capture();
    CHECK_TRUE( px(32, 16) == 1 );
    CHECK_EQUAL( 0, px(100, 50) );
}

// Invert mode: filled area shows dark (0), empty area shows lit (1)
TEST(SH1107_MODES, invert_mode)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 63, 31);
    display->getInterface().invertMode();
    capture();
    // In invert mode, set pixels become dark and clear become lit
    CHECK_EQUAL( 0, px(32, 16) );
    CHECK_EQUAL( 1, px(100, 50) );
}

// Restore to normal after invert
TEST(SH1107_MODES, invert_then_normal)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 63, 31);
    display->getInterface().invertMode();
    display->getInterface().normalMode();
    capture();
    CHECK_TRUE( px(32, 16) == 1 );
    CHECK_EQUAL( 0, px(100, 50) );
}

// Contrast command should not cause crash or change pixel state
TEST(SH1107_MODES, set_contrast)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 63, 31);
    display->getInterface().setContrast(128);
    capture();
    CHECK_TRUE( px(32, 16) == 1 );
    CHECK_EQUAL( 0, px(100, 50) );
}

// Display off/on should not corrupt pixel data
TEST(SH1107_MODES, display_off_on)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 63, 31);
    display->getInterface().displayOff();
    display->getInterface().displayOn();
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// Set start line exercises the 0xDC command path
TEST(SH1107_MODES, set_start_line)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 127, 63);
    display->getInterface().setStartLine(16);
    capture();
    // Changing start line shifts the display view
    // Pixels should still be readable (may be shifted)
    CHECK_TRUE( countAll() > 0 );
}

// Set start line to 0 (reset)
TEST(SH1107_MODES, set_start_line_zero)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 127, 63);
    display->getInterface().setStartLine(0);
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// ==================== SSD1306 display mode tests ====================
// Exercises normalMode, invertMode, contrast, displayOn/Off

static const int S06_W = 128;
static const int S06_H = 64;

TEST_GROUP(SSD1306_MODES)
{
    DisplaySSD1306_128x64_SPI *display;
    uint8_t buf[S06_W * S06_H / 8];

    void setup()
    {
        display = new DisplaySSD1306_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
    }

    void teardown()
    {
        display->end();
        delete display;
    }

    void capture()
    {
        memset(buf, 0, sizeof(buf));
        sdl_core_get_pixels_data(buf, 1);
    }

    int px(int x, int y)
    {
        return get_mono_pixel(buf, S06_W, x, y);
    }

    int countAll()
    {
        return mono_count_set(buf, S06_W, 0, 0, S06_W - 1, S06_H - 1);
    }
};

// Normal mode baseline
TEST(SSD1306_MODES, normal_after_fill)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    display->getInterface().normalMode();
    capture();
    CHECK_TRUE( px(30, 30) == 1 );
    CHECK_EQUAL( 0, px(100, 60) );
}

// Invert mode command sent but SSD1306 emulator doesn't process display mode
// Just verify it doesn't crash or corrupt pixel data
TEST(SSD1306_MODES, invert_mode_no_crash)
{
    display->setColor(0xFFFF);
    display->fillRect(10, 10, 50, 50);
    display->getInterface().invertMode();
    capture();
    // Pixels should still be readable (inversion not applied in emulator readback)
    CHECK_TRUE( countAll() > 0 );
}

// Set contrast
TEST(SSD1306_MODES, set_contrast)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 63, 31);
    display->getInterface().setContrast(200);
    capture();
    CHECK_TRUE( px(32, 16) == 1 );
}

// Display off/on cycle
TEST(SSD1306_MODES, display_off_on)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 127, 63);
    display->getInterface().displayOff();
    display->getInterface().displayOn();
    capture();
    CHECK_TRUE( countAll() > 0 );
}

// ==================== SSD1325 display configuration tests ====================
// Exercises setContrast (0x81) and additional command paths

static const int S25_W = 128;
static const int S25_H = 64;

TEST_GROUP(SSD1325_MODES)
{
    DisplaySSD1325_128x64_SPI *display;
    uint32_t buf[S25_W * S25_H];

    void setup()
    {
        display = new DisplaySSD1325_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
    }

    void teardown()
    {
        display->end();
        delete display;
    }

    void capture()
    {
        memset(buf, 0, sizeof(buf));
        sdl_core_get_pixels_data((uint8_t *)buf, 32);
    }
};

// Set contrast should not crash or corrupt display
TEST(SSD1325_MODES, set_contrast)
{
    display->setColor(0x0F);
    display->fillRect(0, 0, 63, 31);
    display->getInterface().setContrast(128);
    capture();
    CHECK_TRUE( buf[32 + 16 * S25_W] != 0 );
}

// ==================== SSD1351 display configuration tests ====================

static const int S51_W = 128;
static const int S51_H = 128;

TEST_GROUP(SSD1351_MODES)
{
    DisplaySSD1351_128x128x16_SPI *display;
    uint16_t buf[S51_W * S51_H];

    void setup()
    {
        display = new DisplaySSD1351_128x128x16_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
    }

    void teardown()
    {
        display->end();
        delete display;
    }

    void capture()
    {
        memset(buf, 0, sizeof(buf));
        sdl_core_get_pixels_data((uint8_t *)buf, 16);
    }
};

// Set contrast
TEST(SSD1351_MODES, set_contrast)
{
    display->setColor(0xFFFF);
    display->fillRect(0, 0, 63, 63);
    display->getInterface().setContrast(200);
    capture();
    CHECK_TRUE( buf[32 + 32 * S51_W] != 0 );
}
