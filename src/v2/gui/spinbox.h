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
 * @file v2/gui/spinbox.h Integer spinbox widget
 */

#ifndef _LCDGFX_SPINBOX_H_
#define _LCDGFX_SPINBOX_H_

#include "nano_gfx_types.h"
#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/font.h"
#include <stdio.h>

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * Class implements an integer spinbox widget for lcdgfx library.
 *
 * Up/down keys change the value by a configurable step. Optional wrap mode
 * causes overflow above the max to roll over to the min and vice-versa.
 * Otherwise the value is clamped at the bounds.
 */
class LcdGfxSpinbox
{
public:
    /**
     * Creates a spinbox widget.
     *
     * @param rect screen area for the spinbox
     * @param minValue minimum value (inclusive)
     * @param maxValue maximum value (inclusive)
     * @param value initial value (clamped into [min, max])
     * @param step increment / decrement size (must be >= 1)
     * @param wrap if true, up/down wrap around the bounds; otherwise they clamp
     */
    LcdGfxSpinbox(const NanoRect &rect, int16_t minValue, int16_t maxValue, int16_t value, int16_t step = 1,
                  bool wrap = false);

    /**
     * Renders the spinbox on the display: a border with a centered numeric
     * value and "<" ">" arrows on the sides.
     */
    template <typename D> void show(D &d)
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", (int)m_value);
        lcduint_t fontH = d.getFont().getHeader().height;
        lcduint_t arrowW = d.getFont().getTextSize("<");
        lcduint_t textW = d.getFont().getTextSize(buf);
        lcdint_t innerLeft = m_rect.p1.x + 2 + (lcdint_t)arrowW + 2;
        lcdint_t innerRight = m_rect.p2.x - 2 - (lcdint_t)arrowW - 2;
        lcdint_t cy = m_rect.p1.y + (m_rect.height() - (lcdint_t)fontH) / 2;
        lcdint_t cx = innerLeft + ((innerRight - innerLeft) - (lcdint_t)textW) / 2;

        // Clear only the central text band so a shorter number doesn't
        // leave digits behind. Then redraw border + arrows + value.
        uint16_t color = d.getColor();
        d.setColor(0x0000);
        d.fillRect(innerLeft, m_rect.p1.y + 1, innerRight, m_rect.p2.y - 1);
        d.setColor(color);
        d.drawRect(m_rect);
        d.printFixed(m_rect.p1.x + 2, cy, "<");
        d.printFixed(m_rect.p2.x - 2 - (lcdint_t)d.getFont().getTextSize(">"), cy, ">");
        d.printFixed(cx, cy, buf);
    }

    /**
     * Increments the value (wraps or clamps depending on configuration).
     */
    void up();

    /**
     * Decrements the value (wraps or clamps depending on configuration).
     */
    void down();

    /**
     * Sets the current value, clamped into [min, max].
     */
    void setValue(int16_t value);

    /**
     * Returns the current value.
     */
    int16_t value() const
    {
        return m_value;
    }

    int16_t minValue() const
    {
        return m_min;
    }

    int16_t maxValue() const
    {
        return m_max;
    }

    bool wraps() const
    {
        return m_wrap;
    }

    const NanoRect &getRect() const
    {
        return m_rect;
    }

private:
    NanoRect m_rect;
    int16_t m_min;
    int16_t m_max;
    int16_t m_value;
    int16_t m_step;
    bool m_wrap;
};

/**
 * @}
 */

#endif
