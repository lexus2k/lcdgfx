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
 * @file v2/lcd/lcd_common.h global lcd settings
 */

#ifndef _LCD_COMMON_V2_H_
#define _LCD_COMMON_V2_H_

#include "lcd_hal/io.h"
#include "lcd_common.inl"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @defgroup LCD_INTERFACE_API_V2 OLEDs: initialization and service functions
     * @{
     * @brief LCD initialization classes and service functions for 2.0.0 and higher
     *
     * @details This group contains API functions for OLED displays initialization and
     *          direct programming of GDRAM. This API can be used to create your own
     *          graphics functions.
     */

    /**
     * @brief Does hardware reset for oled controller.
     *
     * Does hardware reset for oled controller. The function pulls up rstPin
     * for 10 milliseconds, then pulls down rstPin for delayMs milliseconds
     * and then finally pulls rstPin up.
     *
     * @param rstPin reset pin number. If -1, then reset will not be performed
     * @param delayMs delay in milliseconds to hold rstPin in low state
     */
    void ssd1306_resetController2(int8_t rstPin, uint8_t delayMs);

    /**
     * @}
     */

#ifdef __cplusplus
}
#endif

#endif /* _LCD_COMMON_V2_H_ */
