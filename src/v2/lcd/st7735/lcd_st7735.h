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
/**
 * @file lcd_st7735.h support for LCD ST7735 display
 */

#pragma once

#include "lcd_hal/io.h"
#include "v2/lcd/lcd_common.h"
#include "v2/lcd/base/display.h"

/**
 * @ingroup LCD_INTERFACE_API_V2
 * @{
 */

/**
 * Class implements interface functions to ST7735 displays
 */
template <class I> class InterfaceST7735: public I
{
public:
    /**
     * Creates instance of interface to LCD display.
     *
     * @param base Reference to base class, which represents Display
     * @param dc Data/command control pin number, for i2c communication should be -1
     * @param data variable argument list, accepted by platform interface (PlatformI2c, PlatformSpi)
     */
    template <typename... Args>
    InterfaceST7735(NanoDisplayBase<InterfaceST7735<I>> &base, int8_t dc, Args &&... data)
        : I(data...)
        , m_dc(dc)
        , m_base(base)
    {
    }

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * For ST7735 it uses horizontal addressing mode, while for
     * sh1106 the function uses page addressing mode.
     * Width can be specified as 0, thus the library will set the right boundary to
     * region of RAM block to the right column of the display.
     * @param x - column (left region)
     * @param y - row (top region)
     * @param w - width of the block in pixels to control
     *
     * @warning - this function initiates session (i2c or spi) and does not close it.
     *            To close session, please, call endBlock().
     */
    void startBlock(lcduint_t x, lcduint_t y, lcduint_t w);

    /**
     * Switches to the start of next RAM page for the block, specified by
     * startBlock().
     * For ST7735 it does nothing, while for sh1106 the function moves cursor to
     * next page.
     */
    void nextBlock();

    /**
     * Closes data send operation to lcd display.
     */
    void endBlock();

    /**
     * Enables either data or command mode on SPI bus
     * @param mode 1 to enable data mode, or 0 to enable command mode
     */
    void setDataMode(uint8_t mode);

    /**
     * Starts communication with LCD display in command mode.
     * To stop communication use m_intf.end().
     */
    void commandStart();

    /**
     * @brief Sets screen orientation (rotation)
     *
     * Sets screen orientation (rotation): 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
     * @param rotation - screen rotation 0 - normal, 1 - 90 CW, 2 - 180 CW, 3 - 270 CW
     */
    void setRotation(uint8_t rotation);

    /**
     * @brief Sets display offset in GDRAM memory
     *
     * Some lcd displays do not use COM0, ROW0 lines. It depends on display manufacturer.
     * So, in this case you need to use setOffset() method to tell the library about this
     * hardware specific.
     * This method sets GDRAM display offset in pixels
     * @param ox x-axis offset in pixels
     * @param oy y-axis offset in pixels
     */
    void setOffset(lcdint_t ox, lcdint_t oy);

    /**
     * @brief Sets RGB mode used by dispay controller.
     *
     * Sets RGB mode used by display controller. 0 - for BGR, 1 - for RGB
     * @param mode rgb mode for SSD1351 controller: 0 - BGR, 1 - RGB
     */
    void setRgbMode(uint8_t mode);

private:
    const int8_t m_dc = -1;                      ///< data/command pin for SPI, -1 for i2c
    NanoDisplayBase<InterfaceST7735<I>> &m_base; ///< basic lcd display support interface
    uint8_t m_rotation = 0x00;
    uint8_t m_rgb_bit = 0b00000000;
    lcdint_t m_offset_x = 0x00;
    lcdint_t m_offset_y = 0x00;
};
/**
 * Class implements basic functions for 16-bit mode of ST7735-based displays
 */
template <class I> class DisplayST7735x16: public NanoDisplayOps<NanoDisplayOps16<I>, I>
{
public:
    /**
     * Creates instance of ST7735 controller class for 16-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayST7735x16(I &intf, int8_t rstPin)
        : NanoDisplayOps<NanoDisplayOps16<I>, I>(intf)
        , m_rstPin(rstPin)
    {
    }

protected:
    int8_t m_rstPin; ///< indicates hardware reset pin used, -1 if it is not required

    /**
     * Basic ST7735 initialization
     */
    void begin() override;

    /**
     * Basic ST7735 deinitialization
     */
    void end() override;
};

/**
 * Class implements basic functions for 16-bit mode of ST7735-based displays
 */
template <class I> class DisplayST7735_128x128x16: public DisplayST7735x16<I>
{
public:
    /**
     * Creates instance of ST7735 128x128x16 controller class for 16-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayST7735_128x128x16(I &intf, int8_t rstPin)
        : DisplayST7735x16<I>(intf, rstPin)
    {
    }

protected:
    /**
     * Basic ST7735 128x128x16 initialization
     */
    void begin() override;

    /**
     * Basic ST7735 deinitialization
     */
    void end() override;
};
/**
 * Class implements ST7735 128x128x16 lcd display in 16 bit mode over SPI
 */
class DisplayST7735_128x128x16_SPI: public DisplayST7735_128x128x16<InterfaceST7735<PlatformSpi>>
{
public:
    /**
     * @brief Inits 128x128x16 lcd display over spi (based on ST7735 controller): 16-bit mode.
     *
     * Inits 128x128x16 lcd display over spi (based on ST7735 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformSpiConfig.
     */
    explicit DisplayST7735_128x128x16_SPI(int8_t rstPin, const SPlatformSpiConfig &config = {-1, {-1}, -1, 0, -1, -1})
        : DisplayST7735_128x128x16(m_spi, rstPin)
        , m_spi(*this, config.dc,
                SPlatformSpiConfig{
                    config.busId, {config.cs}, config.dc, config.frequency ?: 8000000, config.scl, config.sda})
    {
    }

    /**
     * Initializes ST7735 lcd in 16-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    InterfaceST7735<PlatformSpi> m_spi;
};

/**
 * Template class implements ST7735 128x128x16 lcd display in 16 bit mode over custom SPI implementation
 * (user-defined spi implementation). I - user custom spi class
 */
template <class I> class DisplayST7735_128x128x16_CustomSPI: public DisplayST7735_128x128x16<InterfaceST7735<I>>
{
public:
    /**
     * @brief Inits 128x128x16 lcd display over spi (based on ST7735 controller): 16-bit mode.
     *
     * Inits 128x128x16 lcd display over spi (based on ST7735 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin
     * @param data variable argument list for custom user spi interface.
     */
    template <typename... Args>
    DisplayST7735_128x128x16_CustomSPI(int8_t rstPin, int8_t dcPin, Args &&... data)
        : DisplayST7735_128x128x16<InterfaceST7735<I>>(m_spi, rstPin)
        , m_spi(*this, dcPin, data...)
    {
    }

    /**
     * Initializes ST7735 lcd in 16-bit mode
     */
    void begin() override
    {
        m_spi.begin();
        DisplayST7735_128x128x16<InterfaceST7735<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplayST7735_128x128x16<InterfaceST7735<I>>::end();
        m_spi.end();
    }

private:
    InterfaceST7735<I> m_spi;
};
/**
 * Class implements basic functions for 16-bit mode of ST7735-based displays
 */
template <class I> class DisplayST7735_80x160x16: public DisplayST7735x16<I>
{
public:
    /**
     * Creates instance of ST7735 80x160x16 controller class for 16-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayST7735_80x160x16(I &intf, int8_t rstPin)
        : DisplayST7735x16<I>(intf, rstPin)
    {
    }

protected:
    /**
     * Basic ST7735 80x160x16 initialization
     */
    void begin() override;

    /**
     * Basic ST7735 deinitialization
     */
    void end() override;
};
/**
 * Class implements ST7735 80x160x16 lcd display in 16 bit mode over SPI
 */
class DisplayST7735_80x160x16_SPI: public DisplayST7735_80x160x16<InterfaceST7735<PlatformSpi>>
{
public:
    /**
     * @brief Inits 80x160x16 lcd display over spi (based on ST7735 controller): 16-bit mode.
     *
     * Inits 80x160x16 lcd display over spi (based on ST7735 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformSpiConfig.
     */
    explicit DisplayST7735_80x160x16_SPI(int8_t rstPin, const SPlatformSpiConfig &config = {-1, {-1}, -1, 0, -1, -1})
        : DisplayST7735_80x160x16(m_spi, rstPin)
        , m_spi(*this, config.dc,
                SPlatformSpiConfig{
                    config.busId, {config.cs}, config.dc, config.frequency ?: 8000000, config.scl, config.sda})
    {
    }

    /**
     * Initializes ST7735 lcd in 16-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    InterfaceST7735<PlatformSpi> m_spi;
};

/**
 * Template class implements ST7735 80x160x16 lcd display in 16 bit mode over custom SPI implementation
 * (user-defined spi implementation). I - user custom spi class
 */
template <class I> class DisplayST7735_80x160x16_CustomSPI: public DisplayST7735_80x160x16<InterfaceST7735<I>>
{
public:
    /**
     * @brief Inits 80x160x16 lcd display over spi (based on ST7735 controller): 16-bit mode.
     *
     * Inits 80x160x16 lcd display over spi (based on ST7735 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin
     * @param data variable argument list for custom user spi interface.
     */
    template <typename... Args>
    DisplayST7735_80x160x16_CustomSPI(int8_t rstPin, int8_t dcPin, Args &&... data)
        : DisplayST7735_80x160x16<InterfaceST7735<I>>(m_spi, rstPin)
        , m_spi(*this, dcPin, data...)
    {
    }

    /**
     * Initializes ST7735 lcd in 16-bit mode
     */
    void begin() override
    {
        m_spi.begin();
        DisplayST7735_80x160x16<InterfaceST7735<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplayST7735_80x160x16<InterfaceST7735<I>>::end();
        m_spi.end();
    }

private:
    InterfaceST7735<I> m_spi;
};
/**
 * Class implements basic functions for 16-bit mode of ST7735-based displays
 */
template <class I> class DisplayST7735_128x160x16: public DisplayST7735x16<I>
{
public:
    /**
     * Creates instance of ST7735 128x160x16 controller class for 16-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayST7735_128x160x16(I &intf, int8_t rstPin)
        : DisplayST7735x16<I>(intf, rstPin)
    {
    }

protected:
    /**
     * Basic ST7735 128x160x16 initialization
     */
    void begin() override;

    /**
     * Basic ST7735 deinitialization
     */
    void end() override;
};
/**
 * Class implements ST7735 128x160x16 lcd display in 16 bit mode over SPI
 */
class DisplayST7735_128x160x16_SPI: public DisplayST7735_128x160x16<InterfaceST7735<PlatformSpi>>
{
public:
    /**
     * @brief Inits 128x160x16 lcd display over spi (based on ST7735 controller): 16-bit mode.
     *
     * Inits 128x160x16 lcd display over spi (based on ST7735 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformSpiConfig.
     */
    explicit DisplayST7735_128x160x16_SPI(int8_t rstPin, const SPlatformSpiConfig &config = {-1, {-1}, -1, 0, -1, -1})
        : DisplayST7735_128x160x16(m_spi, rstPin)
        , m_spi(*this, config.dc,
                SPlatformSpiConfig{
                    config.busId, {config.cs}, config.dc, config.frequency ?: 8000000, config.scl, config.sda})
    {
    }

    /**
     * Initializes ST7735 lcd in 16-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    InterfaceST7735<PlatformSpi> m_spi;
};

/**
 * Template class implements ST7735 128x160x16 lcd display in 16 bit mode over custom SPI implementation
 * (user-defined spi implementation). I - user custom spi class
 */
template <class I> class DisplayST7735_128x160x16_CustomSPI: public DisplayST7735_128x160x16<InterfaceST7735<I>>
{
public:
    /**
     * @brief Inits 128x160x16 lcd display over spi (based on ST7735 controller): 16-bit mode.
     *
     * Inits 128x160x16 lcd display over spi (based on ST7735 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin
     * @param data variable argument list for custom user spi interface.
     */
    template <typename... Args>
    DisplayST7735_128x160x16_CustomSPI(int8_t rstPin, int8_t dcPin, Args &&... data)
        : DisplayST7735_128x160x16<InterfaceST7735<I>>(m_spi, rstPin)
        , m_spi(*this, dcPin, data...)
    {
    }

    /**
     * Initializes ST7735 lcd in 16-bit mode
     */
    void begin() override
    {
        m_spi.begin();
        DisplayST7735_128x160x16<InterfaceST7735<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplayST7735_128x160x16<InterfaceST7735<I>>::end();
        m_spi.end();
    }

private:
    InterfaceST7735<I> m_spi;
};
#include "lcd_st7735.inl"

/**
 * @}
 */
