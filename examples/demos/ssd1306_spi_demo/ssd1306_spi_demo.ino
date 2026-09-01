/*
    MIT License

    Copyright (c) 2026, Alexey Dynda

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
 *   This demo is the SPI counterpart of ssd1306_demo. Wiring for a 7-pin
 *   SSD1306 module is RST, CS, DC, CLK (D0/SCK) and MOSI (D1/SDA).
 *
 *   Constructor arguments are the RST pin, then
 *   { BUS id, CS pin, DC pin, FREQ (0 = default), CLK pin, MOSI pin }.
 *   Passing -1 for CLK/MOSI selects the platform default pins.
 *
 *   Atmega328p:  rst=3,  cs=4,  dc=5   (CLK/MOSI are the hardware SPI pins)
 *   ESP8266:     rst=4,  cs=-1, dc=5   (GPIO4 is D2, GPIO5 is D1 on NodeMCU)
 *   Raspberry:   rst=24, cs=0,  dc=23
 *
 *   On ESP32 the default CLK/MOSI depend on the chip: the original ESP32 uses
 *   18/23, while the newer parts default to the SPI2 IOMUX pins documented by
 *   ESP-IDF (for example 6/7 on C3, C5 and C6). Pass the pins explicitly if
 *   your board is wired differently.
 */

#include "lcdgfx.h"

DisplaySSD1306_128x64_SPI display(-1, {-1, 5, 4, 0, -1, -1}); // rst=-1, cs=5, dc=4, default freq/clk/mosi
// DisplaySSD1306_128x64_SPI display(3, {-1, 4, 5, 0, -1, -1});   // Atmega328p
// DisplaySSD1306_128x64_SPI display(22, {-1, 5, 21, 0, -1, -1}); // ESP32 (original), rst=22, cs=5, dc=21
// DisplaySSD1306_128x64_SPI display(4, {-1, -1, 5, 0, -1, -1});  // ESP8266

static const uint8_t heartImage[8] PROGMEM = {
    0B00001110, 0B00011111, 0B00111111, 0B01111110,
    0B01111110, 0B00111101, 0B00011001, 0B00001110,
};

void setup()
{
    display.begin();
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
}

void loop()
{
    display.clear();
    display.printFixed(0, 8, "SPI display demo", STYLE_NORMAL);
    display.drawBitmap1(96, 16, 8, 8, heartImage);
    lcd_delay(2000);

    display.clear();
    display.drawRect(8, 8, 119, 55);
    display.drawLine(8, 8, 119, 55);
    display.drawLine(8, 55, 119, 8);
    lcd_delay(2000);

    display.clear();
    display.printFixed(0, 0, "Inverted text", STYLE_NORMAL);
    display.invertColors();
    display.printFixed(0, 16, "Inverted text", STYLE_NORMAL);
    display.invertColors();
    lcd_delay(2000);
}
