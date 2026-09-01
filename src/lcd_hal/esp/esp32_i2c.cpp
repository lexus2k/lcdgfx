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

#if defined(CONFIG_ESP32_I2C_AVAILABLE) && defined(CONFIG_ESP32_I2C_ENABLE)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdio.h>
#include "driver/i2c.h"
#include "esp_log.h"
#include "soc/soc_caps.h"

// I2C_NUM_1 only exists on chips with two high-power I2C peripherals - ESP-IDF
// gates the enumerator on SOC_HP_I2C_NUM in hal/i2c_types.h, and C3/C5/C6/H2
// have a single one. Mirror that condition instead of naming the port blindly.
#if defined(SOC_HP_I2C_NUM) && (SOC_HP_I2C_NUM >= 2)
#define LCDGFX_I2C_DEFAULT_PORT I2C_NUM_1
#else
#define LCDGFX_I2C_DEFAULT_PORT I2C_NUM_0
#endif

static const char *LCDGFX_I2C_TAG = "lcdgfx.i2c";

EspI2c::EspI2c(int8_t busId, uint8_t sa, int8_t scl, int8_t sda, uint32_t frequency)
    : m_busId(busId)
    , m_sa(sa)
    , m_scl(scl)
    , m_sda(sda)
    , m_frequency(frequency)
{
}

EspI2c::~EspI2c()
{
}

void EspI2c::begin()
{
    if ( m_busId < 0 )
        m_busId = LCDGFX_I2C_DEFAULT_PORT;
#if !defined(SOC_HP_I2C_NUM) || (SOC_HP_I2C_NUM < 2)
    // Only one port exists on this chip, so an explicitly requested bus id
    // cannot be honoured - fall back to the single available port.
    m_busId = I2C_NUM_0;
#endif
    if ( m_sda < 0 )
        m_sda = 21;
    if ( m_scl < 0 )
        m_scl = 22;
    i2c_config_t conf{};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = static_cast<gpio_num_t>(m_sda);
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = static_cast<gpio_num_t>(m_scl);
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = m_frequency;
    esp_err_t err = i2c_param_config(static_cast<i2c_port_t>(m_busId), &conf);
    if ( err != ESP_OK )
    {
        ESP_LOGE(LCDGFX_I2C_TAG, "i2c_param_config(port=%d, sda=%d, scl=%d) failed: %s", static_cast<int>(m_busId),
                 m_sda, m_scl, esp_err_to_name(err));
        return;
    }
    err = i2c_driver_install(static_cast<i2c_port_t>(m_busId), conf.mode, 0, 0, 0);
    if ( err != ESP_OK )
    {
        ESP_LOGE(LCDGFX_I2C_TAG, "i2c_driver_install(port=%d) failed: %s", static_cast<int>(m_busId),
                 esp_err_to_name(err));
    }
}

void EspI2c::end()
{
    i2c_driver_delete(static_cast<i2c_port_t>(m_busId));
}

void EspI2c::start()
{
    m_cmd_handle = i2c_cmd_link_create();
    i2c_master_start(m_cmd_handle);
    i2c_master_write_byte(m_cmd_handle, (m_sa << 1) | I2C_MASTER_WRITE, 0x1);
}

#if !defined(portTICK_RATE_MS)
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

void EspI2c::stop()
{
    i2c_master_stop(m_cmd_handle);
    esp_err_t err = i2c_master_cmd_begin(static_cast<i2c_port_t>(m_busId), m_cmd_handle, 1000 / portTICK_RATE_MS);
    if ( err != ESP_OK )
    {
        // Typically ESP_FAIL when the display does not ACK its address, or
        // ESP_ERR_TIMEOUT when SDA/SCL are not pulled up.
        ESP_LOGE(LCDGFX_I2C_TAG, "i2c transfer to address 0x%02X failed: %s", m_sa, esp_err_to_name(err));
    }
    i2c_cmd_link_delete(m_cmd_handle);
}

void EspI2c::send(uint8_t data)
{
    i2c_master_write_byte(m_cmd_handle, data, 0x1);
}

void EspI2c::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    while ( size-- )
    {
        send(*buffer);
        buffer++;
    }
}

#endif
