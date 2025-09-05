/*
    MIT License

    Copyright 2019-2022 (C) Alexey Dynda

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
/* !!! THIS FILE IS AUTO GENERATED !!! */
#include "lcd_ssd1306b.h"
#include "lcd_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif
void DisplaySSD1306B_64x32_SPI::begin()
{
    m_spi.begin();
    DisplaySSD1306B_64x32::begin();
}

void DisplaySSD1306B_64x32_SPI::end()
{
    DisplaySSD1306B_64x32::end();
    m_spi.end();
}
void DisplaySSD1306B_64x32_I2C::begin()
{
    m_i2c.begin();
    DisplaySSD1306B_64x32::begin();
}

void DisplaySSD1306B_64x32_I2C::end()
{
    DisplaySSD1306B_64x32::end();
    m_i2c.end();
}
void DisplaySSD1306B_64x48_SPI::begin()
{
    m_spi.begin();
    DisplaySSD1306B_64x48::begin();
}

void DisplaySSD1306B_64x48_SPI::end()
{
    DisplaySSD1306B_64x48::end();
    m_spi.end();
}
void DisplaySSD1306B_64x48_I2C::begin()
{
    m_i2c.begin();
    DisplaySSD1306B_64x48::begin();
}

void DisplaySSD1306B_64x48_I2C::end()
{
    DisplaySSD1306B_64x48::end();
    m_i2c.end();
}
void DisplaySSD1306B_128x32_SPI::begin()
{
    m_spi.begin();
    DisplaySSD1306B_128x32::begin();
}

void DisplaySSD1306B_128x32_SPI::end()
{
    DisplaySSD1306B_128x32::end();
    m_spi.end();
}
void DisplaySSD1306B_128x32_I2C::begin()
{
    m_i2c.begin();
    DisplaySSD1306B_128x32::begin();
}

void DisplaySSD1306B_128x32_I2C::end()
{
    DisplaySSD1306B_128x32::end();
    m_i2c.end();
}
void DisplaySSD1306B_128x64_SPI::begin()
{
    m_spi.begin();
    DisplaySSD1306B_128x64::begin();
}

void DisplaySSD1306B_128x64_SPI::end()
{
    DisplaySSD1306B_128x64::end();
    m_spi.end();
}
void DisplaySSD1306B_128x64_I2C::begin()
{
    m_i2c.begin();
    DisplaySSD1306B_128x64::begin();
}

void DisplaySSD1306B_128x64_I2C::end()
{
    DisplaySSD1306B_128x64::end();
    m_i2c.end();
}
