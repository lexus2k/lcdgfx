/*
    MIT License

    Copyright (c) 2020-2021, Alexey Dynda

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
 *   SDA (4) -|    |- (1)
 *   GND     -|____|- (0) - BUTTONS module
 *
 *   Atmega328 PINS: connect LCD to A4/A5,
 *   Z-keypad ADC module on A0 pin.
 */

#include "lcdgfx.h"
#include "lcdgfx_gui.h"

#ifndef A0
#define A0  0
#endif
#define BUTTON_PIN     A0

DisplaySSD1306_128x64_I2C display(-1); // or (-1,{busId, addr, scl, sda, frequency})
NanoEngine1<DisplaySSD1306_128x64_I2C> engine(display);
//DisplaySSD1306_128x64_SPI display(-1,{-1, 0, 1, 0, -1, -1); // Use this line for nano pi (RST not used, 0=CE, gpio1=D/C)
//DisplaySSD1306_128x64_SPI display(3,{-1, 4, 5, 0,-1,-1});   // Use this line for Atmega328p (3=RST, 4=CE, 5=D/C)
//DisplaySSD1306_128x64_SPI display(24,{-1, 0, 23, 0,-1,-1}); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//DisplaySSD1306_128x64_SPI display(22,{-1, 5, 21, 0,-1,-1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)

/* This variable will hold menu state, processed by SSD1306 API functions */
LcdGfxButton button( "Button", {{10,10},{64,30}} );

bool drawAll()
{
    engine.getDisplay().setFixedFont(ssd1306xled_font6x8);
    engine.getCanvas().setFixedFont(ssd1306xled_font6x8);
    engine.getCanvas().clear();
    button.show( engine.getCanvas() );
    return true;
}

void setup()
{
    display.begin();
    engine.begin();
    engine.drawCallback( drawAll );  // Set callback to draw content
    engine.refresh();                // Makes engine to refresh whole display content at start-up
    engine.display();
    engine.connectZKeypad( BUTTON_PIN );     // Connect Z-keypad
}

void loop()
{
    if ( engine.clicked( BUTTON_UP ) )
    {
        button.setFocus( !button.isActive() );
        engine.refresh( button.getRect() );
    }
    if ( engine.clicked( BUTTON_DOWN ) )
    {
        button.setFocus( !button.isActive() );
        engine.refresh( button.getRect() );
    }
    if ( engine.clicked( BUTTON_A ) )
    {
        /* Process button if it is active here */
    }
    engine.display();
}




