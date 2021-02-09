/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

/*
 * @file lcd_hal/stm32/io.h This is stm32 platform file
 */

#ifndef _SSD1306V2_STM32_IO_H_
#define _SSD1306V2_STM32_IO_H_

// TODO: To add support. Any help is welcome

#define SSD1306_STM32_PLATFORM
//========================== I. Include libraries =========================
/* 1. Include all required headers, specific for your platform here */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Progmem attribute for data, located in Flash */
#define PROGMEM

//========================== II. Define options ===========================
/* 2. Uncomment all options, you have support for on your platform   *
 *    Remember that you will need to implement low level intf/i2c or *
 *    intf/spi layers for your platform                              */

/** The macro is defined when STM32 i2c implementation is available */
#define CONFIG_PLATFORM_I2C_AVAILABLE
#define CONFIG_PLATFORM_SPI_AVAILABLE

#endif
