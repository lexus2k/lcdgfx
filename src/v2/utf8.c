/*
    MIT License

    Copyright (c) 2016-2019, Alexey Dynda

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

///////////////////////////////////////////////////////////////////////////////
////// GENERIC FUNCTIONS APPLICABLE FOR ALL DISPLAY TYPES /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "lcdgfx.h"
#include "nano_gfx_types.h"
#include "lcd_hal/io.h"

#ifdef CONFIG_SSD1306_UNICODE_ENABLE
extern uint8_t g_ssd1306_unicode2;
#endif

void ssd1306_enableUtf8Mode2(void)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    g_ssd1306_unicode2 = 1;
#endif
}

void ssd1306_enableAsciiMode2(void)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    g_ssd1306_unicode2 = 0;
#endif
}

