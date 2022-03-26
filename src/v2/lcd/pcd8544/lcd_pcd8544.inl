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

template <class I> void InterfacePCD8544<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    m_width = w;
    m_column = x;
    m_page = y;
    this->start();
    setDataMode(0);
    if ( w == 1 )
        this->send(0x22);
    else
        this->send(0x20);
    this->send(0x80 | x);
    this->send(0x40 | y);
    setDataMode(1);
}

template <class I> void InterfacePCD8544<I>::nextBlock()
{
    if ( m_width != 1 )
    {
        this->stop();
        startBlock(m_column, m_page + 1, m_width);
    }
}

template <class I> void InterfacePCD8544<I>::endBlock()
{
    setDataMode(0);
    this->send(0x00); // Send NOP command to allow last data byte to pass (bug in PCD8544?)
                      // ssd1306 E3h is NOP command
    this->stop();
}

template <class I> void InterfacePCD8544<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfacePCD8544<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

////////////////////////////////////////////////////////////////////////////////
//             PCD8544 basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayPCD8544<I>::begin()
{
}

template <class I> void DisplayPCD8544<I>::end()
{
}

static const PROGMEM uint8_t s_PCD8544_lcd84x48_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_PCD8544,
    0x00,
    0x00,
    0x00,
#endif
    0x20 | 0x01,
    0x00, // switch to extented commands
    0x80 | 0x16,
    0x00, // Set vop contrast
    0x04,
    0x00, // set temp
    0x10 | 0x04,
    0x00, // Set bias mode
    0x20,
    0x00, // switch to basic commands
    0x08 | 0x04,
    0x00, // Normal display
};

////////////////////////////////////////////////////////////////////////////////
//             PCD8544 basic 1-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayPCD8544_84x48<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 20);
    this->m_w = 84;
    this->m_h = 48;
    // Give LCD some time to initialize. Refer to PCD8544 datasheet
    lcd_delay(0);
    _configureSpiDisplayCmdModeOnly<I>(this->m_intf, s_PCD8544_lcd84x48_initData, sizeof(s_PCD8544_lcd84x48_initData));
}

template <class I> void DisplayPCD8544_84x48<I>::end()
{
}
