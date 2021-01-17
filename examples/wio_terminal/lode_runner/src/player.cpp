/*
    MIT License

    Copyright (c) 2018-2019,2021, Alexey Dynda

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

#include "player.h"
#include "sprites.h"

extern void showGameInfo();

enum
{
    MAN_ANIM_FLYING   = 0,
    MAN_ANIM_UP       = 1,
    MAN_ANIM_DOWN     = 1,
    MAN_ANIM_LEFT     = 2,
    MAN_ANIM_RIGHT    = 3,
    MAN_ANIM_RIGHT_PIPE = 4,
    MAN_ANIM_LEFT_PIPE  = 5,

    MAN_ANIM_MAX      = 6,
};

static const PROGMEM uint8_t playerBitmap[MAN_ANIM_MAX][2][16*16] = {
{   // FLYING
    {
        0x00,0x00,
        0x04,0x04,
        0x88,0x88,
        0x4B,0x4B,
        0x3F,0x3F,
        0x48,0x48,
        0x88,0x88,
        0x04,0x04,
        0x00,0x00,
        0x04,0x04,
        0x88,0x88,
        0x4B,0x4B,
        0x3F,0x3F,
        0x48,0x48,
        0x88,0x88,
        0x04,0x04,
    },
    {
        0x00,0x00,
        0x10,0x10,
        0x08,0x08,
        0xCB,0xCB,
        0x3F,0x3F,
        0xC8,0xC8,
        0x08,0x08,
        0x10,0x10,
        0x00,0x00,
        0x10,0x10,
        0x08,0x08,
        0xCB,0xCB,
        0x3F,0x3F,
        0xC8,0xC8,
        0x08,0x08,
        0x10,0x10,
    },
},
{   // UP + DOWN
    {
        0x00,0x00,
        0x00,0x00,
        0x90,0x90,
        0x4B,0x4B,
        0x3F,0x3F,
        0x28,0x28,
        0x66,0x66,
        0x00,0x00,
        0x00,0x00,
        0x00,0x00,
        0x90,0x90,
        0x4B,0x4B,
        0x3F,0x3F,
        0x28,0x28,
        0x66,0x66,
        0x00,0x00,
    },
    {
        0x00,0x00,
        0x00,0x00,
        0x66,0x66,
        0x2B,0x2B,
        0x3F,0x3F,
        0x48,0x48,
        0x90,0x90,
        0x00,0x00,
        0x00,0x00,
        0x00,0x00,
        0x66,0x66,
        0x2B,0x2B,
        0x3F,0x3F,
        0x48,0x48,
        0x90,0x90,
        0x00,0x00,
    },
},
{   // LEFT
    {
        0x00,0x00,
        0x10,0x10,
        0x10,0x10,
        0xCB,0xCB,
        0x3F,0x3F,
        0x48,0x48,
        0x90,0x90,
        0x00,0x00,
        0x00,0x00,
        0x10,0x10,
        0x10,0x10,
        0xCB,0xCB,
        0x3F,0x3F,
        0x48,0x48,
        0x90,0x90,
        0x00,0x00,
    },
    {
        0x00,0x00,
        0x00,0x00,
        0x00,0x00,
        0x1B,0x1B,
        0xFF,0xFF,
        0xD8,0xD8,
        0x00,0x00,
        0x00,0x00,
        0x00,0x00,
        0x00,0x00,
        0x20,0x20,
        0x1B,0x1B,
        0xFF,0xFF,
        0xD8,0xD8,
        0x00,0x00,
        0x00,0x00,
    },
},
{   // RIGHT
    {
        0x00,0x00,
        0x90,0x90,
        0x48,0x48,
        0x3F,0x3F,
        0xCB,0xCB,
        0x10,0x10,
        0x10,0x10,
        0x00,0x00,
        0x00,0x00,
        0x90,0x90,
        0x48,0x48,
        0x3F,0x3F,
        0xCB,0xCB,
        0x10,0x10,
        0x10,0x10,
        0x00,0x00,
    },
    {
        0x00,0x00,
        0x00,0x00,
        0xD8,0xD8,
        0xFF,0xFF,
        0x1B,0x1B,
        0x20,0x20,
        0x00,0x00,
        0x00,0x00,
        0x00,0x00,
        0x00,0x00,
        0xD8,0xD8,
        0xFF,0xFF,
        0x1B,0x1B,
        0x20,0x20,
        0x00,0x00,
        0x00,0x00,
    },
},
{   // RIGHT PIPE
    {
        0B00111100,0B00111100,
        0B11000000,0B11000000,
        0B11111100,0B11111100,
        0B00000000,0B00000000,
        0B11000000,0B11000000,
        0B00111100,0B00111100,
        0B11000000,0B11000000,
        0B11000000,0B11000000,

        0B00000000,0B00000000,
        0B00000000,0B00000000,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
    },
    {
        0B00000000,0B00000000,
        0B00000110,0B00000110,
        0B00111000,0B00111000,
        0B00110000,0B00110000,
        0B00111110,0B00111110,
        0B00110000,0B00110000,
        0B00111000,0B00111000,
        0B00111000,0B00111000,

        0B00000000,0B00000000,
        0B00000000,0B00000000,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
    },
},
{   // LEFT PIPE
    {
        0B11000000,0B11000000,
        0B11000000,0B11000000,
        0B00111100,0B00111100,
        0B11000000,0B11000000,
        0B00000000,0B00000000,
        0B11111100,0B11111100,
        0B11000000,0B11000000,
        0B00111100,0B00111100,

        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00000000,0B00000000,
        0B00000000,0B00000000,
    },
    {
        0B00111000,0B00111000,
        0B00111000,0B00111000,
        0B00110000,0B00110000,
        0B00111110,0B00111110,
        0B00110000,0B00110000,
        0B00111000,0B00111000,
        0B00000110,0B00000110,
        0B00000000,0B00000000,

        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00001111,0B00001111,
        0B00000000,0B00000000,
        0B00000000,0B00000000,
    },
},
};

Player::Player(const NanoPoint &pos)
   :NanoFixedSprite<GraphicsEngine::TilerT>(pos, { 16, 16 }, playerBitmap[0][0])
{
    reset();
}

void Player::draw()
{
    getTiler().getCanvas().setColor(RGB_COLOR16(64,255,255));
    NanoFixedSprite<GraphicsEngine::TilerT>::draw();
}

void Player::reset()
{
    moveTo( { 16, 0 } );
    setBitmap( playerBitmap[MAN_ANIM_FLYING][playerAnimation] );
    goldCollection = 0;
}

void Player::move(uint8_t direction)
{
    bool animated = false;
    uint8_t bottomBlock = block_at(bottom());
    uint8_t feetBlock = block_at(bottom() + (NanoPoint){0,1});
    uint8_t handBlock = block_at(top());
    uint8_t centerBlock = block_at(center());
    uint8_t rightBlock = block_at(right());
    uint8_t leftBlock = block_at(left());

    /* If player doesn't stand on the ground, and doesn't hold the pipe,
     * make the player to fall down. */
    if ( !isSolid(feetBlock) &&
         (!isPipe(handBlock) || !isPipe(bottomBlock)) )
    {
        moveTo( { center().x & ~0x0F, y() + 1 } );
        setBitmap( &playerBitmap[MAN_ANIM_FLYING][playerAnimation][0] );
        animated = true;
    }
    else
    {
        switch (direction)
        {
            case BUTTON_RIGHT:
                if (isWalkable(rightBlock))
                {
                    moveBy( { 1, 0 } );
                    if (isPipe(centerBlock))
                        setBitmap( &playerBitmap[MAN_ANIM_RIGHT_PIPE][playerAnimation][0] );
                    else
                        setBitmap( &playerBitmap[MAN_ANIM_RIGHT][playerAnimation][0] );
                    animated = true;
                }
                break;
            case BUTTON_LEFT:
                if (isWalkable(leftBlock))
                {
                    moveBy( { -1, 0 } );
                    if (isPipe(centerBlock))
                        setBitmap( &playerBitmap[MAN_ANIM_LEFT_PIPE][playerAnimation][0] );
                    else
                        setBitmap( &playerBitmap[MAN_ANIM_LEFT][playerAnimation][0] );
                    animated = true;
                }
                break;
            case BUTTON_UP:
                if (isStair(bottomBlock) || isStair(centerBlock))
                {
                    moveTo( { top().x & ~0x0F, top().y - 1 } );
                    setBitmap( &playerBitmap[MAN_ANIM_UP][playerAnimation][0] );
                    animated = true;
                }
                break;
            case BUTTON_DOWN:
                if ( isStair(feetBlock) ||
                   (!isSolid(feetBlock) &&
                     isPipe(handBlock)) )
                {
                    moveTo( { top().x & ~0x0F, top().y + 1 } );
                    setBitmap( &playerBitmap[MAN_ANIM_DOWN][playerAnimation][0] );
                    animated = true;
                }
                break;
            default:
                break;
        }
    }
    if (animated && ((uint16_t)(millis() - playerAnimationTs) > 150))
    {
        playerAnimationTs = millis();
        playerAnimation = playerAnimation ? 0 : 1;
        beep(10,20);
        if (isGold(centerBlock))
        {
            engine.notify( "GOLD COIN" );
            set_block_at(center(), 0);
            goldCollection++;
            showGameInfo();
            engine.refresh(0,0,63,7);
            /* Produce sound every time the player moves */
            beep(20,40);
            beep(20,80);
            beep(20,120);
            beep(20,80);
            beep(20,40);
        }
    }
}

uint8_t Player::getCoins()
{
    return goldCollection;
}