/*
    MIT License

    Copyright (c) 2018-2020, Alexey Dynda

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

#include "lcd_hal/io.h"

#if 0
void    ssd1306_setRgbColor(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
}

void    ssd1306_setRgbColor8(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
}

static void ssd1306_drawBufferPitch8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_lcd.set_block(x, y, w);
    while (h--)
    {
        lcduint_t line = w;
        while (line--)
        {
            ssd1306_lcd.send_pixels8( *data );
            data++;
        }
        data += pitch - w;
    }
    ssd1306_intf.stop();
}

void ssd1306_drawBufferFast8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    ssd1306_drawBufferPitch8( x, y, w, h, w, data );
}

void ssd1306_drawBufferEx8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_drawBufferPitch8( x, y, w, h, pitch, data );
}

void ssd1306_putColorPixel8(lcdint_t x, lcdint_t y, uint8_t color)
{
    ssd1306_lcd.set_block(x, y, 0);
    ssd1306_lcd.send_pixels8( color );
    ssd1306_intf.stop();
}

#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                            COMMON GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

// template class NanoDisplayOps8<I>;

// template <class I>
// void NanoDisplayOps8<I>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
//{
//    m_cursorX = xpos;
//    m_cursorY = y;
//    m_fontStyle = style;
//    print( ch );
//}

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template <class I> void NanoDisplayOps8<I>::putPixel(lcdint_t x, lcdint_t y)
{
    this->m_intf.startBlock(x, y, 0);
    this->m_intf.send(this->m_color);
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps8<I>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    this->m_intf.startBlock(x1, y1, 0);
    while ( x1 < x2 )
    {
        this->m_intf.send(this->m_color);
        x1++;
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps8<I>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    this->m_intf.startBlock(x1, y1, 1);
    while ( y1 <= y2 )
    {
        this->m_intf.send(this->m_color);
        y1++;
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps8<I>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if ( y1 > y2 )
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if ( x1 > x2 )
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
    }
    this->m_intf.startBlock(x1, y1, x2 - x1 + 1);
    uint32_t count = (x2 - x1 + 1) * (y2 - y1 + 1);
    while ( count-- )
    {
        this->m_intf.send(this->m_color);
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps8<I>::fill(uint16_t color)
{
    this->m_intf.startBlock(0, 0, 0);
    uint32_t count = (uint32_t)this->m_w * (uint32_t)this->m_h;
    while ( count-- )
    {
        this->m_intf.send(color);
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps8<I>::clear()
{
    fill(0x00);
}

template <class I>
void NanoDisplayOps8<I>::drawXBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // TODO:
}

template <class I>
void NanoDisplayOps8<I>::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    uint8_t blackColor = this->m_bgColor;
    uint8_t color = this->m_color;
    this->m_intf.startBlock(xpos, ypos, w);
    while ( h-- )
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = pgm_read_byte(bitmap);
            if ( data & bit )
                this->m_intf.send(color);
            else
                this->m_intf.send(blackColor);
            bitmap++;
        }
        bit <<= 1;
        if ( bit == 0 )
        {
            bit = 1;
        }
        else
        {
            bitmap -= w;
        }
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps8<I>::drawBitmap4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps8<I>::drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while ( count-- )
    {
        this->m_intf.send(pgm_read_byte(bitmap));
        bitmap++;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps8<I>::drawBitmap16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps8<I>::drawBuffer1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    uint8_t bit = 1;
    uint8_t blackColor = this->m_bgColor;
    uint8_t color = this->m_color;
    this->m_intf.startBlock(xpos, ypos, w);
    while ( h-- )
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = *buffer;
            if ( data & bit )
                this->m_intf.send(color);
            else
                this->m_intf.send(blackColor);
            buffer++;
        }
        bit <<= 1;
        if ( bit == 0 )
        {
            bit = 1;
        }
        else
        {
            buffer -= w;
        }
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps8<I>::drawBuffer1Fast(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    this->drawBuffer1(x, y, w, h, buf);
}

template <class I>
void NanoDisplayOps8<I>::drawBuffer4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps8<I>::drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while ( count-- )
    {
        this->m_intf.send(*buffer);
        buffer++;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps8<I>::drawBuffer16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <class I> uint8_t NanoDisplayOps8<I>::printChar(uint8_t c)
{
    uint16_t unicode = this->m_font->unicode16FromUtf8(c);
    if ( unicode == SSD1306_MORE_CHARS_REQUIRED )
        return 0;
    SCharInfo char_info;
    this->m_font->getCharBitmap(unicode, &char_info);
    uint8_t mode = this->m_textMode;
    for ( uint8_t i = 0; i < (this->m_fontStyle == STYLE_BOLD ? 2 : 1); i++ )
    {
        this->drawBitmap1(this->m_cursorX + i, this->m_cursorY, char_info.width, char_info.height, char_info.glyph);
        this->m_textMode |= CANVAS_MODE_TRANSPARENT;
    }
    this->m_textMode = mode;
    this->m_cursorX += (lcdint_t)(char_info.width + char_info.spacing);
    if ( ((this->m_textMode & CANVAS_TEXT_WRAP_LOCAL) &&
          (this->m_cursorX > ((lcdint_t)this->m_w - (lcdint_t)this->m_font->getHeader().width))) ||
         ((this->m_textMode & CANVAS_TEXT_WRAP) &&
          (this->m_cursorX > ((lcdint_t)this->m_w - (lcdint_t)this->m_font->getHeader().width))) )
    {
        this->m_cursorY += (lcdint_t)this->m_font->getHeader().height;
        this->m_cursorX = 0;
        if ( (this->m_textMode & CANVAS_TEXT_WRAP_LOCAL) &&
             (this->m_cursorY > ((lcdint_t)this->m_h - (lcdint_t)this->m_font->getHeader().height)) )
        {
            this->m_cursorY = 0;
        }
    }
    return 1;
}

template <class I> size_t NanoDisplayOps8<I>::write(uint8_t c)
{
    if ( c == '\n' )
    {
        this->m_cursorY += (lcdint_t)this->m_font->getHeader().height;
        this->m_cursorX = 0;
    }
    else if ( c == '\r' )
    {
        // skip non-printed char
    }
    else
    {
        return printChar(c);
    }
    return 1;
}

template <class I> void NanoDisplayOps8<I>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    // TODO: fontstyle not supported
    // m_fontStyle = style;
    this->m_cursorX = xpos;
    this->m_cursorY = y;
    while ( *ch )
    {
        this->write(*ch);
        ch++;
    }
}
