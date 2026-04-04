/*
    MIT License

    Copyright (c) 2020,2022,2025 Alexey Dynda

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

#include "checkbox_menu.h"

LcdGfxCheckboxMenu::LcdGfxCheckboxMenu(const char **items, uint8_t count, const NanoRect &rect)
    : m_checked(0)
{
    menu.items = items;
    menu.count = count > 16 ? 16 : count;
    menu.selection = 0;
    menu.oldSelection = 0;
    menu.scrollPosition = 0;
    menu.top = rect.p1.y;
    menu.left = rect.p1.x;
    menu.width = rect.p2.x ? rect.width() : 0;
    menu.height = rect.p2.y ? rect.height() : 0;
}

void LcdGfxCheckboxMenu::down()
{
    if ( menu.selection < menu.count - 1 )
    {
        menu.selection++;
    }
    else
    {
        menu.selection = 0;
    }
}

void LcdGfxCheckboxMenu::up()
{
    if ( menu.selection > 0 )
    {
        menu.selection--;
    }
    else
    {
        menu.selection = menu.count - 1;
    }
}

void LcdGfxCheckboxMenu::toggle()
{
    m_checked ^= (1u << menu.selection);
}

bool LcdGfxCheckboxMenu::isChecked(uint8_t index)
{
    if ( index >= menu.count )
        return false;
    return (m_checked & (1u << index)) != 0;
}

void LcdGfxCheckboxMenu::setChecked(uint8_t index, bool checked)
{
    if ( index >= menu.count )
        return;
    if ( checked )
    {
        m_checked |= (1u << index);
    }
    else
    {
        m_checked &= ~(1u << index);
    }
}

uint16_t LcdGfxCheckboxMenu::checkedMask()
{
    return m_checked;
}

uint8_t LcdGfxCheckboxMenu::selection()
{
    return menu.selection;
}

void LcdGfxCheckboxMenu::setSelection(uint8_t s)
{
    if ( s >= menu.count )
    {
        menu.selection = menu.count - 1;
    }
    else
    {
        menu.selection = s;
    }
}

void LcdGfxCheckboxMenu::setRect(const NanoRect &rect)
{
    menu.top = rect.p1.y;
    menu.left = rect.p1.x;
    menu.width = rect.p2.x ? rect.width() : 0;
    menu.height = rect.p2.y ? rect.height() : 0;
}

uint8_t LcdGfxCheckboxMenu::size()
{
    return menu.count;
}
