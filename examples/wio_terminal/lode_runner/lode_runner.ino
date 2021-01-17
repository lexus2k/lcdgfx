/*
    MIT License

    Copyright (c) 2021, Alexey Dynda

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
/*
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1) - BUZZER
 *   GND     -|____|- (0) - BUTTONS module
 *
 *   Atmega328 PINS with i2c SSD1306 to A4/A5, BUZZER on D8,
 *   Z-keypad ADC module on A0 pin.
 *
 *   Atmega328 PINS with spi Nokia 5110 LCD:
 *   LCD RST to D3
 *   LCD CES to D4
 *   LCD DC to  D5
 *   LCD DIN to D11
 *   LCD CLK to D13
 *   LCD BL to  VCC
 */

#include "wio_hardware.h"
#ifndef __linux__
#include "lcd_backlight.hpp"
#endif
#include "src/main.h"

#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
#ifdef __linux__
GraphicsDisplay display(3,{-1, 4, 5, 0,-1,-1});
#else
static LCDBackLight backLight;
GraphicsDisplay display;
#endif

GraphicsEngine engine(display);


static bool onDrawMain()
{
    return true;
}

void setup()
{
    display.begin();
    display.getInterface().setRotation(1);
    display.clear();

#ifndef __linux__
    backLight.initialize();
    backLight.setBrightness(150);
#endif
    digitalWrite( LCD_BACKLIGHT, HIGH );

    engine.connectWioKeypad();
    engine.drawCallback( onDrawMain );
    engine.begin();
    engine.setFrameRate( 30 );
    engine.refresh();

    lodeRunnerInit();
}

void loop()
{
    lodeRunnerRun();
#if 0
    if (!engine.nextFrame()) return;
    engine.update();
    engine.display();
#endif
}
