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
*/

#include <CppUTest/TestHarness.h>
#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "sdl_core.h"

TEST_GROUP(SpinboxTests)
{
    DisplaySSD1306_128x64_I2C *display;

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

TEST(SpinboxTests, ConstructionStoresValueAndBounds)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 23, 12);
    CHECK_EQUAL(0, s.minValue());
    CHECK_EQUAL(23, s.maxValue());
    CHECK_EQUAL(12, s.value());
    CHECK_FALSE(s.wraps());
}

TEST(SpinboxTests, ConstructionClampsInitialValue)
{
    LcdGfxSpinbox hi({{0, 0}, {64, 16}}, 0, 10, 50);
    CHECK_EQUAL(10, hi.value());
    LcdGfxSpinbox lo({{0, 0}, {64, 16}}, 5, 10, 0);
    CHECK_EQUAL(5, lo.value());
}

TEST(SpinboxTests, ConstructionSwapsInvertedBounds)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 23, 0, 12);
    CHECK_EQUAL(0, s.minValue());
    CHECK_EQUAL(23, s.maxValue());
}

TEST(SpinboxTests, UpDownInRange)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 100, 50, 5);
    s.up();
    CHECK_EQUAL(55, s.value());
    s.down();
    s.down();
    CHECK_EQUAL(45, s.value());
}

TEST(SpinboxTests, UpClampsAtMaxWhenNotWrapping)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 10, 8, 5, false);
    s.up();
    CHECK_EQUAL(10, s.value());
    s.up();
    CHECK_EQUAL(10, s.value());
}

TEST(SpinboxTests, DownClampsAtMinWhenNotWrapping)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 10, 2, 5, false);
    s.down();
    CHECK_EQUAL(0, s.value());
    s.down();
    CHECK_EQUAL(0, s.value());
}

TEST(SpinboxTests, UpWrapsAroundMaxWhenWrapping)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 23, 22, 1, true);
    s.up();
    CHECK_EQUAL(23, s.value());
    s.up();
    CHECK_EQUAL(0, s.value());
    s.up();
    CHECK_EQUAL(1, s.value());
}

TEST(SpinboxTests, DownWrapsAroundMinWhenWrapping)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 23, 1, 1, true);
    s.down();
    CHECK_EQUAL(0, s.value());
    s.down();
    CHECK_EQUAL(23, s.value());
    s.down();
    CHECK_EQUAL(22, s.value());
}

TEST(SpinboxTests, WrapHandlesLargeStep)
{
    /* Range 0..9, step 25, wrap enabled. Span = 10. From 5, +25 = 30 -> 0. */
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 9, 5, 25, true);
    s.up();
    CHECK_EQUAL(0, s.value());
    s.down();
    CHECK_EQUAL(5, s.value());
}

TEST(SpinboxTests, ZeroOrNegativeStepDefaultsToOne)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 10, 5, 0);
    s.up();
    CHECK_EQUAL(6, s.value());
    LcdGfxSpinbox s2({{0, 0}, {64, 16}}, 0, 10, 5, -3);
    s2.up();
    CHECK_EQUAL(6, s2.value());
}

TEST(SpinboxTests, SetValueClamps)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, 0, 100, 50);
    s.setValue(150);
    CHECK_EQUAL(100, s.value());
    s.setValue(-10);
    CHECK_EQUAL(0, s.value());
    s.setValue(75);
    CHECK_EQUAL(75, s.value());
}

TEST(SpinboxTests, SignedRangeWithWrap)
{
    LcdGfxSpinbox s({{0, 0}, {64, 16}}, -2, 2, 2, 1, true);
    s.up();
    CHECK_EQUAL(-2, s.value());
    s.down();
    CHECK_EQUAL(2, s.value());
}

TEST(SpinboxTests, ShowRendersWithoutCrash)
{
    LcdGfxSpinbox s({{16, 24}, {112, 44}}, 0, 23, 12);
    s.show(*display);
    CHECK_EQUAL(12, s.value());
}

TEST(SpinboxTests, ShowHandlesNegativeValue)
{
    LcdGfxSpinbox s({{16, 24}, {112, 44}}, -100, 100, -42);
    s.show(*display);
    CHECK_EQUAL(-42, s.value());
}
