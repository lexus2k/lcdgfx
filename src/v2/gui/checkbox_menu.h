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
/**
 * @file v2/gui/checkbox_menu.h Checkbox menu definition
 */

#ifndef _LCDGFX_CHECKBOX_MENU_H_
#define _LCDGFX_CHECKBOX_MENU_H_

#include "nano_gfx_types.h"
#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/font.h"

#ifndef lcd_gfx_min
#define lcd_gfx_min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef lcd_gfx_max
#define lcd_gfx_max(x, y) ((x) > (y) ? (x) : (y))
#endif

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * @brief Checkbox menu widget for lcdgfx.
 *
 * Each menu item is displayed with a checkbox indicator (filled box = checked,
 * empty box = unchecked). Navigate with up()/down() and toggle the current
 * item's checkbox with toggle(). Supports up to 16 items.
 *
 * Includes scroll indicators (arrows and scrollbar) when items overflow.
 *
 * @code{.cpp}
 * const char *items[] = {"WiFi", "Bluetooth", "LED"};
 * LcdGfxCheckboxMenu menu(items, 3);
 * menu.setChecked(0, true);  // WiFi starts checked
 * menu.show(display);
 *
 * // On select button:
 * menu.toggle();
 * menu.show(display);
 * @endcode
 */
class LcdGfxCheckboxMenu
{
public:
    /**
     * Creates checkbox menu object.
     *
     * @param items array of null-terminated strings (in SRAM)
     * @param count count of menu items (max 16)
     * @param rect screen area for the menu (0,0 = auto-size to display)
     */
    LcdGfxCheckboxMenu(const char **items, uint8_t count, const NanoRect &rect = {});

    /**
     * Shows the checkbox menu on the display. Includes scroll
     * indicators when items overflow the visible area.
     *
     * @param d display object
     */
    template <typename D> void show(D &d);

    /**
     * Moves selection down by one item. Wraps to first item.
     */
    void down();

    /**
     * Moves selection up by one item. Wraps to last item.
     */
    void up();

    /**
     * Toggles the checkbox state of the currently selected item.
     */
    void toggle();

    /**
     * Returns true if the item at the given index is checked.
     * @param index item index (0-based)
     * @return true if checked
     */
    bool isChecked(uint8_t index);

    /**
     * Sets the checkbox state of a specific item.
     * @param index item index (0-based)
     * @param checked true to check, false to uncheck
     */
    void setChecked(uint8_t index, bool checked);

    /**
     * Returns the full checkbox state as a bitmask.
     * Bit 0 = item 0, bit 1 = item 1, etc.
     * @return bitmask of checked items
     */
    uint16_t checkedMask();

    /**
     * Returns currently selected menu item index.
     */
    uint8_t selection();

    /**
     * Sets the current selection index.
     * @param s item index
     */
    void setSelection(uint8_t s);

    /**
     * Sets rect area for the menu.
     * @param rect rect area to use
     */
    void setRect(const NanoRect &rect = {});

    /**
     * Returns total count of menu items.
     */
    uint8_t size();

    /**
     * Updates size of the object if not set previously.
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
    uint16_t m_checked;

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
        lcduint_t fh = d.getFont().getHeader().height;
        uint16_t color = d.getColor();

        // Checkbox box size scales with font
        lcduint_t boxSize = (fh >= 12) ? 7 : 5;
        lcdint_t boxX = menu.left + 6;
        lcdint_t boxY = item_top + (fh - boxSize) / 2;
        lcdint_t textX = boxX + boxSize + 3;

        // Clear background to the right of text
        d.setColor(0x0000);
        d.fillRect(textX + d.getFont().getTextSize(menu.items[index]), item_top,
                   menu.width + menu.left - 9, item_top + fh - 1);
        // Clear checkbox area background
        d.fillRect(menu.left + 5, item_top, textX - 1, item_top + fh - 1);
        d.setColor(color);

        // Draw checkbox
        if ( m_checked & (1u << index) )
        {
            d.fillRect(boxX, boxY, boxX + boxSize - 1, boxY + boxSize - 1);
        }
        else
        {
            d.drawRect(boxX, boxY, boxX + boxSize - 1, boxY + boxSize - 1);
        }

        // Draw item text
        d.printFixed(textX, item_top, menu.items[index], STYLE_NORMAL);

        if ( index == menu.selection )
        {
            d.invertColors();
        }
    }

    template <typename D> void drawScrollIndicators(D &d, uint8_t maxItems)
    {
        uint16_t color = d.getColor();
        lcdint_t borderTop = 4 + menu.top;
        lcdint_t borderBot = menu.height + menu.top - 5;
        lcdint_t itemsTop = 8 + menu.top;
        lcdint_t itemsBot = itemsTop + maxItems * d.getFont().getHeader().height;
        lcdint_t cx = menu.left + menu.width / 2;

        d.setColor(0x0000);
        d.fillRect(5 + menu.left, borderTop + 1, menu.width + menu.left - 6, itemsTop - 1);
        if ( menu.scrollPosition > 0 )
        {
            d.setColor(color);
            d.drawHLine(cx, borderTop + 1, cx);
            d.drawHLine(cx - 1, borderTop + 2, cx + 1);
            d.drawHLine(cx - 2, borderTop + 3, cx + 2);
        }

        d.setColor(0x0000);
        d.fillRect(5 + menu.left, itemsBot, menu.width + menu.left - 6, borderBot - 1);
        if ( menu.scrollPosition + maxItems < menu.count )
        {
            d.setColor(color);
            d.drawHLine(cx - 2, borderBot - 3, cx + 2);
            d.drawHLine(cx - 1, borderBot - 2, cx + 1);
            d.drawHLine(cx, borderBot - 1, cx);
        }

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

template <typename D> void LcdGfxCheckboxMenu::show(D &d)
{
    updateSize(d);
    d.drawRect(4 + menu.left, 4 + menu.top, menu.width + menu.left - 5, menu.height + menu.top - 5);
    menu.scrollPosition = this->calculateScrollPosition(d, menu.selection);
    uint8_t maxItems = getMaxScreenItems(d);
    for ( uint8_t i = menu.scrollPosition; i < lcd_gfx_min(menu.count, (menu.scrollPosition + maxItems)); i++ )
    {
        this->drawMenuItem(d, i);
    }
    menu.oldSelection = menu.selection;
    if ( menu.count > maxItems )
    {
        this->drawScrollIndicators(d, maxItems);
    }
}

/**
 * @}
 */

#endif
