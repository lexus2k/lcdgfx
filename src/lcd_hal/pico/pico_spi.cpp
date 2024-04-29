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

#if defined(PICO_BOARD)
//////////////////////////////////////////////////////////////////////////////////
//                        PI PICO SPI IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////



PicoSpi::PicoSpi(int8_t csPin, int8_t dcPin, int8_t clkPin, int8_t mosiPin, uint32_t frequency)
    : m_cs(csPin)
    , m_dc(dcPin)
    , m_clk(clkPin)
    , m_mosi(mosiPin)
    , m_frequency(frequency)
{
}

PicoSpi::~PicoSpi()
{
}

void PicoSpi::begin()
{
    if(m_cs >= 0) {
        gpio_init(m_cs);
        gpio_set_dir(m_cs, GPIO_OUT);
        gpio_put(m_cs, 0);
    };
    if(m_dc >= 0) {
        gpio_init(m_dc);
        gpio_set_dir(m_dc, GPIO_OUT);
        gpio_put(m_dc, 0);
    };
    
    gpio_set_function(m_clk, GPIO_FUNC_SPI);
    gpio_set_function(m_mosi, GPIO_FUNC_SPI);
    // set SPI Mode 3; MSB first
    spi_set_format(PICO_SPI, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_init(PICO_SPI, m_frequency);
}

void PicoSpi::end()
{
    spi_deinit(PICO_SPI);
}

void PicoSpi::start()
{
    if(m_cs >= 0)
        gpio_put(m_cs, 0);
}

void PicoSpi::stop()
{
    if(m_cs >= 0)
        gpio_put(m_cs, 1);
}

void PicoSpi::send(uint8_t data)
{
    
    spi_write_blocking(PICO_SPI, &data, 1);
}

void PicoSpi::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    spi_write_blocking(PICO_SPI, buffer, size);
}

#endif // PICO_BOARD
