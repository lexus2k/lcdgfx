/*
    MIT License

    Copyright (c) 2017-2019, 2021 Alexey Dynda

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
/**
 * @file canvas/internal/canvas_types_int.h Internal structures of canvas gfx library
 */

#pragma once

#include "canvas/UserSettings.h"

#if defined(ARDUINO)
#elif defined(__AVR__)
#else
#ifndef DOXYGEN_SHOULD_SKIP_THIS
static inline uint8_t pgm_read_byte(const void *ptr)
{
    return *((const uint8_t *)ptr);
}
#endif
#endif

#ifndef __min
/** Macros returning minimum of 2 numbers */
#define __min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef __max
/** Macros returning maximum of 2 numbers */
#define __max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef canvas_swap_data
/** swaps content of a and b variables of type type */
#define canvas_swap_data(a, b, type)                                                                                   \
    {                                                                                                                  \
        type t = a;                                                                                                    \
        a = b;                                                                                                         \
        b = t;                                                                                                         \
    }
#endif

#pragma pack(push, 1)
/** Structure describes unicode block in font data */
typedef struct
{
    uint16_t start_code; ///< unicode start code
    uint8_t count;       ///< count of unicode chars in block
} SUnicodeBlockRecord;
#pragma pack(pop)
