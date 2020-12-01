/*
    MIT License

    Copyright (c) 2020, Alexey Dynda

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

#include "sdl_sh1107.h"
#include "sdl_oled_basic.h"
#include "sdl_graphics.h"
#include "sdl_core.h"

#define SH1107_MAX_BANKS 16

static int s_activeColumn = 0;
static int s_activePage = 0;
static int s_columnStart = 0;
static int s_columnEnd = 127;
static int s_pageStart = 0;
static int s_pageEnd = 7;
static uint8_t detected = 0;
static uint8_t detectedType = 0;

static uint8_t gdram[128][128];
static uint8_t connect_com_offset = 0;
static uint8_t connect_seg_offset = 0;
static uint8_t pageAddressingMode = 1;

// Reverse condition if display layout is different
static uint8_t comScanDirection = 0;
static uint8_t displayRemap = 0;

static uint8_t displayStartLine = 0;
static uint8_t displayOffset = 0;
static uint8_t multiplexRatio = 15;
static uint8_t displayOn = 0;
static uint8_t displayMode = 0;

static void scan_pixel(uint8_t x, uint8_t y)
{
    uint8_t disable_pixel = 0;
    uint8_t com = (x + connect_com_offset) & 0x7F;
    uint8_t seg = (y + connect_seg_offset) & 0x7F;

    uint8_t ram_col = ( !comScanDirection ? (127 - com): com );

    uint8_t row = ( !displayRemap ? (multiplexRatio - seg): seg ) + displayOffset;
    uint8_t ram_row = row + displayStartLine;
    if ( row >= sdl_sh1107.height ) row -= sdl_sh1107.height;
    if ( row >= multiplexRatio ) disable_pixel = 1; // empty

    uint8_t color = gdram[ram_col & 0x7F][ram_row & 0x7F];
    if ( disable_pixel ) color = 0;
    uint16_t sdl_color = color ? 0xAD59: 0x0000;
    switch (displayMode)
    {
        case 0: sdl_color = color ? 0xAD59: 0x0000; break;
        case 1: sdl_color = color ? 0x0000: 0xAD59; break;
        case 2:
        case 3:
        default: sdl_color = 0xAD59; break;
    }
    sdl_put_pixel( x, y, sdl_color );
}

static void blt_content();
static void blt_single_pixel(uint8_t x, uint8_t y, uint8_t color)
{
    gdram[x & 0x7F ][y & 0x7F] = color;
}

static void blt_content()
{
    for(uint8_t line = 0; line < sdl_sh1107.height; line++)
    {
        for(uint8_t column = 0; column < sdl_sh1107.width; column++)
        {
            scan_pixel(column, line);
        }
    }
}

static void sdl_sh1107_reset(void)
{
    detected = 0;
}

static int sdl_sh1107_detect(uint8_t data)
{
    if (detected)
    {
        switch (data)
        {
            case 0x01: sdl_sh1107.width = 64; sdl_sh1107.height = 128;
                       connect_seg_offset = 32; break;
            default: break;
        }
        return 1;
    }
    detectedType = data;
    detected = (data == SDL_LCD_SH1107);
    return 0;
}

static void sdl_sh1107_commands(uint8_t data)
{
//    printf("%02X (CMD: %02X)\n", data, s_commandId);
    switch (s_commandId)
    {
        case 0x20: // Page Addressing mode
            pageAddressingMode = 1;
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0x21: // Column Addressing mode
            pageAddressingMode = 0;
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0x22:
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xA0:
            displayRemap = 0;
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xA1:
            displayRemap = 1;
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xA4: // Display Off
            displayMode &= ~2; // Normal
            if ( displayOn ) blt_content();
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xA5: // Display On
            displayMode |= 2; // White
            if ( displayOn ) blt_content();
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xA6: // Normal display
            displayMode = 0; // Normal
            if ( displayOn ) blt_content();
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xA7: // Reverse display
            displayMode = 1; // Reverse
            if ( displayOn ) blt_content();
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xA8:
            if (s_cmdArgIndex == 0)
            {
                multiplexRatio = data;
                //sdl_sh1107.height = data + 1;
                sdl_graphics_set_oled_params(sdl_sh1107.width,
                                             sdl_sh1107.height,
                                             sdl_sh1107.bpp,
                                             sdl_sh1107.pixfmt);
                if ( displayOn ) blt_content();
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xAE:
            displayOn = 0;
            blt_content();
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0xAF:
            displayOn = 1;
            blt_content();
            s_commandId = SSD_COMMAND_NONE;
            break;
        case 0x81: // Contrast
            if ( s_cmdArgIndex == 0 )
            {
                // Just skip contrast command
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xD3: // Display offset
            if ( s_cmdArgIndex == 0 )
            {
                displayOffset = data;
                if ( displayOn ) blt_content();
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xDC: // Display start line
            if ( s_cmdArgIndex == 0 )
            {
                displayStartLine = data;
                if ( displayOn ) blt_content();
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        case 0xD5: // Clock divide ratio
        case 0xD9: // Discharge period
        case 0xAD: // Set Charge Pump Enable
        case 0xDB: // VCOM Deselect level
            if ( s_cmdArgIndex == 0 )
            {
                s_commandId = SSD_COMMAND_NONE;
            }
            break;
        default:
            if ((s_commandId >= 0xc0) && (s_commandId <= 0xcf)) // Set scan direction
            {
                comScanDirection = !!(s_commandId & 0x08);
                if ( displayOn ) blt_content();
            }
            /* Other ssd1306 commands, many commands are combined with data */
            if ((s_commandId >= 0xb0) && (s_commandId <= 0xbf))
            {
                s_activePage =  (uint16_t)(s_commandId & 0x0F);
            }
            if ((s_commandId <= 0x0F))
            {
                s_activeColumn = (s_activeColumn & 0xFFF0) | (uint16_t)s_commandId;
            }
            if ((s_commandId <= 0x17) && (s_commandId >= 0x10))
            {
                s_activeColumn = (s_activeColumn & 0x000F) | ((int16_t)(s_commandId & 0x0F) << 4);
            }
            s_commandId = SSD_COMMAND_NONE;
            break;
    }
}

void sdl_sh1107_data(uint8_t data)
{
    int y = s_activePage;
    int x = s_activeColumn;
    for (int i=0; i<8; i++)
    {
        blt_single_pixel( x, (y<<3) + i, data & (1<<i) );
    }
    blt_content();
    if ( pageAddressingMode )
    {
        s_activeColumn++;
        if (s_activeColumn > s_columnEnd)
        {
            s_activeColumn = s_columnStart;
        }
    }
    else
    {
        s_activePage++;
        if (s_activePage > s_pageEnd)
        {
            s_activePage = s_pageStart;
        }
    }
}


sdl_oled_info sdl_sh1107 =
{
    .width = 128,
    .height = 64,
    .bpp = 16,
    .pixfmt = SDL_PIXELFORMAT_RGB565,
    .dataMode = SDMS_AUTO,
    .detect = sdl_sh1107_detect,
    .run_cmd = sdl_sh1107_commands,
    .run_data = sdl_sh1107_data,
    .reset = sdl_sh1107_reset,
};
