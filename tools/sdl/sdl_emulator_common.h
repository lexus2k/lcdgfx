/*
    MIT License

    Copyright (c) 2018-2026, Alexey Dynda

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
 * @file sdl_emulator_common.h
 * @brief Shared helpers for SDL display emulators.
 *
 * Provides common inline functions for pixel cursor advancement
 * used by multiple emulator implementations.
 */

#ifndef _SDL_EMULATOR_COMMON_H_
#define _SDL_EMULATOR_COMMON_H_

/**
 * Advance column/page cursor after writing a pixel.
 *
 * When verticalFirst is true, page increments first; on page overflow
 * it wraps to pageStart and column increments. When false, column
 * increments first with analogous wrapping.
 *
 * Used by SSD1306, SSD1331, SSD1351, IL9163, ILI9341.
 */
static inline void sdl_emu_advance_xy(
    int *col, int *page,
    int colStart, int colEnd, int pageStart, int pageEnd,
    int verticalFirst)
{
    if (verticalFirst)
    {
        (*page)++;
        if (*page > pageEnd)
        {
            *page = pageStart;
            (*col)++;
            if (*col > colEnd)
            {
                *col = colStart;
            }
        }
    }
    else
    {
        (*col)++;
        if (*col > colEnd)
        {
            *col = colStart;
            (*page)++;
            if (*page > pageEnd)
            {
                *page = pageStart;
            }
        }
    }
}

/**
 * Advance a single axis (column or page) with wrapping.
 *
 * Used by emulators with simpler addressing modes like PCD8544 and SH1107
 * where only one axis increments at a time.
 */
static inline void sdl_emu_advance_single(int *pos, int start, int end)
{
    (*pos)++;
    if (*pos > end)
    {
        *pos = start;
    }
}

#endif /* _SDL_EMULATOR_COMMON_H_ */
