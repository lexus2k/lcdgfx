/*
    MIT License

    Copyright (c) 2025, Alexey Dynda

    UTF-8 Cyrillic demo for lcdgfx.

    Demonstrates how to render a UTF-8 encoded string that mixes
    Latin (ASCII) and Cyrillic characters by combining a primary
    ASCII font with the Cyrillic secondary font shipped in
    src/canvas/fonts/. Build flag CONFIG_SSD1306_UNICODE_ENABLE
    is on by default in src/canvas/UserSettings.h, so no extra
    configuration is required.

    Wire any 128x64 SSD1306-class OLED on I2C (SDA -> A4, SCL -> A5
    on Arduino Uno) and flash this sketch.
*/

#include "lcdgfx.h"

DisplaySSD1306_128x64_I2C display(-1);

void setup()
{
    display.begin();
    display.fill(0x00);
    display.setFixedFont(ssd1306xled_font6x8);
    display.getFont().loadSecondaryFont(ssd1306xled_font6x8_Cyrillic);

    // ASCII line (always works, no UTF-8 needed).
    display.printFixed(0, 0, "Hello, lcdgfx!", STYLE_NORMAL);

    // UTF-8 encoded Cyrillic. The bytes below spell "ПРИВЕТ МИР"
    // (U+041F U+0420 U+0418 U+0412 U+0415 U+0422 ' ' U+041C U+0418 U+0420)
    // = D0 9F D0 A0 D0 98 D0 92 D0 95 D0 A2 20 D0 9C D0 98 D0 A0.
    display.printFixed(0, 16,
                       "\xD0\x9F\xD0\xA0\xD0\x98\xD0\x92\xD0\x95\xD0\xA2 "
                       "\xD0\x9C\xD0\x98\xD0\xA0",
                       STYLE_NORMAL);

    // Mixed line: "OK: ХА"
    display.printFixed(0, 32, "OK: \xD0\xA5\xD0\x90", STYLE_NORMAL);
}

void loop()
{
}
