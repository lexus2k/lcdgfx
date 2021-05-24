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

#if defined(__AVR__) && !defined(ARDUINO)

static int s_pin = -1;
static void *s_arg;
static void (*s_pinEventCallback)(void *) = nullptr;

void lcd_delay(unsigned long ms)
{
    while ( ms-- )
    {
        _delay_ms(1);
    }
}

#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define G0_DIR DDRB
#define G0_PORT PORTB
#define G1_DIR DDRB
#define G1_PORT PORTB
#define G2_DIR DDRB
#define G2_PORT PORTB
#elif defined(__AVR_ATmega328P__)
#define G0_DIR DDRD
#define G0_PORT PORTD
#define G1_DIR DDRB
#define G1_PORT PORTB
#define G2_DIR DDRC
#define G2_PORT PORTC
#else
static uint8_t s_stub;
#define G0_DIR s_stub
#define G0_PORT s_stub
#define G1_DIR s_stub
#define G1_PORT s_stub
#define G2_DIR s_stub
#define G2_PORT s_stub
#endif

void lcd_gpioWrite(int pin, int level)
{
    uint8_t mask = (1 << (pin & 0x7));
    if ( pin == s_pin && s_pinEventCallback )
    {
        s_pinEventCallback(s_arg);
    }
    if ( pin < 8 )
    {
        if ( level == LCD_HIGH )
            G0_PORT |= mask;
        else
            G0_PORT &= ~mask;
    }
    else if ( pin < 16 )
    {
        if ( level == LCD_HIGH )
            G1_PORT |= mask;
        else
            G1_PORT &= ~mask;
    }
    else
    {
        if ( level == LCD_HIGH )
            G2_PORT |= mask;
        else
            G2_PORT &= ~mask;
    }
}

void lcd_gpioMode(int pin, int mode)
{
    uint8_t mask = (1 << (pin & 0x7));
    if ( pin < 8 )
    {
        if ( mode == LCD_GPIO_OUTPUT )
            G0_DIR |= mask;
        else
            G0_DIR &= ~mask;
    }
    else if ( pin < 16 )
    {
        if ( mode == LCD_GPIO_OUTPUT )
            G1_DIR |= mask;
        else
            G1_DIR &= ~mask;
    }
    else
    {
        if ( mode == LCD_GPIO_OUTPUT )
            G2_DIR |= mask;
        else
            G2_DIR &= ~mask;
    }
}

int lcd_gpioRead(int pin)
{
    // TODO: Not implemented
    return LCD_LOW;
}

int lcd_adcRead(int pin)
{
    // TODO: Not implemented
    return 0;
}

void lcd_registerGpioEvent(int pin, void (*on_pin_change)(void *), void *arg)
{
    s_pin = pin;
    s_pinEventCallback = on_pin_change;
    s_arg = arg;
}

void lcd_unregisterGpioEvent(int pin)
{
    s_pin = -1;
}

uint32_t lcd_millis()
{
    // TODO: Not implemented
    return 0;
}

void lcd_attachInterrupt(int pin, void (*interrupt)(), int level)
{
    // TODO: Not implemented
}

/*
 The loop executes four CPU cycles per iteration,
 not including the overhead the compiler requires to setup the
 counter register pair
*/
void lcd_delayCycles(uint16_t __count)
{
    __asm__ volatile("1: sbiw %0,1"
                     "\n\t"
                     "brne 1b"
                     : "=w"(__count)
                     : "0"(__count));
}

void lcd_delayUs(unsigned long us)
{
    if ( us <= 8 )
    {
        return;
    }
    else if ( us < 4096 )
    {
        lcd_delayCycles(us / 4 * (F_CPU / 1000000));
    }
    else
    {
        lcd_delay(us / 1000);
    }
}

void lcd_randomSeed(int seed)
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
