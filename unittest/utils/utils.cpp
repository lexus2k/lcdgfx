/*
    MIT License

    Copyright (c) 2019-2020, Alexey Dynda

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

#include "utils.h"
#include <stdio.h>

// ============================================================================

static void print_monochrome_buffer(uint8_t *buffer, int len, int width)
{
    int x = 0;
    fprintf( stderr, "    const uint8_t buffer[] = {\n       " );
    while (len > 0)
    {
        fprintf( stderr, " 0x%02X,", *buffer );
        buffer++;
        x++;
        len--;
        if ( (x%32 == 0   || x >= width) && len )
        {
            if (x >= width) x = 0;
            fprintf( stderr, "\n       " );
        }
    }
    fprintf( stderr, "\n    };\n" );
}

static void print_monochrome_screen(uint8_t *buffer, int len, int width)
{
    fprintf( stderr, "\n" );
    for (int y = 0; y < len / width * 8; y++)
    {
       for (int x = 0; x < width; x++ )
       {
           fprintf( stderr, "%c", ( buffer[ y / 8 * width + x ] & (1 << (y&0x07)) ) ? '#' : '.' );
       }
       fprintf( stderr, "\n" );
    }
}

// ============================================================================

static void print_rgb8_buffer(uint8_t *buffer, int len, int width)
{
    int x = 0;
    fprintf( stderr, "    const uint8_t buffer[] = {\n       " );
    while (len > 0)
    {
        fprintf( stderr, " 0x%02X,", *buffer );
        buffer++;
        x++;
        len--;
        if ( (x%24 == 0   || x >= width) && len )
        {
            if (x >= width) x = 0;
            fprintf( stderr, "\n       " );
        }
    }
    fprintf( stderr, "\n    };\n" );
}

static void print_rgb8_screen(uint8_t *buffer, int len, int width)
{
    fprintf( stderr, "\n" );
    for (int y = 0; y < len / width; y++)
    {
       for (int x = 0; x < width; x++ )
       {
           fprintf( stderr, "%c", ( ( buffer[ y * width + x ] >> 1 ) & 0x6D ) ? '#' : '.' );
       }
       fprintf( stderr, "\n" );
    }
}

// ============================================================================

void print_buffer_data(uint8_t *buffer, int len, uint8_t bpp, int width)
{
    switch ( bpp )
    {
        case 1: print_monochrome_buffer( buffer, len, width ); break;
        case 8: print_rgb8_buffer( buffer, len, width ); break;
        default: break;
    }
}


void print_screen_content(uint8_t *buffer, int len, uint8_t bpp, int width)
{
    switch ( bpp )
    {
        case 1: print_monochrome_screen( buffer, len, width ); break;
        case 8: print_rgb8_screen( buffer, len, width ); break;
        default: break;
    }
}
