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
 * @file v2/gui/yesno.h Yes/No dialog implementation
 */

#ifndef _LCDGFX_YESNO_H_
#define _LCDGFX_YESNO_H_

#include "nano_gfx_types.h"
#include "button.h"
#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/font.h"

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * Class implements YesNo dialog for lcdgfx library
 */
class LcdGfxYesNo
{
public:
    /**
     * Creates yes/no dialog with the provided text.
     *
     * @param text yes/no dialog text
     * @param rect screen area to use for button or 0 if to use full screen area
     */
    explicit LcdGfxYesNo(const char *text, const NanoRect &rect = {});

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
        d.drawRect(m_rect);
        d.printFixed(m_rect.p1.x + 4, m_rect.p1.y + 8, m_text);
        m_yes.show(d);
        m_no.show(d);
    }

    /**
     * Changes selection to Yes.
     * Redraw element using show() method.
     */
    void swapToYes();

    /**
     * Changes selection to No
     * Redraw element using show() method.
     */
    void swapToNo();

    /**
     * Returns true if yes button is active
     */
    bool isYes();

    /**
     * Calculates size for GUI component if it was not set before
     */
    template <typename D> void updateSize(D &d)
    {
        if ( !m_rect.p2.x )
        {
            m_rect.p2.x = d.width() - m_rect.p1.x - 1;
            m_rect.p2.y = d.height() - m_rect.p1.y - 1;
            locateButtons(d);
        }
    }

private:
    const char *m_text;
    NanoRect m_rect;
    LcdGfxButton m_yes;
    LcdGfxButton m_no;

    template <typename D> void locateButtons(D &d)
    {
        m_yes.updateSize(d);
        m_no.updateSize(d);
        NanoPoint size = m_yes.getSize();
        m_yes.setRect({{m_rect.center().x - size.x - 4, m_rect.p2.y - size.y - 8}, {0, 0}});
        m_no.setRect({{m_rect.center().x + 4, m_rect.p2.y - size.y - 8}, {0, 0}});
    }
};

/**
 * @}
 */

#endif
