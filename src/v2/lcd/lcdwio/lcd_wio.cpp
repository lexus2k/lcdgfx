/*
    MIT License

    Copyright (c) 2021,2022, Alexey Dynda

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

#ifdef SEEED_WIO_TERMINAL

#include "lcd_wio.h"
#include "SPI.h"

DisplayWioTerminal_320x240x16::DisplayWioTerminal_320x240x16()
    : DisplayILI9341_240x320x16_CustomSPI<ArduinoSpi>(71Ul, 70Ul, PIN_SPI3_SS, 70Ul, -1, -1, 20000000, &SPI3)
{
}

void DisplayWioTerminal_320x240x16::begin()
{
    DisplayILI9341_240x320x16_CustomSPI<ArduinoSpi>::begin();
    this->getInterface().setRotation(1);
}

void DisplayWioTerminal_320x240x16::end()
{
    DisplayILI9341_240x320x16_CustomSPI<ArduinoSpi>::end();
}

#endif

/**
 * @}
 */
