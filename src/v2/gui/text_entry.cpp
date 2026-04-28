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

#include "text_entry.h"
#include <string.h>

LcdGfxTextEntry::LcdGfxTextEntry(char *buffer, uint8_t maxLen, const NanoRect &rect, const char *charset)
    : m_buffer(buffer)
    , m_charset(charset)
    , m_maxLen(maxLen)
    , m_rect(rect)
{
    m_charsetLen = (uint8_t)strlen(m_charset);
    if ( m_charsetLen == 0 )
    {
        // Defensive: degenerate charset would deadlock cycling. Force
        // a one-character set so behaviour stays defined.
        static const char fallback[] = " ";
        m_charset = fallback;
        m_charsetLen = 1;
    }
    if ( m_maxLen == 0 )
    {
        m_maxLen = 1;
    }
    const char fill = m_charset[0];
    for ( uint8_t i = 0; i < m_maxLen; i++ )
    {
        m_buffer[i] = fill;
    }
    m_buffer[m_maxLen] = '\0';
}

uint8_t LcdGfxTextEntry::indexOfCurrentChar() const
{
    char ch = m_buffer[m_cursor];
    for ( uint8_t i = 0; i < m_charsetLen; i++ )
    {
        if ( m_charset[i] == ch )
        {
            return i;
        }
    }
    return 0;
}

void LcdGfxTextEntry::up()
{
    uint8_t idx = indexOfCurrentChar();
    idx = (uint8_t)((idx + 1) % m_charsetLen);
    m_buffer[m_cursor] = m_charset[idx];
}

void LcdGfxTextEntry::down()
{
    uint8_t idx = indexOfCurrentChar();
    idx = (uint8_t)((idx + m_charsetLen - 1) % m_charsetLen);
    m_buffer[m_cursor] = m_charset[idx];
}

void LcdGfxTextEntry::left()
{
    if ( m_cursor > 0 )
    {
        m_cursor--;
    }
}

void LcdGfxTextEntry::right()
{
    if ( m_cursor + 1 < m_maxLen )
    {
        m_cursor++;
    }
}

void LcdGfxTextEntry::setCursor(uint8_t cursor)
{
    if ( cursor >= m_maxLen )
    {
        cursor = (uint8_t)(m_maxLen - 1);
    }
    m_cursor = cursor;
}
