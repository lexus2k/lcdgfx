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

template <class I> void InterfaceELW3501AA<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    m_column = x;
    m_page = y;
    if ( y < 84 )
    {
        if ( m_cs1 >= 0 )
            lcd_gpioWrite(this->m_cs1, LCD_LOW);
    }
    else
    {
        if ( m_cs2 >= 0 )
            lcd_gpioWrite(this->m_cs2, LCD_LOW);
    }

    commandStart();
    this->send(0x81);
    setDataMode(1); // According to datasheet all args must be passed in data mode
    this->send(x);
    setDataMode(0);
    this->send(0x80);
    setDataMode(1); // According to datasheet all args must be passed in data mode
    this->send(y / 84);
    setDataMode(0);
    this->send(0x83); // Read/write display data
    setDataMode(1);
}

template <class I> void InterfaceELW3501AA<I>::nextBlock()
{
    this->stop();
    startBlock(m_column, m_page + 1, 0);
}

template <class I> void InterfaceELW3501AA<I>::endBlock()
{
    this->stop();
    if ( m_page < 84 )
    {
        if ( m_cs1 >= 0 )
            lcd_gpioWrite(this->m_cs1, LCD_HIGH);
    }
    else
    {
        if ( m_cs2 >= 0 )
            lcd_gpioWrite(this->m_cs2, LCD_HIGH);
    }
}

template <class I> void InterfaceELW3501AA<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfaceELW3501AA<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

template <class I> void InterfaceELW3501AA<I>::setCsPins(lcdint_t cs1, lcdint_t cs2)
{
    this->m_cs1 = cs1;
    this->m_cs2 = cs2;
    lcd_gpioMode(this->m_cs1, LCD_GPIO_OUTPUT);
    lcd_gpioMode(this->m_cs2, LCD_GPIO_OUTPUT);
    lcd_gpioWrite(this->m_cs1, LCD_HIGH);
    lcd_gpioWrite(this->m_cs2, LCD_HIGH);
}

////////////////////////////////////////////////////////////////////////////////
//             ELW3501AA basic 4-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayELW3501AAx4<I>::begin()
{
}

template <class I> void DisplayELW3501AAx4<I>::end()
{
}

static const PROGMEM uint8_t s_ELW3501AA_lcd244x168x4_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_ELW3501AA,
    0x00,
    0b00000000,
    0x00,
#endif
    /*    0xA5, CMD_DELAY,    5,   // SWRESET sw reset. not needed, we do hardware reset
        0xAA, CMD_DELAY,    1,   // exit sleep mode
        0x42, 0x00,              // Int disable
        0x80, 0x01, 0x00,        // Set y pos
        0x81, 0x01, 0x00,        // Set x pos
        0x82, 0x01, 0xE3,        // Set Length of R/W 227
        0xE0, 0x01, 0x53,        // Set screen y pos
        0xE1, 0x01, 0x00,        // Set screen y pos
        0xE2, 0x01, 0x00,        // Set screen x pos
        0xE3, 0x01, 0x00,        // Set screen x pos */
    0x98,
    0x00,
    0xE0,
    0x01,
    0x53,
    0xB0,
    0x00, // Overlap mode
    0xD8,
    0x01,
    0x1C,
    0xE3,
    0x01,
    0x00,
    0xBC,
    0x00, // 0xBD for second
    0xC8,
    0x01,
    0x04, // Pre-charge
    0xD1,
    0x00, // Pre-charge current
    0x68,
    0x00, // 0x6A for second - Horizontal and Vertical Mirror mode
    0xE5,
    0x01,
    0x09, // Setting Blank Period
    0x84,
    0x00, // Read Modify Mode
    0x86,
    0x00, // OR Write mode
    0x78,
    0x00, // Invert Display Brightness
    0xCC,
    0x00, // All-Zero Blank Mode
          //    0x32, 0x00,              // No gradation.
    0x30,
    0x00, // 16-colors
    0x28,
    0x1E, // Setting of Gray-Level table
    0x07,
    0x00,
    0x09,
    0x00,
    0x0E,
    0x00,
    0x12,
    0x00,
    0x17,
    0x00,
    0x1F,
    0x00,
    0x29,
    0x00,
    0x37,
    0x00,
    0x48,
    0x00,
    0x5E,
    0x00,
    0x79,
    0x00,
    0x99,
    0x00,
    0xBD,
    0x00,
    0xE6,
    0x00,
    0x13,
    0x01,
    //    0x28, 0x02, 0x13, 0x01,  // Setting of Gray-Level Table
    0x7D,
    0x00, // Setting of COMV
    0xB8,
    0x01,
    0x33, // Setting of Dot Current
    0x42,
    0x00, // Setting int signal disable
    0x40,
    0x00, // 0x41 for second -> update type
    0x9C,
    0x00, // Mem page (4 gray) TODO
    0xBA,
    0x06,
    0x54,
    0x6C,
    0x84,
    0x9C,
    0xB4,
    0xCC,
    0xBB,
    0x02,
    0x28,
    0x00,
};

////////////////////////////////////////////////////////////////////////////////
//             ELW3501AA basic 4-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayELW3501AA_244x168x4<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 20);
    this->m_w = 244;
    this->m_h = 168;
    // Give LCD some time to initialize. Refer to ELW3501AA datasheet
    lcd_delay(120);
    _configureSpiDisplay<I>(this->m_intf, s_ELW3501AA_lcd244x168x4_initData, sizeof(s_ELW3501AA_lcd244x168x4_initData));
}

template <class I> void DisplayELW3501AA_244x168x4<I>::end()
{
}
