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

EspSpi::EspSpi(int8_t busId, int8_t csPin, int8_t dcPin,
               int8_t clk, int8_t mosi, uint32_t frequency)
   : m_busId( busId )
   , m_cs( csPin )
   , m_dc( dcPin )
   , m_clk( clk )
   , m_mosi( mosi )
//   , m_first_spi_session( true )
   , m_frequency( frequency )
{
}

EspSpi::~EspSpi()
{
}

/*
static void IRAM_ATTR spi_event_callback(int event, void *arg)
{
    switch (event) {
        case SPI_INIT_EVENT: {

        }
        break;

        case SPI_TRANS_START_EVENT: {
            gpio_set_level(OLED_DC_GPIO, oled_dc_level);
        }
        break;

        case SPI_TRANS_DONE_EVENT: {

        }
        break;

        case SPI_DEINIT_EVENT: {
        }
        break;
    }
}
*/

void EspSpi::begin()
{
    // Use HSPI by default
    if ( m_busId < 0 ) m_busId = 0;
    m_spi = m_busId ? CSPI_HOST : HSPI_HOST;
    // If cesPin is not provided, select by default
    // TODO: Not sure, that we need to set cs pin by default. It should be explicitly specified
//    if ( m_cs < 0)
//    {
//        m_cs = m_busId ? 5 : 15;
//    }
    if (m_cs >=0) lcd_gpioMode( m_cs, LCD_GPIO_OUTPUT );
    if (m_dc >= 0) lcd_gpioMode( m_dc, LCD_GPIO_OUTPUT );

    // init your interface here
    spi_config_t buscfg{};
    buscfg.interface.val = SPI_DEFAULT_INTERFACE;
    buscfg.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
    buscfg.interface.cs_en = 0;
    buscfg.interface.miso_en = 0;
    buscfg.interface.cpol = 1;
    buscfg.interface.cpha = 1;
    buscfg.mode = SPI_MASTER_MODE;
    buscfg.clk_div = SPI_10MHz_DIV;
//    buscfg.event_cb = spi_event_callback;
    spi_init( m_spi, &buscfg );
    // THIS IS HACK TO GET NOTIFICATIONS ON DC PIN CHANGE
    ssd1306_registerPinEvent(m_dc, OnDcChange, this);
}

void EspSpi::end()
{
/*    if ( !m_first_spi_session )
    {
        spi_bus_remove_device( m_spi );
    }*/
    spi_deinit( m_spi );
//    spi_bus_free( m_spi );
}

void EspSpi::start()
{
    // ... Open spi channel for your device with specific s_ssd1306_cs, s_ssd1306_dc
/*    if (m_first_spi_session)
    {
        spi_device_interface_config_t devcfg{};
        devcfg.clock_speed_hz = m_frequency;
        devcfg.mode=0;
        devcfg.spics_io_num = m_cs;
        devcfg.queue_size=7;
        spi_bus_add_device( m_spi, &devcfg, &m_spi);
        m_first_spi_session = false;
    }*/
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
        spi_trans_t t{};
        t.bits.mosi = 8 * sz;          // 8 bits
        t.mosi = reinterpret_cast<uint32_t *>(buffer);
        spi_trans( m_spi , t);
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
