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

//
// Verifies that drawLine renders exactly the intended Bresenham pixel set
// (correctness / no visual regression from bus-transaction batching), and that
// the batched implementation issues far fewer bus transactions than a naive
// per-pixel path for shallow / steep lines.
//

#include <CppUTest/TestHarness.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
#include <utility>
#include "lcdgfx.h"
#include "sdl_core.h"
#include "utils/pixel_helpers.h"

// Reference rasterization: an independent copy of the library's integer
// Bresenham traversal. It defines the exact set of pixels the line is expected
// to cover, regardless of how drawLine batches its bus writes.
static void reference_line(std::set<std::pair<int, int>> &pset, int x1, int y1, int x2, int y2)
{
    int dx = x1 > x2 ? (x1 - x2) : (x2 - x1);
    int dy = y1 > y2 ? (y1 - y2) : (y2 - y1);
    int err = 0;
    if ( dy > dx )
    {
        if ( y1 > y2 )
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        for ( ; y1 <= y2; y1++ )
        {
            pset.insert(std::make_pair(x1, y1));
            err += dx;
            if ( err >= dy )
            {
                err -= dy;
                x1 < x2 ? x1++ : x1--;
            }
        }
    }
    else
    {
        if ( x1 > x2 )
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        for ( ; x1 <= x2; x1++ )
        {
            pset.insert(std::make_pair(x1, y1));
            err += dy;
            if ( err >= dx )
            {
                err -= dx;
                y1 < y2 ? y1++ : y1--;
            }
        }
    }
}

// ==================== 1-bit / page-addressed (SSD1306) ====================

TEST_GROUP(LineBatching_Mono)
{
    DisplaySSD1306_128x64_I2C *display;
    std::vector<uint8_t> *pixels;
    static const int W = 128;
    static const int H = 64;

    void setup()
    {
        display = new DisplaySSD1306_128x64_I2C(-1);
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>( sdl_core_get_pixels_len(1), 0 );
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
        sdl_core_get_pixels_data( pixels->data(), 1 );
    }

    int px(int x, int y) { return get_mono_pixel(pixels->data(), W, x, y); }

    // Draw one line and assert the lit pixels are exactly the reference set.
    void check_line(int x1, int y1, int x2, int y2)
    {
        display->clear();
        display->setColor(0xFFFF);
        display->drawLine(x1, y1, x2, y2);
        capture();

        std::set<std::pair<int, int>> expected;
        reference_line(expected, x1, y1, x2, y2);

        int missing = 0, extra = 0;
        for ( auto &p : expected )
            if ( !px(p.first, p.second) ) missing++;
        for ( int y = 0; y < H; y++ )
            for ( int x = 0; x < W; x++ )
                if ( px(x, y) && expected.find(std::make_pair(x, y)) == expected.end() ) extra++;

        CHECK_EQUAL(0, missing);
        CHECK_EQUAL(0, extra);
    }
};

TEST(LineBatching_Mono, shallow_line_exact_pixels)
{
    check_line(2, 4, 120, 30);   // dx >> dy
}

TEST(LineBatching_Mono, steep_line_exact_pixels)
{
    check_line(10, 2, 40, 60);   // dy >> dx (crosses many pages in one column run)
}

TEST(LineBatching_Mono, diagonal_45_exact_pixels)
{
    check_line(0, 0, 60, 60);
}

TEST(LineBatching_Mono, horizontal_line_exact_pixels)
{
    check_line(5, 20, 100, 20);
}

TEST(LineBatching_Mono, vertical_line_exact_pixels)
{
    check_line(64, 5, 64, 60);
}

TEST(LineBatching_Mono, reversed_endpoints_match)
{
    check_line(120, 30, 2, 4);   // same as shallow but reversed
}

// ==================== 8-bit color (SSD1331) ====================

TEST_GROUP(LineBatching_Color8)
{
    DisplaySSD1331_96x64x8_SPI *display;
    std::vector<uint8_t> *pixels;
    static const int W = 96;
    static const int H = 64;

    void setup()
    {
        display = new DisplaySSD1331_96x64x8_SPI(-1, {-1, 0, 1, 0, -1, -1});
        display->begin();
        display->clear();
        pixels = new std::vector<uint8_t>( sdl_core_get_pixels_len(8), 0 );
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

    int lit(int x, int y) { return get_rgb8_pixel(pixels->data(), W, x, y) != 0; }

    void check_line(int x1, int y1, int x2, int y2)
    {
        display->clear();
        display->setColor(0xFFFF);
        display->drawLine(x1, y1, x2, y2);
        capture();

        std::set<std::pair<int, int>> expected;
        reference_line(expected, x1, y1, x2, y2);

        int missing = 0;
        for ( auto &p : expected )
            if ( !lit(p.first, p.second) ) missing++;
        CHECK_EQUAL(0, missing);
    }
};

TEST(LineBatching_Color8, shallow_line_exact_pixels)
{
    check_line(2, 4, 90, 30);
}

TEST(LineBatching_Color8, steep_line_exact_pixels)
{
    check_line(10, 2, 40, 60);
}

TEST(LineBatching_Color8, diagonal_45_exact_pixels)
{
    check_line(0, 0, 60, 60);
}

// ==================== Bus-traffic reduction (the actual win) ====================
//
// Coalescing same-page runs collapses the per-pixel set-window command bursts
// into one burst per run. We measure the total bytes pushed to the controller
// and assert the batched line costs far less than one transaction per pixel.

TEST_GROUP(LineBatching_Traffic)
{
    DisplaySSD1306_128x64_I2C *display;

    void setup()
    {
        display = new DisplaySSD1306_128x64_I2C(-1);
        display->begin();
        display->clear();
    }

    void teardown()
    {
        display->end();
        delete display;
    }

    unsigned long line_bytes(int x1, int y1, int x2, int y2)
    {
        sdl_core_reset_tx_count();
        display->drawLine(x1, y1, x2, y2);
        return sdl_core_get_tx_count();
    }

    // Cost of touching the same pixels one-at-a-time (the pre-batching path):
    // each putPixel emits its own set-window command burst.
    unsigned long per_pixel_bytes(int x1, int y1, int x2, int y2)
    {
        std::set<std::pair<int, int>> pts;
        reference_line(pts, x1, y1, x2, y2);
        sdl_core_reset_tx_count();
        for ( auto &p : pts )
            display->putPixel(p.first, p.second);
        return sdl_core_get_tx_count();
    }
};

TEST(LineBatching_Traffic, shallow_line_far_cheaper_than_per_pixel)
{
    // Near-horizontal line: many columns share a row, so runs are long.
    unsigned long batched = line_bytes(2, 10, 120, 14);
    unsigned long naive = per_pixel_bytes(2, 10, 120, 14);
    // Coalescing must cut bus traffic by well over half.
    CHECK_TRUE(batched * 3 < naive);
}

TEST(LineBatching_Traffic, vertical_line_far_cheaper_than_per_pixel)
{
    // Pure vertical line collapses to a single drawVLine (one window burst).
    unsigned long batched = line_bytes(64, 2, 64, 60);
    unsigned long naive = per_pixel_bytes(64, 2, 64, 60);
    CHECK_TRUE(batched * 3 < naive);
}
