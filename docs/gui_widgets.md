# GUI widgets

`lcdgfx` ships a small set of GUI widgets that work on every supported
display (mono OLED through 16-bit TFT) and require no dynamic memory.
All widgets share the same lifecycle:

```cpp
display.begin();
display.fill(0x00);
widget.show(display);            // initial render
// ... on user input:
widget.up();   widget.show(display);
widget.down(); widget.show(display);
```

| Widget                | Header                     | Example                                  |
|-----------------------|----------------------------|------------------------------------------|
| `LcdGfxMenu`          | `v2/gui/menu.h`            | `examples/gui/menu_demo`                 |
| `LcdGfxCheckboxMenu`  | `v2/gui/checkbox_menu.h`   | `examples/gui/checkbox_demo`             |
| `LcdGfxButton`        | `v2/gui/button.h`          | `examples/gui/button_demo`               |
| `LcdGfxSlider`        | `v2/gui/slider.h`          | `examples/gui/slider_demo`               |
| `LcdGfxSpinbox`       | `v2/gui/spinbox.h`         | `examples/gui/spinbox_demo`              |
| `LcdGfxTextEntry`     | `v2/gui/text_entry.h`      | `examples/gui/text_entry_demo`           |
| `LcdGfxYesNo`         | `v2/gui/yesno.h`           | `examples/gui/yesno_demo`                |

## Touch

Menus accept touch coordinates through `onTouch(x, y)` /
`itemAtPoint(x, y)`; see `examples/gui/menu_touch_demo`. Touch
samples come from any source — typically the bundled
[`LcdGfxXpt2046`](touch.md) driver.

## UTF-8

`LcdGfxTextEntry` cycles through a charset you provide. To accept
Cyrillic input, point the widget's secondary font at
`ssd1306xled_font6x8_Cyrillic` and feed UTF-8 codepoints — the
stateless decoder lives in `canvas/font_utf8.h`.
