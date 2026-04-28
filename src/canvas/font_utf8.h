/*
    MIT License

    Copyright (c) 2024, Alexey Dynda

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
/**
 * @file font_utf8.h Stateless UTF-8 decoder helpers.
 */

#ifndef _NANO_FONT_UTF8_H_
#define _NANO_FONT_UTF8_H_

#include <stdint.h>
#include <stddef.h>

/**
 * Sentinel value returned by nano_utf8_decode() when the input
 * is malformed, truncated, or contains an invalid sequence.
 */
#define NANO_UTF8_INVALID 0xFFFFFFFFu

/**
 * Decode a single UTF-8 codepoint from @p p in a stateless way.
 *
 * Unlike NanoFont::unicode16FromUtf8(), which keeps internal
 * static state and returns a 16-bit truncated codepoint, this
 * helper decodes a full 21-bit Unicode codepoint and never
 * mutates global state, so it is safe to use from re-entrant
 * contexts.
 *
 * On entry @p p must point at the first byte of a UTF-8
 * sequence. On successful return @p p is advanced past the
 * decoded sequence and the codepoint is returned. The decoder
 * stops at the NUL byte; if @p end is non-NULL the decoder
 * never reads past @p end.
 *
 * The following inputs are reported as invalid (return value
 * NANO_UTF8_INVALID, @p p advanced by one byte to allow
 * resynchronisation):
 *   - lone continuation bytes (0x80..0xBF) at the start of a
 *     sequence;
 *   - truncated multi-byte sequences (NUL or end before all
 *     trailing bytes have been seen);
 *   - bytes 0xC0, 0xC1, 0xF5..0xFF (always invalid in UTF-8);
 *   - overlong encodings (e.g. U+0000 encoded as two bytes);
 *   - codepoints in the surrogate range U+D800..U+DFFF;
 *   - codepoints above U+10FFFF.
 *
 * @param p   in/out pointer to the next byte to decode. Must
 *            not be NULL.
 * @param end optional end-of-buffer guard, may be NULL for
 *            NUL-terminated input.
 * @return    decoded codepoint, or NANO_UTF8_INVALID.
 */
static inline uint32_t nano_utf8_decode(const char **p, const char *end)
{
    const unsigned char *s = (const unsigned char *)(*p);
    if ( end != NULL && (const char *)s >= end )
    {
        return NANO_UTF8_INVALID;
    }
    unsigned char b0 = s[0];
    if ( b0 == 0 )
    {
        return NANO_UTF8_INVALID;
    }
    uint32_t cp;
    int extra;
    uint32_t min_cp;

    if ( b0 < 0x80u )
    {
        *p = (const char *)(s + 1);
        return b0;
    }
    if ( b0 < 0xC2u || b0 > 0xF4u )
    {
        // 0x80..0xBF lone continuation, 0xC0/0xC1 overlong, 0xF5..0xFF invalid.
        *p = (const char *)(s + 1);
        return NANO_UTF8_INVALID;
    }
    if ( b0 < 0xE0u )
    {
        cp = b0 & 0x1Fu;
        extra = 1;
        min_cp = 0x80u;
    }
    else if ( b0 < 0xF0u )
    {
        cp = b0 & 0x0Fu;
        extra = 2;
        min_cp = 0x800u;
    }
    else
    {
        cp = b0 & 0x07u;
        extra = 3;
        min_cp = 0x10000u;
    }
    for ( int i = 1; i <= extra; i++ )
    {
        if ( end != NULL && (const char *)(s + i) >= end )
        {
            *p = (const char *)(s + 1);
            return NANO_UTF8_INVALID;
        }
        unsigned char bi = s[i];
        if ( bi == 0 || (bi & 0xC0u) != 0x80u )
        {
            *p = (const char *)(s + 1);
            return NANO_UTF8_INVALID;
        }
        cp = (cp << 6) | (bi & 0x3Fu);
    }
    if ( cp < min_cp || (cp >= 0xD800u && cp <= 0xDFFFu) || cp > 0x10FFFFu )
    {
        *p = (const char *)(s + 1);
        return NANO_UTF8_INVALID;
    }
    *p = (const char *)(s + extra + 1);
    return cp;
}

#endif
