/*
    MIT License

    Copyright (c) 2025, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
*/
/*
 *   This sketch demonstrates the LcdGfxXpt2046 resistive-touch driver
 *   with a 2.8" ILI9341 TFT module that has an XPT2046 touch controller
 *   sharing the SPI bus.
 *
 *   Wiring (Arduino UNO):
 *     ILI9341 CS  -> D10
 *     ILI9341 DC  -> D9
 *     ILI9341 RST -> D8
 *     XPT2046 CS  -> D7
 *     SCK / MOSI / MISO -> hardware SPI pins (D13 / D11 / D12)
 *
 *   The example uses the Arduino SPI library directly for the touch driver
 *   so that the same SPI bus can be shared with the display.
 */

#include "lcdgfx.h"
#include "touch/xpt2046.h"
#include <SPI.h>

#define TOUCH_CS_PIN  7

DisplayILI9341_240x320x16_SPI display(8, {-1, 10, 9, 0, -1, -1});

/**
 * Tiny adapter that satisfies the SpiT requirements of LcdGfxXpt2046 by
 * forwarding to the Arduino SPI singleton. The XPT2046 is happy with low
 * SPI clock speeds; 2 MHz is a safe default that works on most modules.
 */
class ArduinoTouchSpi
{
public:
    explicit ArduinoTouchSpi(uint8_t cs) : m_cs(cs) {}
    void begin()
    {
        pinMode(m_cs, OUTPUT);
        digitalWrite(m_cs, HIGH);
        SPI.begin();
    }
    void end() {}
    void start()
    {
        SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
        digitalWrite(m_cs, LOW);
    }
    void stop()
    {
        digitalWrite(m_cs, HIGH);
        SPI.endTransaction();
    }
    uint8_t transfer(uint8_t b) { return SPI.transfer(b); }
private:
    uint8_t m_cs;
};

ArduinoTouchSpi touchSpi(TOUCH_CS_PIN);
LcdGfxXpt2046<ArduinoTouchSpi> touch(touchSpi);

void setup()
{
    display.begin();
    display.setFixedFont(ssd1306xled_font6x8);
    display.fill(0x0000);

    TouchCalibration cal;
    cal.xMin = 300;  cal.xMax = 3800;
    cal.yMin = 300;  cal.yMax = 3800;
    cal.screenWidth = 240;
    cal.screenHeight = 320;
    touch.setCalibration(cal);
    touch.begin();

    display.setColor(RGB_COLOR16(255, 255, 255));
    display.printFixed(8, 8, "Touch the screen");
}

void loop()
{
    int16_t x, y;
    if (touch.read(x, y))
    {
        display.setColor(RGB_COLOR16(0, 255, 0));
        display.fillRect(x - 2, y - 2, x + 2, y + 2);
    }
    delay(20);
}
