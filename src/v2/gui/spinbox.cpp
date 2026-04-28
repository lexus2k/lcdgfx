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

#include "spinbox.h"

LcdGfxSpinbox::LcdGfxSpinbox(const NanoRect &rect, int16_t minValue, int16_t maxValue, int16_t value, int16_t step,
                             bool wrap)
    : m_rect(rect)
    , m_min(minValue <= maxValue ? minValue : maxValue)
    , m_max(maxValue >= minValue ? maxValue : minValue)
    , m_value(value)
    , m_step(step >= 1 ? step : 1)
    , m_wrap(wrap)
{
    setValue(value);
}

void LcdGfxSpinbox::up()
{
    int32_t v = (int32_t)m_value + m_step;
    if ( v > m_max )
    {
        if ( m_wrap )
        {
            int32_t span = (int32_t)m_max - m_min + 1;
            if ( span <= 0 )
            {
                v = m_min;
            }
            else
            {
                int32_t over = v - m_min;
                v = m_min + ((over % span) + span) % span;
            }
        }
        else
        {
            v = m_max;
        }
    }
    m_value = (int16_t)v;
}

void LcdGfxSpinbox::down()
{
    int32_t v = (int32_t)m_value - m_step;
    if ( v < m_min )
    {
        if ( m_wrap )
        {
            int32_t span = (int32_t)m_max - m_min + 1;
            if ( span <= 0 )
            {
                v = m_min;
            }
            else
            {
                int32_t under = v - m_min;
                v = m_min + ((under % span) + span) % span;
            }
        }
        else
        {
            v = m_min;
        }
    }
    m_value = (int16_t)v;
}

void LcdGfxSpinbox::setValue(int16_t value)
{
    if ( value < m_min ) value = m_min;
    else if ( value > m_max ) value = m_max;
    m_value = value;
}
