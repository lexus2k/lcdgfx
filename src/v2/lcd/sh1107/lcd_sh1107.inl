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

template <class I> void InterfaceSH1107<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    m_column = x;
    m_page = y;
    commandStart();
    this->send(0xB0 | y); // set page
    this->send( ((x + m_seg_offset)       & 0x0f )       ); // low column
    this->send((((x + m_seg_offset) >> 4) & 0x07 ) | 0x10); // high column
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

template <class I> void InterfaceSH1107<I>::nextBlock()
{
    this->stop();
    startBlock(m_column,m_page+1,0);
}

template <class I> void InterfaceSH1107<I>::endBlock()
{
    this->stop();
}

template <class I> void InterfaceSH1107<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfaceSH1107<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

template <class I> void InterfaceSH1107<I>::setStartLine(uint8_t line)
{
    m_startLine = line;
    commandStart();
    this->send( 0xDC );
    this->send( line & 0x7F ); // start line
    this->stop();
}

template <class I> uint8_t InterfaceSH1107<I>::getStartLine()
{
    return m_startLine;
}

template <class I> void InterfaceSH1107<I>::normalMode()
{
    commandStart();
    this->send(0xA6); // Normal display
    this->stop();
}

template <class I> void InterfaceSH1107<I>::invertMode()
{
    commandStart();
    this->send(0xA7); // Invert display
    this->stop();
}

template <class I> void InterfaceSH1107<I>::setContrast(uint8_t contrast)
{
    commandStart();
    this->send(0x81); // set contrast
    this->send(contrast);
    this->stop();
}

template <class I> void InterfaceSH1107<I>::displayOff()
{
    commandStart();
    this->send(0xAE); // display off
    this->stop();
}

template <class I> void InterfaceSH1107<I>::displayOn()
{
    commandStart();
    this->send(0xAF); // display on
    this->stop();
}

template <class I> void InterfaceSH1107<I>::flipHorizontal(uint8_t mode)
{
    commandStart();
    this->send( 0xA0 | (mode ? 0x00: 0x01 ) ); // seg remap
    this->stop();
}

template <class I> void InterfaceSH1107<I>::flipVertical(uint8_t mode)
{
    commandStart();
    this->send( mode ? 0xC0 : 0xC8 );
    this->stop();
}

template <class I> void InterfaceSH1107<I>::setSegOffset(uint8_t offset)
{
    m_seg_offset = offset;
}

template <class I> void InterfaceSH1107<I>::setDisplayOffset(uint8_t offset)
{
    commandStart();
    this->send( 0xD3 );
    this->send( offset & 0x7F ); // start line
    this->stop();
}


////////////////////////////////////////////////////////////////////////////////
//             SH1107 basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySH1107<I>::beginDisplay()
{
}

template <class I> void DisplaySH1107<I>::endDisplay()
{
}

static const PROGMEM uint8_t s_SH1107_lcd128x64_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_SH1107, 0x00,
    0x00, 0x00,
#endif
    0xAE, 0x00,        // display off
    0xC8, 0x00,        // Scan from 127 to 0 (Reverse scan)
    0xDC, 0x01, 0x00,  // First line to start scanning from
    0x81, 0x01, 0x80,  // contast value to 0x2F according to datasheet
    0x20, 0x00,        // Set page addressing mode
    0xA0| 0x01, 0x00,  // Use reverse mapping. 0x00 - is normal mapping
    0xA6, 0x00,        // Normal display
    0xA8, 0x01, 63,    // Reset to default MUX. See datasheet
    0xD3, 0x01, 0x00,  // no offset
    0xD5, 0x01, 0x80,  // set to default ratio/osc frequency
    0xD9, 0x01, 0x22,  // switch precharge to 0x22 // 0xF1
    0xDA, 0x01, 0x12,  // set divide ratio com pins
    0xDB, 0x01, 0x40,  // vcom deselect to 0x20 // 0x40
    0x8D, 0x01, 0x14,  // Enable charge pump
    0xA4, 0x00,        // Display on resume
    0xAF, 0x00,        // Display on
};

////////////////////////////////////////////////////////////////////////////////
//             SH1107 basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySH1107_128x64<I>::beginController()
{
    ssd1306_resetController2(this->m_rstPin, 10);
    this->m_w = 128;
    this->m_h = 64;
    // Give LCD some time to initialize. Refer to SH1107 datasheet
    lcd_delay(100);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf,
                            s_SH1107_lcd128x64_initData,
                            sizeof(s_SH1107_lcd128x64_initData));

}

template <class I> void DisplaySH1107_128x64<I>::endController()
{
}

static const PROGMEM uint8_t s_SH1107_lcd64x128_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_SH1107, 0x00,
    0x01, 0x00,
#endif
    0xAE, 0x00,        // display off
    0xDC, 0x01, 0x00,  // First line to start scanning from
    0x81, 0x01, 0x2F,  // contast value to 0x2F according to datasheet
    0x20, 0x00,        // Set page addressing mode
    0xA0| 0x01, 0x00,  // Use reverse mapping. 0x00 - is normal mapping
    0xCF, 0x00,        // Scan from 127 to 0 (Reverse scan)
    0xA4, 0x00,        // Display on resume
    0xA6, 0x00,        // Normal display
    0xA8, 0x01, 127,   // Reset to default MUX. See datasheet
    0xD3, 0x01, 0x60,  // offset 0x60 according to datasheet
    0xD5, 0x01, 0x80,  // set to default ratio/osc frequency
    0xD9, 0x01, 0x22,  // switch precharge to 0x22 // 0xF1
    0xDB, 0x01, 0x40,  // vcom deselect to 0x20 // 0x40
    0xAD, 0x01, 0x8A,  // Set charge pump enable
    0xAF, 0x00,        // Display on
};

////////////////////////////////////////////////////////////////////////////////
//             SH1107 basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySH1107_64x128<I>::beginController()
{
    ssd1306_resetController2(this->m_rstPin, 10);
    this->m_w = 64;
    this->m_h = 128;
    // Give LCD some time to initialize. Refer to SH1107 datasheet
    lcd_delay(100);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf,
                            s_SH1107_lcd64x128_initData,
                            sizeof(s_SH1107_lcd64x128_initData));
    this->m_intf.setSegOffset( 0 );
}

template <class I> void DisplaySH1107_64x128<I>::endController()
{
}
