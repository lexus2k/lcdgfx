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
 * @file lcd_elw3501aa.h support for LCD ELW3501AA display
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
 * Class implements interface functions to ELW3501AA displays
 */
template <class I> class InterfaceELW3501AA: public I
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
    InterfaceELW3501AA(NanoDisplayBase<InterfaceELW3501AA<I>> &base, int8_t dc, Args &&... data)
        : I(data...)
        , m_dc(dc)
        , m_base(base)
    {
    }

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * For ELW3501AA it uses horizontal addressing mode, while for
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
     * @brief Sets chip select pins for elw3501aa controller
     *
     * ELW3501aa-based display has 2 controller inside. Each controller controls half of OLED matrix.
     * To switch between them, the driver uses cs1 and cs2 pins.
     *
     * @param cs1 chip select pin number for first controller
     * @param cs2 chip select pin number for second controller
     */
    void setCsPins(lcdint_t cs1, lcdint_t cs2);

private:
    const int8_t m_dc = -1;                         ///< data/command pin for SPI, -1 for i2c
    NanoDisplayBase<InterfaceELW3501AA<I>> &m_base; ///< basic lcd display support interface
    lcdint_t m_cs1 = -1;
    lcdint_t m_cs2 = -1;
    lcdint_t m_column = 0;
    lcdint_t m_page = 0;
};
/**
 * Class implements basic functions for 4-bit mode of ELW3501AA-based displays
 */
template <class I> class DisplayELW3501AAx4: public NanoDisplayOps<NanoDisplayOps4<I>, I>
{
public:
    /**
     * Creates instance of ELW3501AA controller class for 4-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayELW3501AAx4(I &intf, int8_t rstPin)
        : NanoDisplayOps<NanoDisplayOps4<I>, I>(intf)
        , m_rstPin(rstPin)
    {
    }

protected:
    int8_t m_rstPin; ///< indicates hardware reset pin used, -1 if it is not required

    /**
     * Basic ELW3501AA initialization
     */
    void begin() override;

    /**
     * Basic ELW3501AA deinitialization
     */
    void end() override;
};

/**
 * Class implements basic functions for 4-bit mode of ELW3501AA-based displays
 */
template <class I> class DisplayELW3501AA_244x168x4: public DisplayELW3501AAx4<I>
{
public:
    /**
     * Creates instance of ELW3501AA 244x168x4 controller class for 4-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplayELW3501AA_244x168x4(I &intf, int8_t rstPin)
        : DisplayELW3501AAx4<I>(intf, rstPin)
    {
    }

protected:
    /**
     * Basic ELW3501AA 244x168x4 initialization
     */
    void begin() override;

    /**
     * Basic ELW3501AA deinitialization
     */
    void end() override;
};
/**
 * Class implements ELW3501AA 244x168x4 lcd display in 4 bit mode over SPI
 */
class DisplayELW3501AA_244x168x4_SPI: public DisplayELW3501AA_244x168x4<InterfaceELW3501AA<PlatformSpi>>
{
public:
    /**
     * @brief Inits 244x168x4 lcd display over spi (based on ELW3501AA controller): 4-bit mode.
     *
     * Inits 244x168x4 lcd display over spi (based on ELW3501AA controller): 4-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformSpiConfig.
     */
    explicit DisplayELW3501AA_244x168x4_SPI(int8_t rstPin, const SPlatformSpiConfig &config = {-1, {-1}, -1, 0, -1, -1})
        : DisplayELW3501AA_244x168x4(m_spi, rstPin)
        , m_spi(*this, config.dc,
                SPlatformSpiConfig{
                    config.busId, {config.cs}, config.dc, config.frequency ?: 1000000, config.scl, config.sda})
    {
    }

    /**
     * Initializes ELW3501AA lcd in 4-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    InterfaceELW3501AA<PlatformSpi> m_spi;
};

/**
 * Template class implements ELW3501AA 244x168x4 lcd display in 4 bit mode over custom SPI implementation
 * (user-defined spi implementation). I - user custom spi class
 */
template <class I> class DisplayELW3501AA_244x168x4_CustomSPI: public DisplayELW3501AA_244x168x4<InterfaceELW3501AA<I>>
{
public:
    /**
     * @brief Inits 244x168x4 lcd display over spi (based on ELW3501AA controller): 4-bit mode.
     *
     * Inits 244x168x4 lcd display over spi (based on ELW3501AA controller): 4-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin
     * @param data variable argument list for custom user spi interface.
     */
    template <typename... Args>
    DisplayELW3501AA_244x168x4_CustomSPI(int8_t rstPin, int8_t dcPin, Args &&... data)
        : DisplayELW3501AA_244x168x4<InterfaceELW3501AA<I>>(m_spi, rstPin)
        , m_spi(*this, dcPin, data...)
    {
    }

    /**
     * Initializes ELW3501AA lcd in 4-bit mode
     */
    void begin() override
    {
        m_spi.begin();
        DisplayELW3501AA_244x168x4<InterfaceELW3501AA<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplayELW3501AA_244x168x4<InterfaceELW3501AA<I>>::end();
        m_spi.end();
    }

private:
    InterfaceELW3501AA<I> m_spi;
};
#include "lcd_elw3501aa.inl"

/**
 * @}
 */
