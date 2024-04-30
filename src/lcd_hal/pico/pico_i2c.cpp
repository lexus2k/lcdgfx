/*
    MIT License

    Copyright (c) 2016-2020, Alexey Dynda

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
#include "pico_i2c.h"

// !!!!! WARNING !!!!!!
// This is a dummy implementation that does not work !!!
#warning "I2C is not implemented for Pico board - this is a dummy"


//////////////////////////////////////////////////////////////////////////////////
//                        ARDUINO I2C IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////////

static uint8_t s_bytesWritten = 0;

PicoI2c::PicoI2c(int8_t scl, int8_t sda, uint8_t sa)
    : m_scl(scl)
    , m_sda(sda)
    , m_sa(sa)
    , m_mode(0)
{
}

PicoI2c::~PicoI2c()
{
}

void PicoI2c::begin()
{
}

void PicoI2c::end()
{
}

void PicoI2c::start()
{
}

void PicoI2c::stop()
{
}

void PicoI2c::send(uint8_t data)
{
}

void PicoI2c::sendBuffer(const uint8_t *buffer, uint16_t size)
{
}


#endif // PICO_BOARD
