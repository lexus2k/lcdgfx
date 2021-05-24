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

#include "../io.h"

#if defined(SSD1306_STM32_PLATFORM)

#include "../interface.h"
#include "stm32f1xx_hal.h"

// TODO: To add support. Any help is welcome

////////////////////////////////////////////////////////////////////////////////////////
// !!! PLATFORM I2C IMPLEMENTATION OPTIONAL !!!
#if defined(CONFIG_PLATFORM_I2C_AVAILABLE) && defined(CONFIG_PLATFORM_I2C_ENABLE)
static uint8_t s_i2c_addr = 0x3C;

static void platform_i2c_start(void)
{
    // ... Open i2c channel for your device with specific s_i2c_addr
}

static void platform_i2c_stop(void)
{
    // ... Complete i2c communication
}

static void platform_i2c_send(uint8_t data)
{
    // ... Send byte to i2c communication channel
}

static void platform_i2c_close(void)
{
    // ... free all i2c resources here
}

static void platform_i2c_send_buffer(const uint8_t *data, uint16_t len)
{
    // ... Send len bytes to i2c communication channel here
}

void ssd1306_platform_i2cInit(int8_t busId, uint8_t addr, int8_t arg)
{
    if ( addr )
        s_i2c_addr = addr;
    if ( HAL_I2C_IsDeviceReady(&hi2c1, s_i2c_addr, 1, 20000) != HAL_OK )
    {
        return;
    }
    ssd1306_intf.spi = 0;
    ssd1306_intf.start = &platform_i2c_start;
    ssd1306_intf.stop = &platform_i2c_stop;
    ssd1306_intf.send = &platform_i2c_send;
    ssd1306_intf.close = &platform_i2c_close;
    ssd1306_intf.send_buffer = &platform_i2c_send_buffer;
    // init your interface here
    //...
}
#endif

////////////////////////////////////////////////////////////////////////////////////////
// !!! PLATFORM SPI IMPLEMENTATION OPTIONAL !!!
#if defined(CONFIG_PLATFORM_SPI_AVAILABLE) && defined(CONFIG_PLATFORM_SPI_ENABLE)

static void platform_spi_start(void)
{
    // ... Open spi channel for your device with specific s_ssd1306_cs, s_ssd1306_dc
}

static void platform_spi_stop(void)
{
    // ... Complete spi communication
}

static void platform_spi_send(uint8_t data)
{
    // ... Send byte to spi communication channel
}

static void platform_spi_close(void)
{
    // ... free all spi resources here
}

static void platform_spi_send_buffer(const uint8_t *data, uint16_t len)
{
    // ... Send len bytes to spi communication channel here
}

void ssd1306_platform_spiInit(int8_t busId, int8_t cesPin, int8_t dcPin)
{
    if ( cesPin >= 0 )
        s_ssd1306_cs = cesPin;
    if ( dcPin >= 0 )
        s_ssd1306_dc = dcPin;
    ssd1306_intf.spi = 1;
    ssd1306_intf.start = &platform_spi_start;
    ssd1306_intf.stop = &platform_spi_stop;
    ssd1306_intf.send = &platform_spi_send;
    ssd1306_intf.close = &platform_spi_close;
    ssd1306_intf.send_buffer = &platform_spi_send_buffer;
    // init your interface here
    //...
}
#endif

int lcd_gpioRead(int pin)
{
    return LCD_LOW;
}

void lcd_gpioWrite(int pin, int level)
{
}

void lcd_gpioMode(int pin, int mode)
{
}

int lcd_adcRead(int pin)
{
    return 0;
}

uint32_t lcd_millis(void)
{
    return 0;
}

uint32_t lcd_micros(void)
{
    return 0;
};

void lcd_delay(unsigned long ms)
{
}

void lcd_delayUs(unsigned long us)
{
}

// !!!  OPTIONAL !!!
void lcd_randomSeed(int seed)
{
}

void lcd_attachInterrupt(int pin, void (*interrupt)(), int level)
{
}

uint8_t lcd_pgmReadByte(const void *ptr)
{
    return *((const uint8_t *)ptr);
}

uint16_t lcd_eepromReadWord(const void *ptr)
{
    return 0;
}

void lcd_eepromWriteWord(const void *ptr, uint16_t val)
{
}

int lcd_random(int max)
{
    return 0;
}

int lcd_random(int min, int max)
{
    return 0;
}

#endif
