/*
    MIT License

    Copyright (c) 2020, Alexey Dynda

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
 *  This is example for Custom monochrome display implementation.
 */

#include "lcdgfx.h"

/**
 * This example only demonstrates, which methods must be implemented to
 * make lcdgfx library to work with any diplsay.
 * If you replace DisplayAny1 with DisplayAny4, DisplayAny8 or DisplayAny16,
 * you will get support for greyscale or color custom displays.
 *
 * Basically you need to implement only display initialization in begin() method
 * and 4 interface methods:
 *   - startBlock() to prepare display for receiving data from lcdgfx
 *                x, y - position of area, which lcdgfx library wants to update
 *                w    - width of area, which lcdgfx library wants to update, can be 0
 *                       if lcdgfx wants to use maximum available width
 *   - nextBlock() switch to next line in case if display doesn't support
 *                 GDRAM block selection
 *   - endBlock() to complete data transfer.
 *   - send() to send data byte to display interface
 *
 * Example below uses already implemented SSD1306 display initialization to make
 * example understanding easier.
 */
class CustomSsd1306Display: public DisplayAny1
{
public:
    CustomSsd1306Display()
        : DisplayAny1(128,64)
        , m_display(-1)
    {
    }

    void begin() override
    {
        m_display.begin();
    }

    void end() override
    {
        m_display.end();
    }

    void startBlock(lcduint_t x, lcduint_t y, lcduint_t w) override
    {
        m_display.getInterface().startBlock(x,y,w);
    }

    void endBlock() override
    {
        m_display.getInterface().endBlock();
    }

    void send(uint8_t data) override
    {
        m_display.getInterface().send(data);
    }

private:
    DisplaySSD1306_128x64_I2C m_display;
};

CustomSsd1306Display display;

void setup()
{
    display.begin();
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    display.drawWindow(0,0,0,0,"Downloading",true);
}

int progress = 0;

void loop()
{
    display.drawProgressBar( progress );
    progress++;
    if ( progress > 100 )
    {
        progress = 0;
        delay(2000);
    }
    else
    {
        delay(50);
    }
}




