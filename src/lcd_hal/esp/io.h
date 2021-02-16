/*
    MIT License

    Copyright (c) 2018-2020, Alexey Dynda

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
 * @file lcd_hal/esp/io.h This is ESP32 platform file
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** Progmem attribute for data, located in Flash */
#define LCD_PROGMEM

#include <sdkconfig.h>

#if !defined(CONFIG_TARGET_PLATFORM_ESP8266) && !defined(CONFIG_IDF_TARGET_ESP8266) && !defined(ESP8266) &&            \
    !defined(__ESP8266__)

/** The macro is defined when ESP32 i2c implementation is available */
#define CONFIG_ESP32_I2C_AVAILABLE
/** The macro is defined when ESP32 spi implementation is available */
#define CONFIG_ESP32_SPI_AVAILABLE

#else

/** The macro is defined when ESP8266 i2c implementation is available */
#define CONFIG_ESP8266_I2C_AVAILABLE
/** The macro is defined when ESP8266 spi implementation is available */
#define CONFIG_ESP8266_SPI_AVAILABLE

#endif
