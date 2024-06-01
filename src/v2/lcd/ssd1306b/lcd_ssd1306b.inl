/*
    MIT License

    Copyright 2019-2022 (C) Alexey Dynda

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
/* !!! THIS FILE IS AUTO GENERATED !!! */
#include "lcd_hal/io.h"
#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif
#include "nano_gfx_types.h"

#ifndef CMD_ARG
#define CMD_ARG 0xFF
#endif

template <class I> void InterfaceSSD1306B<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    commandStart();
    this->send(0x21); // column addr
    this->send(x);
    this->send(w ? (x + w - 1) : (m_base.width() - 1));
    this->send(0x22); // page addr
    this->send(y);
    this->send((m_base.height() >> 3) - 1);
    if ( m_dc >= 0 )
    {
        setDataMode(1);
    }
    else
    {
        this->stop();
        this->start();
        this->send(0x40);
    }
}

template <class I> void InterfaceSSD1306B<I>::nextBlock()
{

}

template <class I> void InterfaceSSD1306B<I>::endBlock()
{
    this->stop();
}

template <class I> void InterfaceSSD1306B<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfaceSSD1306B<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

template <class I> void InterfaceSSD1306B<I>::setStartLine(uint8_t line)
{
    m_startLine = line;
    commandStart();
    this->send( 0x40 | (line & 0x3F) ); // start line
    this->stop();
}

template <class I> uint8_t InterfaceSSD1306B<I>::getStartLine()
{
    return m_startLine;
}

template <class I> void InterfaceSSD1306B<I>::normalMode()
{
    commandStart();
    this->send(0xA6); // Normal display
    this->stop();
}

template <class I> void InterfaceSSD1306B<I>::invertMode()
{
    commandStart();
    this->send(0xA7); // Invert display
    this->stop();
}

template <class I> void InterfaceSSD1306B<I>::setContrast(uint8_t contrast)
{
    commandStart();
    this->send(0x81); // set contrast
    this->send(contrast);
    this->stop();
}

template <class I> void InterfaceSSD1306B<I>::displayOff()
{
    commandStart();
    this->send(0xAE); // display off
    this->stop();
}

template <class I> void InterfaceSSD1306B<I>::displayOn()
{
    commandStart();
    this->send(0xAF); // display on
    this->stop();
}

template <class I> void InterfaceSSD1306B<I>::flipHorizontal(uint8_t mode)
{
    commandStart();
    this->send( 0xA0 | (mode ? 0x00: 0x01 ) ); // seg remap
    this->stop();
}

template <class I> void InterfaceSSD1306B<I>::flipVertical(uint8_t mode)
{
    commandStart();
    this->send( mode ? 0xC0 : 0xC8 );
    this->stop();
}


////////////////////////////////////////////////////////////////////////////////
//             SSD1306B basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1306B<I>::begin()
{
}

template <class I> void DisplaySSD1306B<I>::end()
{
}

static const PROGMEM uint8_t s_SSD1306B_lcd64x32_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_SSD1306B, 0x00,
    0x00, 0x00,
#endif
    0xAE,                // display off
    0x20, 0x00,          // Page horizontal Addressing mode
    0xC8,                // Scan from 127 to 0 (Reverse scan)
    0x40| 0x00,          // First line to start scanning from
    0x81, 0xFF,          // contast value to 0xFF for maximum brightness
    0xA0| 0x01,          // Use reverse mapping. 0x00 - is normal mapping
    0xA6,                // Normal display
    0xA8, 63,            // Reset to default MUX. See datasheet
    0xD3, 0x00,          // no offset
    0xD5, 0x80,          // set to default ratio/osc frequency
    0xD9, 0xF1,          // switch precharge to 0x22 // 0xF1
    0xDA, 0x12,          // set divide ratio
    0xDB, 0x30,          // vcom deselect to 0x30
    0x8D, 0x95,          // Enable charge pump 9V
    0xAD, 0x30,          // Internal Iref setting 30uA
    0xA4,                // Display resume
    0xAF,                // Display on
};

////////////////////////////////////////////////////////////////////////////////
//             SSD1306B basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1306B_64x32<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 10);
    this->m_w = 64;
    this->m_h = 32;
    // Give LCD some time to initialize. Refer to SSD1306B datasheet
    lcd_delay(0);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf,
                            s_SSD1306B_lcd64x32_initData,
                            sizeof(s_SSD1306B_lcd64x32_initData));

}

template <class I> void DisplaySSD1306B_64x32<I>::end()
{
}

static const PROGMEM uint8_t s_SSD1306B_lcd64x48_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_SSD1306B, 0x00,
    0x00, 0x00,
#endif
    0xAE,                // display off
    0x20, 0x00,          // Page horizontal Addressing mode
    0xC8,                // Scan from 127 to 0 (Reverse scan)
    0x40| 0x00,          // First line to start scanning from
    0x81, 0xFF,          // contast value to 0xFF for maximum brightness
    0xA0| 0x01,          // Use reverse mapping. 0x00 - is normal mapping
    0xA6,                // Normal display
    0xA8, 63,            // Reset to default MUX. See datasheet
    0xD3, 0x00,          // no offset
    0xD5, 0x80,          // set to default ratio/osc frequency
    0xD9, 0xF1,          // switch precharge to 0x22 // 0xF1
    0xDA, 0x12,          // set divide ratio
    0xDB, 0x30,          // vcom deselect to 0x30
    0x8D, 0x95,          // Enable charge pump 9V
    0xAD, 0x30,          // Internal Iref setting 30uA
    0xA4,                // Display resume
    0xAF,                // Display on
};

////////////////////////////////////////////////////////////////////////////////
//             SSD1306B basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1306B_64x48<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 10);
    this->m_w = 64;
    this->m_h = 48;
    // Give LCD some time to initialize. Refer to SSD1306B datasheet
    lcd_delay(0);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf,
                            s_SSD1306B_lcd64x48_initData,
                            sizeof(s_SSD1306B_lcd64x48_initData));

}

template <class I> void DisplaySSD1306B_64x48<I>::end()
{
}

static const PROGMEM uint8_t s_SSD1306B_lcd128x32_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_SSD1306B, 0x00,
    0x00, 0x00,
#endif
    0xAE,                // display off
    0x20, 0x00,          // Page horizontal Addressing mode
    0xC8,                // Scan from 127 to 0 (Reverse scan)
    0x40| 0x00,          // First line to start scanning from
    0x81, 0xFF,          // contast value to 0xFF for maximum brightness
    0xA0| 0x01,          // Use reverse mapping. 0x00 - is normal mapping
    0xA6,                // Normal display
    0xA8, 63,            // Reset to default MUX. See datasheet
    0xD3, 0x00,          // no offset
    0xD5, 0x80,          // set to default ratio/osc frequency
    0xD9, 0xF1,          // switch precharge to 0x22 // 0xF1
    0xDA, 0x12,          // set divide ratio
    0xDB, 0x30,          // vcom deselect to 0x30
    0x8D, 0x95,          // Enable charge pump 9V
    0xAD, 0x30,          // Internal Iref setting 30uA
    0xA4,                // Display resume
    0xAF,                // Display on
};

////////////////////////////////////////////////////////////////////////////////
//             SSD1306B basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1306B_128x32<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 10);
    this->m_w = 128;
    this->m_h = 32;
    // Give LCD some time to initialize. Refer to SSD1306B datasheet
    lcd_delay(0);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf,
                            s_SSD1306B_lcd128x32_initData,
                            sizeof(s_SSD1306B_lcd128x32_initData));

}

template <class I> void DisplaySSD1306B_128x32<I>::end()
{
}

static const PROGMEM uint8_t s_SSD1306B_lcd128x64_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_SSD1306B, 0x00,
    0x00, 0x00,
#endif
    0xAE,                // display off
    0x20, 0x00,          // Page horizontal Addressing mode
    0xC8,                // Scan from 127 to 0 (Reverse scan)
    0x40| 0x00,          // First line to start scanning from
    0x81, 0xFF,          // contast value to 0xFF for maximum brightness
    0xA0| 0x01,          // Use reverse mapping. 0x00 - is normal mapping
    0xA6,                // Normal display
    0xA8, 63,            // Reset to default MUX. See datasheet
    0xD3, 0x00,          // no offset
    0xD5, 0x80,          // set to default ratio/osc frequency
    0xD9, 0xF1,          // switch precharge to 0x22 // 0xF1
    0xDA, 0x12,          // set divide ratio
    0xDB, 0x30,          // vcom deselect to 0x30
    0x8D, 0x95,          // Enable charge pump 9V
    0xAD, 0x30,          // Internal Iref setting 30uA
    0xA4,                // Display resume
    0xAF,                // Display on
};

////////////////////////////////////////////////////////////////////////////////
//             SSD1306B basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1306B_128x64<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 10);
    this->m_w = 128;
    this->m_h = 64;
    // Give LCD some time to initialize. Refer to SSD1306B datasheet
    lcd_delay(0);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf,
                            s_SSD1306B_lcd128x64_initData,
                            sizeof(s_SSD1306B_lcd128x64_initData));

}

template <class I> void DisplaySSD1306B_128x64<I>::end()
{
}
