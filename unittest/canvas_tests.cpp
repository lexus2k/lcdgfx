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
// NanoCanvas8 tests (8-bit color, simplest pixel layout)
// ============================================================

static const int CW = 32;
static const int CH = 32;
static uint8_t canvas_buf[CW * CH];

TEST_GROUP(Canvas8)
{
    NanoCanvasOps<8> canvas;

    void setup()
    {
        memset(canvas_buf, 0, sizeof(canvas_buf));
        canvas.begin(CW, CH, canvas_buf);
        canvas.setColor(0xFF);
    }

    void teardown() {}

    uint8_t px(int x, int y) { return canvas_buf[x + y * CW]; }
};

TEST(Canvas8, begin_sets_dimensions)
{
    CHECK_EQUAL(CW, canvas.width());
    CHECK_EQUAL(CH, canvas.height());
}

TEST(Canvas8, clear_zeros_buffer)
{
    memset(canvas_buf, 0xAA, sizeof(canvas_buf));
    canvas.clear();
    for (int i = 0; i < CW * CH; i++)
        CHECK_EQUAL(0, canvas_buf[i]);
}

TEST(Canvas8, putPixel_basic)
{
    canvas.putPixel(5, 10);
    CHECK_EQUAL(0xFF, px(5, 10));
    CHECK_EQUAL(0x00, px(4, 10));
    CHECK_EQUAL(0x00, px(5, 9));
}

TEST(Canvas8, putPixel_NanoPoint)
{
    NanoPoint p = {15, 20};
    canvas.putPixel(p);
    CHECK_EQUAL(0xFF, px(15, 20));
}

TEST(Canvas8, putPixel_out_of_bounds)
{
    canvas.putPixel(-1, 0);
    canvas.putPixel(0, -1);
    canvas.putPixel(CW, 0);
    canvas.putPixel(0, CH);
    for (int i = 0; i < CW * CH; i++)
        CHECK_EQUAL(0, canvas_buf[i]);
}

TEST(Canvas8, drawHLine_basic)
{
    canvas.drawHLine(5, 10, 20);
    for (int x = 5; x <= 20; x++)
        CHECK_EQUAL(0xFF, px(x, 10));
    CHECK_EQUAL(0x00, px(4, 10));
    CHECK_EQUAL(0x00, px(21, 10));
}

TEST(Canvas8, drawVLine_basic)
{
    canvas.drawVLine(10, 5, 20);
    for (int y = 5; y <= 20; y++)
        CHECK_EQUAL(0xFF, px(10, y));
    CHECK_EQUAL(0x00, px(10, 4));
    CHECK_EQUAL(0x00, px(10, 21));
}

TEST(Canvas8, drawLine_diagonal)
{
    canvas.drawLine(0, 0, 31, 31);
    CHECK_EQUAL(0xFF, px(0, 0));
    CHECK_EQUAL(0xFF, px(15, 15));
    CHECK_EQUAL(0xFF, px(31, 31));
    CHECK_EQUAL(0x00, px(0, 1));
    CHECK_EQUAL(0x00, px(1, 0));
}

TEST(Canvas8, fillRect_basic)
{
    canvas.fillRect(5, 5, 15, 15);
    for (int y = 5; y <= 15; y++)
        for (int x = 5; x <= 15; x++)
            CHECK_EQUAL(0xFF, px(x, y));
    CHECK_EQUAL(0x00, px(4, 5));
    CHECK_EQUAL(0x00, px(16, 5));
    CHECK_EQUAL(0x00, px(5, 4));
    CHECK_EQUAL(0x00, px(5, 16));
}

TEST(Canvas8, fillRect_NanoRect)
{
    NanoRect r = {{5, 5}, {15, 15}};
    canvas.fillRect(r);
    CHECK_EQUAL(0xFF, px(5, 5));
    CHECK_EQUAL(0xFF, px(15, 15));
    CHECK_EQUAL(0x00, px(4, 5));
}

TEST(Canvas8, drawRect_outline)
{
    canvas.drawRect(5, 5, 15, 15);
    for (int x = 5; x <= 15; x++)
    {
        CHECK_EQUAL(0xFF, px(x, 5));
        CHECK_EQUAL(0xFF, px(x, 15));
    }
    for (int y = 5; y <= 15; y++)
    {
        CHECK_EQUAL(0xFF, px(5, y));
        CHECK_EQUAL(0xFF, px(15, y));
    }
    CHECK_EQUAL(0x00, px(10, 10));
}

TEST(Canvas8, drawRect_NanoRect)
{
    NanoRect r = {{2, 2}, {10, 10}};
    canvas.drawRect(r);
    CHECK_EQUAL(0xFF, px(2, 2));
    CHECK_EQUAL(0xFF, px(10, 10));
    CHECK_EQUAL(0x00, px(6, 6));
}

TEST(Canvas8, drawCircle_basic)
{
    int cx = 16, cy = 16, r = 8;
    canvas.drawCircle(cx, cy, r);
    CHECK_EQUAL(0xFF, px(cx, cy - r));
    CHECK_EQUAL(0xFF, px(cx, cy + r));
    CHECK_EQUAL(0xFF, px(cx - r, cy));
    CHECK_EQUAL(0xFF, px(cx + r, cy));
    CHECK_EQUAL(0x00, px(cx, cy));
}

TEST(Canvas8, setColor_getColor)
{
    canvas.setColor(0x42);
    CHECK_EQUAL(0x42, canvas.getColor());
    canvas.putPixel(0, 0);
    CHECK_EQUAL(0x42, px(0, 0));
}

TEST(Canvas8, invertColors)
{
    canvas.setColor(0xAA);
    canvas.setBackground(0x55);
    canvas.invertColors();
    CHECK_EQUAL(0x55, canvas.getColor());
}

TEST(Canvas8, getData_returns_buffer)
{
    POINTERS_EQUAL(canvas_buf, canvas.getData());
}

TEST(Canvas8, rect_returns_bounds)
{
    NanoRect r = canvas.rect();
    CHECK_EQUAL(0, r.p1.x);
    CHECK_EQUAL(0, r.p1.y);
    CHECK_EQUAL(CW - 1, r.p2.x);
    CHECK_EQUAL(CH - 1, r.p2.y);
}

TEST(Canvas8, drawLine_NanoRect)
{
    NanoRect r = {{0, 0}, {31, 31}};
    canvas.drawLine(r);
    CHECK_EQUAL(0xFF, px(0, 0));
    CHECK_EQUAL(0xFF, px(31, 31));
}

TEST(Canvas8, multiple_colors)
{
    canvas.setColor(0xAA);
    canvas.putPixel(0, 0);
    canvas.setColor(0x55);
    canvas.putPixel(1, 0);
    CHECK_EQUAL(0xAA, px(0, 0));
    CHECK_EQUAL(0x55, px(1, 0));
}

TEST(Canvas8, setFont_and_printFixed)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.printFixed(0, 0, "A", STYLE_NORMAL);
    // Font 6x8: at least some pixels should be set
    int count = 0;
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 6; x++)
            if (px(x, y)) count++;
    CHECK(count > 0);
}

TEST(Canvas8, printFixed_empty_string)
{
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.printFixed(0, 0, "", STYLE_NORMAL);
    for (int i = 0; i < CW * CH; i++)
        CHECK_EQUAL(0, canvas_buf[i]);
}

TEST(Canvas8, drawLine_horizontal)
{
    canvas.drawLine(5, 10, 25, 10);
    for (int x = 5; x <= 25; x++)
        CHECK_EQUAL(0xFF, px(x, 10));
}

TEST(Canvas8, drawLine_vertical)
{
    canvas.drawLine(10, 5, 10, 25);
    for (int y = 5; y <= 25; y++)
        CHECK_EQUAL(0xFF, px(10, y));
}

// ============================================================
// NanoCanvas16 tests (16-bit color)
// ============================================================

static uint8_t canvas16_buf[CW * CH * 2];

TEST_GROUP(Canvas16)
{
    NanoCanvasOps<16> canvas;

    void setup()
    {
        memset(canvas16_buf, 0, sizeof(canvas16_buf));
        canvas.begin(CW, CH, canvas16_buf);
        canvas.setColor(0xFFFF);
    }

    void teardown() {}

    uint16_t px(int x, int y)
    {
        int idx = (x + y * CW) * 2;
        return (canvas16_buf[idx] << 8) | canvas16_buf[idx + 1];
    }
};

TEST(Canvas16, putPixel_basic)
{
    canvas.putPixel(10, 10);
    CHECK_EQUAL(0xFFFF, px(10, 10));
    CHECK_EQUAL(0x0000, px(9, 10));
}

TEST(Canvas16, drawHLine_basic)
{
    canvas.drawHLine(5, 10, 20);
    for (int x = 5; x <= 20; x++)
        CHECK_EQUAL(0xFFFF, px(x, 10));
    CHECK_EQUAL(0x0000, px(4, 10));
    CHECK_EQUAL(0x0000, px(21, 10));
}

TEST(Canvas16, fillRect_basic)
{
    canvas.fillRect(2, 2, 8, 8);
    for (int y = 2; y <= 8; y++)
        for (int x = 2; x <= 8; x++)
            CHECK_EQUAL(0xFFFF, px(x, y));
    CHECK_EQUAL(0x0000, px(1, 2));
    CHECK_EQUAL(0x0000, px(9, 2));
}

TEST(Canvas16, clear_zeros_buffer)
{
    memset(canvas16_buf, 0xFF, sizeof(canvas16_buf));
    canvas.clear();
    for (int i = 0; i < CW * CH * 2; i++)
        CHECK_EQUAL(0, canvas16_buf[i]);
}

TEST(Canvas16, drawVLine_basic)
{
    canvas.drawVLine(15, 3, 25);
    for (int y = 3; y <= 25; y++)
        CHECK_EQUAL(0xFFFF, px(15, y));
    CHECK_EQUAL(0x0000, px(15, 2));
    CHECK_EQUAL(0x0000, px(15, 26));
}

TEST(Canvas16, color_value)
{
    canvas.setColor(0x1234);
    canvas.putPixel(0, 0);
    CHECK_EQUAL(0x1234, px(0, 0));
}

// ============================================================
// NanoCanvas1 tests (1-bit monochrome, page-byte layout)
// ============================================================

static const int C1W = 32;
static const int C1H = 32;
static uint8_t canvas1_buf[C1W * (C1H / 8)];

TEST_GROUP(Canvas1)
{
    NanoCanvasOps<1> canvas;

    void setup()
    {
        memset(canvas1_buf, 0, sizeof(canvas1_buf));
        canvas.begin(C1W, C1H, canvas1_buf);
        canvas.setColor(0xFF);
    }

    void teardown() {}

    // 1-bit canvas uses page format: byte at [x + (y/8)*w], bit (y%8)
    uint8_t px(int x, int y)
    {
        int page = y / 8;
        int bit = y & 7;
        return (canvas1_buf[x + page * C1W] >> bit) & 1;
    }
};

TEST(Canvas1, begin_dimensions)
{
    CHECK_EQUAL(C1W, canvas.width());
    CHECK_EQUAL(C1H, canvas.height());
}

TEST(Canvas1, putPixel_basic)
{
    canvas.putPixel(5, 10);
    CHECK_EQUAL(1, px(5, 10));
    CHECK_EQUAL(0, px(4, 10));
    CHECK_EQUAL(0, px(5, 9));
}

TEST(Canvas1, putPixel_preserves_neighbors)
{
    canvas.putPixel(5, 1);
    canvas.putPixel(5, 3);
    CHECK_EQUAL(1, px(5, 1));
    CHECK_EQUAL(1, px(5, 3));
    CHECK_EQUAL(0, px(5, 2));
}

TEST(Canvas1, drawHLine_basic)
{
    canvas.drawHLine(5, 10, 20);
    for (int x = 5; x <= 20; x++)
        CHECK_EQUAL(1, px(x, 10));
    CHECK_EQUAL(0, px(4, 10));
    CHECK_EQUAL(0, px(21, 10));
}

TEST(Canvas1, drawVLine_basic)
{
    canvas.drawVLine(10, 2, 20);
    for (int y = 2; y <= 20; y++)
        CHECK_EQUAL(1, px(10, y));
    CHECK_EQUAL(0, px(10, 1));
    CHECK_EQUAL(0, px(10, 21));
}

TEST(Canvas1, fillRect_page_aligned)
{
    canvas.fillRect(5, 0, 15, 7);
    for (int y = 0; y <= 7; y++)
        for (int x = 5; x <= 15; x++)
            CHECK_EQUAL(1, px(x, y));
    CHECK_EQUAL(0, px(4, 0));
    CHECK_EQUAL(0, px(16, 0));
}

TEST(Canvas1, fillRect_cross_page)
{
    canvas.fillRect(5, 5, 15, 12);
    for (int y = 5; y <= 12; y++)
        for (int x = 5; x <= 15; x++)
            CHECK_EQUAL(1, px(x, y));
}

TEST(Canvas1, clear)
{
    canvas.fillRect(0, 0, C1W - 1, C1H - 1);
    canvas.clear();
    for (int i = 0; i < C1W * (C1H / 8); i++)
        CHECK_EQUAL(0, canvas1_buf[i]);
}

TEST(Canvas1, drawRect_outline)
{
    canvas.drawRect(2, 2, 10, 10);
    CHECK_EQUAL(1, px(2, 2));
    CHECK_EQUAL(1, px(10, 10));
    CHECK_EQUAL(1, px(6, 2));
    CHECK_EQUAL(1, px(2, 6));
    CHECK_EQUAL(0, px(6, 6));
}

// ============================================================
// NanoCanvas4 tests (4-bit grayscale)
// ============================================================

static const int C4W = 32;
static const int C4H = 32;
static uint8_t canvas4_buf[C4W * C4H / 2];

TEST_GROUP(Canvas4)
{
    NanoCanvasOps<4> canvas;

    void setup()
    {
        memset(canvas4_buf, 0, sizeof(canvas4_buf));
        canvas.begin(C4W, C4H, canvas4_buf);
        canvas.setColor(0x0F);
    }

    void teardown() {}

    uint8_t px(int x, int y)
    {
        int idx = (x + y * C4W) / 2;
        if ((x + y * C4W) & 1)
            return (canvas4_buf[idx] >> 4) & 0x0F;
        else
            return canvas4_buf[idx] & 0x0F;
    }
};

TEST(Canvas4, putPixel_basic)
{
    canvas.putPixel(5, 10);
    CHECK_EQUAL(0x0F, px(5, 10));
    CHECK_EQUAL(0x00, px(4, 10));
}

TEST(Canvas4, drawHLine_basic)
{
    canvas.drawHLine(5, 10, 20);
    for (int x = 5; x <= 20; x++)
        CHECK_EQUAL(0x0F, px(x, 10));
    CHECK_EQUAL(0x00, px(4, 10));
    CHECK_EQUAL(0x00, px(21, 10));
}

TEST(Canvas4, drawVLine_basic)
{
    canvas.drawVLine(10, 5, 20);
    for (int y = 5; y <= 20; y++)
        CHECK_EQUAL(0x0F, px(10, y));
    CHECK_EQUAL(0x00, px(10, 4));
    CHECK_EQUAL(0x00, px(10, 21));
}

TEST(Canvas4, fillRect_basic)
{
    canvas.fillRect(2, 2, 10, 10);
    for (int y = 2; y <= 10; y++)
        for (int x = 2; x <= 10; x++)
            CHECK_EQUAL(0x0F, px(x, y));
    CHECK_EQUAL(0x00, px(1, 2));
    CHECK_EQUAL(0x00, px(11, 2));
}

TEST(Canvas4, clear)
{
    memset(canvas4_buf, 0xFF, sizeof(canvas4_buf));
    canvas.clear();
    for (int i = 0; i < C4W * C4H / 2; i++)
        CHECK_EQUAL(0, canvas4_buf[i]);
}
