/*
    MIT License

    Copyright (c) 2016-2020, Alexey Dynda

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

#include "../io.h"

#if defined(ARDUINO)

static int s_pin = -1;
static void *s_arg;
static void (*s_pinEventCallback)(void *) = nullptr;

void lcd_registerGpioEvent(int pin, void (*on_pin_change)(void *), void *arg)
{
    s_pin = pin;
    s_pinEventCallback = on_pin_change;
    s_arg = arg;
}

void lcd_unregisterGpioEvent(int pin)
{
    (void)(pin);
    s_pin = -1;
}

void lcd_gpioWrite(int pin, int level)
{
    if ( pin == s_pin && s_pinEventCallback )
    {
        s_pinEventCallback(s_arg);
    }
    digitalWrite(pin, level);
}

#endif
