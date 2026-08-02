/*
    MIT License

    Copyright (c) 2016-2026, Alexey Dynda

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

#define W 128
#define H 64

// Verifies the Adafruit_GFX-compatible facade (GfxCompat) forwards correctly to
// the underlying lcdgfx display and honors Adafruit width/height/color semantics.
TEST_GROUP(GFX_COMPAT)
{
    DisplaySSD1306_128x64_I2C *display;
    GfxCompat<DisplaySSD1306_128x64_I2C> *gfx;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1306_128x64_I2C(-1);
        display->begin();
        display->clear();
        gfx = new GfxCompat<DisplaySSD1306_128x64_I2C>(*display);
        pixels = new std::vector<uint8_t>(sdl_core_get_pixels_len(1), 0);
    }

    void teardown()
    {
        display->end();
        delete pixels;
        delete gfx;
        delete display;
    }

    void capture()
    {
        memset(pixels->data(), 0, pixels->size());
        sdl_core_get_pixels_data(pixels->data(), 1);
    }

    int px(int x, int y)
    {
        return get_mono_pixel(pixels->data(), W, x, y);
    }
};

TEST(GFX_COMPAT, reports_dimensions)
{
    CHECK_EQUAL(W, gfx->width());
    CHECK_EQUAL(H, gfx->height());
}

TEST(GFX_COMPAT, drawPixel_sets_pixel_with_color)
{
    gfx->drawPixel(10, 20, 1);
    capture();
    CHECK_EQUAL(1, px(10, 20));
    CHECK_EQUAL(0, px(11, 20));
    CHECK_EQUAL(0, px(10, 21));
}

TEST(GFX_COMPAT, drawFastHLine_uses_width_semantics)
{
    // Adafruit width w=5 starting at x=3 must cover x=3..7 inclusive.
    gfx->drawFastHLine(3, 5, 5, 1);
    capture();
    CHECK_EQUAL(0, px(2, 5));
    CHECK_EQUAL(1, px(3, 5));
    CHECK_EQUAL(1, px(7, 5));
    CHECK_EQUAL(0, px(8, 5));
}

TEST(GFX_COMPAT, drawFastVLine_uses_height_semantics)
{
    gfx->drawFastVLine(9, 4, 3, 1);
    capture();
    CHECK_EQUAL(0, px(9, 3));
    CHECK_EQUAL(1, px(9, 4));
    CHECK_EQUAL(1, px(9, 6));
    CHECK_EQUAL(0, px(9, 7));
}

TEST(GFX_COMPAT, fillRect_covers_wxh_area)
{
    gfx->fillRect(2, 2, 4, 3, 1);
    capture();
    // Interior fully set: x=2..5, y=2..4
    CHECK_TRUE(mono_region_equals(pixels->data(), W, 2, 2, 5, 4, 1));
    // One pixel beyond the far corner must remain clear.
    CHECK_EQUAL(0, px(6, 2));
    CHECK_EQUAL(0, px(2, 5));
}

TEST(GFX_COMPAT, drawRect_outline_only)
{
    gfx->drawRect(10, 10, 5, 5, 1);
    capture();
    // corners on the outline
    CHECK_EQUAL(1, px(10, 10));
    CHECK_EQUAL(1, px(14, 14));
    // center is not filled by an outline
    CHECK_EQUAL(0, px(12, 12));
}

TEST(GFX_COMPAT, fillScreen_sets_every_pixel)
{
    gfx->fillScreen(1);
    capture();
    CHECK_TRUE(mono_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 1));
}

TEST(GFX_COMPAT, factory_helper_binds_display)
{
    auto g = makeGfxCompat(*display);
    g.drawPixel(40, 30, 1);
    capture();
    CHECK_EQUAL(1, px(40, 30));
}

TEST(GFX_COMPAT, fillCircle_fills_interior)
{
    gfx->fillCircle(64, 32, 5, 1);
    capture();
    CHECK_EQUAL(1, px(64, 32)); // center
    CHECK_EQUAL(1, px(66, 32)); // inside
    CHECK_EQUAL(0, px(72, 32)); // well outside
}

TEST(GFX_COMPAT, fillTriangle_fills_interior)
{
    gfx->fillTriangle(0, 0, 12, 0, 0, 12, 1);
    capture();
    CHECK_EQUAL(1, px(1, 1));  // inside the right-angle corner
    CHECK_EQUAL(0, px(10, 10)); // outside the hypotenuse
}

TEST(GFX_COMPAT, drawRoundRect_rounds_corners)
{
    gfx->drawRoundRect(20, 20, 20, 20, 5, 1);
    capture();
    CHECK_EQUAL(0, px(20, 20)); // hard corner is cut away
    CHECK_EQUAL(1, px(30, 20)); // straight top edge is drawn
}

TEST(GFX_COMPAT, fillRoundRect_fills_interior)
{
    gfx->fillRoundRect(40, 20, 20, 20, 5, 1);
    capture();
    CHECK_EQUAL(1, px(50, 30)); // center filled
    CHECK_EQUAL(0, px(40, 20)); // rounded corner clear
}

// Minimal Adafruit-compatible GFXfont: one 2x2 solid glyph for 'A' (0x41).
static const uint8_t s_testFontBitmaps[] = {0xF0}; // top 4 bits -> 2x2 block
static const GFXglyph s_testFontGlyphs[] = {
    // bitmapOffset, width, height, xAdvance, xOffset, yOffset
    {0, 2, 2, 3, 0, 0},
};
static const GFXfont s_testFont = {
    (uint8_t *)s_testFontBitmaps, (GFXglyph *)s_testFontGlyphs, 0x41, 0x41, 8};

TEST(GFX_COMPAT, gfx_font_renders_glyph)
{
    gfx->setFont(&s_testFont);
    gfx->setTextColor(1);
    gfx->setCursor(10, 10);
    gfx->print("A");
    capture();
    // 2x2 block at the cursor
    CHECK_EQUAL(1, px(10, 10));
    CHECK_EQUAL(1, px(11, 10));
    CHECK_EQUAL(1, px(10, 11));
    CHECK_EQUAL(1, px(11, 11));
    // nothing beyond the glyph box
    CHECK_EQUAL(0, px(12, 10));
    CHECK_EQUAL(0, px(9, 10));
    gfx->setFont(nullptr); // revert so other tests use the native font
}

TEST(GFX_COMPAT, gfx_font_advances_cursor)
{
    gfx->setFont(&s_testFont);
    gfx->setTextColor(1);
    gfx->setCursor(10, 10);
    gfx->print("AA"); // xAdvance = 3, so second glyph starts at x=13
    capture();
    CHECK_EQUAL(1, px(13, 10));
    CHECK_EQUAL(1, px(14, 10));
    gfx->setFont(nullptr);
}
