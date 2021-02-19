/*
    MIT License

    Copyright (c) 2016-2021, Alexey Dynda

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

///////////////////////////////////////////////////////////////////////////////
////// GENERIC FUNCTIONS APPLICABLE FOR ALL DISPLAY TYPES /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "lcd_hal/io.h"

/////////////////////////////////////////////////////////////////////////////////
//
//                             1-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template <class I> void NanoDisplayOps1<I>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    uint8_t i, j = 0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    this->m_intf.startBlock(xpos, y, this->m_w - xpos);
    for ( ;; )
    {
        uint8_t ldata;
        if ( (x > this->m_w - this->m_font->getHeader().width) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if ( y >= (lcdint_t)(this->m_h >> 3) )
            {
                break;
            }
            page_offset++;
            if ( page_offset == this->m_font->getPages() )
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
            this->m_intf.endBlock();
            this->m_intf.startBlock(xpos, y, this->m_w - xpos);
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
        x += char_info.width + char_info.spacing;
        if ( char_info.height > page_offset * 8 )
        {
            char_info.glyph += page_offset * char_info.width;
            for ( i = char_info.width; i > 0; i-- )
            {
                uint8_t data;
                if ( style == STYLE_NORMAL )
                {
                    data = pgm_read_byte(&char_info.glyph[0]);
                }
                else if ( style == STYLE_BOLD )
                {
                    uint8_t temp = pgm_read_byte(&char_info.glyph[0]);
                    data = temp | ldata;
                    ldata = temp;
                }
                else
                {
                    uint8_t temp = pgm_read_byte(&char_info.glyph[1]);
                    data = (temp & 0xF0) | ldata;
                    ldata = (temp & 0x0F);
                }
                this->m_intf.send(data ^ this->m_bgColor);
                char_info.glyph++;
            }
        }
        else
        {
            char_info.spacing += char_info.width;
        }
        for ( i = 0; i < char_info.spacing; i++ )
            this->m_intf.send(this->m_bgColor);
    }
    this->m_intf.endBlock();
}

template <class I> uint8_t NanoDisplayOps1<I>::printChar(uint8_t c)
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

template <class I> size_t NanoDisplayOps1<I>::write(uint8_t c)
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

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class I>
void NanoDisplayOps1<I>::printFixed_oldStyle(uint8_t xpos, uint8_t y, const char *ch, EFontStyle style)
{
    uint8_t i, j = 0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    this->m_intf.startBlock(xpos, y, this->m_w - xpos);
    for ( ;; )
    {
        uint8_t c;
        uint8_t ldata;
        uint16_t offset;
        if ( (x > this->m_w - this->m_font->getHeader().width) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if ( y >= (this->m_h >> 3) )
            {
                break;
            }
            page_offset++;
            if ( page_offset == this->m_font->getPages() )
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
            this->m_intf.endBlock();
            this->m_intf.startBlock(xpos, y, this->m_w - xpos);
        }
        c = ch[j];
        if ( c >= this->m_font->getHeader().ascii_offset )
        {
            c -= this->m_font->getHeader().ascii_offset;
        }
        ldata = 0;
        offset = (c * this->m_font->getPages() + page_offset) * this->m_font->getHeader().width;
        for ( i = this->m_font->getHeader().width; i > 0; i-- )
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = pgm_read_byte(&this->m_font->getPrimaryTable()[offset]);
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = pgm_read_byte(&this->m_font->getPrimaryTable()[offset]);
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = pgm_read_byte(&this->m_font->getPrimaryTable()[offset + 1]);
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            this->m_intf.send(data ^ this->m_bgColor);
            offset++;
        }
        x += this->m_font->getHeader().width;
        j++;
    }
    this->m_intf.endBlock();
}
#endif

template <class I>
void NanoDisplayOps1<I>::printFixedN(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style, uint8_t factor)
{
    uint8_t i, j = 0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    this->m_intf.startBlock(xpos, y, this->m_w - xpos);
    for ( ;; )
    {
        uint8_t ldata;
        if ( (x > this->m_w - (this->m_font->getHeader().width << factor)) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if ( y >= (this->m_h >> 3) )
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
            this->m_intf.endBlock();
            this->m_intf.startBlock(xpos, y, this->m_w - xpos);
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
        x += ((char_info.width + char_info.spacing) << factor);
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
                for ( uint8_t z = (1 << factor); z > 0; z-- )
                {
                    this->m_intf.send(data ^ this->m_bgColor);
                }
                char_info.glyph++;
            }
        }
        else
        {
            char_info.spacing += char_info.width;
        }
        for ( i = 0; i < (char_info.spacing << factor); i++ )
            this->m_intf.send(this->m_bgColor);
    }
    this->m_intf.stop();
}

template <class I> void NanoDisplayOps1<I>::putPixel(lcdint_t x, lcdint_t y)
{
    this->m_intf.startBlock(x, y >> 3, 1);
    this->m_intf.send((1 << (y & 0x07)) ^ this->m_bgColor);
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps1<I>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    this->m_intf.startBlock(x1, y1 >> 3, x2 - x1 + 1);
    for ( uint8_t x = x1; x <= x2; x++ )
    {
        this->m_intf.send((1 << (y1 & 0x07)) ^ (~this->m_color));
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps1<I>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    uint8_t topPage = y1 >> 3;
    uint8_t bottomPage = y2 >> 3;
    uint8_t height = y2 - y1;
    uint8_t y;
    this->m_intf.startBlock(x1, topPage, 1);
    if ( topPage == bottomPage )
    {
        this->m_intf.send(((0xFF >> (0x07 - height)) << (y1 & 0x07)) ^ (~this->m_color));
        this->m_intf.endBlock();
        return;
    }
    this->m_intf.send((0xFF << (y1 & 0x07)) ^ (~this->m_color));
    for ( y = (topPage + 1); y <= (bottomPage - 1); y++ )
    {
        this->m_intf.nextBlock();
        this->m_intf.send(0xFF ^ (~this->m_color));
    }
    this->m_intf.nextBlock();
    this->m_intf.send((0xFF >> (0x07 - (y2 & 0x07))) ^ (~this->m_color));
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps1<I>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    uint8_t templ = this->m_color;
    if ( x1 > x2 )
        return;
    if ( y1 > y2 )
        return;
    if ( (lcduint_t)x2 >= this->m_w )
        x2 = (lcdint_t)this->m_w - 1;
    if ( (lcduint_t)y2 >= this->m_h )
        y2 = (lcdint_t)this->m_h - 1;
    uint8_t bank1 = (y1 >> 3);
    uint8_t bank2 = (y2 >> 3);
    this->m_intf.startBlock(x1, bank1, x2 - x1 + 1);
    for ( uint8_t bank = bank1; bank <= bank2; bank++ )
    {
        uint8_t mask = 0xFF;
        if ( bank1 == bank2 )
        {
            mask = (mask >> ((y1 & 7) + 7 - (y2 & 7))) << (y1 & 7);
        }
        else if ( bank1 == bank )
        {
            mask = (mask << (y1 & 7));
        }
        else if ( bank2 == bank )
        {
            mask = (mask >> (7 - (y2 & 7)));
        }
        for ( uint8_t x = x1; x <= x2; x++ )
        {
            //            m_bytes[BADDR(bank) + x] &= ~mask;
            //            m_bytes[BADDR(bank) + x] |= (templ & mask);
            this->m_intf.send(templ & mask);
        }
        this->m_intf.nextBlock();
    }
    this->m_intf.endBlock();
}

template <class I> void NanoDisplayOps1<I>::clear()
{
    this->fill(0);
    /*    this->m_intf.startBlock(0, 0, 0);
        for(uint8_t m=(this->m_h >> 3); m>0; m--)
        {
            for(uint8_t n=this->m_w; n>0; n--)
            {
                this->m_intf.send( this->m_bgColor );
            }
            this->m_intf.nextBlock();
        }
        this->m_intf.endBlock(); */
}

template <class I>
void NanoDisplayOps1<I>::drawXBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t i, j;
    lcduint_t pitch = (w + 7) >> 3;
    this->m_intf.startBlock(x, y, w);
    for ( j = (h >> 3); j > 0; j-- )
    {
        uint8_t bit = 0;
        for ( i = w; i > 0; i-- )
        {
            uint8_t data = 0;
            for ( uint8_t k = 0; k < 8; k++ )
            {
                data |= (((pgm_read_byte(&bitmap[k * pitch]) >> bit) & 0x01) << k);
            }
            this->m_intf.send(this->m_bgColor ^ data);
            bit++;
            if ( bit >= 8 )
            {
                bitmap++;
                bit = 0;
            }
        }
        if ( bit )
        {
            bitmap++;
        }
        bitmap += pitch * 7;
        this->m_intf.nextBlock();
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps1<I>::drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    lcduint_t origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    lcduint_t i, j;
    if ( y + (lcdint_t)h <= 0 )
        return;
    if ( y >= (lcdint_t)this->m_h )
        return;
    if ( x + (lcdint_t)w <= 0 )
        return;
    if ( x >= (lcdint_t)this->m_w )
        return;
    if ( y < 0 )
    {
        bitmap += ((lcduint_t)((-y) + 7) >> 3) * w;
        h += y;
        y = 0;
        complexFlag = 1;
    }
    if ( x < 0 )
    {
        bitmap += -x;
        w += x;
        x = 0;
    }
    max_pages = (lcduint_t)(h + 15 - offset) >> 3;
    if ( (lcduint_t)((lcduint_t)y + h) > (lcduint_t)this->m_h )
    {
        h = (lcduint_t)(this->m_h - (lcduint_t)y);
    }
    if ( (lcduint_t)((lcduint_t)x + w) > (lcduint_t)this->m_w )
    {
        w = (lcduint_t)(this->m_w - (lcduint_t)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    uint8_t color = this->m_color ? 0xFF : 0x00;
    this->m_intf.startBlock(x, y >> 3, w);
    for ( j = 0; j < pages; j++ )
    {
        if ( j == (lcduint_t)(max_pages - 1) )
            mainFlag = !offset;
        for ( i = w; i > 0; i-- )
        {
            uint8_t data = 0;
            if ( mainFlag )
                data |= ((pgm_read_byte(bitmap) << offset) & color);
            if ( complexFlag )
                data |= ((pgm_read_byte(bitmap - origin_width) >> (8 - offset)) & color);
            bitmap++;
            this->m_intf.send(this->m_bgColor ^ data);
        }
        bitmap += origin_width - w;
        complexFlag = offset;
        this->m_intf.nextBlock();
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps1<I>::gfx_drawMonoBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    lcduint_t origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    lcduint_t i, j;
    if ( y + (lcdint_t)h <= 0 )
        return;
    if ( y >= (lcdint_t)this->m_h )
        return;
    if ( x + (lcdint_t)w <= 0 )
        return;
    if ( x >= (lcdint_t)this->m_w )
        return;
    if ( y < 0 )
    {
        buf += ((lcduint_t)((-y) + 7) >> 3) * w;
        h += y;
        y = 0;
        complexFlag = 1;
    }
    if ( x < 0 )
    {
        buf += -x;
        w += x;
        x = 0;
    }
    max_pages = (lcduint_t)(h + 15 - offset) >> 3;
    if ( (lcduint_t)((lcduint_t)y + h) > (lcduint_t)this->m_h )
    {
        h = (lcduint_t)(this->m_h - (lcduint_t)y);
    }
    if ( (lcduint_t)((lcduint_t)x + w) > (lcduint_t)this->m_w )
    {
        w = (lcduint_t)(this->m_w - (lcduint_t)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    uint8_t color = this->m_color ? 0xFF : 0x00;
    this->m_intf.startBlock(x, y >> 3, w);
    for ( j = 0; j < pages; j++ )
    {
        if ( j == (lcduint_t)(max_pages - 1) )
            mainFlag = !offset;
        for ( i = w; i > 0; i-- )
        {
            uint8_t data = 0;
            if ( mainFlag )
                data |= ((pgm_read_byte(buf) << offset) & color);
            if ( complexFlag )
                data |= ((pgm_read_byte(buf - origin_width) >> (8 - offset)) & color);
            buf++;
            this->m_intf.send(this->m_bgColor ^ data);
        }
        buf += origin_width - w;
        complexFlag = offset;
        this->m_intf.nextBlock();
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps1<I>::drawBitmap4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps1<I>::drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps1<I>::drawBitmap16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps1<I>::drawBuffer1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    lcduint_t origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    lcduint_t i, j;
    if ( y + (lcdint_t)h <= 0 )
        return;
    if ( y >= (lcdint_t)this->m_h )
        return;
    if ( x + (lcdint_t)w <= 0 )
        return;
    if ( x >= (lcdint_t)this->m_w )
        return;
    if ( y < 0 )
    {
        buffer += ((lcduint_t)((-y) + 7) >> 3) * w;
        h += y;
        y = 0;
        complexFlag = 1;
    }
    if ( x < 0 )
    {
        buffer += -x;
        w += x;
        x = 0;
    }
    max_pages = (lcduint_t)(h + 15 - offset) >> 3;
    if ( (lcduint_t)((lcduint_t)y + h) > (lcduint_t)this->m_h )
    {
        h = (lcduint_t)(this->m_h - (lcduint_t)y);
    }
    if ( (lcduint_t)((lcduint_t)x + w) > (lcduint_t)this->m_w )
    {
        w = (lcduint_t)(this->m_w - (lcduint_t)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    this->m_intf.startBlock(x, y >> 3, w);
    for ( j = 0; j < pages; j++ )
    {
        if ( j == (lcduint_t)(max_pages - 1) )
            mainFlag = !offset;
        for ( i = w; i > 0; i-- )
        {
            uint8_t data = 0;
            if ( mainFlag )
                data |= ((*buffer << offset) & this->m_color);
            if ( complexFlag )
                data |= ((*(buffer - origin_width) >> (8 - offset)) & this->m_color);
            buffer++;
            this->m_intf.send(this->m_bgColor ^ data);
        }
        buffer += origin_width - w;
        complexFlag = offset;
        this->m_intf.nextBlock();
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps1<I>::drawBuffer1Fast(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    uint8_t j;
    this->m_intf.startBlock(x, y >> 3, w);
    for ( j = (h >> 3); j > 0; j-- )
    {
        this->m_intf.sendBuffer(buf, w);
        buf += w;
        this->m_intf.nextBlock();
    }
    this->m_intf.endBlock();
}

template <class I>
void NanoDisplayOps1<I>::drawBuffer4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps1<I>::drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <class I>
void NanoDisplayOps1<I>::drawBuffer16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}

template <class I> void NanoDisplayOps1<I>::fill(uint16_t color)
{
    color ^= this->m_bgColor;
    this->m_intf.startBlock(0, 0, 0);
    for ( lcduint_t m = (this->m_h >> 3); m > 0; m-- )
    {
        for ( lcduint_t n = this->m_w; n > 0; n-- )
        {
            this->m_intf.send(color);
        }
        this->m_intf.nextBlock();
    }
    this->m_intf.endBlock();
}
