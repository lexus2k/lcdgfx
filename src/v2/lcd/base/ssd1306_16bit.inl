/*
    MIT License

    Copyright (c) 2018-2021, Alexey Dynda

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
void    ssd1306_setRgbColor16(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR16(r,g,b);
}

static void ssd1306_drawBufferPitch16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_lcd.set_block(x, y, w);
    while (h--)
    {
        lcduint_t line = w << 1;
        while (line--)
        {
            ssd1306_intf.send( *data );
            data++;
        }
        data += pitch - (w << 1);
    }
    ssd1306_intf.stop();
}

void ssd1306_drawBufferFast16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    ssd1306_drawBufferPitch16(x, y, w, h, w<<1, data);
}

void ssd1306_drawBufferEx16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_drawBufferPitch16( x, y, w, h, pitch, data );
}

void ssd1306_putColorPixel16(lcdint_t x, lcdint_t y, uint16_t color)
{
    ssd1306_lcd.set_block(x, y, 0);
    ssd1306_lcd.send_pixels16( color );
    ssd1306_intf.stop();
}

void ssd1306_drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    ssd1306_lcd.set_block(xpos, ypos, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        ssd1306_lcd.send_pixels16( (pgm_read_byte( &bitmap[0] ) << 8) | pgm_read_byte( &bitmap[1] ) );
        bitmap += 2;
    }
    ssd1306_intf.stop();
}

void ssd1306_clearBlock16(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    ssd1306_lcd.set_block(x, y, w);
    uint32_t count = w * h;
    while (count--)
    {
        ssd1306_lcd.send_pixels16( 0x0000 );
    }
    ssd1306_intf.stop();
}

void ssd1306_setCursor16(lcduint_t x, lcduint_t y)
{
    ssd1306_cursorX = x;
    ssd1306_cursorY = y;
}

void ssd1306_printChar16(uint8_t c)
{
    uint16_t unicode = ssd1306_unicode16FromUtf8(c);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return;
    SCharInfo char_info;
    ssd1306_getCharBitmap(unicode, &char_info);
    ssd1306_drawMonoBitmap16(ssd1306_cursorX,
                ssd1306_cursorY,
                char_info.width,
                char_info.height,
                char_info.glyph );
}

size_t ssd1306_write16(uint8_t ch)
{
    if (ch == '\r')
    {
        ssd1306_cursorX = 0;
        return 0;
    }
    else if ( (ssd1306_cursorX > ssd1306_lcd.width - m_font->getHeader().width) || (ch == '\n') )
    {
        ssd1306_cursorX = 0;
        ssd1306_cursorY += m_font->getHeader().height;
        if ( ssd1306_cursorY > ssd1306_lcd.height - m_font->getHeader().height )
        {
            ssd1306_cursorY = 0;
        }
        ssd1306_clearBlock16(0, ssd1306_cursorY, ssd1306_lcd.width, m_font->getHeader().height);
        if (ch == '\n')
        {
            return 0;
        }
    }
    uint16_t unicode = m_font->unicode16FromUtf8(ch);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return 0;
    SCharInfo char_info;
    m_font->getCharBitmap(unicode, &char_info);
    ssd1306_drawMonoBitmap16( ssd1306_cursorX,
                              ssd1306_cursorY,
                              char_info.width,
                              char_info.height,
                              char_info.glyph);
    ssd1306_cursorX += char_info.width + char_info.spacing;
    return 1;
}

size_t ssd1306_print16(const char ch[])
{
    size_t n = 0;
    while (*ch)
    {
        n += ssd1306_write16(*ch);
        ch++;
    }
    return n;
}

uint8_t ssd1306_printFixed16(lcdint_t x, lcdint_t y, const char *ch, EFontStyle style)
{
    ssd1306_cursorX = x;
    ssd1306_cursorY = y;
    return ssd1306_print16(ch);
}

#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                             16-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template <class I> void NanoDisplayOps16<I>::putPixel(lcdint_t x, lcdint_t y)
{
    this->m_intf.startBlock(x, y, 0);
    this->m_intf.send(this->m_color >> 8);
    this->m_intf.send(this->m_color & 0xFF);
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps16<I>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    this->m_intf.startBlock(x1, y1, 0);
    while ( x1 < x2 )
    {
        this->m_intf.send(this->m_color >> 8);
        this->m_intf.send(this->m_color & 0xFF);
        x1++;
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps16<I>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    this->m_intf.startBlock(x1, y1, 1);
    while ( y1 <= y2 )
    {
        this->m_intf.send(this->m_color >> 8);
        this->m_intf.send(this->m_color & 0xFF);
        y1++;
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps16<I>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
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
    uint16_t count = (x2 - x1 + 1) * (y2 - y1 + 1);
    while ( count-- )
    {
        this->m_intf.send(this->m_color >> 8);
        this->m_intf.send(this->m_color & 0xFF);
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps16<I>::fill(uint16_t color)
{
    this->m_intf.startBlock(0, 0, 0);
    uint32_t count = (uint32_t)this->m_w * (uint32_t)this->m_h;
    while ( count-- )
    {
        this->m_intf.send(color >> 8);
        this->m_intf.send(color & 0xFF);
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps16<I>::clear()
{
    fill(0x00);
}

template <class I>
void NanoDisplayOps16<I>::drawXBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // TODO:
}

template <class I>
void NanoDisplayOps16<I>::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    uint16_t blackColor = this->m_bgColor;
    uint16_t color = this->m_color;
    this->m_intf.startBlock(xpos, ypos, w);
    while ( h-- )
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = pgm_read_byte(bitmap);
            if ( data & bit )
            {
                this->m_intf.send(color >> 8);
                this->m_intf.send(color & 0xFF);
            }
            else
            {
                this->m_intf.send(blackColor >> 8);
                this->m_intf.send(blackColor & 0xFF);
            }
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
void NanoDisplayOps16<I>::drawBitmap4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps16<I>::drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while ( count-- )
    {
        uint16_t color = RGB8_TO_RGB16(pgm_read_byte(bitmap));
        this->m_intf.send(color >> 8);
        this->m_intf.send(color & 0xFF);
        bitmap++;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawBitmap16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while ( count-- )
    {
        this->m_intf.send(pgm_read_byte(&bitmap[0]));
        this->m_intf.send(pgm_read_byte(&bitmap[1]));
        bitmap += 2;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawBuffer1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    uint8_t bit = 1;
    uint16_t blackColor = this->m_bgColor;
    uint16_t color = this->m_color;
    this->m_intf.startBlock(xpos, ypos, w);
    while ( h-- )
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = *buffer;
            if ( data & bit )
            {
                this->m_intf.send(color >> 8);
                this->m_intf.send(color & 0xFF);
            }
            else
            {
                this->m_intf.send(blackColor >> 8);
                this->m_intf.send(blackColor & 0xFF);
            }
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
void NanoDisplayOps16<I>::drawBuffer1Fast(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    this->drawBuffer1(x, y, w, h, buf);
}

template <class I>
void NanoDisplayOps16<I>::drawBuffer4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps16<I>::drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while ( count-- )
    {
        uint16_t color = RGB8_TO_RGB16((*buffer));
        this->m_intf.send(color >> 8);
        this->m_intf.send(color & 0xFF);
        buffer++;
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps16<I>::drawBuffer16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    this->m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while ( count-- )
    {
        this->m_intf.send(buffer[0]);
        this->m_intf.send(buffer[1]);
        buffer += 2;
    }
    this->m_intf.endBlock();
}

template <class I> uint8_t NanoDisplayOps16<I>::printChar(uint8_t c)
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
    if ( char_info.height < (lcdint_t)this->m_font->getHeader().height )
    {
        this->invertColors();
        this->fillRect(this->m_cursorX, this->m_cursorY + char_info.height, this->m_cursorX + char_info.width - 1,
                       this->m_cursorY + (lcdint_t)this->m_font->getHeader().height - 1);
        this->invertColors();
    }
    this->m_cursorX += (lcdint_t)(char_info.width + char_info.spacing);
    if ( char_info.spacing > 0 )
    {
        this->invertColors();
        this->fillRect(this->m_cursorX - char_info.spacing, this->m_cursorY, this->m_cursorX - 1,
                       this->m_cursorY + (lcdint_t)this->m_font->getHeader().height - 1);
        this->invertColors();
    }
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

template <class I> size_t NanoDisplayOps16<I>::write(uint8_t c)
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

template <class I> void NanoDisplayOps16<I>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
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

template <class I>
void NanoDisplayOps16<I>::printFixedN(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style, uint8_t factor)
{
    uint8_t i, j = 0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    for ( ;; )
    {
        uint8_t ldata;
        if ( (x > this->m_w - (this->m_font->getHeader().width << factor)) || (ch[j] == '\0') )
        {
            x = xpos;
            y += 8;
            if ( y >= this->m_h )
            {
                break;
            }
            page_offset++;
            if ( page_offset == (this->m_font->getPages() << factor) )
            {
                text_index = j;
                page_offset = 0;
                if ( ch[j] == '\0' )
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
        }
        uint16_t unicode;
        do
        {
            unicode = this->m_font->unicode16FromUtf8(ch[j]);
            j++;
        } while ( unicode == SSD1306_MORE_CHARS_REQUIRED );
        SCharInfo char_info;
        this->m_font->getCharBitmap(unicode, &char_info);
        ldata = 0;
        if ( char_info.height > (page_offset >> factor) * 8 )
        {
            char_info.glyph += (page_offset >> factor) * char_info.width;
            for ( i = char_info.width; i > 0; i-- )
            {
                uint8_t data;
                if ( style == STYLE_NORMAL )
                {
                    data = pgm_read_byte(char_info.glyph);
                }
                else if ( style == STYLE_BOLD )
                {
                    uint8_t temp = pgm_read_byte(char_info.glyph);
                    data = temp | ldata;
                    ldata = temp;
                }
                else
                {
                    uint8_t temp = pgm_read_byte(char_info.glyph + 1);
                    data = (temp & 0xF0) | ldata;
                    ldata = (temp & 0x0F);
                }
                if ( factor > 0 )
                {
                    uint8_t accum = 0;
                    uint8_t mask = ~((0xFF) << (1 << factor));
                    // N=0  ->   right shift is always 0
                    // N=1  ->   right shift goes through 0, 4
                    // N=2  ->   right shift goes through 0, 2, 4, 6
                    // N=3  ->   right shift goes through 0, 1, 2, 3, 4, 5, 6, 7
                    data >>= ((page_offset & ((1 << factor) - 1)) << (3 - factor));
                    for ( uint8_t idx = 0; idx < 1 << (3 - factor); idx++ )
                    {
                        accum |= (((data >> idx) & 0x01) ? (mask << (idx << factor)) : 0);
                    }
                    data = accum;
                }
                this->m_intf.startBlock(x, y, (1 << factor));
                for ( uint8_t b = 0; b < 8; b++ )
                {
                    uint16_t color = (data & (1 << b)) ? this->m_color : this->m_bgColor;
                    for ( uint8_t z = (1 << factor); z > 0; z-- )
                    {
                        this->m_intf.send(color >> 8);
                        this->m_intf.send(color & 0xFF);
                    }
                }
                this->m_intf.endBlock();
                x += (1 << factor);
                char_info.glyph++;
            }
        }
        else
        {
            char_info.spacing += char_info.width;
            x += (char_info.width << factor);
        }
        this->m_intf.startBlock(x, y, (char_info.spacing << factor));
        for ( uint8_t b = 0; b < 8; b++ )
        {
            uint16_t color = this->m_bgColor;
            for ( i = 0; i < (char_info.spacing << factor); i++ )
            {
                this->m_intf.send(color >> 8);
                this->m_intf.send(color & 0xFF);
            }
        }
        x += (char_info.spacing << factor);
        this->m_intf.endBlock();
    }
}
