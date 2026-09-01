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

#if defined(CONFIG_ESP32_SPI_AVAILABLE) && defined(CONFIG_ESP32_SPI_ENABLE)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

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

#include "soc/soc_caps.h"
#if defined(__has_include)
#if __has_include("soc/spi_pins.h")
#include "soc/spi_pins.h"
#endif
#endif

// Do not rely on the legacy HSPI_HOST/VSPI_HOST aliases. ESP-IDF defines them only
// for ESP32 and ESP32-S2 (see hal/spi_types.h, "deprecated for the chips after
// esp32s2"), so every newer target - C3, C5, C6, H2, P4 - fails to compile against
// them. Map the lcdgfx bus id onto a real host through the SoC capabilities
// instead, which every target provides.
//
// Note SPI1_HOST is the flash bus and must never be handed to the SPI master
// driver, so targets with a single usable host always resolve to SPI2_HOST.
#if SOC_SPI_PERIPH_NUM > 2
#define LCDGFX_SPI_HOST(busId) ((busId) ? SPI3_HOST : SPI2_HOST)
#else
#define LCDGFX_SPI_HOST(busId) (SPI2_HOST)
#endif

// Default bus pins. The historical ESP32 values are kept as-is to not change
// behaviour for existing sketches; other targets fall back to the IOMUX pins
// that ESP-IDF documents for SPI2, so no per-chip table has to be maintained here.
#if defined(CONFIG_IDF_TARGET_ESP32)
#define LCDGFX_SPI_DEFAULT_MOSI(busId) ((busId) ? 23 : 13)
#define LCDGFX_SPI_DEFAULT_CLK(busId) ((busId) ? 18 : 14)
#elif defined(SPI2_IOMUX_PIN_NUM_MOSI) && defined(SPI2_IOMUX_PIN_NUM_CLK)
#define LCDGFX_SPI_DEFAULT_MOSI(busId) (SPI2_IOMUX_PIN_NUM_MOSI)
#define LCDGFX_SPI_DEFAULT_CLK(busId) (SPI2_IOMUX_PIN_NUM_CLK)
#else
// soc/spi_pins.h is unavailable on this SDK. Keep the historical numbers so the
// build still succeeds; pass clk/mosi explicitly to the display constructor.
#warning "lcdgfx: no per-chip default SPI pins for this target, specify clk/mosi explicitly"
#define LCDGFX_SPI_DEFAULT_MOSI(busId) ((busId) ? 23 : 13)
#define LCDGFX_SPI_DEFAULT_CLK(busId) ((busId) ? 18 : 14)
#endif

static const char *LCDGFX_SPI_TAG = "lcdgfx.spi";

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
        m_mosi = LCDGFX_SPI_DEFAULT_MOSI(m_busId);
    if ( m_clk < 0 )
        m_clk = LCDGFX_SPI_DEFAULT_CLK(m_busId);
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
    esp_err_t err;
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
        ESP_LOGW(LCDGFX_SPI_TAG, "DMA buffer allocation failed, falling back to non-DMA transfers");
        m_txbuf = m_buffer;
        m_capacity = sizeof(m_buffer);
        buscfg.max_transfer_sz = sizeof(m_buffer);
        err = spi_bus_initialize(LCDGFX_SPI_HOST(m_busId), &buscfg, 0); // 0 - no dma
    }
    else
    {
        buscfg.max_transfer_sz = m_capacity;
        err = spi_bus_initialize(LCDGFX_SPI_HOST(m_busId), &buscfg, LCDGFX_ESP_DMA_CHAN);
    }
#else
    buscfg.max_transfer_sz = 32;
    err = spi_bus_initialize(LCDGFX_SPI_HOST(m_busId), &buscfg, 0); // 0 -no dma
#endif
    if ( err != ESP_OK )
    {
        // Without this the display simply stays blank with no explanation - the
        // failure used to be discarded here, which made a misconfigured bus
        // (wrong host, pins already in use) impossible to diagnose.
        ESP_LOGE(LCDGFX_SPI_TAG, "spi_bus_initialize(host=%d, mosi=%d, clk=%d) failed: %s",
                 static_cast<int>(LCDGFX_SPI_HOST(m_busId)), m_mosi, m_clk, esp_err_to_name(err));
    }
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
        if ( m_spi != nullptr )
        {
            spi_bus_remove_device(m_spi);
            m_spi = nullptr;
        }
        m_first_spi_session = true;
    }
    esp_err_t err = spi_bus_free(LCDGFX_SPI_HOST(m_busId));
    if ( err != ESP_OK )
    {
        ESP_LOGE(LCDGFX_SPI_TAG, "spi_bus_free(host=%d) failed: %s",
                 static_cast<int>(LCDGFX_SPI_HOST(m_busId)), esp_err_to_name(err));
    }
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
        esp_err_t err = spi_bus_add_device(LCDGFX_SPI_HOST(m_busId), &devcfg, &m_spi);
        if ( err != ESP_OK )
        {
            // Leave m_spi null rather than transmitting on an uninitialised
            // handle; forceSpiTransfer() drops data while the device is absent.
            ESP_LOGE(LCDGFX_SPI_TAG, "spi_bus_add_device(host=%d, cs=%d) failed: %s",
                     static_cast<int>(LCDGFX_SPI_HOST(m_busId)), m_cs, esp_err_to_name(err));
            m_spi = nullptr;
        }
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
    if ( m_spi == nullptr )
    {
        // Bus or device init failed earlier and was already reported. Drop the
        // pending bytes so the buffer cannot overflow on the next send().
        m_data_size = 0;
        return;
    }
    while ( m_data_size )
    {
        size_t sz = m_data_size > m_capacity ? m_capacity : m_data_size;
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = 8 * sz; // 8 bits
        t.tx_buffer = buffer;
        // Do not set this flag, it will break SPI communication with many LCD displays
        // t.flags = SPI_TRANS_CS_KEEP_ACTIVE;
        esp_err_t err = spi_device_transmit(m_spi, &t);
        if ( err != ESP_OK )
        {
            ESP_LOGE(LCDGFX_SPI_TAG, "spi_device_transmit(%u bytes) failed: %s", static_cast<unsigned>(sz),
                     esp_err_to_name(err));
            m_data_size = 0;
            return;
        }
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
