/*
    MIT License

    Copyright (c) 2018,2021, Alexey Dynda

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

#include "sdl_ili9341.h"
#include "sdl_oled_basic.h"
#include "sdl_emulator_common.h"
#include "sdl_graphics.h"
#include "sdl_core.h"

static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;
static uint8_t detected = 0;

static void sdl_ili9341_reset(void)
{
    detected = 0;
}

static int sdl_ili9341_detect(uint8_t data)
{
    if (detected)
    {
        return 1;
    }
    detected = (data == SDL_LCD_ILI9341);
    return 0;
}

static uint8_t s_verticalMode = 0;

static void sdl_ili9341_commands(uint8_t data)
{
    switch (s_commandId)
    {
        case 0x36:
            if (s_cmdArgIndex == 0)
            {
                s_verticalMode = data;
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0x2A:
            switch (s_cmdArgIndex)
            {
                case 0:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_columnStart = data;
                         s_activeColumn = data;
                     }
                     else
                     {
                         s_pageStart = data;
                         s_activePage = data;
                     }
                     break;
                case 1:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_columnStart = data | (s_columnStart<<8);
                         s_activeColumn = data | (s_activeColumn<<8);
                     }
                     else
                     {
                         s_pageStart = data | (s_pageStart<<8);
                         s_activePage = data | (s_activePage<<8);
                     }
                     break;
                case 2:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_columnEnd = data;
                     }
                     else
                     {
                         s_pageEnd = data;
                     }
                     break;
                case 3:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_columnEnd = data | (s_columnEnd << 8);
                     }
                     else
                     {
                         s_pageEnd = data | (s_pageEnd << 8);
                     }
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x2B:
            switch (s_cmdArgIndex)
            {
                case 0:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_activePage = data;
                         s_pageStart = s_activePage;
                     }
                     else
                     {
                         s_columnStart = data;
                         s_activeColumn = data;
                     }
                     break;
                case 1:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_activePage = data | (s_activePage << 8);
                         s_pageStart = s_activePage;
                     }
                     else
                     {
                         s_columnStart = data | (s_columnStart << 8);
                         s_activeColumn = data | (s_activeColumn << 8);
                     }
                     break;
                case 2:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_pageEnd = data;
                     }
                     else
                     {
                         s_columnEnd = data;
                     }
                     break;
                case 3:
                     if (!(s_verticalMode & 0b00100000))
                     {
                         s_pageEnd = data | (s_pageEnd << 8);
                     }
                     else
                     {
                         s_columnEnd = data | (s_columnEnd << 8);
                     }
                     s_commandId = SSD_COMMAND_NONE;
                     break;
                default: break;
            }
            break;
        case 0x2C:
            sdl_set_data_mode( SDM_WRITE_DATA );
            s_commandId = SSD_COMMAND_NONE;
            break;
        // 1-arg config commands: consume argument to avoid misinterpretation
        case 0x26: // Gamma Set
        case 0x3A: // Pixel Format Set
        case 0x51: // Write Brightness
        case 0x53: // Write CTRL Display
        case 0x55: // Write Content Adaptive Brightness
        case 0xF6: // Interface Control (often 1 arg variant)
            if ( s_cmdArgIndex == 0 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xB1: // Frame Rate Control (2 args)
        case 0xB5: // Blanking Porch Control (2 args)
        case 0xB6: // Display Function Control (2 args)
            if ( s_cmdArgIndex == 1 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xC0: // Power Control 1 (2 args)
        case 0xC1: // Power Control 2 (2 args)
            if ( s_cmdArgIndex == 1 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xC5: // VCOM Control (2 args)
            if ( s_cmdArgIndex == 1 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xC7: // VCOM Offset (1 arg)
            if ( s_cmdArgIndex == 0 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xE0: // Positive Gamma Correction (15 args)
        case 0xE1: // Negative Gamma Correction (15 args)
            if ( s_cmdArgIndex == 14 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xCB: // Power Control A (5 args)
            if ( s_cmdArgIndex == 4 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xCF: // Power Control B (3 args)
            if ( s_cmdArgIndex == 2 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        default:
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}


void sdl_ili9341_data(uint8_t data)
{
    int y = s_activePage;
    int x = s_activeColumn;
    static uint8_t firstByte = 1;
    static uint8_t dataFirst = 0x00;
    if (firstByte)
    {
        dataFirst = data;
        firstByte = 0;
        return;
    }
    firstByte = 1;
    int rx, ry;
    if (s_verticalMode & 0b00100000)
    {
        rx = (s_verticalMode & 0b01000000) ? x: (sdl_ili9341.width - 1 - x);
        ry = (s_verticalMode & 0b10000000) ? (sdl_ili9341.height - 1 - y) : y;
    }
    else
    {
        rx = (s_verticalMode & 0b10000000) ? x: (sdl_ili9341.width - 1 - x);
        ry = (s_verticalMode & 0b01000000) ? (sdl_ili9341.height - 1 - y) : y;
    }
    if ( rx >= 0 && ry >= 0 && rx < sdl_ili9341.width && ry < sdl_ili9341.height )
    {
        sdl_put_pixel(rx, ry, (dataFirst<<8) | data);
    }

    sdl_emu_advance_xy(&s_activeColumn, &s_activePage,
                       s_columnStart, s_columnEnd, s_pageStart, s_pageEnd,
                       s_verticalMode & 0x20);
}

sdl_oled_info sdl_ili9341 =
{
    .width = 240,
    .height = 320,
    .bpp = 16,
    .pixfmt = SDL_PIXELFORMAT_RGB565,
    .dataMode = SDMS_CONTROLLER,
    .detect = sdl_ili9341_detect,
    .run_cmd = sdl_ili9341_commands,
    .run_data = sdl_ili9341_data,
    .reset = sdl_ili9341_reset,
};
