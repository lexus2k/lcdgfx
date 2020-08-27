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
/* !!! THIS FILE IS AUTO GENERATED !!! */
#include "lcd_any.h"
#include "lcd_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif

/////////////////////// 1 BIT ////////////////////////////

DisplayAny1::DisplayAny1(lcduint_t width, lcduint_t height)
    : NanoDisplayOps(m_any)
    , DisplayInterface()
    , m_any(*this)
{
    this->m_w = width;
    this->m_h = height;
}

void DisplayAny1::begin()
{
}

void DisplayAny1::end()
{
}

/////////////////////// 4 BIT ////////////////////////////

DisplayAny4::DisplayAny4(lcduint_t width, lcduint_t height)
    : NanoDisplayOps(m_any)
    , DisplayInterface()
    , m_any(*this)
{
    this->m_w = width;
    this->m_h = height;
}

void DisplayAny4::begin()
{
}

void DisplayAny4::end()
{
}

/////////////////////// 8 BIT ////////////////////////////

DisplayAny8::DisplayAny8(lcduint_t width, lcduint_t height)
    : NanoDisplayOps(m_any)
    , DisplayInterface()
    , m_any(*this)
{
    this->m_w = width;
    this->m_h = height;
}

void DisplayAny8::begin()
{
}

void DisplayAny8::end()
{
}

/////////////////////// 16 BIT ////////////////////////////

DisplayAny16::DisplayAny16(lcduint_t width, lcduint_t height)
    : NanoDisplayOps(m_any)
    , DisplayInterface()
    , m_any(*this)
{
    this->m_w = width;
    this->m_h = height;
}

void DisplayAny16::begin()
{
}

void DisplayAny16::end()
{
}
