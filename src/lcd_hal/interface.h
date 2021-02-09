/*
    MIT License

    Copyright (c) 2017-2020, Alexey Dynda

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
 * @file lcd_hal/interface.h LCD interface functions.
 */

#ifndef _LCD_HAL_INTERFACE_H_
#define _LCD_HAL_INTERFACE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Structure describes i2c platform configuration
     *
     * @warning Not all fields are used by specific platforms
     */
    typedef struct
    {
        /**
         * bus id number. this parameter is valid for Linux, ESP32.
         * If -1 is pointed, it defaults to platform specific i2c bus (Linux i2c-dev0, esp32 I2C_NUM_0).
         */
        int8_t busId;

        /**
         * Address of i2c device to control. This is mandatory argument for all platforms
         */
        uint8_t addr;

        /**
         * Pin to use as i2c clock pin. This parameter is not used in Linux.
         * If -1 is pointed, the library uses default clock pin for specific platform.
         */
        int8_t scl;

        /**
         * Pin to use as i2c data pin. This parameter is not used in Linux.
         * If -1 is pointed, the library uses default data pin for specific platform.
         */
        int8_t sda;

        /**
         * Frequency in HZ to run spi bus at. Zero value defaults to platform optimal clock
         * speed (100kHz or 400kHz depending on platform).
         */
        uint32_t frequency;
    } SPlatformI2cConfig;

    /**
     * Structure describes spi platform configuration
     *
     * @warning Not all fields are used by specific platforms
     */
    typedef struct
    {
        /**
         * bus id number. this parameter is valid for Linux, ESP32.
         * If -1 is pointed, it defaults to platform specific i2c bus (Linux spidev1.X, esp32 VSPI_HOST).
         */
        int8_t busId;

        /**
         * parameter is optional for all platforms, except Linux.
         * If chip select pin is not used, it should be set to -1
         * For Linux platform devId should be pointed, if -1, it defaults to spidevX.0
         */
        union
        {
            int8_t cs;
            int8_t devId;
        };

        /**
         * Data command control pin number. This pin assignment is mandatory
         */
        int8_t dc;

        /**
         * Frequency in HZ to run spi bus at. If 0, it defaults to max frequency, supported
         * by platform
         */
        uint32_t frequency;

        /**
         * Optional - spi clock pin number. -1 if to use default spi clock pin.
         * This is required for ESP32 platform only.
         */
        int8_t scl; // clk

        /**
         * Optional - spi data MOSI pin number. -1 if to use default spi MOSI pin.
         * This is required for ESP32 platform only.
         */
        int8_t sda; // mosi
    } SPlatformSpiConfig;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    void ssd1306_resetController2(int8_t rstPin, uint8_t delayMs);
#endif

#ifdef __cplusplus
}
#endif

// ----------------------------------------------------------------------------
#endif
