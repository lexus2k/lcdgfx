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

#if defined(__MINGW32__)

#include <cstdlib>

int lcd_gpioRead(int pin)
{
    return sdl_read_digital(pin);
};
void lcd_delay(unsigned long ms)
{
    Sleep(ms);
};
void lcd_delayUs(unsigned long us)
{
    Sleep((us + 500) / 1000);
};
uint32_t lcd_millis(void)
{
    return GetTickCount();
};

uint32_t lcd_micros(void)
{
    return GetTickCount() * 1000;
};

#if defined(SDL_EMULATION)
int lcd_gpioRead(int pin)
{
    return sdl_read_analog(pin);
};
void lcd_gpioWrite(int pin, int level)
{
    sdl_write_digital(pin, level);
};
void lcd_gpioMode(int pin, int mode){};
#endif

void lcd_randomSeed(int seed){};
void lcd_attachInterrupt(int pin, void (*interrupt)(void), int level){};
uint8_t lcd_pgmReadByte(const void *ptr)
{
    return *((const uint8_t *)ptr);
};
uint16_t lcd_eepromReadWord(const void *ptr)
{
    return 0;
};
void lcd_eepromWriteWord(const void *ptr, uint16_t val){};

int lcd_random(int v)
{
    return rand() % v;
}

int lcd_random(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

#endif
