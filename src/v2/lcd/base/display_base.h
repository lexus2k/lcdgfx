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
/**
 * @file display_base.h Basic display capabilities implementation
 */

#ifndef _DISPLAY_BASE_H_
#define _DISPLAY_BASE_H_

#include "lcd_hal/io.h"
#include "nano_gfx_types.h"
#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/font.h"

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

#ifdef __cplusplus
extern "C"
{
#endif
    extern uint8_t s_ssd1306_invertByte;
#ifdef __cplusplus
}
#endif

/**
 * Class implements basic display operations for the library:
 * It stores reference to communication interafce, display size, etc.
 */
template <class I> class NanoDisplayBase
{
public:
    /**
     * Creates new empty base display object.
     * If you this constructor is used, you must call begin() method before
     * working with canvas.
     */
    explicit NanoDisplayBase(I &intf)
        : m_intf(intf)
    {
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /**
     * Sets offset
     * @param ox - X offset in pixels
     * @param oy - Y offset in pixels
     */
    void setOffset(lcdint_t ox, lcdint_t oy){};
#endif

    /**
     * Returns right-bottom point of the canvas in offset terms.
     * If offset is (0,0), then offsetEnd() will return (width-1,height-1).
     */
    const NanoPoint offsetEnd() const
    {
        return (NanoPoint){(lcdint_t)(m_w - 1), (lcdint_t)(m_h - 1)};
    }

    /**
     * Returns rectangle area, covered by canvas in offset terms.
     * If offset is (0,0), then rect() will return ((0,0),(width-1,height-1))
     */
    const NanoRect rect() const
    {
        return {(NanoPoint){0, 0}, offsetEnd()};
    }

    /**
     * Returns width of the display in pixels.
     */
    lcduint_t width()
    {
        return m_w;
    }

    /**
     * Returns height of the display in pixels.
     */
    lcduint_t height()
    {
        return m_h;
    }

    /**
     * Swaps width and height dimensions
     */
    void swapDimensions()
    {
        lcduint_t t = m_w;
        m_w = m_h;
        m_h = t;
    }

    /**
     * Sets color
     * @param color - color to set (refer to RGB_COLOR8, RGB_COLOR16 definition)
     */
    void setColor(uint16_t color)
    {
        m_color = color;
    };

    /**
     * Returns currently set color
     */
    uint16_t getColor()
    {
        return m_color;
    };

    /**
     * Sets background color.
     * @param color color to set (refer to RGB_COLOR8, RGB_COLOR16 definition)
     */
    void setBackground(uint16_t color)
    {
        m_bgColor = color;
    }

    /**
     * Changes foreground and background colors
     */
    void invertColors()
    {
        uint16_t color = m_color;
        m_color = m_bgColor;
        m_bgColor = color;
    }

    /**
     * Returns reference to interface communicator.
     * This interface can be used to use display hardware related
     * features.
     */
    I &getInterface()
    {
        return m_intf;
    }

    /**
     * Sets new font to use with print functions.
     * If multiple oled displays are used in single application,
     * this method allows to use different fonts for different
     * displays.
     *
     * @param font reference to font object (NanoFont)
     */
    void setFont(NanoFont &font)
    {
        m_font = &font;
    }

    /**
     * Sets spacing for currently active font.
     * The same can be done via `getFont().setSpacing()`, but this method is
     * more safe since it checks if font is set.
     * @param spacing spacing in pixels
     */
    void setFontSpacing(uint8_t spacing)
    {
        if ( m_font )
            m_font->setSpacing(spacing);
    }

    /**
     * Returns reference to NanoFont object, currently used by Display
     */
    NanoFont &getFont()
    {
        return *m_font;
    }

    /**
     * Sets new font to use with print functions.
     * If multiple oled displays are used in single application,
     * this method can cause conflicts.
     *
     * @warning use this method only if single display is used in project
     *
     * @param progmemFont pointer to font data in flash (refer to NanoFont::loadFixedFont)
     */
    void setFixedFont(const uint8_t *progmemFont)
    {
        g_canvas_font.loadFixedFont(progmemFont);
        setFont(g_canvas_font);
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void setFixedFont_oldStyle(const uint8_t *progmemFont)
    {
        g_canvas_font.loadFixedFont_oldStyle(progmemFont);
        setFont(g_canvas_font);
    }
#endif

    /**
     * Sets new font to use with print functions.
     * If multiple oled displays are used in single application,
     * this method can cause conflicts.
     *
     * @warning use this method only if single display is used in project
     *
     * @param progmemFont pointer to font data in flash (refer to NanoFont::loadFixedFont)
     * @param secondaryFont pointer to font data in flash (refer to NanoFont::loadSecondaryFont)
     */
    void setFixedFont(const uint8_t *progmemFont, const uint8_t *secondaryFont)
    {
        g_canvas_font.loadFixedFont(progmemFont);
        if ( secondaryFont )
        {
            g_canvas_font.loadSecondaryFont(secondaryFont);
        }
        setFont(g_canvas_font);
    }

    /**
     * Sets new font to use with print functions.
     * If multiple oled displays are used in single application,
     * this method can cause conflicts.
     *
     * @warning use this method only if single display is used in project
     *
     * @param progmemFont pointer to font data in flash (refer to NanoFont::loadFreeFont)
     * @param secondaryFont pointer to font data in flash (refer to NanoFont::loadSecondaryFont)
     */
    void setFreeFont(const uint8_t *progmemFont, const uint8_t *secondaryFont = nullptr)
    {
        g_canvas_font.loadFreeFont(progmemFont);
        if ( secondaryFont )
        {
            g_canvas_font.loadSecondaryFont(secondaryFont);
        }
        setFont(g_canvas_font);
    }

    /**
     * Function sets text cursor position for write() functions
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     */
    void setTextCursor(lcdint_t x, lcdint_t y)
    {
        m_cursorX = x;
        m_cursorY = y;
    }

protected:
    lcduint_t m_w = 0;                     ///< width of NanoCanvas area in pixels
    lcduint_t m_h = 0;                     ///< height of NanoCanvas area in pixels
    lcduint_t m_p = 0;                     ///< number of bits, used by width value: 3 equals to 8 pixels width
    lcdint_t m_cursorX = 0;                ///< current X cursor position for text output
    lcdint_t m_cursorY = 0;                ///< current Y cursor position for text output
    uint8_t m_textMode = 0;                ///< Flags for current NanoCanvas mode
    EFontStyle m_fontStyle = STYLE_NORMAL; ///< currently active font style
    uint16_t m_color = 0xFFFF;             ///< current foreground color
    uint16_t m_bgColor = 0x0000;           ///< current background color
    NanoFont *m_font = nullptr;            ///< currently set font

    I &m_intf; ///< communication interface with the display
};

/**
 * @}
 */

#endif
