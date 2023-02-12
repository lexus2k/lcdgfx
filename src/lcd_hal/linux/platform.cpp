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

#if ( defined(__linux__) || defined(__APPLE__) ) && !defined(ARDUINO)

#include "../io.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#if defined(__linux__)
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#endif

//#include <cstdlib>

#include <map>

#if defined(CONFIG_LINUX_SPI_AVAILABLE) && defined(CONFIG_LINUX_SPI_ENABLE) && !defined(SDL_EMULATION)
#define LINUX_SPI_AVAILABLE
#endif

#define MAX_GPIO_COUNT 256

#ifdef IN
#undef IN
#endif
#define IN 0

#ifdef OUT
#undef OUT
#endif
#define OUT 1

int gpio_export(int pin)
{
    char buffer[4];
    ssize_t bytes_written;
    int fd;
    char path[64];

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);

    if ( access(path, F_OK) == 0 )
    {
        return 0;
    }

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if ( -1 == fd )
    {
        fprintf(stderr, "Failed to allocate gpio pin[%d]: %s%s!\n", pin, strerror(errno),
                getuid() == 0 ? "" : ", need to be root");
        return (-1);
    }

    bytes_written = snprintf(buffer, sizeof(buffer), "%d", pin);
    if ( write(fd, buffer, bytes_written) < 0 )
    {
        fprintf(stderr, "Failed to allocate gpio pin[%d]: %s%s!\n", pin, strerror(errno),
                getuid() == 0 ? "" : ", need to be root");
        close(fd);
        return -1;
    }
    close(fd);
    return (0);
}

int gpio_unexport(int pin)
{
    char buffer[4];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if ( -1 == fd )
    {
        fprintf(stderr, "Failed to free gpio pin resources!\n");
        return (-1);
    }

    bytes_written = snprintf(buffer, sizeof(buffer), "%d", pin);
    if ( write(fd, buffer, bytes_written) < 0 )
    {
        fprintf(stderr, "Failed to free gpio pin resources!\n");
    }
    close(fd);
    return (0);
}

int gpio_direction(int pin, int dir)
{
    static const char s_directions_str[] = "in\0out";

    char path[64];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if ( -1 == fd )
    {
        fprintf(stderr, "Failed to set gpio pin direction1[%d]: %s!\n", pin, strerror(errno));
        return (-1);
    }

    if ( -1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3) )
    {
        fprintf(stderr, "Failed to set gpio pin direction2[%d]: %s!\n", pin, strerror(errno));
        return (-1);
    }

    close(fd);
    return (0);
}

int gpio_read(int pin)
{
    char path[32];
    char value_str[3];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if ( -1 == fd )
    {
        fprintf(stderr, "Failed to read gpio pin value!\n");
        return (-1);
    }

    if ( -1 == read(fd, value_str, 3) )
    {
        fprintf(stderr, "Failed to read gpio pin value!\n");
        return (-1);
    }

    close(fd);

    return (atoi(value_str));
}

int gpio_write(int pin, int value)
{
    static const char s_values_str[] = "01";

    char path[64];
    int fd;

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if ( -1 == fd )
    {
        fprintf(stderr, "Failed to set gpio pin value[%d]: %s%s!\n", pin, strerror(errno),
                getuid() == 0 ? "" : ", need to be root");
        return (-1);
    }

    if ( 1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1) )
    {
        fprintf(stderr, "Failed to set gpio pin value[%d]: %s%s!\n", pin, strerror(errno),
                getuid() == 0 ? "" : ", need to be root");
        return (-1);
    }

    close(fd);
    return (0);
}

#if defined(__KERNEL__) // ============== KERNEL

int lcd_gpioRead(int pin)
{
    // TODO: Not implemented
    return LCD_LOW;
}

void lcd_gpioWrite(int pin, int level)
{
    // TODO: Not implemented
}

void lcd_delay(unsigned long ms)
{
    // TODO: Not implemented
}

void lcd_delayUs(unsigned long us)
{
    // TODO: Not implemented
}

int lcd_adcRead(int pin)
{
    // TODO: Not implemented
    return 0;
}

uint32_t lcd_millis(void)
{
    // TODO: Not implemented
    return 0;
}

uint32_t lcd_micros(void)
{
    // TODO: Not implemented
    return 0;
}

void lcd_gpioMode(int pin, int mode)
{
    // TODO: Not implemented
}

#elif !defined(SDL_EMULATION)

#ifdef LINUX_SPI_AVAILABLE
typedef struct
{
    void (*on_pin_change)(void *);
    void *arg;
} SPinEvent;
#endif

static uint8_t s_exported_pin[MAX_GPIO_COUNT] = {0};
static uint8_t s_pin_mode[MAX_GPIO_COUNT] = {0};
#ifdef LINUX_SPI_AVAILABLE
std::map<int, SPinEvent> s_events;
#endif

void lcd_gpioMode(int pin, int mode)
{
    if ( !s_exported_pin[pin] )
    {
        if ( gpio_export(pin) < 0 )
        {
            return;
        }
        s_exported_pin[pin] = 1;
    }
    if ( mode == LCD_GPIO_OUTPUT )
    {
        gpio_direction(pin, OUT);
        s_pin_mode[pin] = 1;
    }
    if ( mode == LCD_GPIO_INPUT )
    {
        gpio_direction(pin, IN);
        s_pin_mode[pin] = 0;
    }
}

void lcd_gpioWrite(int pin, int level)
{
#ifdef LINUX_SPI_AVAILABLE
    if ( s_events.find(pin) != s_events.end() )
    {
        s_events[pin].on_pin_change(s_events[pin].arg);
    }
#endif

    if ( !s_exported_pin[pin] )
    {
        if ( gpio_export(pin) < 0 )
        {
            return;
        }
        s_exported_pin[pin] = 1;
    }
    if ( !s_pin_mode[pin] )
    {
        pinMode(pin, OUTPUT);
    }
    gpio_write(pin, level);
}

void lcd_registerGpioEvent(int pin, void (*on_pin_change)(void *), void *arg)
{
#ifdef LINUX_SPI_AVAILABLE
    s_events[pin].arg = arg;
    s_events[pin].on_pin_change = on_pin_change;
#endif
}

void lcd_unregisterGpioEvent(int pin)
{
    s_events.erase(pin);
}

int lcd_gpioRead(int pin)
{
    return gpio_read(pin) ? LCD_HIGH : LCD_LOW;
}

int lcd_adcRead(int pin)
{
    // NOT IMPLEMENTED
    return 0;
}

void lcd_delay(unsigned long ms)
{
    usleep(ms * 1000);
}

void lcd_delayUs(unsigned long us)
{
    usleep(us);
}

uint32_t lcd_millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

uint32_t lcd_micros(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

int min(int a, int b)
{
    return a < b ? a : b;
}
int max(int a, int b)
{
    return a > b ? a : b;
}

#else // SDL_EMULATION

int lcd_gpioRead(int pin)
{
    return sdl_read_digital(pin);
}

int lcd_adcRead(int pin)
{
    return sdl_read_analog(pin);
}

void lcd_gpioWrite(int pin, int level)
{
    sdl_write_digital(pin, level);
}

void lcd_gpioMode(int pin, int mode)
{
    // TODO: Not implemented
}

void lcd_delay(unsigned long ms)
{
    usleep(ms * 1000);
}

void lcd_delayUs(unsigned long us)
{
    usleep(us);
}

uint32_t lcd_millis(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

uint32_t lcd_micros(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
};

int min(int a, int b)
{
    return a < b ? a : b;
}
int max(int a, int b)
{
    return a > b ? a : b;
}

#endif // SDL_EMULATION

void lcd_randomSeed(int seed)
{
    // TODO: Not implemented
}

void lcd_attachInterrupt(int pin, void (*interrupt)(void), int level)
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

int lcd_random(int v)
{
    return rand() % v;
}

int lcd_random(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

#endif // __linux__
