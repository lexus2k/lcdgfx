/*
    MIT License

    Copyright (c) 2018-2022, Alexey Dynda

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
 * @file hal/arduino/arduino_spi.h LCDGFX ARDUINO Interface communication functions
 */

#pragma once

#if defined(CONFIG_ARDUINO_SPI_AVAILABLE) && defined(CONFIG_ARDUINO_SPI_ENABLE)

#include <SPI.h>

/**
 * Class implements SPI support for Arduino platforms
 */
class ArduinoSpi
{
public:
    /**
     * Creates instance of spi implementation for Arduino platform.
     * @param csPin chip select pin to use, -1 if not required
     * @param dcPin data command pin to use
     * @param clkPin clk pin to use, -1 to use default. Doesn't work on all controllers
     * @param mosiPin mosi pin to use, -1 to use default. Doesn't work on all controllers
     * @param freq frequency in HZ to run spi bus at
     */
    ArduinoSpi(int8_t csPin = -1, int8_t dcPin = -1, int8_t clkPin = -1, int8_t mosiPin = -1, uint32_t freq = 0,
               SPIClass *spi = &SPI);
    ~ArduinoSpi();

    /**
     * Initializes spi interface
     */
    void begin();

    /**
     * Closes spi interface
     */
    void end();

    /**
     * Starts communication with SSD1306 display.
     */
    void start();

    /**
     * Ends communication with SSD1306 display.
     */
    void stop();

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data);

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

private:
    int8_t m_cs;
    int8_t m_dc;
    int8_t m_clk;
    int8_t m_mosi;
    uint32_t m_frequency;
    SPIClass *m_spi;
};

#endif

#if defined(CONFIG_ARDUINO_SPI2_AVAILABLE) && defined(CONFIG_ARDUINO_SPI_ENABLE)
/**
 * Class implements second SPI interface for some Arduino platforms
 */
class ArduinoSpi2
{
public:
    /**
     * Creates instance of spi implementation for Arduino platform.
     * @param csPin chip select pin to use, -1 if not required
     * @param dcPin data command pin to use
     * @param clkPin clock pin number to use, -1 if by default
     * @param mosiPin MOSI pin to use, -1 by default
     * @param freq frequency in HZ to run spi bus at
     */
    ArduinoSpi2(int8_t csPin = -1, int8_t dcPin = -1, int8_t clkPin = -1, int8_t mosiPin = -1, uint32_t freq = 8000000);
    ~ArduinoSpi2();

    /**
     * Initializes spi interface
     */
    void begin();

    /**
     * Closes spi interface
     */
    void end();

    /**
     * Starts communication with SSD1306 display.
     */
    void start();

    /**
     * Ends communication with SSD1306 display.
     */
    void stop();

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data);

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

private:
    int8_t m_cs;
    int8_t m_dc;
    int8_t m_clk;
    int8_t m_mosi;
    uint32_t m_frequency;
};

#endif
