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
#include "sdl_core.h"

// ============================================================
// Test group for LcdGfxMenu with scroll indicators
// ============================================================
TEST_GROUP(MenuTests)
{
    DisplaySSD1306_128x64_I2C *display;

    const char *menuItems[8] = {
        "Item 1", "Item 2", "Item 3", "Item 4",
        "Item 5", "Item 6", "Item 7", "Item 8",
    };

    void setup()
    {
        display = new DisplaySSD1306_128x64_I2C(-1);
        display->begin();
        display->setFixedFont(ssd1306xled_font6x8);
        display->clear();
    }

    void teardown()
    {
        display->end();
        delete display;
    }
};

TEST(MenuTests, MenuCreation)
{
    LcdGfxMenu menu(menuItems, 8);
    CHECK_EQUAL(8, menu.size());
    CHECK_EQUAL(0, menu.selection());
}

TEST(MenuTests, MenuNavigation)
{
    LcdGfxMenu menu(menuItems, 8);
    menu.down();
    CHECK_EQUAL(1, menu.selection());
    menu.down();
    CHECK_EQUAL(2, menu.selection());
    menu.up();
    CHECK_EQUAL(1, menu.selection());
    menu.up();
    CHECK_EQUAL(0, menu.selection());
    // Wraps to last
    menu.up();
    CHECK_EQUAL(7, menu.selection());
    // Wraps to first
    menu.down();
    CHECK_EQUAL(0, menu.selection());
}

TEST(MenuTests, MenuSetSelection)
{
    LcdGfxMenu menu(menuItems, 8);
    menu.setSelection(5);
    CHECK_EQUAL(5, menu.selection());
    // Clamps to last item
    menu.setSelection(100);
    CHECK_EQUAL(7, menu.selection());
}

TEST(MenuTests, MenuShowNoOverflow)
{
    // Only 3 items — fits on screen, no scroll indicators needed
    LcdGfxMenu menu(menuItems, 3);
    menu.show(*display);
    CHECK_EQUAL(0, menu.selection());
}

TEST(MenuTests, MenuShowWithScroll)
{
    // 8 items with 8-pixel font on 64-pixel display = needs scrolling
    LcdGfxMenu menu(menuItems, 8);
    menu.show(*display);
    // Scroll down to trigger indicators
    for (int i = 0; i < 7; i++)
    {
        menu.down();
        menu.show(*display);
    }
    CHECK_EQUAL(7, menu.selection());
}

TEST(MenuTests, MenuShowWithRect)
{
    LcdGfxMenu menu(menuItems, 8, (NanoRect){{8, 16}, {0, 0}});
    menu.show(*display);
    menu.down();
    menu.show(*display);
    CHECK_EQUAL(1, menu.selection());
}

// ============================================================
// Test group for LcdGfxCheckboxMenu
// ============================================================
TEST_GROUP(CheckboxMenuTests)
{
    DisplaySSD1306_128x64_I2C *display;

    const char *items[6] = {
        "WiFi", "Bluetooth", "LED",
        "Sound", "Vibrate", "GPS",
    };

    void setup()
    {
        display = new DisplaySSD1306_128x64_I2C(-1);
        display->begin();
        display->setFixedFont(ssd1306xled_font6x8);
        display->clear();
    }

    void teardown()
    {
        display->end();
        delete display;
    }
};

TEST(CheckboxMenuTests, Creation)
{
    LcdGfxCheckboxMenu menu(items, 6);
    CHECK_EQUAL(6, menu.size());
    CHECK_EQUAL(0, menu.selection());
    CHECK_EQUAL(0, menu.checkedMask());
}

TEST(CheckboxMenuTests, Navigation)
{
    LcdGfxCheckboxMenu menu(items, 6);
    menu.down();
    CHECK_EQUAL(1, menu.selection());
    menu.up();
    CHECK_EQUAL(0, menu.selection());
    // Wrap
    menu.up();
    CHECK_EQUAL(5, menu.selection());
    menu.down();
    CHECK_EQUAL(0, menu.selection());
}

TEST(CheckboxMenuTests, Toggle)
{
    LcdGfxCheckboxMenu menu(items, 6);
    CHECK_FALSE(menu.isChecked(0));

    menu.toggle();
    CHECK_TRUE(menu.isChecked(0));
    CHECK_EQUAL(0x01, menu.checkedMask());

    menu.toggle();
    CHECK_FALSE(menu.isChecked(0));
    CHECK_EQUAL(0x00, menu.checkedMask());
}

TEST(CheckboxMenuTests, SetChecked)
{
    LcdGfxCheckboxMenu menu(items, 6);
    menu.setChecked(0, true);
    menu.setChecked(2, true);
    menu.setChecked(5, true);
    CHECK_TRUE(menu.isChecked(0));
    CHECK_FALSE(menu.isChecked(1));
    CHECK_TRUE(menu.isChecked(2));
    CHECK_TRUE(menu.isChecked(5));
    CHECK_EQUAL(0x25, menu.checkedMask());  // bits 0, 2, 5

    menu.setChecked(2, false);
    CHECK_FALSE(menu.isChecked(2));
    CHECK_EQUAL(0x21, menu.checkedMask());
}

TEST(CheckboxMenuTests, ToggleMultipleItems)
{
    LcdGfxCheckboxMenu menu(items, 6);
    // Check items 0, 1, 2
    menu.toggle();            // item 0 checked
    menu.down();
    menu.toggle();            // item 1 checked
    menu.down();
    menu.toggle();            // item 2 checked
    CHECK_EQUAL(0x07, menu.checkedMask());
    CHECK_EQUAL(2, menu.selection());
}

TEST(CheckboxMenuTests, OutOfBoundsChecked)
{
    LcdGfxCheckboxMenu menu(items, 6);
    CHECK_FALSE(menu.isChecked(10));
    menu.setChecked(10, true);  // should be ignored
    CHECK_EQUAL(0, menu.checkedMask());
}

TEST(CheckboxMenuTests, ShowAndScroll)
{
    LcdGfxCheckboxMenu menu(items, 6);
    menu.setChecked(0, true);
    menu.setChecked(3, true);
    menu.show(*display);

    // Scroll through all items
    for (int i = 0; i < 5; i++)
    {
        menu.down();
        menu.show(*display);
    }
    CHECK_EQUAL(5, menu.selection());
}

TEST(CheckboxMenuTests, ShowWithRect)
{
    LcdGfxCheckboxMenu menu(items, 6, (NanoRect){{8, 8}, {0, 0}});
    menu.setChecked(1, true);
    menu.show(*display);
    CHECK_EQUAL(0, menu.selection());
}

TEST(CheckboxMenuTests, SetSelection)
{
    LcdGfxCheckboxMenu menu(items, 6);
    menu.setSelection(4);
    CHECK_EQUAL(4, menu.selection());
    menu.setSelection(100);
    CHECK_EQUAL(5, menu.selection());
}

TEST(CheckboxMenuTests, MaxItems16)
{
    const char *manyItems[16] = {
        "A", "B", "C", "D", "E", "F", "G", "H",
        "I", "J", "K", "L", "M", "N", "O", "P",
    };
    LcdGfxCheckboxMenu menu(manyItems, 16);
    CHECK_EQUAL(16, menu.size());
    menu.setChecked(15, true);
    CHECK_TRUE(menu.isChecked(15));
    CHECK_EQUAL(0x8000, menu.checkedMask());
}
