/*
    MIT License

    Copyright (c) 2022 Alexey Dynda

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


/*
 * Heart image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is define from bottom to top (bits), from left to
 * right (bytes).
 */
const PROGMEM uint8_t heartImage[8] =
{
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
};

/*
 * Each pixel in SSD1306 display takes 1 bit of the memory. So, full resolution
 * of 128x64 LCD display will require 128*64/8 = 1024 bytes of SRAM for the buffer.
 * To let this example to run on Attiny devices (they have 256/512 byte SRAM), we
 * will use small canvas buffer: 32x64 (requires 256 bytes of SRAM), so the example
 * would run even on Attiny45.
 */
/* Create canvas object */
// Width must be power of 2, i.e. 16, 32, 64, 128...
// Height must be divided on 8, i.e. 8, 16, 24, 32...
NanoCanvas<64,32,1> canvas;
NanoCanvas<32,64,1> rotatedCanvas;

void setup()
{
    /* Initialize and clear display */
    display.begin();
    display.clear();
    canvas.setMode( CANVAS_MODE_TRANSPARENT );
    canvas.clear();
    canvas.drawBitmap1( 5, 8, 8, 8, heartImage );
    canvas.drawRect(0,0,63,31);
    canvas.rotateCW(rotatedCanvas);
}


void loop()
{
    lcd_delay(40);

    /* Now, draw original canvas on the display */
    display.drawCanvas( 0, 0, canvas );
    /* Now, draw rotated canvas on the display */
    display.drawCanvas( 80, 0, rotatedCanvas );
}
