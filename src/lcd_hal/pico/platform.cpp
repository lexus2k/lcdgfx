/*
    MIT License

    Copyright (c) 2016-2020, Alexey Dynda

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

#if defined(PICO_BOARD)

void lcd_gpioMode(int pin, int mode)
{
    if(pin < 0)
        return;
    
    gpio_init(pin);
    switch(mode){
        case LCD_GPIO_OUTPUT:
            gpio_set_dir(pin, GPIO_OUT);
            break;
        case LCD_GPIO_INPUT:
            gpio_set_dir(pin, GPIO_IN);
            break;
        case LCD_GPIO_INPUT_PULLUP:
            gpio_set_dir(pin, GPIO_IN);
            gpio_pull_up(pin);
            break;
        case LCD_GPIO_INPUT_PULLDOWN:
            gpio_set_dir(pin, GPIO_IN);
            gpio_pull_down(pin);;
            break;
    }
}

void lcd_gpioWrite(int pin, int level)
{
    if (pin < 0)
        return;
    gpio_put(pin, level==LCD_HIGH);
}

void lcd_delay(unsigned long ms)
{
    sleep_ms(ms);
}

uint8_t lcd_pgmReadByte(const void *ptr)
{
    return *(static_cast<const uint8_t *>(ptr));
}

#endif
