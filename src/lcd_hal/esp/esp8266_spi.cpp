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

#include "../io.h"

#if defined(CONFIG_ESP8266_SPI_AVAILABLE) && defined(CONFIG_ESP8266_SPI_ENABLE)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi.h"

static uint8_t start_bit = 0;

EspSpi::EspSpi(int8_t busId, int8_t csPin, int8_t dcPin, int8_t clk, int8_t mosi, uint32_t frequency)
    : m_busId(busId)
    , m_cs(csPin)
    , m_dc(dcPin)
    , m_clk(clk)
    , m_mosi(mosi)
    , m_frequency(frequency)
{
}

EspSpi::~EspSpi()
{
}

static void IRAM_ATTR spi_event_callback(int event, void *arg)
{
    switch ( event )
    {
        case SPI_INIT_EVENT:
        {
        }
        break;

        case SPI_TRANS_START_EVENT:
        {
            start_bit = 1;
        }
        break;

        case SPI_TRANS_DONE_EVENT:
        {
            start_bit = 0;
        }
        break;

        case SPI_DEINIT_EVENT:
        {
        }
        break;
    }
}

void EspSpi::begin()
{
    // Use HSPI by default, because CSPI is connected to flash
    if ( m_busId < 0 )
        m_busId = 1;
    m_spi = m_busId ? HSPI_HOST : CSPI_HOST;
    // If cesPin is not provided, select by default
    // TODO: Not sure, that we need to set cs pin by default. It should be explicitly specified
    //    if ( m_cs < 0)
    //    {
    //        m_cs = m_busId ? 12 : 5;
    //    }
    if ( m_cs >= 0 )
        lcd_gpioMode(m_cs, LCD_GPIO_OUTPUT);
    if ( m_dc >= 0 )
    {
        lcd_gpioMode(m_dc, LCD_GPIO_OUTPUT);
    }

    // init your interface here
    spi_config_t buscfg{};
    buscfg.interface.val = SPI_DEFAULT_INTERFACE;
    buscfg.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
    buscfg.interface.cs_en = 0;
    buscfg.interface.miso_en = 0;
    buscfg.interface.mosi_en = 1;
    buscfg.interface.cpol = 0;
    buscfg.interface.cpha = 0;
    buscfg.mode = SPI_MASTER_MODE;
    buscfg.clk_div = SPI_10MHz_DIV;
    if ( m_frequency < 4000000 )
        buscfg.clk_div = SPI_2MHz_DIV;
    else if ( m_frequency < 5000000 )
        buscfg.clk_div = SPI_4MHz_DIV;
    else if ( m_frequency < 8000000 )
        buscfg.clk_div = SPI_5MHz_DIV;
    else if ( m_frequency < 10000000 )
        buscfg.clk_div = SPI_8MHz_DIV;
    else if ( m_frequency < 16000000 )
        buscfg.clk_div = SPI_10MHz_DIV;
    else if ( m_frequency < 20000000 )
        buscfg.clk_div = SPI_16MHz_DIV;
    else if ( m_frequency < 40000000 )
        buscfg.clk_div = SPI_20MHz_DIV;
    else if ( m_frequency < 80000000 )
        buscfg.clk_div = SPI_40MHz_DIV;
    buscfg.event_cb = spi_event_callback;
    spi_init(m_spi, &buscfg);
    // THIS IS HACK TO GET NOTIFICATIONS ON DC PIN CHANGE
    if ( m_dc >= 0 )
        lcd_registerGpioEvent(m_dc, OnDcChange, this);
}

void EspSpi::end()
{
    spi_deinit(m_spi);
    if ( m_dc >= 0 )
        lcd_unregisterGpioEvent(m_dc);
}

void EspSpi::start()
{
}

void EspSpi::stop()
{
    forceSpiTransfer();
}

void EspSpi::send(uint8_t data)
{
    // We need to fill array in specific way according to ESP8266
    // spi features: 16 32-bit registers, transfer starts from MSB,
    // which corresponds in LE to the 3-rd byte in 4-byte buffer
    m_buffer[(m_data_size & 0xFC) | (3 - (m_data_size & 0x03))] = data;
    m_data_size++;
    if ( m_data_size == sizeof(m_buffer) )
    {
        // ... Send byte to spi communication channel
        // We do not care here about DC line state, because
        // lcdgfx library already set DC pin via setDataMode() before call to send().
        forceSpiTransfer();
    }
}

void EspSpi::forceSpiTransfer()
{
    if ( !m_data_size )
    {
        return;
    }
    uint8_t *buffer = m_buffer;
    while ( m_data_size )
    {
        // The max size must be divided by 4!!!
        size_t sz = m_data_size > 32 ? 32 : m_data_size;
        spi_trans_t t{};
        t.bits.mosi = 8 * sz; // 8 bits
        t.mosi = reinterpret_cast<uint32_t *>(buffer);
        spi_trans(m_spi, t);
        buffer += sz;
        m_data_size -= sz;
    }
    // Wait until transaction ends
    while ( start_bit > 0 )
    {
        lcd_delay(0);
    };

    m_data_size = 0;
}

void EspSpi::OnDcChange(void *arg)
{
    EspSpi *obj = reinterpret_cast<EspSpi *>(arg);
    obj->forceSpiTransfer();
}

void EspSpi::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    // ... Send len bytes to spi communication channel here
    while ( size )
    {
        send(*buffer);
        size--;
        buffer++;
    }
}

#endif
