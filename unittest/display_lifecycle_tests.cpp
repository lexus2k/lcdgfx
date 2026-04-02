/*
    MIT License

    Copyright (c) 2024, Alexey Dynda

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
#include <string.h>
#include "lcdgfx.h"
#include "sdl_core.h"

// ============================================================
// SSD1306 display lifecycle tests
// ============================================================

TEST_GROUP(SSD1306_Lifecycle)
{
    DisplaySSD1306_128x64_SPI *display;

    void setup()
    {
        sdl_core_set_unittest_mode();
        display = new DisplaySSD1306_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
    }

    void teardown()
    {
        display->end();
        delete display;
    }
};

TEST(SSD1306_Lifecycle, begin_end_no_crash)
{
    // If we get here without crash, begin/end work
    CHECK(true);
}

TEST(SSD1306_Lifecycle, display_dimensions)
{
    CHECK_EQUAL(128, display->width());
    CHECK_EQUAL(64, display->height());
}

TEST(SSD1306_Lifecycle, setColor_getColor)
{
    display->setColor(0xFFFF);
    CHECK_EQUAL(0xFFFF, display->getColor());
    display->setColor(0x0000);
    CHECK_EQUAL(0x0000, display->getColor());
}

TEST(SSD1306_Lifecycle, displayOff_on)
{
    display->getInterface().displayOff();
    display->getInterface().displayOn();
    CHECK(true);
}

TEST(SSD1306_Lifecycle, setContrast)
{
    display->getInterface().setContrast(0x00);
    display->getInterface().setContrast(0x7F);
    display->getInterface().setContrast(0xFF);
    CHECK(true);
}

TEST(SSD1306_Lifecycle, invertMode_normalMode)
{
    display->getInterface().invertMode();
    display->getInterface().normalMode();
    CHECK(true);
}

TEST(SSD1306_Lifecycle, fill_and_clear)
{
    display->fill(0xFFFF);
    display->clear();
    CHECK(true);
}

TEST(SSD1306_Lifecycle, setFixedFont)
{
    display->setFixedFont(ssd1306xled_font6x8);
    NanoFont &f = display->getFont();
    SFontHeaderRecord hdr = f.getHeader();
    CHECK_EQUAL(6, hdr.width);
    CHECK_EQUAL(8, hdr.height);
}

TEST(SSD1306_Lifecycle, printFixed_renders)
{
    display->setFixedFont(ssd1306xled_font6x8);
    display->setColor(0xFFFF);
    display->printFixed(0, 0, "Test", STYLE_NORMAL);
    CHECK(true);
}

TEST(SSD1306_Lifecycle, rect_bounds)
{
    NanoRect r = display->rect();
    CHECK_EQUAL(0, r.p1.x);
    CHECK_EQUAL(0, r.p1.y);
    CHECK_EQUAL(127, r.p2.x);
    CHECK_EQUAL(63, r.p2.y);
}

// ============================================================
// SSD1331 display lifecycle tests
// ============================================================

TEST_GROUP(SSD1331_Lifecycle)
{
    DisplaySSD1331_96x64x8_SPI *display;

    void setup()
    {
        sdl_core_set_unittest_mode();
        display = new DisplaySSD1331_96x64x8_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
    }

    void teardown()
    {
        display->end();
        delete display;
    }
};

TEST(SSD1331_Lifecycle, begin_end_no_crash)
{
    CHECK(true);
}

TEST(SSD1331_Lifecycle, display_dimensions)
{
    CHECK_EQUAL(96, display->width());
    CHECK_EQUAL(64, display->height());
}

TEST(SSD1331_Lifecycle, setContrast)
{
    display->getInterface().setContrast(0x00);
    display->getInterface().setContrast(0x7F);
    display->getInterface().setContrast(0xFF);
    CHECK(true);
}

TEST(SSD1331_Lifecycle, fill_and_clear)
{
    display->fill(0xFF);
    display->clear();
    CHECK(true);
}

TEST(SSD1331_Lifecycle, setFixedFont_and_print)
{
    display->setFixedFont(ssd1306xled_font6x8);
    display->setColor(0xFF);
    display->printFixed(0, 0, "Hi", STYLE_NORMAL);
    CHECK(true);
}

TEST(SSD1331_Lifecycle, drawProgressBar)
{
    display->drawProgressBar(50);
    CHECK(true);
}

TEST(SSD1331_Lifecycle, write_string)
{
    display->setFixedFont(ssd1306xled_font6x8);
    display->setColor(0xFF);
    display->setTextCursor(0, 0);
    display->write("Hello");
    CHECK(true);
}

TEST(SSD1331_Lifecycle, print_int)
{
    display->setFixedFont(ssd1306xled_font6x8);
    display->setColor(0xFF);
    display->setTextCursor(0, 0);
    display->print(42);
    CHECK(true);
}

TEST(SSD1331_Lifecycle, invertColors)
{
    display->setColor(0xAA);
    display->setBackground(0x55);
    display->invertColors();
    CHECK_EQUAL(0x55, display->getColor());
}
