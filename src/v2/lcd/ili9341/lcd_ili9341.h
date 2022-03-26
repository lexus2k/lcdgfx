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
 * @file lcd_ili9341.h support for LCD ILI9341 display
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
 * Class implements interface functions to ILI9341 displays
 */
template <class I> class InterfaceILI9341: public I
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
    InterfaceILI9341(NanoDisplayBase<InterfaceILI9341<I>> &base, int8_t dc, Args &&... data)
        : I(data...)
        , m_dc(dc)
        , m_base(base)
    {
    }

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * For ILI9341 it uses horizontal addressing mode, while for
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
     * For ILI9341 it does nothing, while for sh1106 the function moves cursor to
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
     * @brief Sets rotation of all output functions
     *
     * Sets rotation of all output functions. 0 - no rotation, 1 - change by 90 degrees
     * Actually doesn't change screen orientation, only rotates primitives being output
     *
     * @param rotate enable (1) of disable (0)
     * @warning experimental feature, may work incorrectly
     */
    void rotateOutput(uint8_t rotate);

private:
    const int8_t m_dc = -1;                       ///< data/command pin for SPI, -1 for i2c
    NanoDisplayBase<InterfaceILI9341<I>> &m_base; ///< basic lcd display support interface
    uint8_t m_rotation = 0x00;
    uint8_t m_rotate_output = 0x00;
    static const uint8_t m_rgb_bit = 0b00001000;
};
/**
 * Class implements basic functions for 16-bit mode of ILI9341-based displays
 */
template <class I> class DisplayILI9341x16: public NanoDisplayOps<NanoDisplayOps16<I>, I>
{
public:
    /**
     * Creates instance of ILI9341 controller class for 16-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayILI9341x16(I &intf, int8_t rstPin)
        : NanoDisplayOps<NanoDisplayOps16<I>, I>(intf)
        , m_rstPin(rstPin)
    {
    }

protected:
    int8_t m_rstPin; ///< indicates hardware reset pin used, -1 if it is not required

    /**
     * Basic ILI9341 initialization
     */
    void begin() override;

    /**
     * Basic ILI9341 deinitialization
     */
    void end() override;
};

/**
 * Class implements basic functions for 16-bit mode of ILI9341-based displays
 */
template <class I> class DisplayILI9341_240x320x16: public DisplayILI9341x16<I>
{
public:
    /**
     * Creates instance of ILI9341 240x320x16 controller class for 16-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayILI9341_240x320x16(I &intf, int8_t rstPin)
        : DisplayILI9341x16<I>(intf, rstPin)
    {
    }

protected:
    /**
     * Basic ILI9341 240x320x16 initialization
     */
    void begin() override;

    /**
     * Basic ILI9341 deinitialization
     */
    void end() override;
};
/**
 * Class implements ILI9341 240x320x16 lcd display in 16 bit mode over SPI
 */
class DisplayILI9341_240x320x16_SPI: public DisplayILI9341_240x320x16<InterfaceILI9341<PlatformSpi>>
{
public:
    /**
     * @brief Inits 240x320x16 lcd display over spi (based on ILI9341 controller): 16-bit mode.
     *
     * Inits 240x320x16 lcd display over spi (based on ILI9341 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformSpiConfig.
     */
    explicit DisplayILI9341_240x320x16_SPI(int8_t rstPin, const SPlatformSpiConfig &config = {-1, {-1}, -1, 0, -1, -1})
        : DisplayILI9341_240x320x16(m_spi, rstPin)
        , m_spi(*this, config.dc,
                SPlatformSpiConfig{
                    config.busId, {config.cs}, config.dc, config.frequency ?: 10000000, config.scl, config.sda})
    {
    }

    /**
     * Initializes ILI9341 lcd in 16-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    InterfaceILI9341<PlatformSpi> m_spi;
};

/**
 * Template class implements ILI9341 240x320x16 lcd display in 16 bit mode over custom SPI implementation
 * (user-defined spi implementation). I - user custom spi class
 */
template <class I> class DisplayILI9341_240x320x16_CustomSPI: public DisplayILI9341_240x320x16<InterfaceILI9341<I>>
{
public:
    /**
     * @brief Inits 240x320x16 lcd display over spi (based on ILI9341 controller): 16-bit mode.
     *
     * Inits 240x320x16 lcd display over spi (based on ILI9341 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin
     * @param data variable argument list for custom user spi interface.
     */
    template <typename... Args>
    DisplayILI9341_240x320x16_CustomSPI(int8_t rstPin, int8_t dcPin, Args &&... data)
        : DisplayILI9341_240x320x16<InterfaceILI9341<I>>(m_spi, rstPin)
        , m_spi(*this, dcPin, data...)
    {
    }

    /**
     * Initializes ILI9341 lcd in 16-bit mode
     */
    void begin() override
    {
        m_spi.begin();
        DisplayILI9341_240x320x16<InterfaceILI9341<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplayILI9341_240x320x16<InterfaceILI9341<I>>::end();
        m_spi.end();
    }

private:
    InterfaceILI9341<I> m_spi;
};
/**
 * Class implements basic functions for 16-bit mode of ILI9341-based displays
 */
template <class I> class DisplayILI9341_128x160x16: public DisplayILI9341x16<I>
{
public:
    /**
     * Creates instance of ILI9341 128x160x16 controller class for 16-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayILI9341_128x160x16(I &intf, int8_t rstPin)
        : DisplayILI9341x16<I>(intf, rstPin)
    {
    }

protected:
    /**
     * Basic ILI9341 128x160x16 initialization
     */
    void begin() override;

    /**
     * Basic ILI9341 deinitialization
     */
    void end() override;
};
/**
 * Class implements ILI9341 128x160x16 lcd display in 16 bit mode over SPI
 */
class DisplayILI9341_128x160x16_SPI: public DisplayILI9341_128x160x16<InterfaceILI9341<PlatformSpi>>
{
public:
    /**
     * @brief Inits 128x160x16 lcd display over spi (based on ILI9341 controller): 16-bit mode.
     *
     * Inits 128x160x16 lcd display over spi (based on ILI9341 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformSpiConfig.
     */
    explicit DisplayILI9341_128x160x16_SPI(int8_t rstPin, const SPlatformSpiConfig &config = {-1, {-1}, -1, 0, -1, -1})
        : DisplayILI9341_128x160x16(m_spi, rstPin)
        , m_spi(*this, config.dc,
                SPlatformSpiConfig{
                    config.busId, {config.cs}, config.dc, config.frequency ?: 10000000, config.scl, config.sda})
    {
    }

    /**
     * Initializes ILI9341 lcd in 16-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    InterfaceILI9341<PlatformSpi> m_spi;
};

/**
 * Template class implements ILI9341 128x160x16 lcd display in 16 bit mode over custom SPI implementation
 * (user-defined spi implementation). I - user custom spi class
 */
template <class I> class DisplayILI9341_128x160x16_CustomSPI: public DisplayILI9341_128x160x16<InterfaceILI9341<I>>
{
public:
    /**
     * @brief Inits 128x160x16 lcd display over spi (based on ILI9341 controller): 16-bit mode.
     *
     * Inits 128x160x16 lcd display over spi (based on ILI9341 controller): 16-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin
     * @param data variable argument list for custom user spi interface.
     */
    template <typename... Args>
    DisplayILI9341_128x160x16_CustomSPI(int8_t rstPin, int8_t dcPin, Args &&... data)
        : DisplayILI9341_128x160x16<InterfaceILI9341<I>>(m_spi, rstPin)
        , m_spi(*this, dcPin, data...)
    {
    }

    /**
     * Initializes ILI9341 lcd in 16-bit mode
     */
    void begin() override
    {
        m_spi.begin();
        DisplayILI9341_128x160x16<InterfaceILI9341<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplayILI9341_128x160x16<InterfaceILI9341<I>>::end();
        m_spi.end();
    }

private:
    InterfaceILI9341<I> m_spi;
};
#include "lcd_ili9341.inl"

/**
 * @}
 */
