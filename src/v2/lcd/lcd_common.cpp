/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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

#include "v2/lcd/lcd_common.h"
#include <stddef.h>

void ssd1306_resetController2(int8_t rstPin, uint8_t delayMs)
{
    if ( rstPin >= 0 )
    {
        lcd_gpioMode(rstPin, LCD_GPIO_OUTPUT);
        lcd_gpioWrite(rstPin, LCD_HIGH);
        /* Wait at least 10ms after VCC is up for LCD */
        lcd_delay(10);
        /* Perform reset operation of LCD display */
        lcd_gpioWrite(rstPin, LCD_LOW);
        lcd_delay(10);
        lcd_gpioWrite(rstPin, LCD_HIGH);
        lcd_delay(delayMs);
    }
}
