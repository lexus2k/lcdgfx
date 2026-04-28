/*
    MIT License

    Copyright (c) 2025, Alexey Dynda

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
 * @file touch/touch.h Generic touch driver interfaces and calibration helpers
 */

#ifndef _LCDGFX_TOUCH_H_
#define _LCDGFX_TOUCH_H_

#include <stdint.h>

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * Linear calibration mapping raw ADC counts from a resistive touch panel
 * into screen pixel coordinates.
 *
 * The mapping is a per-axis affine transform:
 *
 *     screenX = (raw - rawMin) * screenW / (rawMax - rawMin)
 *     screenY = (raw - rawMin) * screenH / (rawMax - rawMin)
 *
 * with optional axis swap and per-axis inversion to handle different
 * physical mountings of the touch panel relative to the display.
 */
struct TouchCalibration
{
    int16_t xMin = 200;
    int16_t xMax = 3900;
    int16_t yMin = 200;
    int16_t yMax = 3900;
    uint16_t screenWidth = 320;
    uint16_t screenHeight = 240;
    bool swapXY = false;
    bool invertX = false;
    bool invertY = false;

    /**
     * Apply the calibration to a pair of raw ADC samples.
     *
     * The result is clamped to [0, screenWidth-1] and [0, screenHeight-1].
     * Returns x and y by reference.
     */
    void apply(int16_t rawX, int16_t rawY, int16_t &outX, int16_t &outY) const
    {
        if ( swapXY )
        {
            int16_t tmp = rawX;
            rawX = rawY;
            rawY = tmp;
        }
        int32_t spanX = (int32_t)xMax - xMin;
        int32_t spanY = (int32_t)yMax - yMin;
        int32_t x = spanX != 0 ? ((int32_t)(rawX - xMin) * (screenWidth - 1)) / spanX : 0;
        int32_t y = spanY != 0 ? ((int32_t)(rawY - yMin) * (screenHeight - 1)) / spanY : 0;
        if ( invertX ) x = (int32_t)(screenWidth - 1) - x;
        if ( invertY ) y = (int32_t)(screenHeight - 1) - y;
        if ( x < 0 ) x = 0;
        else if ( x >= (int32_t)screenWidth ) x = screenWidth - 1;
        if ( y < 0 ) y = 0;
        else if ( y >= (int32_t)screenHeight ) y = screenHeight - 1;
        outX = (int16_t)x;
        outY = (int16_t)y;
    }
};

/**
 * @}
 */

#endif
