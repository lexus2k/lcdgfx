/*
    MIT License

    Copyright (c) 2022, Alexey Dynda

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
/**
 * @file lcd_wio.h support for LCD ILI9341 display installed in wio terminal console
 */

#pragma once

<<<<<<< HEAD
#if defined(CONFIG_ARDUINO_SPI_AVAILABLE) && defined(CONFIG_ARDUINO_SPI_ENABLE)
=======
#if defined(__XTENSA__) || defined(__linux__)
>>>>>>> 6861f88623bd05e6b024d7601253d5c1cbad735d

#include "lcd_hal/io.h"
#include "v2/lcd/st7789/lcd_st7789.h"

/**
 * @ingroup LCD_INTERFACE_API_V2
 * @{
 */

/**
<<<<<<< HEAD
 * Class implements Wio Terminal 320x240x16 lcd display in 16 bit mode over SPI
 */
class DisplayTTGO_135x240x16: public DisplayST7789_135x240x16_CustomSPI<ArduinoSpi>
{
public:
    DisplayTTGO_135x240x16();

    /**
     * Initializes Wio Terminal display in 16-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
=======
 * Class implements TTGO T-Display 135x240x16 lcd display in 16 bit mode over SPI
 */
class DisplayTTGO_135x240x16: public DisplayST7789_135x240x16_SPI
{
public:
    DisplayTTGO_135x240x16();
>>>>>>> 6861f88623bd05e6b024d7601253d5c1cbad735d
};

#endif

/**
 * @}
 */
