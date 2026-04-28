/*
    MIT License

    Copyright (c) 2025, Alexey Dynda

    LcdGfxTextEntry demo for lcdgfx.

    Hardware: any 128x64 SSD1306-class OLED on I2C plus a Z-keypad
    on A0 (same as the menu_demo example). Up/Down cycle the
    character at the cursor; Left/Right move the cursor; Enter is
    treated as Right.

    Holds the entered string at the bottom row for visual feedback.
*/

#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "buttons.h"

DisplaySSD1306_128x64_I2C display(-1);

static char editBuffer[9]; // 8 visible cells + NUL
LcdGfxTextEntry entry(editBuffer, 8, NanoRect{0, 8, 0, 0});

void setup()
{
    display.begin();
    display.fill(0x00);
    display.setFixedFont(ssd1306xled_font6x8);
    display.printFixed(0, 0, "Edit text:", STYLE_NORMAL);
    entry.show(display);
}

void loop()
{
    Key key = read_key(A0);
    switch ( key )
    {
        case Key::UP:
            entry.up();
            break;
        case Key::DOWN:
            entry.down();
            break;
        case Key::LEFT:
            entry.left();
            break;
        case Key::RIGHT:
        case Key::ENTER:
            entry.right();
            break;
        default:
            return;
    }
    display.fill(0x00);
    display.printFixed(0, 0, "Edit text:", STYLE_NORMAL);
    entry.show(display);
    display.printFixed(0, 48, ">", STYLE_NORMAL);
    display.printFixed(8, 48, entry.getText(), STYLE_NORMAL);
    delay(120);
}
