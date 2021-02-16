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

#include "ninja.h"
#include "sprites.h"

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

static const PROGMEM uint8_t ninjaBitmap[MAN_ANIM_MAX][2][8] = {
{   // FLYING
    {   0x00, 0x04, 0x88, 0x4B, 0x3F, 0x48, 0x88, 0x04 },
    {   0x00, 0x10, 0x08, 0xCB, 0x3F, 0xC8, 0x08, 0x10 },
},
{   // UP + DOWN
    {   0x00, 0x00, 0x90, 0x4B, 0x3F, 0x28, 0x66, 0x00 },
    {   0x00, 0x00, 0x66, 0x2B, 0x3F, 0x48, 0x90, 0x00 },
},
{   // LEFT
    {   0x00, 0x10, 0x10, 0xCB, 0x3F, 0x48, 0x90, 0x00 },
    {   0x00, 0x00, 0x20, 0x1B, 0xFF, 0xD8, 0x00, 0x00 },
},
{   // RIGHT
    {   0x00, 0x90, 0x48, 0x3F, 0xCB, 0x10, 0x10, 0x00 },
    {   0x00, 0x00, 0xD8, 0xFF, 0x1B, 0x20, 0x00, 0x00 },
},
{   // RIGHT PIPE
    {
        0B00000110,
        0B00001000,
        0B00111110,
        0B00110000,
        0B00111000,
        0B00110110,
        0B00111000,
        0B00111000,
    },
    {
        0B00000000,
        0B00000110,
        0B00111000,
        0B00110000,
        0B00111110,
        0B00110000,
        0B00111000,
        0B00111000,
    },
},
{   // LEFT PIPE
    {
        0B00111000,
        0B00111000,
        0B00110110,
        0B00111000,
        0B00110000,
        0B00111110,
        0B00001000,
        0B00000110,
    },
    {
        0B00111000,
        0B00111000,
        0B00110000,
        0B00111110,
        0B00110000,
        0B00111000,
        0B00000110,
        0B00000000,
    },
},
};


Ninja ninja({72, 8});


Ninja::Ninja(NanoPoint pos)
    : NanoFixedSprite<GraphicsEngine::TilerT>(pos, {8,8}, ninjaBitmap[0][0])
{
}

void Ninja::move(const NanoPoint &target)
{
    static uint16_t ninjaAnimationTs = 0;
    static uint8_t  ninjaAnimation = 0;
    bool animated = false;
    uint8_t direction = BUTTON_NONE;
    uint8_t bottomBlock = block_at(bottom());
    uint8_t feetBlock = block_at(bottom() + (NanoPoint){0,1});
    uint8_t handBlock = block_at(top());
    uint8_t centerBlock = block_at(center());
    uint8_t rightBlock = block_at(right());
    uint8_t leftBlock = block_at(left());
    if ( !isSolid(feetBlock) &&
         (!isPipe(handBlock) || !isPipe(bottomBlock)) )
    {
        moveTo( { center().x & ~0x0F, m_rect.p1.y + 1 } );
        setBitmap( &ninjaBitmap[MAN_ANIM_FLYING][ninjaAnimation][0] );
        animated = true;
    }
    else
    {
        if (target.y < m_rect.p1.y - 1)
        {
            bool right = true;
            bool left = true;
            // search for stairs
            for (int8_t i=0; i < 80; i=i+8)
            {
                if (right)
                {
                    uint8_t block = block_at(center() + (NanoPoint){i,0});
                    if (!isWalkable(block)) { right = false; }
                    if (isStair(block)) { direction = BUTTON_RIGHT; break; }
                }
                if (left)
                {
                    uint8_t block = block_at(center() - (NanoPoint){i,0});
                    if (!isWalkable(block)) { left = false; }
                    if (isStair(block)) { direction = BUTTON_LEFT; break; }
                }
            }
            if (isStair(centerBlock) || isStair(bottomBlock)) direction = BUTTON_UP;
        }
        else if (target.y > m_rect.p1.y + 1)
        {
            if (isPipe(handBlock))
            {
                direction = BUTTON_DOWN;
            }
            else
            {
                bool right = true;
                bool left = true;
                // search for stairs
                for (int8_t i=0; i < 80; i=i+8)
                {
                    if (right)
                    {
                        uint8_t block = block_at(center() + (NanoPoint){i,0});
                        if (!isWalkable(block)) right = false;
                        else
                        {
                            block = block_at(bottom() + (NanoPoint){i,1});
                            if (isWalkable(block)) { direction = BUTTON_RIGHT; break; }
                        }
                    }
                    if (left)
                    {
                        uint8_t block = block_at(center() - (NanoPoint){i,0});
                        if (!isWalkable(block)) left = false;
                        else
                        {
                            block = block_at(bottom() + (NanoPoint){-i,1});
                            if (isWalkable(block)) { direction = BUTTON_LEFT; break; }
                        }
                    }
                }
                if (isWalkable(feetBlock)) direction = BUTTON_DOWN;
            }
        }
        else if (target.x > m_rect.p1.x)
        {
            if (isWalkable(rightBlock)) direction = BUTTON_RIGHT;
        }
        else if (target.x < m_rect.p1.x)
        {
            if (isWalkable(leftBlock)) direction = BUTTON_LEFT;
        }
        switch (direction)
        {
            case BUTTON_RIGHT:
                moveBy( { 1, 0 } );
                if (isPipe(centerBlock))
                    setBitmap( &ninjaBitmap[MAN_ANIM_RIGHT_PIPE][ninjaAnimation][0] );
                else
                    setBitmap( &ninjaBitmap[MAN_ANIM_RIGHT][ninjaAnimation][0] );
                animated = true;
                break;
            case BUTTON_LEFT:
                moveBy( { -1, 0 } );
                if (isPipe(centerBlock))
                    setBitmap( &ninjaBitmap[MAN_ANIM_LEFT_PIPE][ninjaAnimation][0] );
                else
                    setBitmap( &ninjaBitmap[MAN_ANIM_LEFT][ninjaAnimation][0] );
                animated = true;
                break;
            case BUTTON_UP:
                moveTo( { top().x & ~0x0F, top().y - 1 } );
                setBitmap( &ninjaBitmap[MAN_ANIM_UP][ninjaAnimation][0] );
                animated = true;
                break;
            case BUTTON_DOWN:
                moveTo( { top().x & ~0x0F, top().y + 1 } );
                setBitmap( &ninjaBitmap[MAN_ANIM_DOWN][ninjaAnimation][0] );
                animated = true;
                break;
            default:
                break;
        }
    }
    if (animated && ((uint16_t)(millis() - ninjaAnimationTs) > 150))
    {
        ninjaAnimationTs = millis();
        ninjaAnimation = ninjaAnimation ? 0 : 1;
    }
}

void Ninja::draw()
{
    getTiler().getCanvas().setColor(RGB_COLOR16(64,64,255));
    NanoFixedSprite::draw();
}
