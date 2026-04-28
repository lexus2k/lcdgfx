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
 *   This sketch demonstrates the LcdGfxSpinbox widget. Up/Down change the
 *   value by the configured step; the widget wraps around when reaching
 *   the bounds because wrap=true was passed to the constructor.
 */

#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "buttons.h"

#ifndef A0
#define A0  0
#endif
#define BUTTON_PIN     A0

DisplaySSD1306_128x64_I2C display(-1);

/* Range 0..23 (e.g. an hour selector), step 1, wrap-around enabled. */
LcdGfxSpinbox spinbox( {{16, 24}, {112, 44}}, 0, 23, 12, 1, true );

static Key btn;

void setup()
{
    display.begin();
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    spinbox.show(display);
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
            spinbox.up();
            display.clear();
            spinbox.show(display);
            break;
        case Key::BT_DOWN:
        case Key::BT_LEFT:
            spinbox.down();
            display.clear();
            spinbox.show(display);
            break;
        case Key::BT_SELECT:
            /* Confirm the value: spinbox.value() */
            break;
        default:
            break;
    }
}
