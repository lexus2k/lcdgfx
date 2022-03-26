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

template <class I> void InterfaceSSD1325<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    lcduint_t rx = w ? (x + w - 1) : (m_base.width() - 1);
    commandStart();
    this->send(0x15);
    this->send(x / 2);
    this->send((rx < m_base.width() ? rx : (m_base.width() - 1)) / 2);
    this->send(0x75);
    this->send(y);
    this->send(m_base.height() - 1);
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

template <class I> void InterfaceSSD1325<I>::nextBlock()
{
}

template <class I> void InterfaceSSD1325<I>::endBlock()
{
    this->stop();
}

template <class I> void InterfaceSSD1325<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfaceSSD1325<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

////////////////////////////////////////////////////////////////////////////////
//             SSD1325 basic 4-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1325<I>::begin()
{
}

template <class I> void DisplaySSD1325<I>::end()
{
}

static const PROGMEM uint8_t s_SSD1325_lcd128x64_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_SSD1325,
    0x00,
    SDL_LCD_SSD1325_GENERIC,
    0x00,
#endif
    0xAE,
    0x00, // OFF                         /* display off */
    0xB3,
    0x01,
    0x91, // CLK
    0xA8,
    0x01,
    0x3F, // multiplex 64
    0xA2,
    0x01,
    0x00, // Display offset
    0xA1,
    0x01,
    0x00, // Start line
    0xAD,
    0x01,
    0x02, // VCOMH
    0xA0,
    0x01,
    0x40 | 0x10 | 0x00 | (0x02 | 0x01), // REMAP: horizontal increment mode
    0x86,
    0x00, // CURRENT
    0x81,
    0x01,
    0x70, // CONTRAST
    0xB2,
    0x01,
    0x51, // FREQ
    0xB1,
    0x01,
    0x55, // PHASE
    0xBC,
    0x01,
    0x10, // PRECHARGE
    0xBE,
    0x01,
    0x1C, // VCOMH voltage
    0xA4,
    0x00, // NORMAL
};

////////////////////////////////////////////////////////////////////////////////
//             SSD1325 basic 4-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1325_128x64<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 10);
    this->m_w = 128;
    this->m_h = 64;
    // Give LCD some time to initialize. Refer to SSD1325 datasheet
    lcd_delay(0);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf, s_SSD1325_lcd128x64_initData,
                                       sizeof(s_SSD1325_lcd128x64_initData));
}

template <class I> void DisplaySSD1325_128x64<I>::end()
{
}
