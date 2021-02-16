/*
    MIT License

    Copyright (c) 2020, Alexey Dynda

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
/**
 * @file lcd_any.h support for Any LCD display
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
 * Interface to custom display commnucation
 */
class DisplayInterface
{
public:
    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * Width can be specified as 0, thus the library will set the right boundary to
     * region of RAM block to the right column of the display.
     * @param x - column (left region)
     * @param y - row (top region)
     * @param w - width of the block in pixels to control
     *
     * @warning - this function initiates session (i2c or spi) and does not close it.
     *            To close session, please, call endBlock().
     */
    virtual void startBlock(lcduint_t x, lcduint_t y, lcduint_t w) = 0;

    /**
     * Switches to the start of next RAM page for the block, specified by
     * startBlock().
     */
    virtual void nextBlock()
    {
    }

    /**
     * Closes data send operation to lcd display.
     */
    virtual void endBlock() = 0;

    /**
     * Sends byte to display device
     * @param data - byte to send
     */
    virtual void send(uint8_t data) = 0;
};

/**
 * Class implements custom interface functions for custom displays
 */
class InterfaceAny
{
public:
    /**
     * Creates instance of interface to LCD display.
     *
     * @param intf DisplayInterface object
     */
    explicit InterfaceAny(DisplayInterface &intf)
        : m_intf(intf)
    {
    }

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * Width can be specified as 0, thus the library will set the right boundary to
     * region of RAM block to the right column of the display.
     * @param x - column (left region)
     * @param y - row (top region)
     * @param w - width of the block in pixels to control
     *
     * @warning - this function initiates session (i2c or spi) and does not close it.
     *            To close session, please, call endBlock().
     */
    void startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
    {
        m_intf.startBlock(x, y, w);
    }

    /**
     * Switches to the start of next RAM page for the block, specified by
     * startBlock().
     */
    void nextBlock()
    {
        m_intf.nextBlock();
    }

    /**
     * Closes data send operation to lcd display.
     */
    void endBlock()
    {
        m_intf.endBlock();
    }

    /**
     * Sends byte to display device
     * @param data - byte to send
     */
    void send(uint8_t data)
    {
        m_intf.send(data);
    }

private:
    DisplayInterface &m_intf; ///< basic display communication interface
};

/**
 * Class implements basic functionality for custom 1-bit lcd display
 */
class DisplayAny1: public NanoDisplayOps<NanoDisplayOps1<InterfaceAny>, InterfaceAny>, DisplayInterface
{
public:
    /**
     * @brief Inits custom 1-bit lcd display
     *
     * Inits custom 1-bit lcd display
     * All hardware initialization is expected to be done by a user
     *
     * @param width width of custom display in pixels
     * @param height height of custom display in pixels
     */
    DisplayAny1(lcduint_t width, lcduint_t height);

    /**
     * Override this method if you need to do some initialization for custom display
     */
    void begin() override;

    /**
     * Override this method if you need to do some close actions for custom display
     */
    void end() override;

private:
    InterfaceAny m_any;
};

/**
 * Class implements basic functionality for custom 4-bit lcd display
 */
class DisplayAny4: public NanoDisplayOps<NanoDisplayOps4<InterfaceAny>, InterfaceAny>, DisplayInterface
{
public:
    /**
     * @brief Inits custom 4-bit lcd display
     *
     * Inits custom 4-bit lcd display
     * All hardware initialization is expected to be done by a user
     *
     * @param width width of custom display in pixels
     * @param height height of custom display in pixels
     */
    DisplayAny4(lcduint_t width, lcduint_t height);

    /**
     * Override this method if you need to do some initialization for custom display
     */
    void begin() override;

    /**
     * Override this method if you need to do some close actions for custom display
     */
    void end() override;

private:
    InterfaceAny m_any;
};

/**
 * Class implements basic functionality for custom 8-bit lcd display
 */
class DisplayAny8: public NanoDisplayOps<NanoDisplayOps8<InterfaceAny>, InterfaceAny>, DisplayInterface
{
public:
    /**
     * @brief Inits custom 8-bit lcd display
     *
     * Inits custom 8-bit lcd display
     * All hardware initialization is expected to be done by a user
     *
     * @param width width of custom display in pixels
     * @param height height of custom display in pixels
     */
    DisplayAny8(lcduint_t width, lcduint_t height);

    /**
     * Override this method if you need to do some initialization for custom display
     */
    void begin() override;

    /**
     * Override this method if you need to do some close actions for custom display
     */
    void end() override;

private:
    InterfaceAny m_any;
};

/**
 * Class implements basic functionality for custom 16-bit lcd display
 */
class DisplayAny16: public NanoDisplayOps<NanoDisplayOps16<InterfaceAny>, InterfaceAny>, DisplayInterface
{
public:
    /**
     * @brief Inits custom 16-bit lcd display
     *
     * Inits custom 16-bit lcd display
     * All hardware initialization is expected to be done by a user
     *
     * @param width width of custom display in pixels
     * @param height height of custom display in pixels
     */
    DisplayAny16(lcduint_t width, lcduint_t height);

    /**
     * Override this method if you need to do some initialization for custom display
     */
    void begin() override;

    /**
     * Override this method if you need to do some close actions for custom display
     */
    void end() override;

private:
    InterfaceAny m_any;
};

/**
 * @}
 */
