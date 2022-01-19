/*
    MIT License

    Copyright (c) 2016-2022, Alexey Dynda

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

#include "../io.h"

#if defined(ARDUINO)

//////////////////////////////////////////////////////////////////////////////////
//                        ARDUINO SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_ARDUINO_SPI_AVAILABLE) && defined(CONFIG_ARDUINO_SPI_ENABLE)

/* STANDARD branch */
#include <SPI.h>

ArduinoSpi::ArduinoSpi(int8_t csPin, int8_t dcPin, int8_t clkPin, int8_t mosiPin, uint32_t frequency, SPIClass *spi)
    : m_cs(csPin)
    , m_dc(dcPin)
    , m_clk(clkPin)
    , m_mosi(mosiPin)
    , m_frequency(frequency)
    , m_spi(spi)
{
}

ArduinoSpi::~ArduinoSpi()
{
}

void ArduinoSpi::begin()
{
    if ( m_cs >= 0 )
        pinMode(m_cs, OUTPUT);
    if ( m_dc >= 0 )
        pinMode(m_dc, OUTPUT);
#if defined(ESP32) || defined(ESP31B)
    //  clk, miso, mosi, cs - cs is controlled manually
    m_spi->begin(m_clk, -1, m_mosi, -1);
#else
    m_spi->begin();
#endif
}

void ArduinoSpi::end()
{
    m_spi->end();
}

void ArduinoSpi::start()
{
    /* anyway, oled ssd1331 cannot work faster, clock cycle should be > 150ns: *
     * 1s / 150ns ~ 6.7MHz                                                     */
    m_spi->beginTransaction(SPISettings(m_frequency, MSBFIRST, SPI_MODE0));
    if ( m_cs >= 0 )
    {
        lcd_gpioWrite(m_cs, LCD_LOW);
    }
}

void ArduinoSpi::stop()
{
    if ( m_cs >= 0 )
    {
        lcd_gpioWrite(m_cs, LCD_HIGH);
    }
    m_spi->endTransaction();
}

void ArduinoSpi::send(uint8_t data)
{
    m_spi->transfer(data);
}

void ArduinoSpi::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    /* Do not use m_spi->transfer(buffer, size)! this method corrupts buffer content */
    while ( size-- )
    {
        m_spi->transfer(*buffer);
        buffer++;
    };
}

#endif

#if defined(CONFIG_ARDUINO_SPI2_AVAILABLE) && defined(CONFIG_ARDUINO_SPI_ENABLE)

SPIClass SPI2_(HSPI);

ArduinoSpi2::ArduinoSpi2(int8_t csPin, int8_t dcPin, int8_t clkPin, int8_t mosiPin, uint32_t freq)
    : m_cs(csPin)
    , m_dc(dcPin)
    , m_clk(clkPin)
    , m_mosi(mosiPin)
    , m_frequency(freq)
{
}

ArduinoSpi2::~ArduinoSpi2()
{
}

void ArduinoSpi2::begin()
{
    if ( m_cs >= 0 )
        pinMode(m_cs, OUTPUT);
    if ( m_dc >= 0 )
        pinMode(m_dc, OUTPUT);
    // sck, miso, mosi, ss
    SPI2_.begin(m_clk, -1, m_mosi, m_cs);
}

void ArduinoSpi2::end()
{
    SPI2_.end();
}

void ArduinoSpi2::start()
{
    /* anyway, oled ssd1331 cannot work faster, clock cycle should be > 150ns: *
     * 1s / 150ns ~ 6.7MHz                                                     */
    SPI2_.beginTransaction(SPISettings(m_frequency, MSBFIRST, SPI_MODE0));
    if ( m_cs >= 0 )
    {
        lcd_gpioWrite(m_cs, LCD_LOW);
    }
}

void ArduinoSpi2::stop()
{
    if ( m_cs >= 0 )
    {
        lcd_gpioWrite(m_cs, LCD_HIGH);
    }
    SPI2_.endTransaction();
}

void ArduinoSpi2::send(uint8_t data)
{
    SPI2_.transfer(data);
}

void ArduinoSpi2::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    /* Do not use SPI2.transfer(buffer, size)! this method corrupts buffer content */
    while ( size-- )
    {
        SPI2_.transfer(*buffer);
        buffer++;
    };
}

#endif

#endif // ARDUINO
