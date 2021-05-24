/*
    MIT License

    Copyright (c) 2018-2020, Alexey Dynda

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

#if defined(__XTENSA__) && !defined(ARDUINO)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <map>

typedef struct
{
    void (*on_pin_change)(void *);
    void *arg;
} SPinEvent;

static std::map<int, SPinEvent> s_events;

void lcd_registerGpioEvent(int pin, void (*on_pin_change)(void *), void *arg)
{
    s_events[pin].arg = arg;
    s_events[pin].on_pin_change = on_pin_change;
}

void lcd_unregisterGpioEvent(int pin)
{
    s_events.erase(pin);
}

int lcd_gpioRead(int pin)
{
    return gpio_get_level(static_cast<gpio_num_t>(pin));
}

void lcd_gpioWrite(int pin, int level)
{
    if ( s_events.find(pin) != s_events.end() )
    {
        s_events[pin].on_pin_change(s_events[pin].arg);
    }
    gpio_set_level(static_cast<gpio_num_t>(pin), level);
}

void lcd_gpioMode(int pin, int mode)
{
    if ( mode == LCD_GPIO_INPUT )
        gpio_set_direction(static_cast<gpio_num_t>(pin), GPIO_MODE_INPUT);
    else if ( mode == LCD_GPIO_OUTPUT )
        gpio_set_direction(static_cast<gpio_num_t>(pin), GPIO_MODE_OUTPUT);
}

uint32_t lcd_millis(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

void lcd_delay(unsigned long ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

int lcd_adcRead(int pin)
{
    // TODO: Not implemented
    return 0;
}

uint32_t lcd_micros(void)
{
    // TODO: Not implemented
    return 0;
}

void lcd_delayUs(unsigned long us)
{
    // TODO: Not implemented
}

void lcd_randomSeed(int seed)
{
    // TODO: Not implemented
}

void lcd_attachInterrupt(int pin, void (*interrupt)(), int level)
{
    // TODO: Not implemented
}

uint8_t lcd_pgmReadByte(const void *ptr)
{
    return *((const uint8_t *)ptr);
}

uint16_t lcd_eepromReadWord(const void *ptr)
{
    // TODO: Not implemented
    return 0;
}

void lcd_eepromWriteWord(const void *ptr, uint16_t val)
{
    // TODO: Not implemented
}

int lcd_random(int max)
{
    // TODO: Not implemented
    return 0;
}

int lcd_random(int min, int max)
{
    // TODO: Not implemented
    return 0;
}

#endif
