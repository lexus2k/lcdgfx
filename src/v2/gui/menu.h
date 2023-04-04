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
/**
 * @file v2/gui/menu.h Menu object definition
 */

#ifndef _LCDGFX_MENU_H_
#define _LCDGFX_MENU_H_

#include "nano_gfx_types.h"
#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/font.h"

#ifndef min
/** Custom min function */
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
/** Custom max function */
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * Class implements menu objects for lcdgfx library
 */
class LcdGfxMenu
{
public:
    /**
     * Creates menu object with the provided list of menu items.
     * List of menu items (strings) must exist all until menu object is no longer needed.
     * Selection is set to the first item by default.
     *
     * @param items array of null-termintated strings (located in SRAM)
     * @param count count of menu items in the array
     * @param rect screen area to use for menu
     */
    LcdGfxMenu(const char **items, uint8_t count, const NanoRect &rect = {});

    /**
     * Shows menu items on the display. If menu items cannot fit the display,
     * the function provides scrolling.
     *
     * @param d display object
     */
    template <typename D> void show(D &d);

    /**
     * Moves selection pointer down by 1 item. If there are no items below,
     * it will set selection pointer to the first item.
     * Use show() to refresh menu state on the display.
     */
    void down();

    /**
     * Moves selection pointer up by 1 item. If selected item is the first one,
     * then selection pointer will set to the last item in menu list.
     * Use show() to refresh menu state on the display.
     */
    void up();

    /**
     * Returns currently selected menu item.
     * First item has zero-index.
     */
    uint8_t selection();

    void setSelection(uint8_t s);

    /**
     * Sets rect area for the menu.
     *
     * @param rect rect area to use for menu
     */
    void setRect(const NanoRect &rect = {});

    /**
     * Returns total count of menu items in menu.
     */
    uint8_t size();

    /**
     * Updates size of the object, if it was not set previously
     */
    template <typename D> void updateSize(D &d)
    {
        if ( !menu.width )
        {
            menu.width = d.width() - menu.left;
        }
        if ( !menu.height )
        {
            menu.height = d.height() - menu.top;
        }
    }

private:
    SAppMenu menu;

    template <typename D> uint8_t getMaxScreenItems(D &d)
    {
        return (menu.height - 16) / d.getFont().getHeader().height;
    }

    template <typename D> uint8_t calculateScrollPosition(D &d, uint8_t selection)
    {
        if ( selection < menu.scrollPosition )
        {
            return selection;
        }
        else if ( selection - menu.scrollPosition > getMaxScreenItems(d) - 1 )
        {
            return selection - getMaxScreenItems(d) + 1;
        }
        return menu.scrollPosition;
    }

    template <typename D> void drawMenuItem(D &d, uint8_t index)
    {
        if ( index == menu.selection )
        {
            d.invertColors();
        }
        lcdint_t item_top = 8 + menu.top + (index - menu.scrollPosition) * d.getFont().getHeader().height;
        uint16_t color = d.getColor();
        d.setColor(0x0000);
        d.fillRect(menu.left + 8 + d.getFont().getTextSize(menu.items[index]), item_top, menu.width + menu.left - 9,
                   item_top + d.getFont().getHeader().height - 1);
        d.setColor(color);
        d.printFixed(menu.left + 8, item_top, menu.items[index], STYLE_NORMAL);
        if ( index == menu.selection )
        {
            d.invertColors();
        }
    }
};

template <typename D> void LcdGfxMenu::show(D &d)
{
    updateSize(d);
    d.drawRect(4 + menu.left, 4 + menu.top, menu.width + menu.left - 5, menu.height + menu.top - 5);
    menu.scrollPosition = this->calculateScrollPosition(d, menu.selection);
    for ( uint8_t i = menu.scrollPosition; i < min(menu.count, (menu.scrollPosition + getMaxScreenItems(d))); i++ )
    {
        this->drawMenuItem(d, i);
    }
    menu.oldSelection = menu.selection;
}

/**
 * @}
 */

#endif
