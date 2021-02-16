/*
    MIT License

    Copyright (c) 2020, Alexey Dynda

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
 * @file v2/gui/button.h Button object definition
 */

#ifndef _LCDGFX_BUTTON_H_
#define _LCDGFX_BUTTON_H_

#include "nano_gfx_types.h"
#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/font.h"

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * Class implements button object for lcdgfx library
 */
class LcdGfxButton
{
public:
    /**
     * Creates button object with the provided text.
     *
     * @param text button text
     * @param rect screen area to use for button
     */
    LcdGfxButton(const char *text, const NanoRect &rect);

    /**
     * Shows button on the display.
     *
     * @param d display object
     */
    template <typename D> void show(D &d)
    {
        updateSize(d);
        d.invertColors();
        d.fillRect(m_rect);
        d.invertColors();
        if ( m_focus )
        {
            d.fillRect(m_rect);
            d.invertColors();
        }
        else
        {
            d.drawRect(m_rect);
        }
        d.printFixed(m_rect.p1.x + (m_rect.width() - d.getFont().getTextSize(m_text)) / 2,
                     m_rect.p1.y + (m_rect.height() - d.getFont().getHeader().height) / 2, m_text);
        if ( m_focus )
        {
            d.invertColors();
        }
    }

    /**
     * Sets or removes focus on button. After this method call show()
     * to update button state on the display.
     *
     * @param focus focus state for the button
     */
    void setFocus(bool focus);

    /**
     * returns true if button in focus
     */
    bool isActive();

    /**
     * Sets new area for the button. Use show() method to update button state on the display.
     *
     * @param rect new button area
     */
    void setRect(const NanoRect &rect);

    /**
     * Returns size of button in pixels
     */
    const NanoPoint getSize();

    /**
     * Recturns rectangle area of the button
     */
    const NanoRect &getRect() const
    {
        return m_rect;
    }

    /**
     * Auto updates buttons size if it is not set
     */
    template <typename D> void updateSize(D &d)
    {
        if ( !m_rect.p2.x )
        {
            lcduint_t width = d.getFont().getTextSize(m_text);
            lcduint_t height = d.getFont().getHeader().height;
            m_rect.p2.x = m_rect.p1.x + width - 1 + 8;
            m_rect.p2.y = m_rect.p1.y + height - 1 + 8;
        }
    }

private:
    bool m_focus = false;
    const char *m_text;
    NanoRect m_rect;
};

/**
 * @}
 */

#endif
