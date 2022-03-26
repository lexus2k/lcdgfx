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

template <class I> void InterfaceSSD1327<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
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

template <class I> void InterfaceSSD1327<I>::nextBlock()
{
}

template <class I> void InterfaceSSD1327<I>::endBlock()
{
    this->stop();
}

template <class I> void InterfaceSSD1327<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfaceSSD1327<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

////////////////////////////////////////////////////////////////////////////////
//             SSD1327 basic 4-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1327<I>::begin()
{
}

template <class I> void DisplaySSD1327<I>::end()
{
}

static const PROGMEM uint8_t s_SSD1327_lcd128x128_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_SSD1327, 0x00, SDL_LCD_SSD1327_GENERIC, 0x00,
#endif
    0xFD, 0x01, 0x12,                               // Unlock OLED
    0xAE, 0x00,                                     // OFF                         /* display off */
    0xA8, 0x01, 0x7F,                               // multiplex 128
    0xA1, 0x01, 0x00,                               // Start line
    0xA2, 0x01, 0x00,                               // Display offset
    0xA0, 0x01, 0x40 | 0x10 | 0x00 | (0x02 | 0x01), // REMAP: horizontal increment mode
    0xAB, 0x01, 0x01,                               // VDD internal
    0x81, 0x01, 0x70,                               // CONTRAST
    0xB1, 0x01, 0x55,                               // PHASE 0x51
    0xB3, 0x01, 0x01,                               // CLK
                                                    //   0xB9,         //Reload grey scale
    0xBC, 0x01, 0x08,                               // PRECHARGE
    0xBE, 0x01, 0x07,                               // VCOMH voltage
    0xB6, 0x01, 0x01,                               // Second pre-charge
    0xA4, 0x00,                                     // NORMAL
    0x2E, 0x00,                                     // Deactivate scroll
    0xAF, 0x00,                                     // Display ON
};

////////////////////////////////////////////////////////////////////////////////
//             SSD1327 basic 4-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplaySSD1327_128x128<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 10);
    this->m_w = 128;
    this->m_h = 128;
    // Give LCD some time to initialize. Refer to SSD1327 datasheet
    lcd_delay(0);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf, s_SSD1327_lcd128x128_initData,
                                       sizeof(s_SSD1327_lcd128x128_initData));
}

template <class I> void DisplaySSD1327_128x128<I>::end()
{
}
