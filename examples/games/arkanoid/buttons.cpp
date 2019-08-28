/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

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

#include "buttons.h"

#include "lcdgfx.h"

Key getPressedButton(uint8_t analogPin)
{
#ifdef USE_Z_KEYPAD
    int buttonValue = lcd_adcRead(analogPin);
    if (buttonValue < 100) return Key::BT_RIGHT;
    if (buttonValue < 200) return Key::BT_UP;
    if (buttonValue < 400) return Key::BT_DOWN;
    if (buttonValue < 600) return Key::BT_LEFT;
    if (buttonValue < 800) return Key::BT_SELECT;
#else
    if (lcd_gpioRead(RIGHT_BTN) != LCD_LOW) return Key::BT_RIGHT;
    if (lcd_gpioRead(LEFT_BTN) != LCD_LOW) return  Key::BT_LEFT;
#endif
    return Key::BT_NONE;
}
