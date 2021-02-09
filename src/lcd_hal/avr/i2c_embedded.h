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
 * @file lcd_hal/avr/i2c_embedded.h embedded LCDGFX AVR i2c communication functions
 */

#pragma once

#if defined(CONFIG_SOFTWARE_I2C_AVAILABLE) && defined(CONFIG_SOFTWARE_I2C_ENABLE)

#include <stdint.h>

/**
 * Class implements i2c software interface for AVR platform
 */
class SoftwareI2c
{
public:
    /**
     * Creates i2c interface instance for SDL Emulation mode
     *
     * @param scl pin number to use as clock
     * @param sda pin number to use as data line
     * @param sa i2c address of the display (7 bits)
     */
    SoftwareI2c(int8_t scl = -1, int8_t sda = -1, uint8_t sa = 0x00);

    ~SoftwareI2c();

    /**
     * Initializes i2c interface
     */
    void begin();

    /**
     * Closes i2c interface
     */
    void end();

    /**
     * Starts communication with slave device.
     */
    void start();

    /**
     * Starts communication with slave device.
     * @param sa slave address (low 7 bits)
     * @param read_op set it to true, if read operation is requested, otherwise set it to false
     */
    void start(uint8_t sa, bool read_op = false);

    /**
     * Ends communication with slave display.
     */
    void stop();

    /**
     * Sends byte to slave device
     * @param data - byte to send
     */
    void send(uint8_t data);

    /**
     * Receives byte from remote device
     * @param last set to true, if last byte is requested from i2c slave
     * @return byte read from slave device
     */
    uint8_t receive(bool last = false);

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size);

    /**
     * Sets i2c address for communication
     * This API is required for some led displays having multiple
     * i2c addresses for different types of data.
     *
     * @param addr i2c address to set (7 bits)
     */
    void setAddr(uint8_t addr)
    {
        m_sa = addr;
    }

private:
    int8_t m_scl;
    int8_t m_sda;
    uint8_t m_sa;
};

#endif
