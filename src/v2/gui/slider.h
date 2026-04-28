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
 * @file v2/gui/slider.h Slider widget
 */

#ifndef _LCDGFX_SLIDER_H_
#define _LCDGFX_SLIDER_H_

#include "nano_gfx_types.h"
#include "canvas/point.h"
#include "canvas/rect.h"

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * Orientation of the slider track.
 */
enum class LcdGfxSliderOrientation
{
    Horizontal,
    Vertical,
};

/**
 * Class implements a value slider widget for lcdgfx library.
 *
 * Renders a track with a knob that represents the current value within
 * [min, max]. Works with monochrome and color displays - drawing uses
 * only the generic NanoDisplayOps API.
 */
class LcdGfxSlider
{
public:
    /**
     * Creates a slider widget.
     *
     * @param rect screen area for the slider
     * @param minValue minimum value (inclusive)
     * @param maxValue maximum value (inclusive)
     * @param value initial value (clamped into [min, max])
     * @param step increment / decrement size for up()/down() (must be >= 1)
     * @param orientation Horizontal or Vertical
     */
    LcdGfxSlider(const NanoRect &rect, int16_t minValue, int16_t maxValue, int16_t value, int16_t step = 1,
                 LcdGfxSliderOrientation orientation = LcdGfxSliderOrientation::Horizontal);

    /**
     * Renders the slider on the display.
     *
     * @param d display object (any NanoDisplayOps-derived type)
     */
    template <typename D> void show(D &d)
    {
        d.drawRect(m_rect);
        if ( m_orientation == LcdGfxSliderOrientation::Horizontal )
        {
            lcdint_t innerW = m_rect.width() - 4;
            if ( innerW < 1 ) innerW = 1;
            lcdint_t span = m_max - m_min;
            lcdint_t knobX = m_rect.p1.x + 2 + (span > 0 ? (lcdint_t)(((int32_t)(m_value - m_min) * innerW) / span) : 0);
            lcdint_t knobW = m_rect.height() / 3;
            if ( knobW < 2 ) knobW = 2;
            d.fillRect(knobX - knobW / 2, m_rect.p1.y + 2, knobX + knobW / 2, m_rect.p2.y - 2);
        }
        else
        {
            lcdint_t innerH = m_rect.height() - 4;
            if ( innerH < 1 ) innerH = 1;
            lcdint_t span = m_max - m_min;
            lcdint_t knobY = m_rect.p1.y + 2 + (span > 0 ? (lcdint_t)(((int32_t)(m_value - m_min) * innerH) / span) : 0);
            lcdint_t knobH = m_rect.width() / 3;
            if ( knobH < 2 ) knobH = 2;
            d.fillRect(m_rect.p1.x + 2, knobY - knobH / 2, m_rect.p2.x - 2, knobY + knobH / 2);
        }
    }

    /**
     * Increments value by one step (clamped to max).
     */
    void up();

    /**
     * Decrements value by one step (clamped to min).
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

    /**
     * Returns the configured minimum.
     */
    int16_t minValue() const
    {
        return m_min;
    }

    /**
     * Returns the configured maximum.
     */
    int16_t maxValue() const
    {
        return m_max;
    }

    /**
     * Returns the rectangle area used by the slider.
     */
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
    LcdGfxSliderOrientation m_orientation;
};

/**
 * @}
 */

#endif
