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

TEST_GROUP(SliderTests)
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

TEST(SliderTests, ConstructionStoresValueAndBounds)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 0, 100, 25);
    CHECK_EQUAL(0, s.minValue());
    CHECK_EQUAL(100, s.maxValue());
    CHECK_EQUAL(25, s.value());
}

TEST(SliderTests, ConstructionClampsInitialValue)
{
    LcdGfxSlider hi({{0, 0}, {64, 8}}, 0, 50, 200);
    CHECK_EQUAL(50, hi.value());
    LcdGfxSlider lo({{0, 0}, {64, 8}}, 10, 50, -5);
    CHECK_EQUAL(10, lo.value());
}

TEST(SliderTests, ConstructionSwapsInvertedBounds)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 50, 0, 25);
    CHECK_EQUAL(0, s.minValue());
    CHECK_EQUAL(50, s.maxValue());
    CHECK_EQUAL(25, s.value());
}

TEST(SliderTests, UpIncrementsByStep)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 0, 100, 0, 5);
    s.up();
    CHECK_EQUAL(5, s.value());
    s.up();
    CHECK_EQUAL(10, s.value());
}

TEST(SliderTests, DownDecrementsByStep)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 0, 100, 50, 7);
    s.down();
    CHECK_EQUAL(43, s.value());
    s.down();
    CHECK_EQUAL(36, s.value());
}

TEST(SliderTests, UpClampsAtMax)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 0, 10, 8, 5);
    s.up();
    CHECK_EQUAL(10, s.value());
    s.up();
    CHECK_EQUAL(10, s.value());
}

TEST(SliderTests, DownClampsAtMin)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 0, 10, 2, 5);
    s.down();
    CHECK_EQUAL(0, s.value());
    s.down();
    CHECK_EQUAL(0, s.value());
}

TEST(SliderTests, ZeroOrNegativeStepDefaultsToOne)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 0, 10, 5, 0);
    s.up();
    CHECK_EQUAL(6, s.value());
    LcdGfxSlider s2({{0, 0}, {64, 8}}, 0, 10, 5, -3);
    s2.up();
    CHECK_EQUAL(6, s2.value());
}

TEST(SliderTests, SetValueClamps)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 0, 100, 50);
    s.setValue(150);
    CHECK_EQUAL(100, s.value());
    s.setValue(-50);
    CHECK_EQUAL(0, s.value());
    s.setValue(75);
    CHECK_EQUAL(75, s.value());
}

TEST(SliderTests, SignedRange)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, -50, 50, -50, 25);
    s.up();
    CHECK_EQUAL(-25, s.value());
    s.up();
    CHECK_EQUAL(0, s.value());
    s.up();
    CHECK_EQUAL(25, s.value());
}

TEST(SliderTests, ShowHorizontalRendersWithoutCrash)
{
    LcdGfxSlider s({{8, 16}, {120, 32}}, 0, 100, 50);
    s.show(*display);
    CHECK_EQUAL(50, s.value());
}

TEST(SliderTests, ShowVerticalRendersWithoutCrash)
{
    LcdGfxSlider s({{8, 8}, {24, 56}}, 0, 100, 25, 1, LcdGfxSliderOrientation::Vertical);
    s.show(*display);
    CHECK_EQUAL(25, s.value());
}

TEST(SliderTests, ShowHandlesDegenerateRange)
{
    LcdGfxSlider s({{0, 0}, {64, 8}}, 5, 5, 5);
    s.show(*display);
    CHECK_EQUAL(5, s.value());
}

TEST(SliderTests, GetRectReturnsConfiguredArea)
{
    NanoRect r{{8, 16}, {120, 32}};
    LcdGfxSlider s(r, 0, 100, 50);
    CHECK_EQUAL(8, s.getRect().p1.x);
    CHECK_EQUAL(16, s.getRect().p1.y);
    CHECK_EQUAL(120, s.getRect().p2.x);
    CHECK_EQUAL(32, s.getRect().p2.y);
}
