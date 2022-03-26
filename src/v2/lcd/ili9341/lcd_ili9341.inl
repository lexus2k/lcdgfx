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

template <class I> void InterfaceILI9341<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    lcduint_t rx = w ? (x + w - 1) : (m_base.width() - 1);
    rx = rx < m_base.width() ? rx : (m_base.width() - 1);
    this->start();
    setDataMode(0);
    this->send(0x2A);
    setDataMode(1); // According to datasheet all args must be passed in data mode
    this->send(x >> 8);
    this->send(x & 0xFF);
    this->send(rx >> 8);
    this->send(rx & 0xFF);
    setDataMode(0);
    this->send(0x2B);
    setDataMode(1); // According to datasheet all args must be passed in data mode
    this->send(y >> 8);
    this->send(y & 0xFF);
    this->send((m_base.height() - 1) >> 8);
    this->send((m_base.height() - 1) & 0xFF);
    setDataMode(0);
    this->send(0x2C);
    setDataMode(1);
}

template <class I> void InterfaceILI9341<I>::nextBlock()
{
}

template <class I> void InterfaceILI9341<I>::endBlock()
{
    this->stop();
}

template <class I> void InterfaceILI9341<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfaceILI9341<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

template <class I> void InterfaceILI9341<I>::setRotation(uint8_t rotation)
{
    uint8_t ram_mode;
    if ( (rotation ^ m_rotation) & 0x01 )
    {
        m_base.swapDimensions();
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
            ram_mode = m_rotate_output ? 0b11100100 : 0b10000100;
            break;
        case 1: // 90 degree CW
            ram_mode = 0b11100000;
            break;
        case 2: // 180 degree CW
            ram_mode = 0b01010100;
            break;
        default: // 270 degree CW
            ram_mode = 0b00100000;
            break;
    }
    this->send(ram_mode | m_rgb_bit);
    setDataMode(0);
    this->send(0x29);
    this->stop();
}

template <class I> void InterfaceILI9341<I>::rotateOutput(uint8_t rotate)
{
    m_rotate_output = rotate;
    this->setRotation(m_rotation);
}

////////////////////////////////////////////////////////////////////////////////
//             ILI9341 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayILI9341x16<I>::begin()
{
}

template <class I> void DisplayILI9341x16<I>::end()
{
}

static const PROGMEM uint8_t s_ILI9341_lcd240x320x16_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_ILI9341, 0x00, 0x00, 0x00,
#endif
    0x01, 0x00,       // sw reset. not needed, we do hardware reset
    0x11, 0x00,       // exit sleep mode
    0x3A, 0x01, 0x05, // set 16-bit pixel format
    0x26, 0x01, 0x04, // set gamma curve: valid values 1, 2, 4, 8
    0xF2, 0x01, 0x01, // enable gamma adjustment, 0 - to disable
    0xE0, 15, 0x3F, 0x25, 0x1C, 0x1E, 0x20, 0x12, 0x2A, 0x90, 0x24, 0x11, 0x00, 0x00, 0x00, 0x00,
    0x00, // positive gamma correction
    0xE1, 15, 0x20, 0x20, 0x20, 0x20, 0x05, 0x00, 0x15, 0xA7, 0x3D, 0x18, 0x25, 0x2A, 0x2B, 0x2B,
    0x3A,                   // negative gamma correction
                            //    0xB1,  CMD_ARG,  0x08, CMD_ARG, 0x08, // frame rate control 1, use by default
                            //    0xB4,  CMD_ARG, 0x07,                 // display inversion, use by default
    0xC0, 0x02, 0x0A, 0x02, // power control 1
    0xC1, 0x01, 0x02,       // power control 2
    0xC5, 0x02, 0x50, 0x5B, // vcom control 1
    0xC7, 0x01, 0x40,       // vcom offset
    0x36, 0x01, 0b10001100, // set addressing mode
    0x29, 0x00,             // display on
};

////////////////////////////////////////////////////////////////////////////////
//             ILI9341 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayILI9341_240x320x16<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 100);
    this->m_w = 240;
    this->m_h = 320;
    // Give LCD some time to initialize. Refer to ILI9341 datasheet
    lcd_delay(100);
    _configureSpiDisplay<I>(this->m_intf, s_ILI9341_lcd240x320x16_initData, sizeof(s_ILI9341_lcd240x320x16_initData));
}

template <class I> void DisplayILI9341_240x320x16<I>::end()
{
}

static const PROGMEM uint8_t s_ILI9341_lcd128x160x16_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_ST7735, 0x00, 0b00000011, 0x00,
#endif
    0x01, CMD_DELAY, 150,         // SWRESET sw reset. not needed, we do hardware reset
    0x11, CMD_DELAY, 255,         // SLPOUT exit sleep mode
    0xB1, 0x03, 0x01, 0x2C, 0x2D, // FRMCTR1 frame rate control 1, use by default
    0xB2, 0x03, 0x01, 0x2C, 0x2D, // FRMCTR2, Frame Rate Control (In Idle mode/ 8-colors)
    0xB3, 0x06,                   // FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors)
    0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D, 0xB4, 0x01, 0x07, // INVCTR display inversion, use by default
    0xB6, 0x02, 0x15, 0x02,                               // DISSET5
    0xC0, 0x03, 0xA2, 0x02, 0x84,                         // PWCTR1 power control 1
    0xC1, 0x01, 0xC5,                                     // PWCTR2 power control 2
    0xC2, 0x02, 0x0A, 0x00,                               // PWCTR3 power control 3
    0xC3, 0x02, 0x8A, 0x2A,                               // PWCTR4 (C3h): Power Control 4 (in Idle mode/ 8-colors)
    0xC4, 0x02, 0x8A, 0xEE, // PWCTR5 (C4h): Power Control 5 (in Partial mode/ full-colors)
    0xC5, 0x01, 0x0E,       // VMCTR vcom control 1
    0x20, 0x00,             // INVOFF (20h): Display Inversion Off
                            //    0xFC, 0x02, 0x11, 0x15,  // PWCTR6
    0x36, 0x01, 0b00000000, // MADCTL
    0x3A, 0x01, 0x05,       // COLMOD set 16-bit pixel format
                            //    0x26, 1, 0x08,        // GAMSET set gamma curve: valid values 1, 2, 4, 8
                            //    0xF2, 1, 0x01,        // enable gamma adjustment, 0 - to disable
    0xE0, 0x10,             // GMCTRP1 positive gamma correction
    0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22, 0x1F, 0x1B, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10, 0xE1,
    0x10, // GMCTRN1 negative gamma correction
    0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30, 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10,
    //    0xC7,  1,  0x40,                // vcom offset
    //    0x2A,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x7F,   // set column address, not needed. set
    //    by direct API 0x2B,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x9F,   // set page address, not
    //    needed. set by direct API
    0x29, CMD_DELAY, 100, // DISPON display on
    0x13, CMD_DELAY, 10,  // NORON
};

////////////////////////////////////////////////////////////////////////////////
//             ILI9341 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayILI9341_128x160x16<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 100);
    this->m_w = 128;
    this->m_h = 160;
    // Give LCD some time to initialize. Refer to ILI9341 datasheet
    lcd_delay(100);
    _configureSpiDisplay<I>(this->m_intf, s_ILI9341_lcd128x160x16_initData, sizeof(s_ILI9341_lcd128x160x16_initData));
}

template <class I> void DisplayILI9341_128x160x16<I>::end()
{
}
