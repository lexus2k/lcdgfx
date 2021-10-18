/*
    MIT License

    Copyright (c) 2019-2021, Alexey Dynda

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

#include "font.h"
#include "canvas/internal/canvas_types_int.h"

enum
{
    SSD1306_OLD_FIXED_FORMAT = 0x00,
    SSD1306_NEW_FIXED_FORMAT = 0x01,
    SSD1306_NEW_FORMAT = 0x02,
    SSD1306_SQUIX_FORMAT = 0x03,
};

#ifdef CONFIG_SSD1306_UNICODE_ENABLE
uint8_t g_ssd1306_unicode2 = 1;
#endif
NanoFont g_canvas_font;

static const uint8_t *ssd1306_readUnicodeRecord(SUnicodeBlockRecord *r, const uint8_t *p)
{
    r->start_code = ((static_cast<uint16_t>(pgm_read_byte(&p[0])) << 8)) | static_cast<uint16_t>(pgm_read_byte(&p[1]));
    r->count = pgm_read_byte(&p[2]);
    return (r->count > 0) ? (&p[3]) : nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// OLD FORMAT: 1.7.6 and below
/// OLD FORMAT is supported by old and latest versions of ssd1306 library

static const uint8_t *ssd1306_getCharGlyph(const SFixedFontInfo &font, char ch)
{
    return &font.primary_table[(ch - font.h.ascii_offset) * font.glyph_size +
                               (font.h.type == 0x01 ? sizeof(SUnicodeBlockRecord) : 0)];
}

#ifdef CONFIG_SSD1306_UNICODE_ENABLE
static const uint8_t *ssd1306_searchCharGlyph(const SFixedFontInfo &font, const uint8_t *unicode_table,
                                              uint16_t unicode)
{
    SUnicodeBlockRecord r;
    const uint8_t *data = unicode_table;
    // looking for required unicode table
    while ( 1 )
    {
        ssd1306_readUnicodeRecord(&r, data);
        if ( r.count == 0 )
        {
            break;
        }
        data += sizeof(SUnicodeBlockRecord);
        if ( (unicode >= r.start_code) && (unicode < (r.start_code + r.count)) )
        {
            break;
        }
        data += r.count * font.glyph_size;
    }
    if ( r.count == 0 )
    {
        // Sorry, no glyph found for the specified character
        return NULL;
    }
    return &data[(unicode - r.start_code) * font.glyph_size];
}
#endif

static const uint8_t *ssd1306_getU16CharGlyph(const SFixedFontInfo &font, uint16_t unicode)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    if ( g_ssd1306_unicode2 )
    {
        const uint8_t *glyph = NULL;
        if ( (unicode < 128) && (font.h.type == 0x00) && (font.primary_table != NULL) )
        {
            return ssd1306_getCharGlyph(font, unicode);
        }
        if ( font.primary_table )
        {
            glyph = ssd1306_searchCharGlyph(font, font.primary_table, unicode);
        }
        if ( !glyph && font.secondary_table )
        {
            glyph = ssd1306_searchCharGlyph(font, font.secondary_table, unicode);
        }
        if ( !glyph )
        {
            return ssd1306_getCharGlyph(font, font.h.ascii_offset);
        }
        return glyph;
    }
    else
#endif
    {
        return ssd1306_getCharGlyph(font, unicode);
    }
}

static void _ssd1306_oldFormatGetBitmap(const SFixedFontInfo &font, uint16_t unicode, SCharInfo *info)
{
    if ( info )
    {
        info->width = font.h.width;
        info->height = font.h.height;
        info->spacing = font.spacing;
        info->glyph = ssd1306_getU16CharGlyph(font, unicode);
    }
}

void NanoFont::loadFixedFont(const uint8_t *progmemFont)
{
    m_fixedFont.h.type = pgm_read_byte(&progmemFont[0]);
    m_fixedFont.h.width = pgm_read_byte(&progmemFont[1]);
    m_fixedFont.h.height = pgm_read_byte(&progmemFont[2]);
    m_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[3]);
    m_fixedFont.primary_table = progmemFont + 4;
    m_getCharBitmap = _ssd1306_oldFormatGetBitmap;
    m_fixedFont.pages = (m_fixedFont.h.height + 7) >> 3;
    m_fixedFont.glyph_size = m_fixedFont.pages * m_fixedFont.h.width;
    m_fixedFont.spacing = 0;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    m_fixedFont.secondary_table = NULL;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// NEW FORMAT: 1.7.8 and later
/// NEW FORMAT is supported only by latest versions of ssd1306 library

static void _ssd1306_newFormatGetBitmap(const SFixedFontInfo &font, uint16_t unicode, SCharInfo *info)
{
    if ( info )
    {
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
        uint8_t table_index = 0;
#endif
        const uint8_t *data = font.primary_table;
        while ( data )
        {
            SUnicodeBlockRecord r;
            data = ssd1306_readUnicodeRecord(&r, data);
            if ( !data )
            {
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
                if ( table_index == 0 )
                {
                    table_index++;
                    data = font.secondary_table;
                    continue;
                }
#endif
                break;
            }
            /* Check that unicode in the section being processed */
            if ( (unicode < r.start_code) || (unicode >= (r.start_code + r.count)) )
            {
                // skip jump table
                data += static_cast<uint16_t>(r.count) * 4;
                // skip block bitmap data
                uint16_t offset = ((static_cast<uint16_t>(pgm_read_byte(&data[0])) << 8) |
                                   static_cast<uint16_t>(pgm_read_byte(&data[1]))) +
                                  2;
                //                printf("JMP OFFSET: %d\n", offset);
                data += offset;
                //                printf("JMP DONE: %02X %02X %02X\n", data[0], data[1], data[2]);
                continue;
            }
            /* At this point data points to jump table (offset|offset|bytes|width) */
            unicode -= r.start_code;
            data += unicode * 4;
            uint16_t offset = (pgm_read_byte(&data[0]) << 8) | (pgm_read_byte(&data[1]));
            uint8_t glyph_width = pgm_read_byte(&data[2]);
            uint8_t glyph_height = pgm_read_byte(&data[3]);
            info->width = glyph_width;
            info->height = glyph_height;
            info->spacing = glyph_width ? font.spacing : (font.h.width >> 1);
            info->glyph = data + (r.count - unicode) * 4 + 2 + offset;
            break;
        }
        if ( !info->glyph )
        {
            info->width = 0;
            info->height = 0;
            info->spacing = font.h.width >> 1;
            info->glyph = font.primary_table;
        }
    }
}

void NanoFont::loadFreeFont(const uint8_t *progmemFont)
{
    m_fixedFont.h.type = pgm_read_byte(&progmemFont[0]);
    m_fixedFont.h.width = pgm_read_byte(&progmemFont[1]);
    m_fixedFont.h.height = pgm_read_byte(&progmemFont[2]);
    m_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[3]);
    m_fixedFont.primary_table = progmemFont + 4;
    m_getCharBitmap = _ssd1306_newFormatGetBitmap;
    m_fixedFont.spacing = 1;
    m_fixedFont.pages = (m_fixedFont.h.height + 7) >> 3;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    m_fixedFont.secondary_table = NULL;
#endif
}

void NanoFont::loadSecondaryFont(const uint8_t *progmemUnicode)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    m_fixedFont.secondary_table = progmemUnicode;
    if ( m_fixedFont.secondary_table != NULL )
    {
        m_fixedFont.secondary_table += sizeof(SFontHeaderRecord);
    }
#endif
}

void NanoFont::loadFixedFont_oldStyle(const uint8_t *progmemFont)
{
    m_fixedFont.h.type = pgm_read_byte(&progmemFont[0]);
    m_fixedFont.h.width = pgm_read_byte(&progmemFont[1]);
    m_fixedFont.h.height = pgm_read_byte(&progmemFont[2]);
    m_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[3]);
    m_fixedFont.primary_table = progmemFont + 4;
    m_fixedFont.pages = (m_fixedFont.h.height + 7) >> 3;
    m_fixedFont.glyph_size = m_fixedFont.pages * m_fixedFont.h.width;
    m_fixedFont.spacing = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// SQUIX FORMAT: 1.7.8 and later
/// SQUIX FORMAT is not fully supported. Use it at your own risk

static void _ssd1306_squixFormatGetBitmap(const SFixedFontInfo &font, uint16_t unicode, SCharInfo *info)
{
    if ( info )
    {
        const uint8_t *data = font.primary_table;
        /* Check that unicode in the section being processed */
        if ( !data || (unicode < font.h.ascii_offset) || (unicode >= (font.h.ascii_offset + font.count)) )
        {
            info->width = 0;
            info->height = 0;
            info->spacing = font.h.width >> 1;
            info->glyph = font.primary_table;
            return;
        }
        /* At this point data points to jump table (offset|offset|bytes|width) */
        const uint8_t *bitmap_data = data + (uint16_t)font.count * 4;
        unicode -= font.h.ascii_offset;
        data += (unicode * 4);
        uint16_t offset = (pgm_read_byte(&data[0]) << 8) | pgm_read_byte(&data[1]);
        uint8_t glyph_bytes = pgm_read_byte(&data[2]);
        //        uint8_t width = pgm_read_byte(&data[3]);
        info->width = glyph_bytes; //(glyph_bytes + font.pages - 1)  / font.pages;
        info->height = font.h.height / 2;
        info->spacing = font.spacing;
        //        uint8_t index=0;
        info->glyph = bitmap_data;
        if ( offset != 0xFFFF )
        {
            info->glyph += offset;
        }
    }
}

void NanoFont::loadSquixFont(const uint8_t *progmemFont)
{
    m_fixedFont.h.type = SSD1306_SQUIX_FORMAT;
    m_fixedFont.h.width = pgm_read_byte(&progmemFont[0]);
    m_fixedFont.h.height = pgm_read_byte(&progmemFont[1]);
    m_fixedFont.h.ascii_offset = pgm_read_byte(&progmemFont[2]);
    m_fixedFont.count = pgm_read_byte(&progmemFont[3]);
    m_fixedFont.primary_table = progmemFont + 4;
    m_getCharBitmap = _ssd1306_squixFormatGetBitmap;
    m_fixedFont.pages = (m_fixedFont.h.height + 7) >> 3;
    m_fixedFont.glyph_size = m_fixedFont.pages * m_fixedFont.h.width;
    m_fixedFont.spacing = 1;
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    m_fixedFont.secondary_table = NULL;
#endif
}

void NanoFont::getCharBitmap(uint16_t ch, SCharInfo *info)
{
    m_getCharBitmap(m_fixedFont, ch, info);
}

lcduint_t NanoFont::getTextSize(const char *text, lcduint_t *height)
{
    lcduint_t width = 0;
    while ( *text )
    {
        if ( *text == '\r' || *text == '\n' )
        {
            break;
        }
        uint16_t unicode = unicode16FromUtf8(*text);
        if ( unicode != SSD1306_MORE_CHARS_REQUIRED )
        {
            SCharInfo char_info;
            getCharBitmap(unicode, &char_info);
            width += char_info.width + char_info.spacing;
            if ( height )
                *height = char_info.height;
        }
        text++;
    }
    return width;
}

uint16_t NanoFont::unicode16FromUtf8(uint8_t ch)
{
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    static uint16_t unicode = 0;
    ch &= 0x00FF;
    if ( !unicode )
    {
        if ( ch >= 0xc0 )
        {
            unicode = ch;
            return SSD1306_MORE_CHARS_REQUIRED;
        }
        return ch;
    }
    uint16_t code = ((unicode & 0x1f) << 6) | (ch & 0x3f);
    unicode = 0;
    return code;
#else
    return ch;
#endif
}
