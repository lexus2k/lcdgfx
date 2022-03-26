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

template <class I> void InterfaceST7735<I>::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    lcduint_t rx = w ? (x + w - 1) : (m_base.width() - 1);
    commandStart();
    this->send(0x2A);
    setDataMode(1); // According to datasheet all args must be passed in data mode
    this->send(0);
    this->send(x + m_offset_x);
    this->send(0);
    this->send((rx < m_base.width() ? rx : (m_base.width() - 1)) + m_offset_x);
    setDataMode(0);
    this->send(0x2B);
    setDataMode(1); // According to datasheet all args must be passed in data mode
    this->send(0);
    this->send(y + m_offset_y);
    this->send(0);
    this->send(m_base.height() - 1 + m_offset_y);
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

template <class I> void InterfaceST7735<I>::nextBlock()
{
}

template <class I> void InterfaceST7735<I>::endBlock()
{
    this->stop();
}

template <class I> void InterfaceST7735<I>::setDataMode(uint8_t mode)
{
    if ( m_dc >= 0 )
    {
        lcd_gpioWrite(m_dc, mode ? LCD_HIGH : LCD_LOW);
    }
}

template <class I> void InterfaceST7735<I>::commandStart()
{
    this->start();
    if ( m_dc >= 0 )
        setDataMode(0);
    else
        this->send(0x00);
}

template <class I> void InterfaceST7735<I>::setRotation(uint8_t rotation)
{
    uint8_t ram_mode;
    if ( (rotation ^ m_rotation) & 0x01 )
    {
        m_base.swapDimensions();
        lcduint_t temp = m_offset_x;
        m_offset_x = m_offset_y;
        m_offset_y = temp;
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

template <class I> void InterfaceST7735<I>::setOffset(lcdint_t ox, lcdint_t oy)
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

template <class I> void InterfaceST7735<I>::setRgbMode(uint8_t mode)
{
    this->m_rgb_bit = mode ? 0b00001000 : 0b00000000;
    this->setRotation(m_rotation);
}

////////////////////////////////////////////////////////////////////////////////
//             ST7735 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayST7735x16<I>::begin()
{
}

template <class I> void DisplayST7735x16<I>::end()
{
}

static const PROGMEM uint8_t s_ST7735_lcd128x128x16_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_ST7735, 0x00, 0b00000000, 0x00,
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
//             ST7735 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayST7735_128x128x16<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 20);
    this->m_w = 128;
    this->m_h = 128;
    // Give LCD some time to initialize. Refer to ST7735 datasheet
    lcd_delay(120);
    _configureSpiDisplay<I>(this->m_intf, s_ST7735_lcd128x128x16_initData, sizeof(s_ST7735_lcd128x128x16_initData));
}

template <class I> void DisplayST7735_128x128x16<I>::end()
{
}

static const PROGMEM uint8_t s_ST7735_lcd80x160x16_initData[] = {
#ifdef SDL_EMULATION
    SDL_LCD_ST7735, 0x00, 0b00000100, 0x00,
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
//             ST7735 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayST7735_80x160x16<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 20);
    this->m_w = 80;
    this->m_h = 160;
    // Give LCD some time to initialize. Refer to ST7735 datasheet
    lcd_delay(120);
    _configureSpiDisplay<I>(this->m_intf, s_ST7735_lcd80x160x16_initData, sizeof(s_ST7735_lcd80x160x16_initData));
    this->m_intf.setOffset(26, 0);
}

template <class I> void DisplayST7735_80x160x16<I>::end()
{
}

static const PROGMEM uint8_t s_ST7735_lcd128x160x16_initData[] = {
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
//             ST7735 basic 16-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void DisplayST7735_128x160x16<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, 20);
    this->m_w = 128;
    this->m_h = 160;
    // Give LCD some time to initialize. Refer to ST7735 datasheet
    lcd_delay(120);
    _configureSpiDisplay<I>(this->m_intf, s_ST7735_lcd128x160x16_initData, sizeof(s_ST7735_lcd128x160x16_initData));
}

template <class I> void DisplayST7735_128x160x16<I>::end()
{
}
