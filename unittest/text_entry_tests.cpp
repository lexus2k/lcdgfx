/*
    MIT License

    Copyright (c) 2025, Alexey Dynda

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
#include "lcdgfx_gui.h"

TEST_GROUP(TextEntryTests)
{
};

TEST(TextEntryTests, BufferIsInitialisedToBlank)
{
    char buf[6];
    LcdGfxTextEntry entry(buf, 5, NanoRect{});
    STRCMP_EQUAL("     ", entry.getText());
    CHECK_EQUAL(0, entry.getCursor());
    CHECK_EQUAL(5, entry.getMaxLen());
}

TEST(TextEntryTests, BufferIsNullTerminatedAtMaxLen)
{
    char buf[5];
    memset(buf, 'X', sizeof(buf));
    LcdGfxTextEntry entry(buf, 4, NanoRect{});
    CHECK_EQUAL('\0', buf[4]);
}

TEST(TextEntryTests, UpCyclesCharsetForward)
{
    char buf[3];
    LcdGfxTextEntry entry(buf, 2, NanoRect{}, "AB");
    STRCMP_EQUAL("AA", entry.getText());
    entry.up();
    STRCMP_EQUAL("BA", entry.getText());
    entry.up();
    STRCMP_EQUAL("AA", entry.getText());
}

TEST(TextEntryTests, DownCyclesCharsetBackward)
{
    char buf[3];
    LcdGfxTextEntry entry(buf, 2, NanoRect{}, "ABC");
    entry.down();
    STRCMP_EQUAL("CA", entry.getText());
    entry.down();
    STRCMP_EQUAL("BA", entry.getText());
}

TEST(TextEntryTests, LeftRightMoveCursorWithClamping)
{
    char buf[4];
    LcdGfxTextEntry entry(buf, 3, NanoRect{}, "AB");
    CHECK_EQUAL(0, entry.getCursor());
    entry.left();
    CHECK_EQUAL(0, entry.getCursor()); // clamped at 0
    entry.right();
    CHECK_EQUAL(1, entry.getCursor());
    entry.right();
    CHECK_EQUAL(2, entry.getCursor());
    entry.right();
    CHECK_EQUAL(2, entry.getCursor()); // clamped at maxLen-1
    entry.left();
    CHECK_EQUAL(1, entry.getCursor());
}

TEST(TextEntryTests, EditAtMovedCursorOnlyChangesThatCell)
{
    char buf[4];
    LcdGfxTextEntry entry(buf, 3, NanoRect{}, "AB");
    entry.right();
    entry.up();
    STRCMP_EQUAL("ABA", entry.getText());
    entry.right();
    entry.up();
    STRCMP_EQUAL("ABB", entry.getText());
    entry.left();
    entry.left();
    entry.up();
    STRCMP_EQUAL("BBB", entry.getText());
}

TEST(TextEntryTests, SetCursorClampsToMaxLen)
{
    char buf[4];
    LcdGfxTextEntry entry(buf, 3, NanoRect{}, "AB");
    entry.setCursor(99);
    CHECK_EQUAL(2, entry.getCursor());
    entry.setCursor(0);
    CHECK_EQUAL(0, entry.getCursor());
}

TEST(TextEntryTests, EmptyCharsetFallsBackToSafeDefault)
{
    char buf[3];
    LcdGfxTextEntry entry(buf, 2, NanoRect{}, "");
    // Should not crash, should not deadlock, should produce some character.
    entry.up();
    entry.down();
    entry.up();
    CHECK(buf[0] != '\0');
}

TEST(TextEntryTests, ZeroMaxLenIsCoercedToOne)
{
    char buf[2];
    LcdGfxTextEntry entry(buf, 0, NanoRect{}, "AB");
    CHECK_EQUAL(1, entry.getMaxLen());
    CHECK_EQUAL('\0', buf[1]);
    entry.up();
    STRCMP_EQUAL("B", entry.getText());
}

TEST(TextEntryTests, ShowDoesNotCrashOnDisplay)
{
    DisplaySSD1306_128x64_I2C display(-1);
    display.begin();
    display.fill(0x00);
    display.setFixedFont(ssd1306xled_font6x8);

    char buf[6];
    LcdGfxTextEntry entry(buf, 5, NanoRect{8, 8, 0, 0});
    entry.up();
    entry.right();
    entry.up();
    entry.show(display);
    // No assertion: just verify show() runs against a real display
    // without crashing and that the auto-sized rect is non-empty.
    CHECK(entry.getRect().p2.x > entry.getRect().p1.x);
    CHECK(entry.getRect().p2.y > entry.getRect().p1.y);
    display.end();
}
