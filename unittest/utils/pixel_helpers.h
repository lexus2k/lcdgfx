/*
    MIT License

    Copyright (c) 2019-2026, Alexey Dynda

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

#pragma once

#include <stdint.h>

// Read monochrome pixel from SSD1306 page-addressed buffer
static inline int get_mono_pixel(const uint8_t *buf, int width, int x, int y)
{
    return (buf[(y / 8) * width + x] >> (y & 7)) & 1;
}

// Read 8-bit color pixel from linear buffer
static inline uint8_t get_rgb8_pixel(const uint8_t *buf, int width, int x, int y)
{
    return buf[y * width + x];
}

// Check that a rectangular region in monochrome buffer matches val (0 or 1)
static inline bool mono_region_equals(const uint8_t *buf, int width,
                                      int x1, int y1, int x2, int y2, int val)
{
    for ( int y = y1; y <= y2; y++ )
        for ( int x = x1; x <= x2; x++ )
            if ( get_mono_pixel(buf, width, x, y) != val )
                return false;
    return true;
}

// Check that a rectangular region in 8-bit buffer matches val
static inline bool rgb8_region_equals(const uint8_t *buf, int width,
                                      int x1, int y1, int x2, int y2, uint8_t val)
{
    for ( int y = y1; y <= y2; y++ )
        for ( int x = x1; x <= x2; x++ )
            if ( get_rgb8_pixel(buf, width, x, y) != val )
                return false;
    return true;
}

// Read 4-bit grayscale pixel (nibble-packed: low nibble = even x, high nibble = odd x)
static inline uint8_t get_gray4_pixel(const uint8_t *buf, int width, int x, int y)
{
    uint8_t byte = buf[x / 2 + y * (width / 2)];
    return (x & 1) ? (byte >> 4) : (byte & 0x0F);
}

// Check that a rectangular region in 4-bit buffer matches val (0-15)
static inline bool gray4_region_equals(const uint8_t *buf, int width,
                                       int x1, int y1, int x2, int y2, uint8_t val)
{
    for ( int y = y1; y <= y2; y++ )
        for ( int x = x1; x <= x2; x++ )
            if ( get_gray4_pixel(buf, width, x, y) != val )
                return false;
    return true;
}

// Read 16-bit pixel from linear buffer
static inline uint16_t get_rgb16_pixel(const uint16_t *buf, int width, int x, int y)
{
    return buf[y * width + x];
}

// Check that a rectangular region in 16-bit buffer matches val
static inline bool rgb16_region_equals(const uint16_t *buf, int width,
                                       int x1, int y1, int x2, int y2, uint16_t val)
{
    for ( int y = y1; y <= y2; y++ )
        for ( int x = x1; x <= x2; x++ )
            if ( get_rgb16_pixel(buf, width, x, y) != val )
                return false;
    return true;
}

// Count non-zero pixels in a monochrome region
static inline int mono_count_set(const uint8_t *buf, int width,
                                 int x1, int y1, int x2, int y2)
{
    int count = 0;
    for ( int y = y1; y <= y2; y++ )
        for ( int x = x1; x <= x2; x++ )
            if ( get_mono_pixel(buf, width, x, y) )
                count++;
    return count;
}

// Count non-zero pixels in a 16-bit region
static inline int rgb16_count_set(const uint16_t *buf, int width,
                                  int x1, int y1, int x2, int y2)
{
    int count = 0;
    for ( int y = y1; y <= y2; y++ )
        for ( int x = x1; x <= x2; x++ )
            if ( get_rgb16_pixel(buf, width, x, y) != 0 )
                count++;
    return count;
}

// Count non-zero pixels in a 4-bit region
static inline int gray4_count_set(const uint8_t *buf, int width,
                                  int x1, int y1, int x2, int y2)
{
    int count = 0;
    for ( int y = y1; y <= y2; y++ )
        for ( int x = x1; x <= x2; x++ )
            if ( get_gray4_pixel(buf, width, x, y) != 0 )
                count++;
    return count;
}
