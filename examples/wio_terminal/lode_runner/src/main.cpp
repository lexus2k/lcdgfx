/*
    MIT License

    Copyright (c) 2021, Alexey Dynda

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
/*
 *   Attiny85 PINS
 *             ____
 *   RESET   -|_|  |- 3V
 *   SCL (3) -|    |- (2)
 *   SDA (4) -|    |- (1) - BUZZER
 *   GND     -|____|- (0) - BUTTONS module
 *
 *   Atmega328 PINS with i2c SSD1306 to A4/A5, BUZZER on D8,
 *   Z-keypad ADC module on A0 pin.
 *
 *   Atmega328 PINS with spi Nokia 5110 LCD:
 *   LCD RST to D3
 *   LCD CES to D4
 *   LCD DC to  D5
 *   LCD DIN to D11
 *   LCD CLK to D13
 *   LCD BL to  VCC
 */

#include "game_basic.h"
#include "ninja.h"
#include "player.h"
#include "sprites.h"
#include "levels.h"


#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__)
#define BUZZER      1
#define BUTTON_PIN  0
#else // For Arduino Nano/Atmega328 we use different pins
#define BUZZER      8
#define BUTTON_PIN  0

#endif

//const NanoRect game_window = { {0, 64}, {239, 319} };
// Commented line is for 90 degree
const NanoRect game_window = { {0, 16}, {319, 239} };

uint16_t blockColors[] = {
    RGB_COLOR16(255,96,0),
    RGB_COLOR16(255,255,192),
    RGB_COLOR16(255,255,255),
    RGB_COLOR16(255,255,64),
    RGB_COLOR16(128,128,128),
};

/**
 * Just produces some sound depending on params
 */
void beep(int bCount,int bDelay);

Player player( { 8, 8 } );

void showGameInfo()
{
    engine.getCanvas().setMode(CANVAS_MODE_TRANSPARENT);
    engine.getCanvas().setColor(RGB_COLOR16(0,0,0));
    engine.getCanvas().drawBitmap1(1, 1, 8, 8, coinImage);
    engine.getCanvas().setColor(RGB_COLOR16(255,255,0));
    engine.getCanvas().drawBitmap1(0, 0, 8, 8, coinImage);

    engine.getCanvas().setFixedFont(ssd1306xled_font8x16);
    char score[3] = { player.getCoins() / 10 + '0', player.getCoins() % 10 + '0', 0 };
    engine.getCanvas().setColor(RGB_COLOR16(0,0,0));
    engine.getCanvas().printFixed(9,1,score);
    engine.getCanvas().setColor(RGB_COLOR16(255,255,255));
    engine.getCanvas().printFixed(8,0,score);
    engine.getCanvas().setFixedFont(ssd1306xled_font8x16);
}

static bool onDraw()
{
    engine.getCanvas().clear();
    engine.getCanvas().setMode(CANVAS_MODE_BASIC);
    engine.localCoordinates();
    if (game_window.containsPartOf( engine.getCanvas().rect() ))
    {
        engine.worldCoordinates();
        NanoRect blocks = rect_to_blocks( engine.getCanvas().rect() );
        for (uint8_t row = max(0,blocks.p1.y);
                     row <= min(13,blocks.p2.y); row++)
        {
            for (uint8_t col = max(0,blocks.p1.x);
                         col <= min((B_WIDTH-1),blocks.p2.x); col++)
            {
                uint8_t blockType = block_value({col,row});
                if (blockType != 0)
                {
                    engine.getCanvas().setColor(blockColors[blockType - 1]);
                    NanoPoint pos = block_to_pos({col,row});
                    engine.getCanvas().drawBitmap1(pos.x, pos.y,
                                          16, 16, bgSprites[blockType - 1]);
                }
            }
        }
        engine.localCoordinates();
    }
    showGameInfo();
    engine.worldCoordinates();
    engine.getCanvas().setMode(CANVAS_MODE_TRANSPARENT);
    return true;
}

static NanoPoint calc_new_screen_position()
{
    NanoPoint position = engine.getPosition() + game_window.p1;
    if (player.x() - position.x >= game_window.width() - 48)
    {
        position.x = min(player.x() - (game_window.width() - 96), 128);
    }
    else if (player.x() - position.x < 48)
    {
        position.x = max(0, player.x() - 96);
    }
    if (player.y() - position.y >= game_window.height() - 48)
    {
        position.y = min(player.y() - (game_window.height() - 96), 64);
    }
    else if (player.y() - position.y < 48)
    {
        position.y = max(0, player.y() - 96);
    }
    return position - game_window.p1;
}

static void moveGameScreen()
{
    NanoPoint position = calc_new_screen_position();
    if (position != engine.getPosition())
    {
        engine.moveTo( position );
        engine.refresh( game_window );
    }
}


void lodeRunnerInit()
{
    display.setFixedFont(ssd1306xled_font8x16);
    display.getInterface().setRotation(1);
    display.clear();

    engine.drawCallback( onDraw );
    engine.setFrameRate( 60 );
    engine.refresh();
    engine.insert( player );
    engine.insert( ninja );
    engine.refresh();
    lcd_gpioMode(BUZZER, LCD_GPIO_OUTPUT);
    lodeRunnerLoadLevel( 1 );
}

void lodeRunnerRun()
{
    if (!engine.nextFrame()) return;
    moveGameScreen();
    player.move(engine.buttonsState());
    ninja.move(player.getPosition());
    engine.update();
    engine.display();
}

void lodeRunnerDeinit()
{
    engine.remove( player );
    engine.remove( ninja );
}


void beep(int bCount,int bDelay)
{
    for (int i = 0; i<=bCount*2; i++)
    {
        lcd_gpioWrite(BUZZER,i&1);
        for(int i2=0; i2<bDelay; i2++)
        {
            __asm__("nop\n\t");
#if F_CPU > 8000000
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            __asm__("nop\n\t");
#endif
        }
    }
    lcd_gpioWrite(BUZZER,LCD_LOW);
}
