/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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
extern "C" {
#endif

/**
 * @defgroup LCD_HW_INTERFACE_API I2C/SPI: physical interface functions
 * @{
 *
 * @brief i2c/spi initialization functions for different platforms
 *
 * @details This group of API functions serves to prepare the library to work via specific hardware
 *          interface. There are a bunch of functions for different platforms. In general display
 *          initialization goes in two steps: hardware interface initialization, and then display
 *          driver initialization. But there are functions, which combine 2 steps in single call:
 *          ssd1306_128x64_i2c_initEx(), ssd1351_128x128_spi_init(), etc.
 */

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

/** Describes low level hardware API for C-style code */
typedef struct
{
    /**
     * Indicates if spi or i2c interface is used.
     */
    uint8_t spi;
    /**
     * Starts communication with SSD1306 display.
     */
    void (*start)(void);
    /**
     * Ends communication with SSD1306 display.
     */
    void (*stop)(void);
    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void (*send)(uint8_t data);
    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void (*send_buffer)(const uint8_t *buffer, uint16_t size);
    /**
     * @brief deinitializes internal resources, allocated for interface.
     *
     * Deinitializes internal resources, allocated for interface.
     * There is no need to use this function for microcontrollers. In general
     * the function has meaning in Linux-like systems.
     */
    void (*close)(void);
} ssd1306_interface_t;

/**
 * Holds pointers to functions of currently initialized interface.
 */
extern ssd1306_interface_t ssd1306_intf;

/**
 * chip enable pin to controll lcd display over spi
 * @warning C-style API only
 */
extern int8_t s_ssd1306_cs;

/**
 * data/command control pin for spi interface of lcd display
 * @warning C-style API only
 */
extern int8_t s_ssd1306_dc;

/**
 * maximum SPI clock, supported by OLED display
 * @warning C-style API only
 */
extern uint32_t s_ssd1306_spi_clock;

/**
 * Sends command to SSD1306 device: includes initiating of
 * transaction, sending data and completing transaction.
 * @param command - command to send
 * @warning C-style API only
 */
void ssd1306_sendCommand(uint8_t command);

/**
 * Starts transaction for sending commands.
 * @warning C-style API only
 */
void ssd1306_commandStart(void);

/**
 * Starts transaction for sending bitmap data.
 * @warning C-style API only
 */
void ssd1306_dataStart(void);

/**
 * Switches spi to data(1) or command(0) mode.
 * @param mode - 1 data mode
 *               0 command mode
 * @warning C-style API only
 */
void ssd1306_spiDataMode(uint8_t mode);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void ssd1306_resetController(int8_t rstPin, uint8_t delayMs);
void ssd1306_resetController2(int8_t rstPin, uint8_t delayMs);
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

// ----------------------------------------------------------------------------
#endif
