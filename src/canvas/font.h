/*
    MIT License

    Copyright (c) 2019-2020, Alexey Dynda

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
 * @file font.h Font class
 */

#ifndef _NANO_FONT_H_
#define _NANO_FONT_H_

#include "canvas_types.h"

/**
 * @ingroup NANO_ENGINE_API_V2
 * @{
 */

/** Flag means that more chars are required to decode utf-8 */
#define SSD1306_MORE_CHARS_REQUIRED 0xffff

/**
 * NanoFont class implements work with fonts provided by
 * the library: loading fonts, providing their parameters
 */
class NanoFont
{
public:
    /**
     * Creates empty NanoFont object. Please, use loadFixedFont(),
     * loadFreeFont(), loadSecondaryFont() functions after creating
     * the object.
     */
    NanoFont()
    {
    }

    /**
     * Creates NanoFont object and loads font of fixed type from flash memory
     */
    explicit NanoFont(const uint8_t *progmemFont)
    {
        loadFixedFont(progmemFont);
    }

    /**
     * Function allows to set another fixed font for the library.
     * By default, the font supports only first 128 - 32 ascii chars.
     * Please refer to github wiki on how to generate new fonts.
     * @param progmemFont - font to setup located in Flash area
     */
    void loadFixedFont(const uint8_t *progmemFont);

    /**
     * Function allows to set another free font for the library.
     * By default, the font supports only first 128 - 32 ascii chars.
     * Please refer to github wiki on how to generate new fonts.
     * @param progmemFont - font to setup located in Flash area
     */
    void loadFreeFont(const uint8_t *progmemFont);

    /**
     * Function allows sets secondary font for specific language.
     * Use it if you want to use additional font to combine capabilities of
     * ascii fonts and language specific font.
     * @param progmemUnicode font containing unicode table (refer to
     *        ssd1306xled_font6x8_German as example).
     */
    void loadSecondaryFont(const uint8_t *progmemUnicode);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void loadFixedFont_oldStyle(const uint8_t *progmemFont);
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    /**
     * Function allows to set and use squix font.
     * @param progmemFont - font to setup located in Flash area
     * @warning Squix fonts are not fully supported, use them at your own risk
     */
    void loadSquixFont(const uint8_t *progmemFont);
#endif

    /**
     * @brief returns char data for currently set (active) font.
     *
     * Function returns char data for currently set font: pointer to progmem
     * data for specified char, and width, height of the char. You can use these
     * data to draw char manually using NanoDisplayOps1::drawBitmap1(),
     * NanoDisplayOps8::drawBitmap1(), NanoDisplayOps16::drawBitmap1()
     * or NanoCanvasOps::drawBitmap1().
     *
     * @param ch char to read from set font
     * @param info pointer to SCharInfo structure to fill with char data
     *
     * @see loadFixedFont
     */
    void getCharBitmap(uint16_t ch, SCharInfo *info);

    /**
     * @brief returns text width in pixels (and its height if height is requested)
     *
     * Returns text width in pixels (and its height if height is requested)
     * @param text pointer to null-terminated string
     * @param height pointer to lcduint_t variable to store height to. nullptr if
     *        height is not required
     */
    lcduint_t getTextSize(const char *text, lcduint_t *height = nullptr);

    /**
     * Returns 16-bit unicode char, encoded in utf8
     *         SSD1306_MORE_CHARS_REQUIRED if more characters is expected
     * @param ch character byte to decode
     * @return 16-bit unicode char, encoded in utf8
     *         SSD1306_MORE_CHARS_REQUIRED if more characters is expected
     */
    static uint16_t unicode16FromUtf8(uint8_t ch);

    /**
     * Returns reference to SFontHeaderRecord. This record contains
     * some useful information on font like width, height, etc.
     */
    const SFontHeaderRecord &getHeader()
    {
        return m_fixedFont.h;
    }

    /**
     * Sets spacing in pixels between 2 nearest characters
     *
     * @param spacing spacing in pixels
     */
    void setSpacing(uint8_t spacing)
    {
        m_fixedFont.spacing = spacing;
    }

    /**
     * Returns how many pages in terms of ssd1306 display are required for
     * the font height.
     */
    uint8_t getPages()
    {
        return m_fixedFont.pages;
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    const uint8_t *getPrimaryTable()
    {
        return m_fixedFont.primary_table;
    }
#endif

private:
    SFixedFontInfo m_fixedFont{};

    void (*m_getCharBitmap)(const SFixedFontInfo &font, uint16_t unicode, SCharInfo *info) = nullptr;
};

extern NanoFont g_canvas_font;

/**
 * @}
 */

#endif
