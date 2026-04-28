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
     * Shows the checkbox menu on the display. Includes scroll indicators
     * when items overflow the visible area.
     *
     * The first call after construction (or after invalidate() / setRect())
     * performs a full redraw. Subsequent calls are flicker-free: only the
     * previously- and newly-selected rows (and any items whose checked bit
     * flipped) are repainted; the border and other items are left
     * untouched. When the visible window changes a full redraw is performed
     * automatically.
     *
     * @param d display object
     */
    template <typename D> void show(D &d);

    /**
     * Equivalent to show(). Provided for backward compatibility.
     */
    template <typename D> void update(D &d) { show(d); }

    /**
     * Forces the next show() to perform a full redraw (border + all visible
     * items + scroll indicators).
     */
    void invalidate() { m_initialized = false; }

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
    uint16_t m_oldChecked = 0;
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
        lcduint_t fh = d.getFont().getHeader().height;
        lcdint_t item_top = 8 + menu.top + (index - menu.scrollPosition) * fh;

        // Checkbox box size scales with font
        lcduint_t boxSize = (fh >= 12) ? 7 : 5;
        lcdint_t boxX = menu.left + 6;
        lcdint_t boxY = item_top + (fh - boxSize) / 2;
        lcdint_t textX = boxX + boxSize + 3;

        // Clear this row's interior (between border and scrollbar) so
        // leftover characters or a previous box-fill don't bleed through
        // when a different item lands on this screen row after scrolling.
        uint16_t fg = d.getColor();
        d.setColor(0x0000);
        d.fillRect(menu.left + 5, item_top, menu.width + menu.left - 9, item_top + fh - 1);
        d.setColor(fg);

        // Checkbox box: drawn with the current (non-inverted) colors so it
        // stays visible on the highlighted row.
        if ( m_checked & (1u << index) )
        {
            d.fillRect(boxX, boxY, boxX + boxSize - 1, boxY + boxSize - 1);
        }
        else
        {
            d.drawRect(boxX, boxY, boxX + boxSize - 1, boxY + boxSize - 1);
        }

        // Text strip: invert only here so the highlight bar covers just
        // the label, not the checkbox.
        if ( index == menu.selection )
        {
            d.invertColors();
        }
        d.printFixed(textX, item_top, menu.items[index], STYLE_NORMAL);
        if ( index == menu.selection )
        {
            d.invertColors();
        }
    }

    template <typename D> void drawScrollIndicators(D &d, uint8_t maxItems)
    {
        // Scrollbar on the right edge is the sole overflow indicator. It
        // lives entirely in page-aligned item rows so partial fillRect for
        // the thumb is safe even on SSD1306-style 1bpp drivers.
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

template <typename D> void LcdGfxCheckboxMenu::show(D &d)
{
    updateSize(d);
    uint8_t maxItems = getMaxScreenItems(d);
    uint8_t newScroll = this->calculateScrollPosition(d, menu.selection);

    // Incremental fast path: only the selection moved within the visible
    // window and/or some checkbox bits flipped — repaint just the affected
    // page-aligned rows.
    if ( m_initialized && newScroll == m_oldScrollPosition )
    {
        menu.scrollPosition = newScroll;
        uint16_t changedCheck = m_checked ^ m_oldChecked;
        if ( menu.selection == menu.oldSelection && changedCheck == 0 )
        {
            return;
        }
        if ( menu.oldSelection != menu.selection )
        {
            this->drawMenuItem(d, menu.oldSelection);
            this->drawMenuItem(d, menu.selection);
        }
        else if ( changedCheck & (1u << menu.selection) )
        {
            // Selection didn't move, but its checkbox was toggled —
            // redraw it so the user sees the change immediately.
            this->drawMenuItem(d, menu.selection);
        }
        // Redraw any other on-screen item whose checked-state flipped.
        for ( uint8_t i = menu.scrollPosition; i < lcd_gfx_min(menu.count, (menu.scrollPosition + maxItems)); i++ )
        {
            if ( (changedCheck & (1u << i)) && i != menu.selection && i != menu.oldSelection )
            {
                this->drawMenuItem(d, i);
            }
        }
        menu.oldSelection = menu.selection;
        m_oldChecked = m_checked;
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
    m_oldChecked = m_checked;
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
