# Migrating from the v1 API to the v2 API

lcdgfx evolved from the older `ssd1306` library. The v2 API
(under `src/v2/`) is the preferred entry point — it is the only API
that supports colour displays (TFT, SSD1331, SSD1351), `NanoCanvas`
templates parameterised by colour depth, and the modern `NanoEngine`.

This guide is a side-by-side cheatsheet for projects originally written
against the legacy v1 (`ssd1306.h` style) API, and for v1 patterns that
still appear in older examples.

> **TL;DR**: Include `lcdgfx.h`, instantiate a `Display…` object, call
> `display.begin()`, and use member functions instead of free functions.
> Pixel coordinates and font handles are unchanged.

## Header includes

| v1 (legacy) | v2 (current) |
|---|---|
| `#include "ssd1306.h"` | `#include "lcdgfx.h"` |
| `#include "nano_gfx.h"` | included transitively from `lcdgfx.h` |
| `#include "nano_engine.h"` | included transitively from `lcdgfx.h` |
| `#include "intf/i2c/ssd1306_i2c.h"` | not needed — bus selection is via the display class name |
| `#include "ssd1306_console.h"` | included transitively from `lcdgfx.h` |
| `#include "menu.h"` | `#include "lcdgfx_gui.h"` |

## Display construction

| v1 (free functions, mutates global state) | v2 (member functions on a Display object) |
|---|---|
| `ssd1306_128x64_i2c_init();` | `DisplaySSD1306_128x64_I2C display(-1);` then `display.begin();` |
| `ssd1306_128x64_spi_init(rstPin, ces, dc);` | `DisplaySSD1306_128x64_SPI display(rstPin, {-1, ces, dc, 0, -1, -1});` then `display.begin();` |
| `il9163_128x128_spi_init(rst, ces, dc);` | `DisplayIL9163_128x128x16_SPI display(rst, {-1, ces, dc, 0, -1, -1});` |
| `ssd1331_96x64_spi_init(rst, ces, dc);` | `DisplaySSD1331_96x64x16_SPI display(rst, {-1, ces, dc, 0, -1, -1});` |

The v2 display class name encodes the controller, resolution, and (for
colour controllers) the bit depth. See
[docs/controller_matrix.md](controller_matrix.md) for the full list.

## Drawing primitives

All the pixel coordinates, colour conventions, and font handles are
identical between v1 and v2. The only change is that you call them on
the `display` object instead of as free functions.

| v1 | v2 |
|---|---|
| `ssd1306_clearScreen()`                                    | `display.clear()`                              |
| `ssd1306_fillScreen(0xFF)`                                 | `display.fill(0xFF)`                           |
| `ssd1306_putPixel(x, y)`                                   | `display.putPixel(x, y)`                       |
| `ssd1306_drawLine(x1, y1, x2, y2)`                         | `display.drawLine(x1, y1, x2, y2)`             |
| `ssd1306_drawRect(x1, y1, x2, y2)`                         | `display.drawRect(x1, y1, x2, y2)`             |
| `ssd1306_drawHLine(x1, y1, x2)`                            | `display.drawHLine(x1, y1, x2)`                |
| `ssd1306_drawVLine(x1, y1, y2)`                            | `display.drawVLine(x1, y1, y2)`                |
| `ssd1306_drawBitmap(x, y, w, h, bmp)`                      | `display.drawBitmap1(x, y, w, h, bmp)` (mono)  |
| `ssd1306_drawXBitmap(x, y, w, h, bmp)`                     | `display.drawXBitmap(x, y, w, h, bmp)`         |
| `ssd1306_setColor(c)`                                      | `display.setColor(c)`                          |
| `ssd1306_negativeMode()` / `ssd1306_positiveMode()`        | `display.invertColors()` / `display.normalColors()` (semantics preserved) |

For colour displays, prefer the explicit-depth variants:
`drawBitmap1` (1 bpp), `drawBitmap4` (4 bpp), `drawBitmap8` (8 bpp),
`drawBitmap16` (16 bpp).

## Text rendering

| v1 | v2 |
|---|---|
| `ssd1306_setFixedFont(ssd1306xled_font6x8)` | `display.setFixedFont(ssd1306xled_font6x8)` |
| `ssd1306_printFixed(0, 8, "hi", STYLE_NORMAL)` | `display.printFixed(0, 8, "hi", STYLE_NORMAL)` |
| `ssd1306_printFixedN(0, 8, "BIG", STYLE_NORMAL, FONT_SIZE_2X)` | `display.printFixedN(0, 8, "BIG", STYLE_NORMAL, FONT_SIZE_2X)` |
| `ssd1306_setFont6x8(...)` *(deprecated even in v1)* | use `setFixedFont` with a 6x8 font handle |

Font binaries themselves (`ssd1306xled_font6x8`, `digital_font5x7`,
etc.) are unchanged. The only runtime difference is that v2 stores the
active font on the `display` object rather than as a global.

## Display modes / control

| v1 | v2 |
|---|---|
| `ssd1306_displayOff()` / `ssd1306_displayOn()`     | `display.getInterface().displayOff()` / `displayOn()` *(controller-dependent — see capability matrix)* |
| `ssd1306_setContrast(level)`                       | `display.getInterface().setContrast(level)`           |
| `ssd1306_flipHorizontal(1)` / `ssd1306_flipVertical(1)` | `display.getInterface().flipHorizontal(1)` / `flipVertical(1)` |
| `ssd1306_setStartLine(n)`                          | `display.getInterface().setStartLine(n)` *(mono OLEDs only)* |

The `getInterface()` call returns a reference to the controller-specific
implementation, which exposes only the operations that controller
actually supports. See `docs/controller_matrix.md`.

## NanoCanvas (offscreen buffer)

v2 introduces depth-parameterised canvases.

| v1 | v2 |
|---|---|
| `NanoCanvas1 canvas(W, H, buf);`     | `NanoCanvas<W,H,1> canvas;` (template) or `NanoCanvas1 canvas(W,H,buf)` (still available) |
| `NanoCanvas8 canvas(W, H, buf);`     | `NanoCanvas<W,H,8> canvas;`                    |
| `ssd1306_drawCanvas(x, y, canvas);`  | `display.drawCanvas(x, y, canvas);`           |

The template form sizes the buffer at compile time (no manual
`buf[W*H/8]` array), which is the recommended pattern for new code.

## NanoEngine (sprite / tile engine)

v1's `NanoEngine` was tied to monochrome SSD1306. v2 generalises it
across all controllers.

| v1 | v2 |
|---|---|
| `NanoEngine<TILE_8x8_MONO> engine;`             | `NanoEngine1<DisplaySSD1306_128x64_I2C> engine(display);` |
| `engine.begin(); engine.refresh(); engine.display();` | identical method names |
| `engine.canvas.drawRect(...)`                  | `engine.canvas.drawRect(...)` (canvas type matches the display) |
| `engine.insertSprite(x, y, w, h, bmp);`        | identical signature                                        |

The integer suffix on the engine class (`NanoEngine1`, `NanoEngine8`,
`NanoEngine16`) selects the colour depth.

## Menu / GUI widgets

| v1 (`menu.h`)                  | v2 (`lcdgfx_gui.h`)               |
|---|---|
| `SAppMenu menu; ssd1306_createMenu(&menu, items, count);` | `LcdGfxMenu menu(items, count);` |
| `ssd1306_showMenu(&menu);`      | `menu.show(display);`            |
| `ssd1306_menuDown(&menu);`      | `menu.down();`                    |
| `ssd1306_menuUp(&menu);`        | `menu.up();`                      |
| `ssd1306_menuSelection(&menu);` | `menu.selection();`              |

v2 additionally ships `LcdGfxCheckboxMenu`, `LcdGfxButton`, and
`LcdGfxYesNo` — see `examples/gui/`.

## Common pitfalls

- **Two displays in one sketch.** v1's free-function API made this
  impossible. v2 supports it — just instantiate two display objects.
- **Forgetting `display.begin()`.** v1's `*_init()` did this implicitly
  on first call; v2 requires an explicit `begin()`.
- **Mixing v1 and v2 headers.** Don't include both `ssd1306.h` and
  `lcdgfx.h` in the same translation unit. Pick one.
- **Colour displays and `setColor`.** The `RGB_COLOR8` and
  `RGB_COLOR16` macros (and, soon, the `Color` helper in
  `src/canvas/color.h`) are the supported ways to compose values for
  `setColor()` on colour controllers.
- **Rotation.** v1 had software rotation only. v2 colour displays use
  hardware rotation via `setRotation(0..3)`. See the capability matrix
  for which controllers support which.

## Where to look next

- `examples/` — every example in the repo uses the v2 API.
- [docs/controller_matrix.md](controller_matrix.md) — per-controller
  feature support.
- `src/lcdgfx.h` and `src/lcdgfx_gui.h` — top-level entry headers.
- `src/v2/` — implementation. Note that `src/v2/lcd/<controller>/` is
  auto-generated; edit templates in `tools/templates/lcd/<controller>/`
  and re-run the code generator.
