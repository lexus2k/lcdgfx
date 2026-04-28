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

#ifndef lcd_gfx_min
/** Custom min function */
#define lcd_gfx_min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef lcd_gfx_max
/** Custom max function */
#define lcd_gfx_max(x, y) ((x) > (y) ? (x) : (y))
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
     * The first call after construction (or after invalidate() / setRect())
     * performs a full redraw (border, every visible item, scroll indicators).
     * Subsequent calls are flicker-free: when the selection moved within the
     * already-visible window only the previously- and newly-selected item
     * rows are redrawn; the border and untouched items are left alone. When
     * the visible window changes (selection scrolls off-screen) a full
     * redraw is performed automatically.
     *
     * @param d display object
     */
    template <typename D> void show(D &d);

    /**
     * Equivalent to show(). Provided for backward compatibility and code
     * that wants to make the incremental-refresh intent explicit.
     */
    template <typename D> void update(D &d) { show(d); }

    /**
     * Forces the next show() to perform a full redraw (border + all visible
     * items + scroll indicators). Use this when the surrounding screen has
     * been cleared / repainted by other code and the menu needs to repaint
     * its border too.
     */
    void invalidate() { m_initialized = false; }

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

    /**
     * Returns the menu item index at the given screen coordinate, or 0xFF
     * if the point lies outside any item row (e.g. on the border or scroll
     * indicators).
     *
     * @param d display object (needed for font metrics)
     * @param x screen x coordinate
     * @param y screen y coordinate
     */
    template <typename D> uint8_t itemAtPoint(D &d, lcdint_t x, lcdint_t y)
    {
        updateSize(d);
        if ( x < menu.left || x >= (lcdint_t)(menu.left + menu.width) ) return 0xFF;
        if ( y < menu.top || y >= (lcdint_t)(menu.top + menu.height) ) return 0xFF;
        lcdint_t itemsTop = 8 + menu.top;
        lcduint_t itemH = d.getFont().getHeader().height;
        if ( itemH == 0 ) return 0xFF;
        uint8_t maxItems = getMaxScreenItems(d);
        lcdint_t itemsBot = itemsTop + maxItems * (lcdint_t)itemH;
        if ( y < itemsTop || y >= itemsBot ) return 0xFF;
        uint8_t row = (uint8_t)((y - itemsTop) / itemH);
        uint8_t index = menu.scrollPosition + row;
        if ( index >= menu.count ) return 0xFF;
        return index;
    }

    /**
     * Handles a touch event at the given screen coordinates. Touches in the
     * empty band between the border and the first/last item scroll the menu
     * by one item (when more items are available in that direction).
     * Touches on an item row select that item. Returns true if the touch
     * was handled.
     *
     * Call show() afterwards to refresh the display.
     *
     * @param d display object
     * @param x screen x coordinate of the touch
     * @param y screen y coordinate of the touch
     */
    template <typename D> bool onTouch(D &d, lcdint_t x, lcdint_t y)
    {
        updateSize(d);
        if ( x < menu.left || x >= (lcdint_t)(menu.left + menu.width) ) return false;
        if ( y < menu.top || y >= (lcdint_t)(menu.top + menu.height) ) return false;
        lcdint_t borderTop = 4 + menu.top;
        lcdint_t borderBot = menu.height + menu.top - 5;
        lcdint_t itemsTop = 8 + menu.top;
        lcduint_t itemH = d.getFont().getHeader().height;
        if ( itemH == 0 ) return false;
        uint8_t maxItems = getMaxScreenItems(d);
        lcdint_t itemsBot = itemsTop + maxItems * (lcdint_t)itemH;
        if ( y >= borderTop && y < itemsTop )
        {
            if ( menu.scrollPosition > 0 )
            {
                up();
                return true;
            }
            return false;
        }
        if ( y >= itemsBot && y < borderBot )
        {
            if ( menu.scrollPosition + maxItems < menu.count )
            {
                down();
                return true;
            }
            return false;
        }
        if ( y >= itemsTop && y < itemsBot )
        {
            uint8_t row = (uint8_t)((y - itemsTop) / itemH);
            uint8_t index = menu.scrollPosition + row;
            if ( index < menu.count )
            {
                setSelection(index);
                return true;
            }
        }
        return false;
    }

private:
    SAppMenu menu;
    uint8_t m_oldScrollPosition = 0;
    bool m_initialized = false;

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
        // Clear only the trailing portion (from the end of the new label
        // to just inside the right border / scrollbar). printFixed paints
        // the cell backgrounds under the new text itself, so this single
        // trailing fillRect is enough — even after scrolling, when a
        // shorter label replaces a longer one at the same screen row.
        d.fillRect(menu.left + 8 + d.getFont().getTextSize(menu.items[index]), item_top,
                   menu.width + menu.left - 9, item_top + d.getFont().getHeader().height - 1);
        d.setColor(color);
        d.printFixed(menu.left + 8, item_top, menu.items[index], STYLE_NORMAL);
        if ( index == menu.selection )
        {
            d.invertColors();
        }
    }

    template <typename D> void drawScrollIndicators(D &d, uint8_t maxItems)
    {
        // Scrollbar on the right edge conveys the visible-window position;
        // it lives entirely in the item-area pages (page-aligned) so a
        // partial fillRect for the thumb is safe even on SSD1306-style
        // 1bpp drivers that stream whole 8-pixel page bytes.
        uint16_t color = d.getColor();
        lcdint_t itemsTop = 8 + menu.top;
        lcdint_t itemsBot = itemsTop + maxItems * d.getFont().getHeader().height;
        lcdint_t sbX = menu.width + menu.left - 8;
        lcdint_t sbH = itemsBot - itemsTop - 1;
        if ( sbH > 4 )
        {
            lcdint_t thumbH = lcd_gfx_max((lcdint_t)2, (lcdint_t)(sbH * maxItems / menu.count));
            lcdint_t maxScroll = menu.count - maxItems;
            lcdint_t thumbY = itemsTop;
            if ( maxScroll > 0 )
            {
                thumbY = itemsTop + (lcdint_t)((long)(sbH - thumbH) * menu.scrollPosition / maxScroll);
            }
            d.setColor(0x0000);
            d.fillRect(sbX, itemsTop, sbX + 1, itemsTop + sbH);
            d.setColor(color);
            d.fillRect(sbX, thumbY, sbX + 1, thumbY + thumbH);
        }
        d.setColor(color);
    }
};

template <typename D> void LcdGfxMenu::show(D &d)
{
    updateSize(d);
    uint8_t maxItems = getMaxScreenItems(d);
    uint8_t newScroll = this->calculateScrollPosition(d, menu.selection);

    // Incremental fast path: nothing about the visible window changed,
    // only the selection moved within it. Repaint just the two affected
    // item rows. They live on page-aligned y offsets so redrawing them
    // does not disturb the border or neighbouring items even on
    // SSD1306-style 1bpp drivers that stream whole 8-pixel page bytes.
    if ( m_initialized && newScroll == m_oldScrollPosition )
    {
        if ( menu.selection != menu.oldSelection )
        {
            menu.scrollPosition = newScroll;
            uint8_t prev = menu.oldSelection;
            this->drawMenuItem(d, prev);
            this->drawMenuItem(d, menu.selection);
            menu.oldSelection = menu.selection;
        }
        return;
    }

    // Full redraw: first call, or visible window scrolled. Each
    // drawMenuItem clears its own row, so no full-interior fillRect is
    // needed here — the border stays painted without flicker.
    menu.scrollPosition = newScroll;
    d.drawRect(4 + menu.left, 4 + menu.top, menu.width + menu.left - 5, menu.height + menu.top - 5);
    for ( uint8_t i = menu.scrollPosition; i < lcd_gfx_min(menu.count, (menu.scrollPosition + maxItems)); i++ )
    {
        this->drawMenuItem(d, i);
    }
    menu.oldSelection = menu.selection;
    m_oldScrollPosition = menu.scrollPosition;
    m_initialized = true;
    if ( menu.count > maxItems )
    {
        this->drawScrollIndicators(d, maxItems);
    }
}

/**
 * @}
 */

#endif
