/*
    MIT License

    Copyright (c) 2017-2019, Alexey Dynda

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
 * @file nano_gfx_types.h Basic structures of nano gfx library
 */

#ifndef _NANO_GFX_TYPES_H_
#define _NANO_GFX_TYPES_H_

#include "lcd_hal/io.h"
#include "canvas/canvas_types.h"

#ifndef lcd_gfx_min
/** Macros returning minimum of 2 numbers */
#define lcd_gfx_min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef lcd_gfx_max
/** Macros returning maximum of 2 numbers */
#define lcd_gfx_max(a, b) ((a) > (b) ? (a) : (b))
#endif

/**
 * Rectangle region. not used now
 */
typedef struct
{
    /// left
    uint8_t left;
    /// top
    uint8_t top;
    /// right
    uint8_t right;
    /// bottom
    uint8_t bottom;
} SSD1306_RECT;

/**
 * SPRITE structure represents logical graphics object
 * @deprecated Use NanoSprite or NanoFixedSprite instead.
 */
typedef struct SPRITE
{
    /// draw position X on the screen
    uint8_t x;
    /// draw position Y on the screen
    uint8_t y;
    /// sprite width
    uint8_t w;
    /// last draw position X on the screen
    uint8_t lx;
    /// last draw position Y on the screen
    uint8_t ly;
    /// Pointer to PROGMEM data, representing sprite image
    const uint8_t *data;

#ifdef __cplusplus
    /**
     * Updates active position of the sprite (doesn't redraw it)
     * @param x - horizontal position
     * @param y - vertical position
     */
    void setPos(uint8_t x, uint8_t y);

    /**
     * Draws sprite on the display. Position can be changed by
     * updating x and y fields of SPRITE structure.
     */
    void draw();

    /**
     * Clears some sprite parts in old position on the display.
     */
    void eraseTrace();

    /**
     * Clears sprite from the display leaving black rectangle.
     */
    void erase();

    /**
     * Returns true if sprite is moved not far from previous position,
     * and old and new rects have intersection.
     */
    inline bool isNearMove() const
    {
        /* We emulate abs function for unsigned vars here */
        return (((uint8_t)(x - lx) < w) || ((uint8_t)(lx - x) < w)) &&
               (((uint8_t)(y - ly) < 8) || ((uint8_t)(ly - y) < 8));
    };

    /**
     * Returns area in 8-pixel blocks, which is used by the sprite.
     *
     * For example, if sprite pixels coordinates are 10,18 and size is 8x8,
     * the rect will be (left:1,top:2,right:2,bottom:3).
     * if sprite pixels coordinates are 32,16 and size is 8x8,
     * the rect will be (left:4,top:2,right:4,bottom:2).
     */
    inline SSD1306_RECT getRect() const
    {
        uint8_t right = ((x + w - 1) >> 3);
        uint8_t bottom = ((y + 7) >> 3);
        uint8_t left = x >> 3;
        left = left < right ? left : 0;
        uint8_t top = y >> 3;
        top = top < bottom ? top : 0;
        return (SSD1306_RECT){left, top, right, bottom};
    };

    /**
     * Returns area in 8-pixel blocks, which was used by the sprite last time
     * For example, if sprite pixels coordinates are 10,18 and size is 8x8,
     * the rect will be (left:1,top:2,right:2,bottom:3).
     * if sprite pixels coordinates are 32,16 and size is 8x8,
     * the rect will be (left:4,top:2,right:4,bottom:2).
     */
    inline SSD1306_RECT getLRect() const
    {
        uint8_t left = lx;
        uint8_t top = ly;
        uint8_t right = (uint8_t)(lx + w - 1);
        uint8_t bottom = (uint8_t)(ly + 7);
        left = left < right ? left : 0;
        top = top < bottom ? top : 0;
        return (SSD1306_RECT){left, top, right, bottom};
    };

    /**
     * Returns area in 8-pixel blocks, which includes old and new position
     * For example, if sprite pixels coordinates are 12,18 and size is 8x8,
     * and sprite is moved to the right by 6 pixels, the rect will be
     * (left:1,top:2,right:3,bottom:3).
     */
    inline SSD1306_RECT getUpdateRect() const
    {
        uint8_t left = lcd_gfx_min(x, lx);
        uint8_t top = lcd_gfx_min(y, ly);
        uint8_t right = lcd_gfx_max((uint8_t)(x + w - 1), (uint8_t)(lx + w - 1));
        if ( ((uint8_t)(lx + w - 1) < w) && (right > 2 * w) )
        {
            right = (uint8_t)(lx + w - 1);
        }
        uint8_t bottom = lcd_gfx_max((uint8_t)(y + 7), (uint8_t)(ly + 7));
        if ( ((uint8_t)(ly + 7) < 8) && (bottom > 16) )
        {
            bottom = (uint8_t)(ly + 7);
        }
        if ( left > right )
            left = 0;
        if ( top > bottom )
            top = 0;
        return (SSD1306_RECT){left, top, right, bottom};
    };
#endif
} SPRITE;

// ----------------------------------------------------------------------------
#endif // _NANO_GFX_TYPES_H_
