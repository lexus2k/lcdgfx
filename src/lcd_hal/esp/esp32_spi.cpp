/*
    MIT License

    Copyright 2018-2020,2022 (C) Alexey Dynda

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

#if defined(CONFIG_ESP32_SPI_AVAILABLE) && defined(CONFIG_ESP32_SPI_ENABLE) && defined(ESP32)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#if defined(CONFIG_ESP32_SPI_DMA_ENABLE)
#include "esp_heap_caps.h"
// Size of the DMA-capable flush buffer. Larger values let bulk pixel writes
// (e.g. a NanoCanvas flush) go out as fewer, bigger DMA transactions, which is
// where the throughput win comes from. Tune per available SRAM.
#ifndef CONFIG_ESP32_SPI_DMA_BUFFER_SIZE
#define CONFIG_ESP32_SPI_DMA_BUFFER_SIZE 512
#endif
// SPI_DMA_CH_AUTO exists since ESP-IDF v4.3; fall back to channel 1 on older IDF.
#if defined(SPI_DMA_CH_AUTO)
#define LCDGFX_ESP_DMA_CHAN SPI_DMA_CH_AUTO
#else
#define LCDGFX_ESP_DMA_CHAN 1
#endif
#endif

#if !defined(HSPI_HOST) || !defined(VSPI_HOST)
  #if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32C3)
    #define HSPI_HOST   SPI2_HOST
    #if defined(SPI3_HOST)
      #define VSPI_HOST   SPI3_HOST
    #else
      #define VSPI_HOST   SPI1_HOST
    #endif
  #elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    // SPI_HOST (SPI1_HOST) is not supported by the SPI Master and SPI Slave driver on ESP32-S2 and later
    // #define SPI_HOST    SPI1_HOST
    // #define FSPI_HOST   SPI2_HOST
    #define HSPI_HOST   SPI3_HOST
    #if defined(SPI3_HOST)
      #define VSPI_HOST   SPI3_HOST
    #else
      #define VSPI_HOST   SPI1_HOST
    #endif
  #endif
#endif

// Uncomment to use software SPI (for debugging)
// #define USE_SW_SPI

EspSpi::EspSpi(int8_t busId, int8_t csPin, int8_t dcPin, int8_t clk, int8_t mosi, uint32_t frequency)
    : m_busId(busId)
    , m_cs(csPin)
    , m_dc(dcPin)
    , m_clk(clk)
    , m_mosi(mosi)
    , m_first_spi_session(true)
    , m_frequency(frequency)
{
}

EspSpi::~EspSpi()
{
}

void EspSpi::begin()
{
    // Use VSPI by default
    if ( m_busId < 0 )
        m_busId = 1;

    if ( m_mosi < 0 )
        m_mosi = m_busId ? 23 : 13;
    if ( m_clk < 0 )
        m_clk = m_busId ? 18 : 14;
    if ( m_cs >= 0 )
    {
        lcd_gpioMode(m_cs, LCD_GPIO_OUTPUT);
        lcd_gpioWrite(m_cs, LCD_HIGH);
    }
    if ( m_dc >= 0 )
        lcd_gpioMode(m_dc, LCD_GPIO_OUTPUT);
    if ( m_clk >= 0 )
    {
        lcd_gpioMode(m_clk, LCD_GPIO_OUTPUT);
        // Initialize Serial interface of ST7789 controller
        // lcd_gpioWrite(m_clk, LCD_HIGH);
    }

#ifdef USE_SW_SPI
    if ( m_mosi >= 0 )
    {
        lcd_gpioMode(m_mosi, LCD_GPIO_OUTPUT);
    }
#else
    // init your interface here
    spi_bus_config_t buscfg{};
    buscfg.miso_io_num = -1; // Do not use MISO line
    buscfg.mosi_io_num = m_mosi;
    buscfg.sclk_io_num = m_clk;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
#if defined(CONFIG_ESP32_SPI_DMA_ENABLE)
    // Opt-in DMA path. Allocate a DMA-capable, word-aligned flush buffer (ESP-IDF
    // requires DMA tx buffers to live in DMA-capable memory) and let the SPI
    // master use a DMA channel. Transfers stay blocking, so the DC-line ordering
    // (flush-before-DC-toggle) is preserved exactly as in the non-DMA path.
    m_capacity = CONFIG_ESP32_SPI_DMA_BUFFER_SIZE;
    m_txbuf = static_cast<uint8_t *>(heap_caps_malloc(m_capacity, MALLOC_CAP_DMA | MALLOC_CAP_8BIT));
    if ( m_txbuf == nullptr )
    {
        // Allocation failed: fall back to the internal buffer and disable DMA.
        m_txbuf = m_buffer;
        m_capacity = sizeof(m_buffer);
        buscfg.max_transfer_sz = sizeof(m_buffer);
        spi_bus_initialize(m_busId ? VSPI_HOST : HSPI_HOST, &buscfg, 0); // 0 - no dma
    }
    else
    {
        buscfg.max_transfer_sz = m_capacity;
        spi_bus_initialize(m_busId ? VSPI_HOST : HSPI_HOST, &buscfg, LCDGFX_ESP_DMA_CHAN);
    }
#else
    buscfg.max_transfer_sz = 32;
    spi_bus_initialize(m_busId ? VSPI_HOST : HSPI_HOST, &buscfg, 0); // 0 -no dma
#endif
#endif
    // THIS IS HACK TO GET NOTIFICATIONS ON DC PIN CHANGE
    if ( m_dc >= 0 )
        lcd_registerGpioEvent(m_dc, OnDcChange, this);
}

void EspSpi::end()
{
#ifdef USE_SW_SPI

#else
    if ( !m_first_spi_session )
    {
        spi_bus_remove_device(m_spi);
        m_first_spi_session = true;
    }
    spi_bus_free(m_busId ? VSPI_HOST : HSPI_HOST);
#if defined(CONFIG_ESP32_SPI_DMA_ENABLE)
    // Release the DMA-capable flush buffer (only ever heap-allocated in begin()).
    if ( m_txbuf != m_buffer )
    {
        heap_caps_free(m_txbuf);
        m_txbuf = m_buffer;
        m_capacity = sizeof(m_buffer);
    }
#endif
#endif
    if ( m_dc >= 0 )
        lcd_unregisterGpioEvent(m_dc);
}

void EspSpi::start()
{
#ifdef USE_SW_SPI
    // Going low before CS signal, since some controllers capture data on the falling edge
    lcd_gpioWrite(m_clk, LCD_LOW);
    if ( m_cs > -1 )
    {
        lcd_gpioWrite(m_cs, LCD_LOW);
    }
#else
    // ... Open spi channel for your device with specific s_ssd1306_cs, s_ssd1306_dc
    if ( m_first_spi_session )
    {
        spi_device_interface_config_t devcfg{};
        devcfg.clock_speed_hz = m_frequency;
        devcfg.cs_ena_pretrans = 1;
        devcfg.cs_ena_posttrans = 1;
        devcfg.mode = 0;
        devcfg.spics_io_num = m_cs;
        devcfg.queue_size = 7;
        spi_bus_add_device(m_busId ? VSPI_HOST : HSPI_HOST, &devcfg, &m_spi);
        m_first_spi_session = false;
    }
#endif
}

void EspSpi::stop()
{
    forceSpiTransfer();
#ifdef USE_SW_SPI
    if ( m_cs > -1 )
    {
        lcd_gpioWrite(m_cs, LCD_HIGH);
    }
    lcd_gpioWrite(m_clk, LCD_HIGH);
#endif
}

void EspSpi::send(uint8_t data)
{
    m_txbuf[m_data_size] = data;
    m_data_size++;
    if ( m_data_size == m_capacity )
    {
        // ... Send byte to spi communication channel
        // We do not care here about DC line state, because
        // lcdgfx library has already set DC pin via setDataMode() before call to send().
        forceSpiTransfer();
    }
}

void EspSpi::forceSpiTransfer()
{
    if ( !m_data_size )
    {
        return;
    }
    uint8_t *buffer = m_txbuf;
#ifdef USE_SW_SPI
    while ( m_data_size )
    {
        uint8_t byte = *buffer;
        for ( int i = 0; i < 8; i++ )
        {
            lcd_gpioWrite(m_mosi, (byte & 0x80) ? LCD_HIGH : LCD_LOW);
            lcd_gpioWrite(m_clk, LCD_HIGH);
            byte <<= 1;
            lcd_gpioWrite(m_clk, LCD_LOW);
        }
        buffer++;
        m_data_size--;
    }
#else
    while ( m_data_size )
    {
        size_t sz = m_data_size > m_capacity ? m_capacity : m_data_size;
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = 8 * sz; // 8 bits
        t.tx_buffer = buffer;
        // Do not set this flag, it will break SPI communication with many LCD displays
        // t.flags = SPI_TRANS_CS_KEEP_ACTIVE;
        spi_device_transmit(m_spi, &t);
        buffer += sz;
        m_data_size -= sz;
    }
#endif
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
