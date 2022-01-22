/*
    MIT License

    Copyright (c) 2019-2021, Alexey Dynda

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
 *   Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(DIN), D13(CLK)
 *   Attiny SPI PINS:     connect LCD to D4 (D/C), GND (CS), D3 (RES), D1(DIN), D2(CLK)
 *   ESP8266: connect LCD to D1(D/C), D2(CS), RX(RES), D7(DIN), D5(CLK)
 */

/* !!! THIS DEMO RUNS in SSD1306 COMPATIBLE MODE */

#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "sova.h"

// The parameters are  RST pin, BUS number, CS pin, DC pin, FREQ (0 means default), CLK pin, MOSI pin
DisplaySSD1327_128x128_SPI display(3,{-1, 4, 5, 0,-1,-1});   // Use this line for Atmega328p (3=RST, 4=CE, 5=D/C)
//DisplaySSD1327_128x128_I2C display(-1);                    // or (-1,{busId, addr, scl, sda, frequency}). This line is suitable for most platforms by default
//DisplaySSD1327_128x128_SPI display(-1,{-1, 0, 1, 0, -1, -1); // Use this line for nano pi (RST not used, 0=CE, gpio1=D/C)
//DisplaySSD1327_128x128_SPI display(24,{-1, 0, 23, 0,-1,-1}); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//DisplaySSD1327_128x128_SPI display(22,{-1, 5, 21, 0,-1,-1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)
//DisplaySSD1327_128x128_SPI display(4,{-1, -1, 5, 0,-1,-1});  // Use this line for ESP8266 Arduino style rst=4, CS=-1, DC=5
                                                             // And ESP8266 RTOS IDF. GPIO4 is D2, GPIO5 is D1 on NodeMCU boards

/*
 * Heart image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to
 * right (bytes).
 */
const PROGMEM uint8_t heartImage[8] =
{
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
};

const PROGMEM uint8_t heartImage4[4 * 8] =
{
    0xF0, 0x0F, 0xF0, 0x0F,
    0xFF, 0xFF, 0x7F, 0xF2,
    0xFF, 0xFF, 0xCF, 0xF7,
    0xFF, 0xFF, 0xFF, 0xF9,
    0xF0, 0xFF, 0xFF, 0x3F,
    0x00, 0xFF, 0xFF, 0x03,
    0x00, 0xF0, 0x3F, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
const int spriteWidth = sizeof(heartImage);

const char *menuItems[] =
{
    "draw bitmap",
    "sprites",
    "fonts",
    "canvas gfx",
    "draw lines",
};

LcdGfxMenu menu( menuItems, sizeof(menuItems) / sizeof(char *) );

static void bitmapDemo()
{
    display.clear();
    display.setColor(GRAY_COLOR4(128));
    display.drawBitmap1(0, 0, 96, 64, Sova);
    display.drawBitmap4(8, 72, 8, 8, heartImage4);
    display.drawBitmap4(18, 74, 8, 8, heartImage4);
    display.drawBitmap4(11, 82, 8, 8, heartImage4);
    delay(3000);
}

/* Sprites are not implemented for color modes.
 * But there is NanoEngine support
 * To make example clear, we use lambda as function pointer. Since lambda can be
 * passed to function only if it doesn't capture, all variables should be global.
 * Refer to C++ documentation.
 */
NanoPoint sprite;
NanoEngine4<DisplaySSD1327_128x128_SPI> engine(display);
static void spriteDemo()
{
    // We not need to clear screen, engine will do it for us
    engine.begin();
    // Force engine to refresh the screen
    engine.refresh();
    // Set function to draw our sprite
    engine.drawCallback( []()->bool {
        engine.getCanvas().clear();
        engine.getCanvas().setColor( GRAY_COLOR4(192) );
        engine.getCanvas().drawBitmap1( sprite.x, sprite.y, 8, 8, heartImage );
        return true;
    } );
    sprite.x = 0;
    sprite.y = 0;
    for (int i=0; i<250; i++)
    {
        lcd_delay(15);
        // Tell the engine to refresh screen at old sprite position
        engine.refresh( sprite.x, sprite.y, sprite.x + 8 - 1, sprite.y + 8 - 1 );
        sprite.x++;
        if (sprite.x >= display.width())
        {
            sprite.x = 0;
        }
        sprite.y++;
        if (sprite.y >= display.height())
        {
            sprite.y = 0;
        }
        // Tell the engine to refresh screen at new sprite position
        engine.refresh( sprite.x, sprite.y, sprite.x + 8 - 1, sprite.y + 8 - 1 );
        // Do refresh required parts of screen
        engine.display();
    }
}

static void textDemo()
{
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    display.setColor(GRAY_COLOR4(255));
    display.printFixed(0,  8, "Normal text", STYLE_NORMAL);
    display.setColor(GRAY_COLOR4(192));
    display.printFixed(0, 16, "Bold text", STYLE_BOLD);
    display.setColor(GRAY_COLOR4(128));
    display.printFixed(0, 24, "Italic text", STYLE_ITALIC);
    display.setColor(GRAY_COLOR4(164));
    display.invertColors();
    display.printFixed(0, 32, "Inverted bold", STYLE_BOLD);
    display.invertColors();
    delay(3000);
}

static void canvasDemo()
{
    uint8_t buffer[32*16/2];
    NanoCanvas4 canvas(32,16, buffer);
    display.clear();
    canvas.clear();
    canvas.setColor(GRAY_COLOR4(127));
    canvas.fillRect(10, 3, 80, 5);
    display.drawCanvas((display.width()-64)/2, 1, canvas);
    delay(500);
    canvas.setColor(GRAY_COLOR4(255));
    canvas.fillRect(16, 1, 20, 15);
    display.drawCanvas((display.width()-64)/2, 1, canvas);
    delay(1500);
    canvas.setFixedFont(ssd1306xled_font6x8);
    canvas.setColor(GRAY_COLOR4(64));
    canvas.printFixed(3, 1, "DEMO", STYLE_BOLD );
    display.drawCanvas((display.width()-64)/2, 1, canvas);
    delay(3000);
}

static void drawLinesDemo()
{
    display.clear();
    display.setColor(GRAY_COLOR4(128));
    for (uint8_t y = 0; y < display.height(); y += 8)
    {
        display.drawLine(0,0, display.width() -1, y);
    }
    display.setColor(GRAY_COLOR4(255));
    for (uint8_t x = display.width() - 1; x > 7; x -= 8)
    {
        display.drawLine(0,0, x, display.height() - 1);
    }
    delay(3000);
}

void setup()
{
    display.begin();
    display.setFixedFont(ssd1306xled_font6x8);

    display.clear( );
    display.setColor(GRAY_COLOR4(255));
    menu.show( display );
}

void loop()
{
    delay(1000);
    switch (menu.selection())
    {
        case 0:
            bitmapDemo();
            break;

        case 1:
            spriteDemo();
            break;

        case 2:
            textDemo();
            break;

        case 3:
            canvasDemo();
            break;

        case 4:
            drawLinesDemo();
            break;

        default:
            break;
    }
    display.clear( );
    display.setColor(GRAY_COLOR4(255));
    menu.show( display );
    delay(500);
    menu.down();
    menu.show( display );
}
