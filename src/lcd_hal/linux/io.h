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

/*
 * @file lcd_hal/linux/io.h SSD1306 LINUX IO communication functions
 */

#ifndef _SSD1306V2_LINUX_IO_H_
#define _SSD1306V2_LINUX_IO_H_

#define CONFIG_LINUX_I2C_AVAILABLE
#define CONFIG_LINUX_SPI_AVAILABLE

#include "../UserSettings.h"

#if defined(SDL_EMULATION) // SDL Emulation mode includes
#include "sdl_core.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/** Pure linux implementation of the library doesn't support data, located in code area */
#define LCD_PROGMEM

#ifdef __cplusplus
extern "C"
{
#endif

    int min(int a, int b);
    int max(int a, int b);

    static inline char *utoa(unsigned int num, char *str, int radix)
    {
        char temp[17]; // an int can only be 16 bits long
                       // at radix 2 (binary) the string
                       // is at most 16 + 1 null long.
        int temp_loc = 0;
        int str_loc = 0;

        // construct a backward string of the number.
        do
        {
            int digit = (unsigned int)num % radix;
            if ( digit < 10 )
                temp[temp_loc++] = digit + '0';
            else
                temp[temp_loc++] = digit - 10 + 'A';
            num = ((unsigned int)num) / radix;
        } while ( (unsigned int)num > 0 );

        temp_loc--;

        // now reverse the string.
        while ( temp_loc >= 0 )
        { // while there are still chars
            str[str_loc++] = temp[temp_loc--];
        }
        str[str_loc] = 0; // add null termination.

        return str;
    }

#ifdef __cplusplus
}
#endif

#endif
