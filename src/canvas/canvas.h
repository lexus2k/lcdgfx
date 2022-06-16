/*
    MIT License

    Copyright 2018-2020,2022 (C) Alexey Dynda

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
 * @file canvas.h Drawing in memory buffer
 */

#ifndef _NANO_CANVAS_H_
#define _NANO_CANVAS_H_

#include "point.h"
#include "rect.h"
#include "font.h"
#include "canvas_types.h"

/**
 * @ingroup NANO_ENGINE_API_V2
 * @{
 */

/**
 * NanoCanvasOps provides operations for drawing in memory buffer.
 * Depending on BPP argument, this class can work with 1,8,16-bit canvas areas.
 */
template <uint8_t BPP> class NanoCanvasOps
{
public:
    /** Base type for canvas class specific operations */
    typedef NanoCanvasOps<BPP> T;

    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = BPP;

    /** Fixed offset for all operation of NanoCanvasOps in pixels */
    NanoPoint offset;

    /**
     * Creates new empty canvas object.
     * If you this constructor is used, you must call begin() method before
     * working with canvas.
     */
    NanoCanvasOps()
    {
    }

    /**
     * Creates new canvas object.
     * Width can be of any value.
     * Height should be divided by 8.
     * Memory buffer must be not less than w * h.
     *
     * @param w - width
     * @param h - height
     * @param bytes - pointer to memory buffer to use
     */
    NanoCanvasOps(lcdint_t w, lcdint_t h, uint8_t *bytes)
    {
        begin(w, h, bytes);
    }

    /**
     * Initializes canvas object.
     * Width can be of any value.
     * Height should be divided by 8.
     * Memory buffer must be not less than w * h.
     *
     * @param w - width
     * @param h - height
     * @param bytes - pointer to memory buffer to use
     */
    void begin(lcdint_t w, lcdint_t h, uint8_t *bytes);

    /**
     * Sets offset
     * @param ox - X offset in pixels
     * @param oy - Y offset in pixels
     */
    void setOffset(lcdint_t ox, lcdint_t oy)
    {
        offset.x = ox;
        offset.y = oy;
    };

    /**
     * Returns right-bottom point of the canvas in offset terms.
     * If offset is (0,0), then offsetEnd() will return (width-1,height-1).
     */
    const NanoPoint offsetEnd() const
    {
        return offset + (NanoPoint){(lcdint_t)(m_w - 1), (lcdint_t)(m_h - 1)};
    }

    /**
     * Returns rectangle area, covered by canvas in offset terms.
     * If offset is (0,0), then rect() will return ((0,0),(width-1,height-1))
     */
    const NanoRect rect() const
    {
        return {offset, offsetEnd()};
    }

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    void putPixel(lcdint_t x, lcdint_t y);

    /**
     * Draws pixel on specified position
     * @param p - NanoPoint
     * @note color can be set via setColor()
     */
    void putPixel(const NanoPoint &p);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2);

    /**
     * Draws line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);

    /**
     * Draws line
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void drawLine(const NanoRect &rect);

    /**
     * Draws rectangle
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2) __attribute__((noinline));

    /**
     * Draws rectangle
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void drawRect(const NanoRect &rect);

    /**
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2) __attribute__((noinline));

    /**
     * Fills rectangle area
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void fillRect(const NanoRect &rect);

    /**
     * Draws circle
     * @param x horizontal position of circle center in pixels
     * @param y vertical position of circle center in pixels
     * @param r circle radius in pixels
     */
    void drawCircle(lcdint_t x, lcdint_t y, lcdint_t r) __attribute__((noinline));

    /**
     * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - monochrome bitmap data, located in flash
     *
     * @note There are 2 modes: transparent and non-transparent mode, - and 2 colors available: black and white.
     *       In non-transparent mode, when black color is selected, the monochrome image just inverted.
     *       In transparent mode, those pixels of source monochrome image, which are black, do not overwrite pixels
     *       in the screen buffer.
     */
    void drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) __attribute__((noinline));

    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) __attribute__((noinline));

    /**
     * @brief Draws 16-bit color bitmap in color buffer.
     * Draws 16-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 16-bit color bitmap data, located in flash
     */
    void drawBitmap16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
        __attribute__((noinline));

    /**
     * Clears canvas
     */
    void clear() __attribute__((noinline));

    /**
     * Writes single character to canvas
     * @param c - character code to print
     */
    size_t write(uint8_t c);

    /**
     * Draws single character to canvas
     * @param c - character code to print
     * @returns 0 if char is not printed
     */
    uint8_t printChar(uint8_t c);

    /**
     * Print text at specified position to canvas
     *
     * @param xpos  position in pixels
     * @param y     position in pixels
     * @param ch    pointer to NULL-terminated string.
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style = STYLE_NORMAL)
        __attribute__((noinline));

    /**
     * Print text at specified position to canvas
     *
     * @param xpos  position in pixels
     * @param y     position in pixels
     * @param ch    pointer to NULL-terminated string, located in flash
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style = STYLE_NORMAL);

    /**
     * @brief Sets canvas drawing mode
     * Sets canvas drawing mode. The set flags define transparency of output images
     * @param modeFlags - combination of flags: CANVAS_TEXT_WRAP, CANVAS_MODE_TRANSPARENT
     */
    void setMode(uint8_t modeFlags)
    {
        m_textMode = modeFlags;
    };

    /**
     * Sets color for monochrome operations
     * @param color - color to set (refer to RGB_COLOR8 definition)
     */
    void setColor(uint16_t color)
    {
        m_color = color;
    }

    /**
     * Returns currently set color.
     */
    uint16_t getColor()
    {
        return m_color;
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
     * Sets background color
     *
     * @param color Background color
     */
    void setBackground(uint16_t color)
    {
        m_bgColor = color;
    }

    /**
     * Sets new font to use with print functions.
     * If multiple canvases are used in single application,
     * this method allows to use different fonts for different
     * canvases.
     *
     * @param font reference to font object (NanoFont)
     */
    void setFont(NanoFont &font)
    {
        m_font = &font;
    };

    /**
     * Returns reference to NanoFont object, currently used by Display
     */
    NanoFont &getFont()
    {
        return *m_font;
    }

    /**
     * Sets font spacing for currently active font
     * @param spacing spacing in pixels
     */
    void setFontSpacing(uint8_t spacing)
    {
        if ( m_font )
            m_font->setSpacing(spacing);
    }

    /**
     * Sets new font to use with print functions.
     * If multiple canvases are used in single application,
     * this method can cause conflicts.
     *
     * @warning use this method only if single canvas is used in project
     *
     * @param progmemFont pointer to font data in flash (refer to NanoFont::loadFixedFont)
     */
    void setFixedFont(const uint8_t *progmemFont)
    {
        g_canvas_font.loadFixedFont(progmemFont);
        setFont(g_canvas_font);
    }

    /**
     * Sets new font to use with print functions.
     * If multiple canvases are used in single application,
     * this method can cause conflicts.
     *
     * @warning use this method only if single canvas is used in project
     *
     * @param progmemFont pointer to font data in flash (refer to NanoFont::loadFreeFont)
     * @param secondaryFont pointer to font data in flash (refer to NanoFont::loadSecondaryFont)
     */
    void setFreeFont(const uint8_t *progmemFont, const uint8_t *secondaryFont = nullptr)
    {
        (void)(secondaryFont);
        g_canvas_font.loadFreeFont(progmemFont);
        setFont(g_canvas_font);
    }

    /** Return pointer to canvas pixels data */
    uint8_t *getData()
    {
        return m_buf;
    }

    /** Returns canvas width in pixels */
    lcduint_t width()
    {
        return m_w;
    }

    /** Returns canvas height in pixels */
    lcduint_t height()
    {
        return m_h;
    }

    /** Rotates the canvas clock-wise */
    void rotateCW(T &out);

protected:
    lcduint_t m_w;              ///< width of NanoCanvas area in pixels
    lcduint_t m_h;              ///< height of NanoCanvas area in pixels
    lcdint_t m_cursorX;         ///< current X cursor position for text output
    lcdint_t m_cursorY;         ///< current Y cursor position for text output
    uint8_t m_textMode;         ///< Flags for current NanoCanvas mode
    EFontStyle m_fontStyle;     ///< currently active font style
    uint8_t *m_buf;             ///< Canvas data
    uint16_t m_color;           ///< current color
    uint16_t m_bgColor;         ///< current background color
    NanoFont *m_font = nullptr; ///< current set font to use with NanoCanvas
};

/**
 * Base class for all NanoCanvas childs
 */
template <uint8_t BPP> class NanoCanvasBase: public NanoCanvasOps<BPP>
{
public:
    using NanoCanvasOps<BPP>::NanoCanvasOps;
};

/**
 * Template for user-defined canvas object.
 * template parameters are: width, height and bits per pixels.
 * If object is defined locally, the pixels buffer is located in stack
 */
template <lcduint_t W, lcduint_t H, uint8_t BPP> class NanoCanvas: public NanoCanvasBase<BPP>
{
public:
    NanoCanvas()
        : NanoCanvasBase<BPP>(W, H, m_buffer)
    {
    }

private:
    uint8_t m_buffer[W * H * BPP / 8]{};
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             1-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

enum
{
    BLACK = 0x00, ///< Black color
    WHITE = 0xFF, ///< White color
};

/**
 * NanoCanvas1 represents objects for drawing in memory buffer
 * NanoCanvas1 represents each pixel as single bit: 0/1
 */
class NanoCanvas1: public NanoCanvasBase<1>
{
public:
    using NanoCanvasBase::NanoCanvasBase;
};

/**
 * NanoCanvas1_8 represents objects for drawing in memory buffer
 * NanoCanvas1_8 represents each pixel as single bit: 0/1
 * Unlike NanoCanvas1, it works with RBG color displays in normal mode.
 */
class NanoCanvas1_8: public NanoCanvasBase<1>
{
public:
    using NanoCanvasBase::NanoCanvasBase;
};

/**
 * NanoCanvas1_16 represents objects for drawing in memory buffer
 * NanoCanvas1_16 represents each pixel as single bit: 0/1
 * Unlike NanoCanvas1, it works with RBG color displays in normal mode.
 */
class NanoCanvas1_16: public NanoCanvasBase<1>
{
public:
    using NanoCanvasBase::NanoCanvasBase;
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * NanoCanvas4 represents objects for drawing in memory buffer
 * NanoCanvas4 represents each pixel as 4-bits. Each byte contains 2 pixels: 22221111
 */
class NanoCanvas4: public NanoCanvasBase<4>
{
public:
    using NanoCanvasBase::NanoCanvasBase;
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * NanoCanvas8 represents objects for drawing in memory buffer
 * NanoCanvas8 represents each pixel as single byte with RGB bits: RRRGGGBB
 */
class NanoCanvas8: public NanoCanvasBase<8>
{
public:
    using NanoCanvasBase::NanoCanvasBase;
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             16-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * NanoCanvas16 represents objects for drawing in memory buffer
 * NanoCanvas16 represents each pixel as 2-bytes with RGB bits: RRRRRGGG-GGGBBBBB
 */
class NanoCanvas16: public NanoCanvasBase<16>
{
public:
    using NanoCanvasBase::NanoCanvasBase;
};

/**
 * @}
 */

#endif
