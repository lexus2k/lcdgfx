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
/*
 *   Atmega328 PINS: connect LCD to A4/A5,
 *   Z-keypad ADC module on A0 pin.
 *
 *   This sketch demonstrates the LcdGfxSlider widget. LEFT/DOWN decreases
 *   and RIGHT/UP increases the slider value. The current value is printed
 *   above the slider track.
 */

#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "buttons.h"
#include <stdio.h>

#ifndef A0
#define A0  0
#endif
#define BUTTON_PIN     A0

DisplaySSD1306_128x64_I2C display(-1);

/* Range 0..100, initial 50, step 5, horizontal track 8x40 wide. */
LcdGfxSlider slider( {{8, 32}, {120, 48}}, 0, 100, 50, 5,
                     LcdGfxSliderOrientation::Horizontal );

static Key btn;

static void redraw()
{
    display.clear();
    char buf[16];
    snprintf(buf, sizeof(buf), "Value: %d", (int)slider.value());
    display.printFixed(8, 8, buf);
    slider.show(display);
}

void setup()
{
    display.begin();
    display.setFixedFont(ssd1306xled_font6x8);
    redraw();
    btn = getPressedButton(BUTTON_PIN);
}

void loop()
{
    Key newButton = getPressedButton(BUTTON_PIN);
    if (newButton == btn)
    {
        return;
    }
    btn = newButton;
    switch (btn)
    {
        case Key::BT_UP:
        case Key::BT_RIGHT:
            slider.up();
            redraw();
            break;
        case Key::BT_DOWN:
        case Key::BT_LEFT:
            slider.down();
            redraw();
            break;
        case Key::BT_SELECT:
            /* Confirm the value: slider.value() */
            break;
        default:
            break;
    }
}
