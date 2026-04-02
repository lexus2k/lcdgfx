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
#include "canvas/canvas.h"
#include "canvas/fonts/fonts.h"

// ============================================================
// NanoFont tests using NanoCanvas8 as rendering surface
// ============================================================

static const int FW = 64;
static const int FH = 32;
static uint8_t font_buf[FW * FH];

TEST_GROUP(Font)
{
    NanoCanvasOps<8> canvas;

    void setup()
    {
        memset(font_buf, 0, sizeof(font_buf));
        canvas.begin(FW, FH, font_buf);
        canvas.setColor(0xFF);
    }

    void teardown() {}

    uint8_t px(int x, int y) { return font_buf[x + y * FW]; }

    int countLitPixels(int x1, int y1, int x2, int y2)
    {
        int count = 0;
        for (int y = y1; y <= y2; y++)
            for (int x = x1; x <= x2; x++)
                if (px(x, y)) count++;
        return count;
    }
};

TEST(Font, loadFixedFont_6x8)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    NanoFont &f = canvas.getFont();
    SFontHeaderRecord hdr = f.getHeader();
    CHECK_EQUAL(6, hdr.width);
    CHECK_EQUAL(8, hdr.height);
}

TEST(Font, loadFixedFont_5x7)
{
    canvas.setFixedFont(ssd1306xled_font5x7);
    NanoFont &f = canvas.getFont();
    SFontHeaderRecord hdr = f.getHeader();
    CHECK_EQUAL(5, hdr.width);
    CHECK_EQUAL(7, hdr.height);
}

TEST(Font, getTextSize_single_char)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    NanoFont &f = canvas.getFont();
    lcduint_t height;
    lcduint_t width = f.getTextSize("A", &height);
    CHECK(width > 0);
    CHECK(height > 0);
}

TEST(Font, getTextSize_string)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    NanoFont &f = canvas.getFont();
    lcduint_t w1 = f.getTextSize("A", nullptr);
    lcduint_t w3 = f.getTextSize("ABC", nullptr);
    CHECK(w3 > w1);
}

TEST(Font, getTextSize_empty)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    NanoFont &f = canvas.getFont();
    lcduint_t width = f.getTextSize("", nullptr);
    CHECK_EQUAL(0, width);
}

TEST(Font, printFixed_renders_pixels)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.printFixed(0, 0, "A", STYLE_NORMAL);
    int lit = countLitPixels(0, 0, 5, 7);
    CHECK(lit > 0);
}

TEST(Font, printFixed_different_chars_differ)
{
    canvas.setFixedFont(ssd1306xled_font6x8);

    // Render 'A'
    uint8_t a_buf[FW * FH];
    canvas.printFixed(0, 0, "A", STYLE_NORMAL);
    memcpy(a_buf, font_buf, sizeof(a_buf));

    // Render 'B'
    memset(font_buf, 0, sizeof(font_buf));
    canvas.printFixed(0, 0, "B", STYLE_NORMAL);

    // At least some pixels should differ
    int diffs = 0;
    for (int i = 0; i < FW * FH; i++)
        if (a_buf[i] != font_buf[i]) diffs++;
    CHECK(diffs > 0);
}

TEST(Font, printFixed_at_offset)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.printFixed(20, 10, "X", STYLE_NORMAL);
    int lit = countLitPixels(20, 10, 25, 17);
    CHECK(lit > 0);
    // Area before x=20 at row 10 should be empty
    int before = countLitPixels(0, 10, 19, 17);
    CHECK_EQUAL(0, before);
}

TEST(Font, printFixed_multichar)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.printFixed(0, 0, "AB", STYLE_NORMAL);
    // First char occupies 0..5, second starts at 6+
    int litFirst = countLitPixels(0, 0, 5, 7);
    int litSecond = countLitPixels(6, 0, 11, 7);
    CHECK(litFirst > 0);
    CHECK(litSecond > 0);
}

TEST(Font, getCharBitmap_returns_data)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    NanoFont &f = canvas.getFont();
    SCharInfo info;
    f.getCharBitmap('A', &info);
    CHECK(info.width > 0);
    CHECK(info.height > 0);
    CHECK(info.glyph != nullptr);
}

TEST(Font, setFontSpacing)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setFontSpacing(4);
    NanoFont &f = canvas.getFont();
    lcduint_t w_wide = f.getTextSize("AB", nullptr);

    canvas.setFontSpacing(0);
    lcduint_t w_tight = f.getTextSize("AB", nullptr);
    CHECK(w_wide > w_tight);
}

TEST(Font, printFixed_bold_style)
{
    canvas.setFixedFont(ssd1306xled_font6x8);

    // Normal
    canvas.printFixed(0, 0, "A", STYLE_NORMAL);
    uint8_t normal_buf[FW * FH];
    memcpy(normal_buf, font_buf, sizeof(normal_buf));
    int litNormal = countLitPixels(0, 0, 7, 7);

    // Bold
    memset(font_buf, 0, sizeof(font_buf));
    canvas.printFixed(0, 0, "A", STYLE_BOLD);
    int litBold = countLitPixels(0, 0, 7, 7);

    // Bold should have equal or more pixels
    CHECK(litBold >= litNormal);
}

TEST(Font, font_8x16_dimensions)
{
    canvas.setFixedFont(ssd1306xled_font8x16);
    NanoFont &f = canvas.getFont();
    SFontHeaderRecord hdr = f.getHeader();
    CHECK_EQUAL(8, hdr.width);
    CHECK_EQUAL(16, hdr.height);
}

TEST(Font, getPages)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    NanoFont &f = canvas.getFont();
    CHECK_EQUAL(1, f.getPages());

    canvas.setFixedFont(ssd1306xled_font8x16);
    CHECK_EQUAL(2, f.getPages());
}
