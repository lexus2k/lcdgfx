/*
    MIT License

    Copyright (c) 2026 Alexey Dynda

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
 *   This example shows how to drive an lcdgfx display through the
 *   Adafruit_GFX-compatible facade (GfxCompat). Existing sketches written
 *   against the Adafruit_GFX API - drawPixel/drawLine/fillRect/print/... with
 *   int16_t coordinates and a trailing uint16_t color - work unchanged, with
 *   NO external Adafruit_GFX library required.
 *
 *   Color convention matches Adafruit_GFX: for monochrome displays use 0 (off)
 *   and 1 (on); for color displays pass an RGB565/RGB332 value (e.g.
 *   RGB_COLOR16(255, 0, 0)).
 *
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

// Wrap the display in the Adafruit_GFX-compatible adapter. The template
// argument is simply the display type. GfxCompat holds a reference to the
// display, so keep both objects alive for the same scope.
GfxCompat<DisplaySSD1306_128x64_I2C> gfx(display);

void setup()
{
    // The display is still initialized with its native API.
    display.begin();
    display.clear();

    // From here on everything uses familiar Adafruit_GFX calls.
    gfx.fillScreen(0);                  // clear to black (0 = off on mono)
    gfx.drawRect(0, 0, gfx.width(), gfx.height(), 1); // border
    gfx.fillCircle(104, 20, 10, 1);
    gfx.drawTriangle(80, 60, 100, 40, 120, 60, 1);
    gfx.fillRoundRect(6, 40, 40, 18, 5, 1);

    gfx.setTextColor(1);
    gfx.setCursor(8, 8);
    gfx.print("Adafruit GFX API");
    gfx.setCursor(8, 20);
    gfx.print("on lcdgfx :)");
}

void loop()
{
    // Animate a pixel sweeping across the top, Adafruit_GFX style.
    static int16_t x = 1;

    gfx.drawPixel(x, 30, 1);           // draw current
    gfx.drawPixel(x - 1 < 1 ? gfx.width() - 2 : x - 1, 30, 0); // erase previous

    x++;
    if (x >= gfx.width() - 1)
    {
        x = 1;
    }
    lcd_delay(30);
}
