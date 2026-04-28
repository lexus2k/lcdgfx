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
 *   Menu navigated by an XPT2046 resistive touch controller.
 *
 *   Wiring (Arduino UNO):
 *     ILI9341 CS  -> D10
 *     ILI9341 DC  -> D9
 *     ILI9341 RST -> D8
 *     XPT2046 CS  -> D7
 *     SCK / MOSI / MISO -> hardware SPI pins
 *
 *   Touch on a menu item to select it; touch the up/down arrow stripes to scroll.
 */

#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "touch/xpt2046.h"
#include <SPI.h>

#define TOUCH_CS_PIN  7

DisplayILI9341_240x320x16_SPI display(8, {-1, 10, 9, 0, -1, -1});

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

const char *menuItems[] = {
    "Settings", "Wi-Fi", "Display", "Sound", "About",
    "Update", "Diagnostics", "Reboot",
};
LcdGfxMenu menu(menuItems, 8, {{0, 0}, {239, 200}});

void setup()
{
    display.begin();
    display.setFixedFont(ssd1306xled_font6x8);
    display.fill(0x0000);
    display.setColor(RGB_COLOR16(255, 255, 255));

    TouchCalibration cal;
    cal.xMin = 300;  cal.xMax = 3800;
    cal.yMin = 300;  cal.yMax = 3800;
    cal.screenWidth = 240;
    cal.screenHeight = 320;
    touch.setCalibration(cal);
    touch.begin();

    menu.show(display);
}

void loop()
{
    int16_t x, y;
    if (touch.read(x, y))
    {
        if (menu.onTouch(display, x, y))
        {
            display.fill(0x0000);
            menu.show(display);
        }
        delay(150); // simple debounce
    }
}
