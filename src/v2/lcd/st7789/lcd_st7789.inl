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

template <class I> void InterfaceST7789<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    lcduint_t rx = x + (w ? w : m_base.width()) - 1;
    commandStart();
    this->send(0x2A);
    setDataMode(1); // According to datasheet all args must be passed in data mode
    lcduint_t pos = x + m_offset_x;
    this->send(pos >> 8);
    this->send(pos & 0xFF);
    pos = rx + m_offset_x;
    this->send(pos >> 8);
    this->send(pos & 0xFF);
    setDataMode(0);
    this->send(0x2B);
    setDataMode(1); // According to datasheet all args must be passed in data mode
    pos = y + m_offset_y;
    this->send(pos >> 8);
    this->send(pos & 0xFF);
    pos = m_base.height() - 1 + m_offset_y;
    this->send(pos >> 8);
    this->send(pos & 0xFF);
    setDataMode(0);
    this->send(0x2C);
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

template <class I> void InterfaceST7789<I>::nextBlock()
{
}

template <class I> void InterfaceST7789<I>::endBlock()
{
    this->stop();
}

template <class I> void InterfaceST7789<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfaceST7789<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

template <class I> void InterfaceST7789<I>::setRotation(uint8_t rotation)
{
    uint8_t ram_mode;
    if ( (rotation ^ m_rotation) & 0x01 )
    {
        m_base.swapDimensions();
        lcduint_t temp = m_offset_x;
        m_offset_x = m_offset_y;
        m_offset_y = temp;
    }
    if ( (rotation ^ m_rotation) == 0x01 || (rotation ^ m_rotation) == 0x11 )
    {
        // if one of the dimensions is odd
        if ( (m_base.width() & 0x01) || (m_base.height() & 0x01) )
        {
            switch ( rotation )
            {
                case 0: m_offset_x--; break;
                case 1: m_offset_y++; break;
                case 2: m_offset_x++; break;
                case 3: m_offset_y--; break;
                default: break;
            }
        }
    }
    m_rotation = (rotation & 0x03);
    this->start();
    setDataMode(0);
    this->send(0x28);
    this->send(0x36);
    setDataMode(1);
    switch ( m_rotation )
    {
        case 0: // 0 degree CW
            ram_mode = 0b00000000;
            break;
        case 1: // 90 degree CW
            ram_mode = 0b01100000;
            break;
        case 2: // 180 degree CW
            ram_mode = 0b11000000;
            break;
        default: // 270 degree CW
            ram_mode = 0b10100000;
            break;
    }
    this->send(ram_mode | m_rgb_bit);
    setDataMode(0);
    this->send(0x29);
    this->stop();
}

template <class I> void InterfaceST7789<I>::setOffset(lcdint_t ox, lcdint_t oy)
{
    if ( m_rotation & 0x01 )
    {
        m_offset_x = oy;
        m_offset_y = ox;
    }
    else
    {
        m_offset_x = ox;
        m_offset_y = oy;
    }
}

////////////////////////////////////////////////////////////////////////////////
//             ST7789 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayST7789x16<I>::begin()
{
}

template <class I> void DisplayST7789x16<I>::end()
{
}

static const PROGMEM uint8_t s_ST7789_lcd135x240x16_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_ST7789, 0x00, 0b00010000, 0x00,
#endif
    0x01, CMD_DELAY, 150,                     // SWRESET sw reset. not needed, we do hardware reset
    0x11, CMD_DELAY, 20,                      // SLPOUT exit sleep mode
    0x13, CMD_DELAY, 10,                      // NORON
    0x3A, 0x01, 0x55,                         // COLMOD set 16-bit pixel format 0x55
    0x36, 0x01, 0b00001000,                   // MADCTL 08 Adafruit
    0xB6, 0x02, 0x0A, 0x82,                   // DISSET5
    0xB2, 0x05, 0x0C, 0x0C, 0x00, 0x33, 0x33, // FRMCTR2 / PORCTL, Frame Rate Control (In Idle mode/ 8-colors)
    0xB7, 0x01, 0x35,                         // VGH / VGL
    0xBB, 0x01, 0x28,                         // VCOM
    0xC0, 0x01, 0x0C,                         // LCM / PWCTR1 power control 1
    0xC2, 0x02, 0x01, 0xFF,                   // VDV PWCTR3 power control 3
    0xC3, 0x01, 0x10,                         // VRH
    0xC4, 0x01, 0x20,                         // VDV
    0xC6, 0x01, 0x0F,                         // FRCTR2
    0xD0, 0x02, 0xA4, 0xA1,                   // PWCTRL1
    0xE0, 0x0E,                               // GMCTRP1 positive gamma correction
    0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0E, 0x12, 0x14, 0x17, 0xE1,
    0x0E, // GMCTRN1 negative gamma correction
    0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54, 0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E,
    //    0x2A,  0x04,  0x00,  0x00,  0x00,  0x7F,   // set column address, not needed. set by direct API
    //    0x2B,  0x04,  0x00,  0x00,  0x00,  0x9F,   // set page address, not needed. set by direct API
    0x21, CMD_DELAY, 10,  // INVON (21h): Display Inversion On
    0x29, CMD_DELAY, 120, // DISPON display on
};

////////////////////////////////////////////////////////////////////////////////
//             ST7789 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayST7789_135x240x16<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 20);
    this->m_w = 135;
    this->m_h = 240;
    // Give LCD some time to initialize. Refer to ST7789 datasheet
    lcd_delay(120);
    _configureSpiDisplay<I>(this->m_intf, s_ST7789_lcd135x240x16_initData, sizeof(s_ST7789_lcd135x240x16_initData));
}

template <class I> void DisplayST7789_135x240x16<I>::end()
{
}

static const PROGMEM uint8_t s_ST7789_lcd240x240x16_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_ST7789, 0x00, 0b00010000, 0x00,
#endif
    0x01, CMD_DELAY, 150,                     // SWRESET sw reset. not needed, we do hardware reset
    0x11, CMD_DELAY, 20,                      // SLPOUT exit sleep mode
    0x13, CMD_DELAY, 10,                      // NORON
    0x3A, 0x01, 0x55,                         // COLMOD set 16-bit pixel format 0x55
    0x36, 0x01, 0b00001000,                   // MADCTL 08 Adafruit
    0xB6, 0x02, 0x0A, 0x82,                   // DISSET5
    0xB2, 0x05, 0x0C, 0x0C, 0x00, 0x33, 0x33, // FRMCTR2 / PORCTL, Frame Rate Control (In Idle mode/ 8-colors)
    0xB7, 0x01, 0x35,                         // VGH / VGL
    0xBB, 0x01, 0x28,                         // VCOM
    0xC0, 0x01, 0x0C,                         // LCM / PWCTR1 power control 1
    0xC2, 0x02, 0x01, 0xFF,                   // VDV PWCTR3 power control 3
    0xC3, 0x01, 0x10,                         // VRH
    0xC4, 0x01, 0x20,                         // VDV
    0xC6, 0x01, 0x0F,                         // FRCTR2
    0xD0, 0x02, 0xA4, 0xA1,                   // PWCTRL1
    0xE0, 0x0E,                               // GMCTRP1 positive gamma correction
    0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0E, 0x12, 0x14, 0x17, 0xE1,
    0x0E, // GMCTRN1 negative gamma correction
    0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54, 0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E,
    //    0x2A,  0x04,  0x00,  0x00,  0x00,  0x7F,   // set column address, not needed. set by direct API
    //    0x2B,  0x04,  0x00,  0x00,  0x00,  0x9F,   // set page address, not needed. set by direct API
    0x21, CMD_DELAY, 10,  // INVON (21h): Display Inversion On
    0x29, CMD_DELAY, 120, // DISPON display on
};

////////////////////////////////////////////////////////////////////////////////
//             ST7789 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayST7789_240x240x16<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 20);
    this->m_w = 240;
    this->m_h = 240;
    // Give LCD some time to initialize. Refer to ST7789 datasheet
    lcd_delay(120);
    _configureSpiDisplay<I>(this->m_intf, s_ST7789_lcd240x240x16_initData, sizeof(s_ST7789_lcd240x240x16_initData));
}

template <class I> void DisplayST7789_240x240x16<I>::end()
{
}
