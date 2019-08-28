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

/**
 * @file lcd_hal/io.h SSD1306 HAL IO communication functions
 */

#ifndef _LCD_HAL_IO_H_
#define _LCD_HAL_IO_H_

/**
 * @defgroup SSD1306_HAL_API HAL: ssd1306 library hardware abstraction layer
 * @{
 *
 * @brief i2c/spi ssd1306 library hardware abstraction layer
 *
 * @details ssd1306 library hardware abstraction layer
 */

#include "UserSettings.h"
#include "interface.h"

#if defined(ARDUINO)
#include "arduino/io.h"
#ifdef __cplusplus
#include "arduino/arduino_spi.h"
#include "arduino/arduino_wire.h"
#include "avr/avr_spi.h"
#include "avr/avr_twi.h"
#include "avr/ssd1306_spi_usi.h"
#include "avr/ssd1306_i2c_embedded.h"
#include "esp/esp32_i2c.h"
#include "esp/esp32_spi.h"
#endif
#elif defined(__AVR__) && !defined(ARDUINO)
#include "avr/io.h"
#ifdef __cplusplus
#include "avr/avr_spi.h"
#include "avr/avr_twi.h"
#include "avr/ssd1306_spi_usi.h"
#include "avr/ssd1306_i2c_embedded.h"
#endif
#elif defined(__XTENSA__) && !defined(ARDUINO)
#include "esp/io.h"
#ifdef __cplusplus
#include "esp/esp32_i2c.h"
#include "esp/esp32_spi.h"
#endif
#elif defined(STM32F1) || defined(STM32F2) || defined(STM32F4)
#include "stm32/io.h"
#elif defined(__linux__)
#include "linux/io.h"
#ifdef __cplusplus
#include "linux/linux_i2c.h"
#include "linux/linux_spi.h"
#include "linux/sdl_i2c.h"
#include "linux/sdl_spi.h"
#endif
#elif defined(__MINGW32__)
#include "mingw/io.h"
#ifdef __cplusplus
#include "linux/sdl_i2c.h"
#include "linux/sdl_spi.h"
#endif
#else
#warning "Platform is not supported. Use template to add support"
#endif

#include <stdint.h>

#ifndef LCDINT_TYPES_DEFINED
/** Macro informs if lcdint_t type is defined */
#define LCDINT_TYPES_DEFINED
/** internal int type, used by ssd1306 library. Important for uC with low SRAM */
typedef int lcdint_t;
/** internal int type, used by ssd1306 library. Important for uC with low SRAM */
typedef unsigned int lcduint_t;
#endif

/** swaps content of a and b variables of type type */
#define ssd1306_swap_data(a, b, type)  { type t = a; a = b; b = t; }

////////////////////////////////////////////////////////////////////////////////
//                   HAL Layer functions
////////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define ARDUINO_COMPAT
#endif

#if defined(ARDUINO)

#define LCD_LOW LOW
#define LCD_HIGH HIGH
#define LCD_GPIO_INPUT INPUT
#define LCD_GPIO_OUTPUT OUTPUT
#define LCD_PROGMEM PROGMEM

#define lcd_gpioRead digitalRead
#define lcd_gpioWrite digitalWrite
#define lcd_gpioMode pinMode
#define lcd_adcRead analogRead

#define lcd_pgmReadByte pgm_read_byte
#define lcd_eepromReadWord eeprom_read_word
#define lcd_eepromWriteWord eeprom_write_word

#define lcd_millis millis
#define lcd_micros micros
#define lcd_delay delay
#define lcd_delayUs delayMicroseconds

#define lcd_random random
#define lcd_randomSeed randomSeed

#else

/** Constant corresponds to low level of gpio pin */
#define LCD_LOW 0
/** Constant corresponds to high level of gpio pin */
#define LCD_HIGH 1
/** Constant corresponds to input mode of gpio */
#define LCD_GPIO_INPUT 0
/** Consrant corresponds to output mode of gpio */
#define LCD_GPIO_OUTPUT 1
// LCD_PROMEM is platform specific definition
#ifndef LCD_PROGMEM
/** LCD_PROGMEM constant is used to specify data stored in flash, platform specific */
#define LCD_PROGMEM PROGMEM
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Sets gpio pin mode
 * @param pin pin number to change mode of
 * @param mode new gpio mode: LCD_GPIO_INPUT or LCD_GPIO_OUTPUT
 */
void lcd_gpioMode(int pin, int mode);

/**
 * Reads gpio pin value
 * @param pin gpio pin number to read
 * @return LCD_HIGH or LCD_LOW
 */
int  lcd_gpioRead(int pin);

/**
 * Writes value to gpio
 * @param pin gpio pin number to change
 * @param level LCD_HIGH or LCD_LOW
 */
void lcd_gpioWrite(int pin, int level);

/**
 * Read ADC data
 * @param pin adc pin to read (platform-specific)
 * @return integer value corresponding to provided gpio pin.
 *         actual value range depends on platform and ADC mode.
 */
int lcd_adcRead(int pin);

/**
 * returns 32-bit timestamp from system power-up in milliseconds
 */
uint32_t lcd_millis(void);

/**
 * returns 32-bit timestamp from system power-up in microseconds
 */
uint32_t lcd_micros(void);

/**
 * Initializes RND device
 * @param seed unique number to use for initialization
 */
void lcd_randomSeed(int seed);

/**
 * Attaches interrupt handler to pin. Not implemented on many platforms
 * @param pin gpio pin number to attach interrupt handler to
 * @param interrupt interrupt handler
 * @param level gpio state to aim interrupt
 */
void attachInterrupt(int pin, void (*interrupt)(), int level);

/**
 * Forces current thread to sleeps for specified number of milliseconds
 * @param ms time in milliseconds
 */
void lcd_delay(unsigned long ms);

/**
 * Forces current thread to sleeps for specified number of microseconds
 * @param us time in microseconds
 */
void lcd_delayUs(unsigned long us);

/**
 * Read single data byte directly from flash. This function is valid only
 * for AVR platform. For other platforms, it reads byte, pointed by ptr.
 * @param ptr pointer to data in flash
 * @return returns single byte read.
 */
uint8_t lcd_pgmReadByte(const void *ptr);

/**
 * Reads 16-bit from eeprom
 * @param ptr pointer to eeprom memory to read
 * @return 16-bit number from eeprom
 */
uint16_t lcd_eepromReadWord(const void *ptr);

/**
 * Writes 16-bit to eeprom
 * @param ptr pointer to eeprom memory to write data to
 * @param val 16-bit value to write
 */
void lcd_eepromWriteWord(const void *ptr, uint16_t val);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/**
 * Returns random number in range [0;max]
 * @param max upper limit for number being generated
 */
int lcd_random(int max);

/**
 * Returns random number in range [min;max]
 * @param min lower limit for number being generated
 * @param max upper limit for number being generated
 */
int lcd_random(int min, int max);
#endif

#ifdef ARDUINO_COMPAT
/** @copydoc LCD_LOW */
#define LOW LCD_LOW
/** @copydoc LCD_HIGH */
#define HIGH LCD_HIGH
/** @copydoc LCD_GPIO_INPUT */
#define INPUT LCD_GPIO_INPUT
/** @copydoc LCD_GPIO_OUTPUT */
#define OUTPUT LCD_GPIO_OUTPUT
#ifndef PROGMEM
/** @copydoc LCD_PROGMEM */
#define PROGMEM LCD_PROGMEM
#endif

/** @copydoc lcd_gpioRead */
#define digitalRead lcd_gpioRead
/** @copydoc digitalRead */
#define digitalWrite lcd_gpioWrite
/** @copydoc lcd_gpioMode */
#define pinMode lcd_gpioMode
/** @copydoc lcd_adcRead */
#define analogRead lcd_adcRead

#if !defined(__AVR__)
/** @copydoc lcd_pgmReadByte */
#define pgm_read_byte lcd_pgmReadByte
/** @copydoc lcd_eepromReadWord */
#define eeprom_read_word lcd_eepromReadWord
/** @copydoc lcd_eepromWriteWord */
#define eeprom_write_word lcd_eepromWriteWord
#endif

/** @copydoc lcd_millis */
#define millis lcd_millis
/** @copydoc lcd_micros */
#define micros lcd_micros
/** @copydoc lcd_delay */
#define delay lcd_delay
/** @copydoc lcd_delayUs */
#define delayMicroseconds lcd_delayUs

/** @copydoc lcd_random */
#define random lcd_random
/** @copydoc lcd_randomSeed */
#define randomSeed lcd_randomSeed

#endif

#endif



#ifdef __cplusplus

////////////////////////////////////////////////////////////////////////////////
//                    I2C PLATFORM
////////////////////////////////////////////////////////////////////////////////

#if defined(CONFIG_ARDUINO_I2C_AVAILABLE) && defined(CONFIG_ARDUINO_I2C_ENABLE)
/**
 * PlatformI2c implementation for current platform.
 */
class PlatformI2c: public ArduinoI2c
{
public:
    /**
     * Creates instance of i2c implementation for current platform.
     * @param config i2c platform configuration. Refer to SPlatformI2cConfig.
     */
     PlatformI2c(const SPlatformI2cConfig &config)
         : ArduinoI2c(config.scl, config.sda, config.addr) {}
};

#elif defined(CONFIG_TWI_I2C_AVAILABLE) && defined(CONFIG_TWI_I2C_ENABLE)

/**
 * PlatformI2c implementation for current platform.
 */
class PlatformI2c: public TwiI2c
{
public:
    /**
     * Creates instance of i2c implementation for current platform.
     * @param config i2c platform configuration. Refer to SPlatformI2cConfig.
     */
    PlatformI2c(const SPlatformI2cConfig &config)
        : TwiI2c(config.addr) {}
};

#elif defined(CONFIG_LINUX_I2C_AVAILABLE) && defined(CONFIG_LINUX_I2C_ENABLE)

#if defined(SDL_EMULATION)
/**
 * PlatformI2c implementation for current platform.
 */
class PlatformI2c: public SdlI2c
{
public:
    /**
     * Creates instance of i2c implementation for current platform.
     * @param config i2c platform configuration. Refer to SPlatformI2cConfig.
     */
    PlatformI2c(const SPlatformI2cConfig &config)
        : SdlI2c(config.scl, config.sda, config.addr) {}
};
#else
/**
 * PlatformI2c implementation for current platform.
 */
class PlatformI2c: public LinuxI2c
{
public:
    /**
     * Creates instance of i2c implementation for current platform.
     * @param config i2c platform configuration. Refer to SPlatformI2cConfig.
     */
    PlatformI2c(const SPlatformI2cConfig &config)
        : LinuxI2c( config.busId, config.addr ) {}
};
#endif

#elif defined(CONFIG_ESP32_I2C_AVAILABLE) && defined(CONFIG_ESP32_I2C_ENABLE)

/**
 * PlatformI2c implementation for current platform.
 */
class PlatformI2c: public EspI2c
{
public:
    /**
     * Creates instance of i2c implementation for current platform.
     * @param config i2c platform configuration. Refer to SPlatformI2cConfig.
     */
    PlatformI2c(const SPlatformI2cConfig &config)
        : EspI2c( config.busId, config.addr, config.scl, config.sda, 400000) {}
};

#elif defined(CONFIG_SOFTWARE_I2C_AVAILABLE) && defined(CONFIG_SOFTWARE_I2C_ENABLE)

/**
 * PlatformI2c implementation for current platform.
 */
class PlatformI2c: public SoftwareI2c
{
public:
    /**
     * Creates instance of i2c implementation for current platform.
     * @param config i2c platform configuration. Refer to SPlatformI2cConfig.
     */
     PlatformI2c(const SPlatformI2cConfig &config)
         : SoftwareI2c(config.scl, config.sda, config.addr) {}
};

#else

#error "Platform not supported"

#endif

////////////////////////////////////////////////////////////////////////////////
//                    SPI PLATFORM
////////////////////////////////////////////////////////////////////////////////

#if defined(CONFIG_AVR_SPI_AVAILABLE) && defined(CONFIG_AVR_SPI_ENABLE)

/**
 * PlatformSpi implementation for current platform
 */
class PlatformSpi: public AvrSpi
{
public:
    /**
     * Creates instance of PlatformSpi implementation for current platform
     * @param config spi platform configuration. Refer to SPlatformSpiConfig.
     */
    PlatformSpi(const SPlatformSpiConfig &config)
        : AvrSpi( config.cs, config.dc, config.frequency ) {}
};

#elif defined(CONFIG_ARDUINO_SPI_AVAILABLE) && defined(CONFIG_ARDUINO_SPI_ENABLE)

/**
 * PlatformSpi implementation for current platform
 */
class PlatformSpi: public ArduinoSpi
{
public:
    /**
     * Creates instance of PlatformSpi implementation for current platform
     * @param config spi platform configuration. Refer to SPlatformSpiConfig.
     */
    PlatformSpi(const SPlatformSpiConfig &config)
        : ArduinoSpi(config.cs, config.dc, config.frequency) {}
};

#elif defined(CONFIG_LINUX_SPI_AVAILABLE) && defined(CONFIG_LINUX_SPI_ENABLE)

#if defined(SDL_EMULATION)
/**
 * PlatformSpi implementation for current platform
 */
class PlatformSpi: public SdlSpi
{
public:
    /**
     * Creates instance of PlatformSpi implementation for current platform
     * @param config spi platform configuration. Refer to SPlatformSpiConfig.
     */
    PlatformSpi(const SPlatformSpiConfig &config)
        : SdlSpi(config.dc) {}
};
#else
/**
 * PlatformSpi implementation for current platform
 */
class PlatformSpi: public LinuxSpi
{
public:
    /**
     * Creates instance of PlatformSpi implementation for current platform
     * @param config spi platform configuration. Refer to SPlatformSpiConfig.
     */
    PlatformSpi(const SPlatformSpiConfig &config)
        : LinuxSpi( config.busId, config.devId, config.dc, config.frequency ) {}
};
#endif

#elif defined(CONFIG_ESP32_SPI_AVAILABLE) && defined(CONFIG_ESP32_SPI_ENABLE)
/**
 * PlatformSpi implementation for current platform
 */
class PlatformSpi: public EspSpi
{
public:
    /**
     * Creates instance of PlatformSpi implementation for current platform
     * @param config spi platform configuration. Refer to SPlatformSpiConfig.
     */
    PlatformSpi(const SPlatformSpiConfig &config)
        : EspSpi( config.busId, config.cs, config.dc, config.frequency ) {}
};

#elif defined(CONFIG_USI_SPI_AVAILABLE) && defined(CONFIG_USI_SPI_ENABLE)

/**
 * PlatformSpi implementation for current platform
 */
class PlatformSpi: public UsiSpi
{
public:
    /**
     * Creates instance of PlatformSpi implementation for current platform
     * @param config spi platform configuration. Refer to SPlatformSpiConfig.
     */
    PlatformSpi(const SPlatformSpiConfig &config)
        : UsiSpi( config.cs, config.dc ) {}
};

#else

#error "Platform not supported"

#endif



extern "C" {
#endif // end ifdef __cplusplus

// !!! PLATFORM I2C IMPLEMENTATION OPTIONAL !!!

/**
 * Structure describes i2c pins
 */
typedef struct
{
    int8_t sda; ///< data pin
    int8_t scl; ///< clock pin
} ssd1306_platform_i2cConfig_t;

/**
 * @brief Initializes i2c interface for platform being used.
 *
 * Initializes i2c interface for platform being used. i2c implementation
 * depends on platform.
 *
 * @param busId i2c bus number. Some platforms have several i2c buses. so, this
 *        argument identifies bus to use. For several platforms busId is not used.
 *        If you want to use default i2c bus for specific platform, please pass -1.
 * @param addr i2c address of oled driver, connected to i2c bus. If you want to use default
 *        i2c display address, please, pass 0.
 * @param cfg Specify scl and sda for the platform. If you want to use default pin numbers,
 *        please pass -1 for both members.
 */
void ssd1306_platform_i2cInit(int8_t busId, uint8_t addr, ssd1306_platform_i2cConfig_t * cfg);

// !!! PLATFORM SPI IMPLEMENTATION OPTIONAL !!!

/**
 * @brief Initializes spi interface for platform being used.
 *
 * Initializes spi interface for platform being used. spi implementation
 * depends on platform.
 *
 * @param busId spi bus number if device has several spi buses. For most AVR platforms
 *        busId is not used. If you want to use default spi bus for specific platform, please
 *        pass -1.
 * @param cesPin chip select (chip enable) pin number. If you want to use default pin,
 *        hard coded by ssd1306 library, please, pass -1. For Linux platform cesPin means
 *        second number in spidev device.
 * @param dcPin data/command pin number. For most oled displays this pin number is used to
 *        select data or command mode for the bus. If you want to use default pin number, please
 *        pass -1.
 */
void ssd1306_platform_spiInit(int8_t busId, int8_t cesPin, int8_t dcPin);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif


