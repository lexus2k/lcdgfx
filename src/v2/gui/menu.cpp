/*
    MIT License

    Copyright (c) 2020,2022, Alexey Dynda

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

#include "menu.h"

#ifdef lcd_gfx_use_menu_add
LcdGfxMenu::LcdGfxMenu(const NanoRect &rect)
{
    menu.items = nullptr;
    menu.count = 0;
    menu.selection = 0;
    menu.oldSelection = 0;
    menu.scrollPosition = 0;
    m_buttonCount = 0;
    m_buttonsSelection = -1;

    setRect(rect);
}
#else

LcdGfxMenu::LcdGfxMenu(const char **items, uint8_t count, CallbackFunc *_itemCallback, const NanoRect &rect)
{
    m_buttonCount = 0;
    m_buttonsSelection = -1;

    setRect(rect);
    setItens(count, items, _itemCallback);
}

LcdGfxMenu::LcdGfxMenu()
{
    menu.items = nullptr;
    menu.count = 0;
    menu.selection = 0;
    menu.oldSelection = 0;
    menu.scrollPosition = 0;
    m_buttonCount = 0;
    m_buttonsSelection = -1;

    setRect();
}

#endif

LcdGfxMenu::~LcdGfxMenu()
{

#ifdef lcd_gfx_use_menu_add

    if ( m_buttonCount > 0 )
    {
        for ( uint_fast8_t i = 0; i < m_buttonCount; i++ )
        {
            delete m_buttons[i];
        }
        delete[] m_buttons;
    }

    if ( menu.count > 0 )
    {
#ifdef ENABLE_FLASH_STRING
        for ( uint_fast8_t i = 0; i < menu.count; i++ )
        {
        //    delete[] menu.items[i];
        }
#endif
        delete[] menu.items;
        delete[] m_CallbackItens;
    }

#endif

}

void LcdGfxMenu::down()
{
    if ( (menu.selection < menu.count - 1) && !isButtonSelected() )
    {
        menu.selection++;
    }
    else
    {
        if ( (m_buttonsSelection < m_buttonCount - 1) && m_buttonCount > 0 )
        {
            m_buttonsSelection++;
        }
        else
        {
            menu.selection = 0;
            m_buttonsSelection = -1;
        }
    }

    /*
    if ( menu.selection < menu.count - 1 )
    {
        menu.selection++;
    }
    else
    {
        menu.selection = 0;
    }
    */
}

void LcdGfxMenu::up()
{
    /*
    if ( menu.selection > 0 )
    {
        menu.selection--;
    }
    else
    {
        menu.selection = menu.count - 1;
    }
    */

    if ( (menu.selection > 0) && (m_buttonsSelection == -1) )
    {
        menu.selection--;
    }
    else
    {
        if ( (m_buttonsSelection == -1) && m_buttonCount > 0 )
        {
            m_buttonsSelection = m_buttonCount - 1;
        }
        else if ( m_buttonsSelection > 0 )
        {
            m_buttonsSelection--;
        }
        else
        {
            menu.selection = menu.count - 1;
            m_buttonsSelection = -1;
        }
    }
}

uint8_t LcdGfxMenu::selection()
{
    if (isButtonSelected())
        return m_buttonsSelection;
    else
        return menu.selection;
}

void LcdGfxMenu::setSelection(uint8_t s)
{
    if ( s >= menu.count )
    {
        menu.selection = menu.count - 1;
    }
    else {
        menu.selection = s;
    }
}

void LcdGfxMenu::setRect(const NanoRect &rect)
{
    menu.top = rect.p1.y;
    menu.left = rect.p1.x;
    menu.width = rect.p2.x ? rect.width() : 0;
    menu.height = rect.p2.y ? rect.height() : 0;
}

bool LcdGfxMenu::isButtonSelected()
{
    return m_buttonsSelection != -1;
}

LcdGfxButton **LcdGfxMenu::getButtons(){
    return m_buttons;
}

#ifdef lcd_gfx_use_menu_add

LcdGfxButton *LcdGfxMenu::addButton(const char *_text, const NanoRect &rect)
{
    if ( m_buttonCount == 0 )
    {
        // No Items yet
        m_buttons = (LcdGfxButton **)malloc(sizeof(LcdGfxButton *));
    }
    else
    {
        // Items alreay present
        m_buttons = (LcdGfxButton **)realloc(m_buttons, sizeof(LcdGfxButton *) * (m_buttonCount + 1));
    }

    // Create new DMenu and assign it to last array pointer
    LcdGfxButton *NewItem = new LcdGfxButton(_text, rect);
    m_buttons[m_buttonCount] = NewItem;
    // Inc items counter
    m_buttonCount++;

    return NewItem;
}

void LcdGfxMenu::addItem(const char *_text, CallbackFunc _itemCallback)
{
    if ( menu.count == 0 )
    {
        // No Items yet
        // Allocate first item pointer
        menu.items = (const char **)malloc(sizeof(char *));
        m_CallbackItens = (CallbackFunc *)malloc(sizeof(CallbackFunc));
    }
    else
    {
        // Items alreay present
        // try to allocate an other item pointer
        menu.items = (const char **)realloc(menu.items, sizeof(char *) * (menu.count + 1));
        m_CallbackItens = (CallbackFunc *)realloc(m_CallbackItens, sizeof(CallbackFunc) * (menu.count + 1));
    }

    m_CallbackItens[menu.count] = _itemCallback;
    menu.items[menu.count] = _text;
    //menu.items[menu.count] = lcd_newChar::FlashString_to_New_char(_text);
    
    // Inc items counter
    menu.count++;
}

#else

void LcdGfxMenu::setButtons(uint_fast8_t _count, LcdGfxButton **_buttons)
{
    m_buttons = _buttons;
    m_buttonCount = _count;
    m_buttonsSelection = -1;
  //  isSetButton = true;
}

void LcdGfxMenu::setItens(uint_fast8_t _count, const char **_items, CallbackFunc *_itemCallback)
{
    menu.items = _items;
    m_CallbackItens = _itemCallback;
    menu.count = _count;
    menu.selection = 0;
    menu.oldSelection = 0;
    menu.scrollPosition = 0;
}

#endif

CallbackFunc LcdGfxMenu::getItem()
{
    return m_CallbackItens[menu.selection];
}

void LcdGfxMenu::setMaxScreenItems(uint_fast8_t _maxScreenItems)
{
    m_CustomMaxScreenItems = _maxScreenItems;
}

void LcdGfxMenu::setVerticalOffset(uint_fast8_t _offset)
{
    menu_offset = _offset;
}

void LcdGfxMenu::setTopOffset(uint_fast8_t _offset)
{
    item_top = _offset;
}

void LcdGfxMenu::setLineSpace(uint_fast8_t _space)
{
    text_space = _space;
}

#ifdef lcd_gfx_menu_custom_colors

void LcdGfxMenu::setBackgroundColor(uint16_t color)
{
    m_bgColor = color;
}

void LcdGfxMenu::setTextSelectionColor(uint16_t color)
{
    m_TextSelectionColor= color;
}

void LcdGfxMenu::setTextColor(uint16_t color)
{
    m_textColor = color;
}

void LcdGfxMenu::setSelectionColor(uint16_t color)
{
    m_selectionColor = color;
}

void LcdGfxMenu::setContornoColor(uint16_t color)
{
    m_contornoColor = color;
}

#endif

uint8_t LcdGfxMenu::size()
{
    return menu.count;
}
