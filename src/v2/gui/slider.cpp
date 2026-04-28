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

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "slider.h"

LcdGfxSlider::LcdGfxSlider(const NanoRect &rect, int16_t minValue, int16_t maxValue, int16_t value, int16_t step,
                           LcdGfxSliderOrientation orientation)
    : m_rect(rect)
    , m_min(minValue <= maxValue ? minValue : maxValue)
    , m_max(maxValue >= minValue ? maxValue : minValue)
    , m_value(value)
    , m_step(step >= 1 ? step : 1)
    , m_orientation(orientation)
{
    setValue(value);
}

void LcdGfxSlider::up()
{
    int32_t v = (int32_t)m_value + m_step;
    if ( v > m_max ) v = m_max;
    m_value = (int16_t)v;
}

void LcdGfxSlider::down()
{
    int32_t v = (int32_t)m_value - m_step;
    if ( v < m_min ) v = m_min;
    m_value = (int16_t)v;
}

void LcdGfxSlider::setValue(int16_t value)
{
    if ( value < m_min ) value = m_min;
    else if ( value > m_max ) value = m_max;
    m_value = value;
}
