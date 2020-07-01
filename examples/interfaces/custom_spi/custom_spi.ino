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
 *  This is example for Arduino standard SPI support
 */

#include "lcdgfx.h"
#include <SPI.h>


class CustomSpi: public _ICustom<16>
{
public:
    CustomSpi( uint32_t frequency, int8_t dc )
        : _ICustom( dc )
        , m_frequency( frequency )
    {
    }

    void start() override
    {
        SPI.beginTransaction(SPISettings(m_frequency, MSBFIRST, SPI_MODE0));
    }

    void stop() override
    {
        SPI.endTransaction();
    }

    void transferToHw(const uint8_t *buffer, uint16_t size) override
    {
        /* Do not use SPI.transfer(buffer, size)! this method corrupts buffer content */
        while (size--)
        {
            SPI.transfer(*buffer);
            buffer++;
        };
    }

private:
    uint32_t m_frequency;
};

// Use this line for Atmega328p (3=RST, CE is not used, 5=D/C)
// DC pin is used here twice, first time it is passed as argument for
// display login, and the last one for our CustomSpi class
// 0 mean that we use lcd defined frequnecy
DisplaySSD1306_128x64_Custom<CustomSpi> display(3, 5, 0, 5);

void setup()
{
    pinMode( 3, OUTPUT ); // RST pin config
    pinMode( 5, OUTPUT ); // D/C pin config
    SPI.begin();

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




