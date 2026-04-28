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
/**
 * @file v2/gui/text_entry.h Single-line text-entry widget definition.
 */

#ifndef _LCDGFX_TEXT_ENTRY_H_
#define _LCDGFX_TEXT_ENTRY_H_

#include "nano_gfx_types.h"
#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/font.h"

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * Single-line text-entry widget.
 *
 * Typical embedded UX where the user moves a cursor along a fixed
 * buffer and cycles the character at the cursor through a charset
 * with up/down. The widget operates on a single-byte charset
 * (ASCII by default); each cell holds exactly one byte. UTF-8
 * strings can still be displayed if the active font's secondary
 * table covers the codepoints, but each cell is one byte — the
 * charset itself must therefore be single-byte. The default
 * charset is uppercase A-Z, digits and a few punctuation marks
 * plus a leading space (used as the "blank" character).
 *
 * Key bindings:
 *   - up()    : cycle char at cursor forward through charset
 *   - down()  : cycle char at cursor backward
 *   - left()  : move cursor one cell left (clamped at 0)
 *   - right() : move cursor one cell right (clamped at maxLen-1)
 */
class LcdGfxTextEntry
{
public:
    /**
     * Creates a text-entry widget bound to the user's buffer.
     *
     * @param buffer  user-owned NUL-terminated buffer to edit. Must
     *                have at least @p maxLen + 1 bytes available.
     *                On construction, every byte up to and including
     *                @p maxLen is initialised to charset[0] (the
     *                first character of the charset, which acts as
     *                the "blank" placeholder), then a NUL is written
     *                at offset @p maxLen.
     * @param maxLen  maximum number of editable cells (excludes the
     *                trailing NUL).
     * @param rect    screen area for the widget. Pass an empty rect
     *                to have show() auto-size from the active font.
     * @param charset NUL-terminated single-byte charset. The first
     *                character is treated as the blank/space.
     *                Defaults to space + A-Z + 0-9 + a small
     *                punctuation block.
     */
    LcdGfxTextEntry(char *buffer, uint8_t maxLen, const NanoRect &rect,
                    const char *charset = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-_");

    /**
     * Cycles the character at the cursor forward through the charset.
     */
    void up();

    /**
     * Cycles the character at the cursor backward through the charset.
     */
    void down();

    /**
     * Moves the cursor one cell to the left, clamped at 0.
     */
    void left();

    /**
     * Moves the cursor one cell to the right, clamped at maxLen - 1.
     */
    void right();

    /**
     * Sets the cursor position. Clamped to [0, maxLen - 1].
     */
    void setCursor(uint8_t cursor);

    /**
     * Returns the current cursor position.
     */
    uint8_t getCursor() const
    {
        return m_cursor;
    }

    /**
     * Returns the underlying NUL-terminated text buffer.
     */
    const char *getText() const
    {
        return m_buffer;
    }

    /**
     * Returns the maximum number of editable cells.
     */
    uint8_t getMaxLen() const
    {
        return m_maxLen;
    }

    /**
     * Renders the widget to the display.
     *
     * Draws the buffer left-to-right inside the widget rectangle and
     * underlines the cursor cell so the user can see which cell is
     * being edited.
     *
     * @param d display object
     */
    template <typename D> void show(D &d)
    {
        updateSize(d);
        d.drawRect(m_rect);
        const lcduint_t fontH = d.getFont().getHeader().height;
        const lcduint_t fontW = d.getFont().getHeader().width;
        const lcdint_t baseX = m_rect.p1.x + 2;
        const lcdint_t baseY = m_rect.p1.y + (m_rect.height() - fontH) / 2;
        d.printFixed(baseX, baseY, m_buffer);
        const lcdint_t cursorX = baseX + (lcdint_t)m_cursor * (lcdint_t)fontW;
        const lcdint_t cursorY = baseY + (lcdint_t)fontH;
        d.drawHLine(cursorX, cursorY, cursorX + (lcdint_t)fontW - 2);
    }

    /**
     * Auto-sizes the widget rectangle from the active font if a zero
     * rect was supplied to the constructor.
     */
    template <typename D> void updateSize(D &d)
    {
        if ( !m_rect.p2.x )
        {
            const lcduint_t fontH = d.getFont().getHeader().height;
            const lcduint_t fontW = d.getFont().getHeader().width;
            m_rect.p2.x = m_rect.p1.x + (lcdint_t)fontW * (lcdint_t)m_maxLen + 4 - 1;
            m_rect.p2.y = m_rect.p1.y + (lcdint_t)fontH + 4 - 1;
        }
    }

    /**
     * Returns the rectangle occupied by the widget.
     */
    const NanoRect &getRect() const
    {
        return m_rect;
    }

private:
    char *m_buffer;
    const char *m_charset;
    uint8_t m_charsetLen;
    uint8_t m_maxLen;
    uint8_t m_cursor = 0;
    NanoRect m_rect;

    uint8_t indexOfCurrentChar() const;
};

/**
 * @}
 */

#endif
