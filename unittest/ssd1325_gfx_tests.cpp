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

// SSD1325: 4-bit grayscale, 128x64
// NOTE: 4-bit displays pack 2 pixels per byte (nibble-pair).
// drawVLine at even x overwrites adjacent odd x (no GDRAM read-modify-write).
// fillRect now correctly handles all 16 colours and partial-edge column-pairs.
// Tests focus on operations that work correctly through the full pipeline.

static const int W = 128;
static const int H = 64;

TEST_GROUP(SSD1325_GFX)
{
    DisplaySSD1325_128x64_SPI *display;
    std::vector<uint8_t> *pixels;

    void setup()
    {
        display = new DisplaySSD1325_128x64_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>( sdl_core_get_pixels_len(4), 0 );
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
        sdl_core_get_pixels_data( pixels->data(), 4 );
    }

    uint8_t px(int x, int y)
    {
        return get_gray4_pixel(pixels->data(), W, x, y);
    }
};

TEST(SSD1325_GFX, clear_sets_all_pixels_to_zero)
{
    capture();
    CHECK_TRUE( gray4_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 0) );
}

TEST(SSD1325_GFX, fill_sets_all_pixels)
{
    display->fill(0xFF);
    capture();
    for ( int y = 0; y < H; y++ )
        for ( int x = 0; x < W; x++ )
            CHECK_TRUE( px(x, y) != 0 );
}

TEST(SSD1325_GFX, putPixel_single)
{
    display->setColor(0x0F);
    display->putPixel(10, 10);
    capture();
    CHECK_TRUE( px(10, 10) != 0 );
}

TEST(SSD1325_GFX, putPixel_even_odd)
{
    display->setColor(0x0F);
    display->putPixel(20, 10);
    capture();
    CHECK_TRUE( px(20, 10) != 0 );

    display->clear();
    display->setColor(0x0F);
    display->putPixel(21, 10);
    capture();
    CHECK_TRUE( px(21, 10) != 0 );
}

TEST(SSD1325_GFX, drawHLine)
{
    display->setColor(0x0F);
    display->drawHLine(5, 10, 20);
    capture();
    for ( int x = 5; x <= 20; x++ )
        CHECK_TRUE( px(x, 10) != 0 );
    CHECK_EQUAL( 0, px(4, 10) );
    CHECK_EQUAL( 0, px(21, 10) );
}

TEST(SSD1325_GFX, drawHLine_even_start)
{
    display->setColor(0x0F);
    display->drawHLine(10, 20, 20);
    capture();
    for ( int x = 10; x <= 20; x++ )
        CHECK_TRUE( px(x, 20) != 0 );
    CHECK_EQUAL( 0, px(9, 20) );
    CHECK_EQUAL( 0, px(21, 20) );
}

TEST(SSD1325_GFX, drawVLine)
{
    display->setColor(0x0F);
    display->drawVLine(15, 5, 25);
    capture();
    for ( int y = 5; y <= 25; y++ )
        CHECK_TRUE( px(15, y) != 0 );
    CHECK_EQUAL( 0, px(15, 4) );
    CHECK_EQUAL( 0, px(15, 26) );
}

TEST(SSD1325_GFX, drawLine_diagonal)
{
    display->setColor(0x0F);
    display->drawLine(0, 0, 31, 31);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(15, 15) != 0 );
    CHECK_TRUE( px(31, 31) != 0 );
    int count = gray4_count_set(pixels->data(), W, 0, 0, 31, 31);
    CHECK_EQUAL( 32, count );
}

TEST(SSD1325_GFX, drawLine_horizontal)
{
    display->setColor(0x0F);
    display->drawLine(5, 20, 50, 20);
    capture();
    for ( int x = 5; x <= 50; x++ )
        CHECK_TRUE( px(x, 20) != 0 );
}

TEST(SSD1325_GFX, setContrast)
{
    display->getInterface().setContrast(128);
    display->getInterface().setContrast(0);
    display->getInterface().setContrast(255);
}

TEST(SSD1325_GFX, boundary_pixel_corners)
{
    display->setColor(0x0F);
    display->putPixel(0, 0);
    display->putPixel(W - 1, 0);
    display->putPixel(0, H - 1);
    display->putPixel(W - 1, H - 1);
    capture();
    CHECK_TRUE( px(0, 0) != 0 );
    CHECK_TRUE( px(W - 1, 0) != 0 );
    CHECK_TRUE( px(0, H - 1) != 0 );
    CHECK_TRUE( px(W - 1, H - 1) != 0 );
}

TEST(SSD1325_GFX, drawHLine_single_pixel)
{
    display->setColor(0x0F);
    display->drawHLine(50, 30, 50);
    capture();
    CHECK_TRUE( px(50, 30) != 0 );
    CHECK_EQUAL( 0, px(49, 30) );
    CHECK_EQUAL( 0, px(51, 30) );
}

TEST(SSD1325_GFX, fillRect_produces_pixels)
{
    display->setColor(0x0F);
    display->fillRect(10, 10, 30, 30);
    capture();
    int count = gray4_count_set(pixels->data(), W, 10, 10, 30, 30);
    CHECK_TRUE( count > 0 );
}

// --- Regression tests for the 4bpp fillRect nibble-packing bug ---
// Before the fix, fillRect sent the colour byte verbatim, leaving every
// odd column blank, and miscounted bytes for partial-column rectangles,
// leaving the tail of the GDDRAM window unwritten.
//
// These tests verify the corrected behaviour for all 16 colours and for
// every combination of {even,odd} edges.

TEST(SSD1325_GFX, fillRect_aligned_full_coverage_all_colors)
{
    // x1 even (0), x2 odd (W-1): no partial column-pairs. Both nibbles
    // of every byte must equal the requested colour.
    for ( uint8_t c = 0; c <= 0x0F; c++ )
    {
        display->clear();
        display->setColor(c);
        display->fillRect(0, 0, W - 1, 7);
        capture();
        CHECK_TRUE( gray4_region_equals(pixels->data(), W, 0, 0, W - 1, 7, c) );
    }
}

TEST(SSD1325_GFX, fillRect_odd_columns_set_with_color_0x0F)
{
    // Direct regression for the original bug: setColor(0x0F) used to
    // leave every odd column at 0 because the colour was packed only
    // into the low nibble of each byte.
    display->setColor(0x0F);
    display->fillRect(10, 10, 30, 30);
    capture();
    for ( int y = 10; y <= 30; y++ )
    {
        for ( int x = 10; x <= 30; x++ )
        {
            CHECK_EQUAL( 0x0F, px(x, y) );
        }
    }
}

TEST(SSD1325_GFX, fillRect_left_edge_partial_pair)
{
    // x1 odd (3): the column-pair containing x1 spans columns 2..3.
    // Column 2 is outside the rect and must be left as 0; column 3
    // (and everything up to x2) must equal the colour.
    display->setColor(0x0F);
    display->fillRect(3, 5, 9, 9);
    capture();
    // Inside rect:
    for ( int y = 5; y <= 9; y++ )
    {
        for ( int x = 3; x <= 9; x++ )
            CHECK_EQUAL( 0x0F, px(x, y) );
        // Outside rect, in same column-pair:
        CHECK_EQUAL( 0, px(2, y) );
    }
}

TEST(SSD1325_GFX, fillRect_right_edge_partial_pair)
{
    // x2 even (8): column-pair contains 8..9. Column 9 is outside
    // and must be 0; columns x1..8 must equal the colour.
    display->setColor(0x0F);
    display->fillRect(2, 5, 8, 9);
    capture();
    for ( int y = 5; y <= 9; y++ )
    {
        for ( int x = 2; x <= 8; x++ )
            CHECK_EQUAL( 0x0F, px(x, y) );
        CHECK_EQUAL( 0, px(9, y) );
    }
}

TEST(SSD1325_GFX, fillRect_single_column_pair_partial_both_edges)
{
    // x1=3 (odd), x2=4 (even): single column-pair (cols 2..5 → pair
    // 1=2..3, pair 2=4..5). Cols 3 and 4 inside, cols 2 and 5 outside.
    display->setColor(0x0A);
    display->fillRect(3, 1, 4, 2);
    capture();
    for ( int y = 1; y <= 2; y++ )
    {
        CHECK_EQUAL( 0,    px(2, y) );  // outside (left)
        CHECK_EQUAL( 0x0A, px(3, y) );  // inside
        CHECK_EQUAL( 0x0A, px(4, y) );  // inside
        CHECK_EQUAL( 0,    px(5, y) );  // outside (right)
    }
}

TEST(SSD1325_GFX, fillRect_single_pixel_via_fillRect)
{
    // fillRect(x, y, x, y) — pair_start == pair_end, edge masks combine.
    display->setColor(0x0C);
    display->fillRect(7, 7, 7, 7);
    capture();
    CHECK_EQUAL( 0x0C, px(7, 7) );
    CHECK_EQUAL( 0,    px(6, 7) );
    CHECK_EQUAL( 0,    px(8, 7) );
}

TEST(SSD1325_GFX, fillRect_does_not_clobber_outside)
{
    // Verify that the corrected byte-count math no longer leaves a
    // tail of unwritten pixels and does not run off the end of the
    // GDDRAM window into the next row.
    display->setColor(0x0F);
    display->fillRect(0, 0, W - 1, H - 1);
    capture();
    // Every pixel of the panel must be 0x0F.
    CHECK_TRUE( gray4_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 0x0F) );
}

TEST(SSD1325_GFX, clear_after_draw)
{
    display->setColor(0x0F);
    display->drawHLine(0, 0, W - 1);
    display->clear();
    capture();
    CHECK_TRUE( gray4_region_equals(pixels->data(), W, 0, 0, W - 1, H - 1, 0) );
}
