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
#include "button.h"

#ifndef lcd_gfx_min
/** Custom min function */
#define lcd_gfx_min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef lcd_gfx_max
/** Custom max function */
#define lcd_gfx_max(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifndef lcd_gfx_array_length
#define lcd_gfx_array_length(array) ((sizeof(array)) / (sizeof(array[0])))
#endif

//#define lcd_gfx_use_menu_add
//#define lcd_gfx_menu_custom_colors

//typedef TNanoEngineOnDraw CallbackFunc; //bool function
// using CallbackFunc = bool (*)();
typedef bool (*CallbackFunc)(void);

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

#ifdef lcd_gfx_use_menu_add
    LcdGfxMenu(const NanoRect &rect = {});
#else
    LcdGfxMenu(const char **items, uint8_t count, CallbackFunc *_itemCallback = nullptr, const NanoRect &rect = {});
    LcdGfxMenu();
#endif

    virtual ~LcdGfxMenu();

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
    virtual void down();

    /**
     * Moves selection pointer up by 1 item. If selected item is the first one,
     * then selection pointer will set to the last item in menu list.
     * Use show() to refresh menu state on the display.
     */
    virtual void up();

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

    /// @brief define o numero maximo de menus na tela
    void setMaxScreenItems(uint_fast8_t _maxScreenItems);
    
    /// @brief distancia das bordas do menu
    void setVerticalOffset(uint_fast8_t _offset);

    /// @brief distance superior do primeiro item
    void setTopOffset(uint_fast8_t _offset);

    /**
     *  @brief espaço usado entre as linhas.
     * @note isso aumenta o tamanho do menu, mas não reduz 'ScreenItems'.
     * por exemplo uma area de 128x128 com uma fonte de 6x8, caberia ao total de 16 ScreenItems.
     * ao definir setLineSpace(5) teriamos agora 10 item + 6 item fora da area do menu.
     * Então para corrigir faça setVerticalOffset(getLineHeight(d) * 6) e setTopOffset(0);
     */
    void setLineSpace(uint_fast8_t _space);

#ifdef lcd_gfx_menu_custom_colors
    void setBackgroundColor(uint16_t color);
    void setTextSelectionColor(uint16_t color);
    void setTextColor(uint16_t color);
    void setSelectionColor(uint16_t color);
    void setContornoColor(uint16_t color);
#endif

    /**
     * Returns total count of menu items in menu.
     */
    uint8_t size();

    /// @brief determina se o focu esta em um botão ou em um menu item.
    bool isButtonSelected();

    /// @brief return button list
    LcdGfxButton **getButtons();

    /// @brief returns to the function of the selected item
    CallbackFunc getItem();

#ifdef lcd_gfx_use_menu_add
    LcdGfxButton *addButton(const char *text, const NanoRect &rect);
    /// @brief .addItem(("name"), &main::setup);
    void addItem(const char *_text, CallbackFunc _itemCallback = nullptr);
#else
    void setButtons(uint_fast8_t _count, LcdGfxButton **_buttons);
    void setItens(uint_fast8_t _count, const char **_items, CallbackFunc *_itemCallback = nullptr);
#endif

    template <typename D>
    uint_fast8_t getLineHeight(D &d){
        return pixelsToLine = (menu.height - menu_offset) / getMaxScreenItems(d); 
    }

    /**
     * Updates size of the object, if it was not set previously
     */
    template <typename D>
    void updateSize(D &d)
    {
        if (!menu.width)
        {
            menu.width = d.width() - menu.left;
        }
        if (!menu.height)
        {
            menu.height = d.height() - menu.top;
        }
    }

protected:
    /// @brief ponteiro pro array de botoes
    LcdGfxButton **m_buttons;
    /// @brief tamanho do array de botoes
    uint_fast8_t m_buttonCount;
    int_fast8_t m_buttonsSelection;

protected:
    SAppMenu menu;
    /// @brief caso nao seja definido ele ira calcular altomaticamente o numero de menus que cabem na tela.
    uint_fast8_t m_CustomMaxScreenItems = 0;
    uint_fast8_t m_MaxScreenItems = 0;
    /// @brief pixels em cada linha
    uint_fast8_t pixelsToLine;
    /// @brief deslocamento do menu.
    uint_fast8_t menu_offset = 0;
    /// @brief espaço em pixels entre os itens do menu.
    uint_fast8_t text_space = 0;
    /// @brief fixa o inicio dos itens em uma posiçao especifica. use -1 para centralizar os itens 
    lcdint_t item_top = -1;

private:
    CallbackFunc *m_CallbackItens;

#ifdef lcd_gfx_menu_custom_colors
    uint16_t m_oldColor = 0xFFFF;             ///< current foreground color
    uint16_t m_oldBgColor = 0x0000;           ///< current background color

    uint16_t m_bgColor = 0x0000;
    uint16_t m_TextSelectionColor = 0x0000;

    uint16_t m_textColor = 0xFFFF;
    uint16_t m_selectionColor = 0xFFFF;
    uint16_t m_contornoColor = 0xFFFF;
#endif

    template <typename D>
    uint8_t getMaxScreenItems(D &d)
    {
        if (m_CustomMaxScreenItems < 1)
            m_MaxScreenItems = lcd_gfx_min(menu.count, (menu.height - menu_offset) / (d.getFont().getHeader().height));
        else
            m_MaxScreenItems = m_CustomMaxScreenItems;
        return m_MaxScreenItems;
    }

    template <typename D>
    uint8_t calculateScrollPosition(D &d, uint8_t selection)
    {
        if (selection < menu.scrollPosition)
        {
            return selection;
        }
        else if (selection - menu.scrollPosition > m_MaxScreenItems - 1)
        {
            return selection - m_MaxScreenItems + 1;
        }
        return menu.scrollPosition;
    }

    template <typename D>
    void drawMenuItem(D &d, uint8_t index)
    {
        uint_fast8_t Top_offset = ((menu.height - menu_offset) - pixelsToLine * m_MaxScreenItems) >> 1;         //pexels sobressalentes da tela, usado para centralizar o menu 
        
        if(item_top < 0){
             Top_offset += menu_offset >> 1;
        }else{
            Top_offset += item_top;
        }
        
        lcdint_t line_Position = Top_offset + menu.top + (index - menu.scrollPosition) * (pixelsToLine + text_space); // onde a linha começa
        lcdint_t line_Center = line_Position + ((pixelsToLine - d.getFont().getHeader().height) >> 1);// imprimir o texto no centro da linha

        if (index == menu.selection && !isButtonSelected())
        {
#ifdef lcd_gfx_menu_custom_colors
            d.setColor(m_selectionColor);
            d.setBackground(m_selectionColor);
#endif
            d.fillRect(menu.left + 1, line_Position, menu.left + menu.width - 2, line_Position + pixelsToLine - 0);
#ifdef lcd_gfx_menu_custom_colors
            d.setColor(m_TextSelectionColor);
#else
            d.invertColors();
#endif
        }
        else
        {
#ifdef lcd_gfx_menu_custom_colors
            d.setBackground(m_bgColor);
            d.setColor(m_bgColor);
            d.fillRect(menu.left + 1, line_Position, menu.left + menu.width - 2, line_Position + pixelsToLine);
            d.setColor(m_textColor);
           
#else
            d.invertColors();
            d.fillRect(menu.left + 1, line_Position, menu.left + menu.width - 2, line_Position + pixelsToLine);
            d.invertColors();
#endif
        }

        d.printFixed(menu.left + 3, line_Center, menu.items[index], STYLE_NORMAL);

        if (index == menu.selection && !isButtonSelected() ){
#ifdef lcd_gfx_menu_custom_colors
            d.setBackground(m_bgColor);
#else
            d.invertColors();
#endif
        }
/*
       Serial.print(" pixelsToLine ");
            Serial.print(pixelsToLine  );

                   Serial.print(" line_Position ");
            Serial.print(line_Position  );
            
                   Serial.print(" line_Center ");
            Serial.print(line_Center  );

                Serial.print(" Top_offset ");
            Serial.print(Top_offset  );

            Serial.print(" m_MaxScreenItems ");
            Serial.println(m_MaxScreenItems  );
*/
    }

    template <typename D>
    void scrollText(D &d, const char *ch)
    {
        //   (message); // 12 = 6 pixels/character * text size 2
        static int index = 0; // scroll speed
        static unsigned long lastMilles = millis();
        unsigned long Milles = millis();
        int leng;
        int minX;

        if (Milles - lastMilles > 1000)
        {
          //  Serial.println("update");
            index++;
            lastMilles = Milles;
            leng = index * d.getFont().getHeader().width;
            minX = menu.width - d.getFont().getTextSize(ch);

            minX = minX + leng;

            if (minX >= 0)
            {
                index = 0;
            }
        }

        for (int i = 0; *ch; i++, ch++)
        {
            if (i >= index)
            {
                d.write(*ch);
            }
        }
    }
};

template <typename D>
void LcdGfxMenu::show(D &d)
{

    updateSize(d);
    pixelsToLine = getLineHeight(d);
    menu.scrollPosition = this->calculateScrollPosition(d, menu.selection);

#ifdef lcd_gfx_menu_custom_colors
    m_oldColor = d.getColor();
    d.invertColors();
    m_oldBgColor = d.getColor();           ///< current background color
    d.invertColors();
#endif

    for (uint8_t i = menu.scrollPosition; i < lcd_gfx_min(menu.count, (menu.scrollPosition + m_MaxScreenItems)); i++)
    {
        this->drawMenuItem(d, i);
    }
    menu.oldSelection = menu.selection;

#ifdef lcd_gfx_menu_custom_colors
    d.setColor(m_contornoColor);
#endif

    d.drawRect(menu.left, menu.top, menu.width + menu.left - 1, menu.height + menu.top - 1);

#ifdef lcd_gfx_menu_custom_colors
    d.setColor(m_oldColor);
    d.setBackground(m_oldBgColor);
#endif

    for ( int_fast8_t i = 0; i < m_buttonCount; i++ )
    {
        m_buttons[i]->setFocus(i == m_buttonsSelection);
        m_buttons[i]->show(d);
    }
}

#endif