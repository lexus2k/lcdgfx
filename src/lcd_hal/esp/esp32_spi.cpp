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

#if defined(CONFIG_ESP32_SPI_AVAILABLE) && defined(CONFIG_ESP32_SPI_ENABLE)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

EspSpi::EspSpi(int8_t busId, int8_t csPin, int8_t dcPin,
               int8_t clk, int8_t mosi, uint32_t frequency)
   : m_busId( busId )
   , m_cs( csPin )
   , m_dc( dcPin )
   , m_clk( clk )
   , m_mosi( mosi )
   , m_first_spi_session( true )
   , m_frequency( frequency )
{
}

EspSpi::~EspSpi()
{
}

void EspSpi::begin()
{
    // Use VSPI by default
    if ( m_busId < 0 ) m_busId = 1;

    // If cesPin is not provided, select by default
    // TODO: Not sure, that we need to set cs pin by default. It should be explicitly specified
//    if ( m_cs < 0)
//    {
//        m_cs = m_busId ? 5 : 15;
//    }
    if (m_cs >=0) lcd_gpioMode( m_cs, LCD_GPIO_OUTPUT );
    if (m_dc >= 0) lcd_gpioMode( m_dc, LCD_GPIO_OUTPUT );

    // init your interface here
    spi_bus_config_t buscfg{};
    // TODO: Do we need really miso pins? We don't use miso line for lcd displays
    buscfg.miso_io_num = m_busId ? 19 : 12;
    buscfg.mosi_io_num = m_mosi > -1 ? m_mosi : ( m_busId ? 23 : 13 );
    buscfg.sclk_io_num = m_clk > -1 ? m_clk : ( m_busId ? 18 : 14 );
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 32;
    spi_bus_initialize( m_busId ? VSPI_HOST : HSPI_HOST, &buscfg, 0 ); // 0 -no dma
    // THIS IS HACK TO GET NOTIFICATIONS ON DC PIN CHANGE
    ssd1306_registerPinEvent(m_dc, OnDcChange, this);
}

void EspSpi::end()
{
    if ( !m_first_spi_session )
    {
        spi_bus_remove_device( m_spi );
    }
    spi_bus_free( m_busId ? VSPI_HOST : HSPI_HOST );
}

void EspSpi::start()
{
    // ... Open spi channel for your device with specific s_ssd1306_cs, s_ssd1306_dc
    if (m_first_spi_session)
    {
        spi_device_interface_config_t devcfg{};
        devcfg.clock_speed_hz = m_frequency;
        devcfg.mode=0;
        devcfg.spics_io_num = m_cs;
        devcfg.queue_size=7;
        spi_bus_add_device( m_busId ? VSPI_HOST : HSPI_HOST, &devcfg, &m_spi);
        m_first_spi_session = false;
    }
}

void EspSpi::stop()
{
    forceSpiTransfer();
}

void EspSpi::send(uint8_t data)
{
    m_buffer[m_data_size] = data;
    m_data_size++;
    if ( m_data_size == sizeof(m_buffer) )
    {
        forceSpiTransfer();
    }
    // ... Send byte to spi communication channel
    // We do not care here about DC line state, because
    // ssd1306 library already set DC pin via ssd1306_spiDataMode() before call to send().
//    spi_transaction_t t;
//    memset(&t, 0, sizeof(t));
//    t.length = 8;          // 8 bits
//    t.tx_buffer=&data;
//    spi_device_transmit(m_spi, &t);  // Transmit!
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
        size_t sz = m_data_size > 64 ? 64: m_data_size;
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length=8*sz;          // 8 bits
        t.tx_buffer=buffer;
        spi_device_transmit(m_spi, &t);
        buffer += sz;
        m_data_size-=sz;
    }
    m_data_size = 0;
}

void EspSpi::OnDcChange(void *arg)
{
    EspSpi *obj = reinterpret_cast<EspSpi*>(arg);
    obj->forceSpiTransfer();
}

void EspSpi::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    // ... Send len bytes to spi communication channel here
    while (size)
    {
        send(*buffer);
        size--;
        buffer++;
    }
}

#endif
