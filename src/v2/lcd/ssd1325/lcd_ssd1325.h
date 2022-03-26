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
 * @file lcd_ssd1325.h support for LCD SSD1325 display
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
 * Class implements interface functions to SSD1325 displays
 */
template <class I> class InterfaceSSD1325: public I
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
    InterfaceSSD1325(NanoDisplayBase<InterfaceSSD1325<I>> &base, int8_t dc, Args &&... data)
        : I(data...)
        , m_dc(dc)
        , m_base(base)
    {
    }

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * For SSD1325 it uses horizontal addressing mode, while for
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
     * For SSD1325 it does nothing, while for sh1106 the function moves cursor to
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

private:
    const int8_t m_dc = -1;                       ///< data/command pin for SPI, -1 for i2c
    NanoDisplayBase<InterfaceSSD1325<I>> &m_base; ///< basic lcd display support interface
};
/**
 * Class implements basic functions for 4-bit mode of SSD1325-based displays
 */
template <class I> class DisplaySSD1325: public NanoDisplayOps<NanoDisplayOps4<I>, I>
{
public:
    /**
     * Creates instance of SSD1325 controller class for 4-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplaySSD1325(I &intf, int8_t rstPin)
        : NanoDisplayOps<NanoDisplayOps4<I>, I>(intf)
        , m_rstPin(rstPin)
    {
    }

protected:
    int8_t m_rstPin; ///< indicates hardware reset pin used, -1 if it is not required

    /**
     * Basic SSD1325 initialization
     */
    void begin() override;

    /**
     * Basic SSD1325 deinitialization
     */
    void end() override;
};

/**
 * Class implements basic functions for 4-bit mode of SSD1325-based displays
 */
template <class I> class DisplaySSD1325_128x64: public DisplaySSD1325<I>
{
public:
    /**
     * Creates instance of SSD1325 128x64 controller class for 4-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplaySSD1325_128x64(I &intf, int8_t rstPin)
        : DisplaySSD1325<I>(intf, rstPin)
    {
    }

protected:
    /**
     * Basic SSD1325 128x64 initialization
     */
    void begin() override;

    /**
     * Basic SSD1325 deinitialization
     */
    void end() override;
};
/**
 * Class implements SSD1325 128x64 lcd display in 4 bit mode over SPI
 */
class DisplaySSD1325_128x64_SPI: public DisplaySSD1325_128x64<InterfaceSSD1325<PlatformSpi>>
{
public:
    /**
     * @brief Inits 128x64 lcd display over spi (based on SSD1325 controller): 4-bit mode.
     *
     * Inits 128x64 lcd display over spi (based on SSD1325 controller): 4-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformSpiConfig.
     */
    explicit DisplaySSD1325_128x64_SPI(int8_t rstPin, const SPlatformSpiConfig &config = {-1, {-1}, -1, 0, -1, -1})
        : DisplaySSD1325_128x64(m_spi, rstPin)
        , m_spi(*this, config.dc,
                SPlatformSpiConfig{
                    config.busId, {config.cs}, config.dc, config.frequency ?: 10000000, config.scl, config.sda})
    {
    }

    /**
     * Initializes SSD1325 lcd in 4-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    InterfaceSSD1325<PlatformSpi> m_spi;
};

/**
 * Template class implements SSD1325 128x64 lcd display in 4 bit mode over custom SPI implementation
 * (user-defined spi implementation). I - user custom spi class
 */
template <class I> class DisplaySSD1325_128x64_CustomSPI: public DisplaySSD1325_128x64<InterfaceSSD1325<I>>
{
public:
    /**
     * @brief Inits 128x64 lcd display over spi (based on SSD1325 controller): 4-bit mode.
     *
     * Inits 128x64 lcd display over spi (based on SSD1325 controller): 4-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin
     * @param data variable argument list for custom user spi interface.
     */
    template <typename... Args>
    DisplaySSD1325_128x64_CustomSPI(int8_t rstPin, int8_t dcPin, Args &&... data)
        : DisplaySSD1325_128x64<InterfaceSSD1325<I>>(m_spi, rstPin)
        , m_spi(*this, dcPin, data...)
    {
    }

    /**
     * Initializes SSD1325 lcd in 4-bit mode
     */
    void begin() override
    {
        m_spi.begin();
        DisplaySSD1325_128x64<InterfaceSSD1325<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplaySSD1325_128x64<InterfaceSSD1325<I>>::end();
        m_spi.end();
    }

private:
    InterfaceSSD1325<I> m_spi;
};
/**
 * Class implements SSD1325 128x64 lcd display in 4 bit mode over I2C
 */
class DisplaySSD1325_128x64_I2C: public DisplaySSD1325_128x64<InterfaceSSD1325<PlatformI2c>>
{
public:
    /**
     * @brief Inits 128x64 lcd display over i2c (based on SSD1325 controller): 4-bit mode.
     *
     * Inits 128x64 lcd display over i2c (based on SSD1325 controller): 4-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform i2c configuration. Please refer to SPlatformI2cConfig.
     */
    explicit DisplaySSD1325_128x64_I2C(int8_t rstPin, const SPlatformI2cConfig &config = {-1, 0x3C, -1, -1, 0})
        : DisplaySSD1325_128x64(m_i2c, rstPin)
        , m_i2c(*this, -1,
                SPlatformI2cConfig{config.busId, static_cast<uint8_t>(config.addr ?: 0x3C), config.scl, config.sda,
                                   config.frequency ?: 400000})
    {
    }

    /**
     * Initializes SSD1325 lcd in 4-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    InterfaceSSD1325<PlatformI2c> m_i2c;
};

/**
 * Template class implements SSD1325 128x64 lcd display in 4 bit mode over custom I2C implementation
 * (user-defined i2c implementation). I - user custom i2c class
 */
template <class I> class DisplaySSD1325_128x64_CustomI2C: public DisplaySSD1325_128x64<InterfaceSSD1325<I>>
{
public:
    /**
     * @brief Inits 128x64 lcd display over i2c (based on SSD1325 controller): 4-bit mode.
     *
     * Inits 128x64 lcd display over i2c (based on SSD1325 controller): 4-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param data variable argument list for custom user i2c interface.
     */
    template <typename... Args>
    DisplaySSD1325_128x64_CustomI2C(int8_t rstPin, Args &&... data)
        : DisplaySSD1325_128x64<InterfaceSSD1325<I>>(m_i2c, rstPin)
        , m_i2c(*this, -1, data...)
    {
    }

    /**
     * Initializes SSD1325 lcd in 4-bit mode
     */
    void begin() override
    {
        m_i2c.begin();
        DisplaySSD1325_128x64<InterfaceSSD1325<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplaySSD1325_128x64<InterfaceSSD1325<I>>::end();
        m_i2c.end();
    }

private:
    InterfaceSSD1325<I> m_i2c;
};
#include "lcd_ssd1325.inl"

/**
 * @}
 */
