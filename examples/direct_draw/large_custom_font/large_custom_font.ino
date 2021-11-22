/*
    MIT License

    Copyright (c) 2016-2019, 2021 Alexey Dynda

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
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0)
 *
 *   Atmega328 PINS: connect LCD to A4/A5
 */

#include "lcdgfx.h"

DisplaySSD1306_128x64_I2C display(-1); // or (-1,{busId, addr, scl, sda, frequency})
//DisplaySSD1306_128x64_SPI display(-1,{-1, 0, 1, 0, -1, -1); // Use this line for nano pi (RST not used, 0=CE, gpio1=D/C)
//DisplaySSD1306_128x64_SPI display(3,{-1, 4, 5, 0,-1,-1});   // Use this line for Atmega328p (3=RST, 4=CE, 5=D/C)
//DisplaySSD1306_128x64_SPI display(24,{-1, 0, 23, 0,-1,-1}); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//DisplaySSD1306_128x64_SPI display(22,{-1, 5, 21, 0,-1,-1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)

const uint8_t g_customFont_5x16[] PROGMEM =
{
    0x00,  // 0x00 means fixed font type - the only supported by the library
    0x05,  // 0x05 = 5 - font width in pixels
    0x10,  // 0x10 = 16 - font height in pixels
    0x30,  // 0x30 = 48 - first ascii character number in the font ('0' = ascii code 48)
    // '0'
    0b00000000,  // upper 8 pixels of character
    0b11111100,
    0b00000011,
    0b00000011,
    0b11111100,

    0b00000000,  // lower 8 pixels of character
    0b00011111,
    0b01100000,
    0b01100000,
    0b00011111,
    // '1'
    0b00000000,  // upper 8 pixels of character
    0b00001100,
    0b11111111,
    0b00000000,
    0b00000000,

    0b00000000,  // lower 8 pixels of character
    0b01100000,
    0b01111111,
    0b01100000,
    0b00000000,

    0x00, // End of font
};

void setup()
{
    display.begin();
    display.clear();
    display.setFixedFont( g_customFont_5x16 );
    display.printFixed(0,  0,  "01100011", STYLE_NORMAL );
}


void loop()
{
}




