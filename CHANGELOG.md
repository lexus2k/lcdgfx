# Changelog

All notable changes to **lcdgfx** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- **Opt-in DMA for ESP32 hardware SPI** (`CONFIG_ESP32_SPI_DMA_ENABLE`, off by
  default): routes the ESP32 SPI transport through a DMA channel with a
  DMA-capable flush buffer (size via `CONFIG_ESP32_SPI_DMA_BUFFER_SIZE`, default
  512), so bulk pixel writes go out as large DMA transactions. Transfers stay
  blocking, so data/command (DC) line ordering is preserved; with the macro
  undefined the SPI path is byte-for-byte identical to before. Drivers are
  unchanged (the DMA logic lives entirely in the SPI HAL).
- **Per-group exact font width in `fontgenerator.py`** (issue #139): the `-g`
  option now takes an optional 3rd argument — `-g <first> <count> <width>` —
  that forces every char in that group to an exact pixel width, keeping the
  glyph centered (narrow chars padded, wider chars cropped). The width is
  stored per char in the new-format jump table, so it is free at runtime and
  works with `setSpacing()`. Ideal for widening thin chars (space, colon,
  period) without expanding the whole font to a single fixed width (`-fw`).
- **Adafruit_GFX-compatible facade** (`src/canvas/gfx_compat.h`): new header-only
  `GfxCompat<D>` adapter (plus `makeGfxCompat()` helper) that exposes the
  Adafruit_GFX drawing API on top of any lcdgfx display or canvas, with **no
  external Adafruit_GFX library required**.
  - Geometry: `drawPixel`, `drawFast{H,V}Line`, `drawLine`, `draw`/`fillRect`,
    `fillScreen`, `draw`/`fillCircle`, `draw`/`fillTriangle`,
    `draw`/`fillRoundRect`, `drawBitmap`.
  - Text: `setCursor`, `setTextColor`, `setTextSize`, `write`, `print`/`println`
    overloads, and Adafruit `GFXfont` custom-font rendering via
    `setFont(const GFXfont*)` (works on paged monochrome and colour displays;
    PROGMEM fonts supported).
  - Monochrome `0`/`1` colour is mapped to lcdgfx's mask at compile time (zero
    runtime cost); colour values pass through unchanged.
  - Docs: [docs/gfx_compat.md](docs/gfx_compat.md); example:
    `examples/direct_draw/gfx_compat`.

### Changed
- **`drawLine` now coalesces bus transactions.** Instead of addressing the
  controller once per pixel, diagonal lines are emitted as runs of same-row /
  same-column spans (a single `drawHLine`/`drawVLine` per run). Near-horizontal
  and near-vertical lines drop from one set-window burst per pixel to a small
  handful, cutting bus traffic several-fold at zero RAM cost. Output is
  pixel-identical for all existing use; no driver or API changes.

### Fixed
- **Steep and vertical `drawLine` on 1-bit (page-addressed) displays.** In
  bufferless mode each `putPixel` rewrites a whole 8-pixel page byte, so a
  per-pixel diagonal/vertical line clobbered earlier pixels sharing a page and
  rendered with gaps. Routing runs through `drawVLine` builds the correct page
  bytes, so these lines are now solid. Colour displays were unaffected.

## [1.3.1] - 2026-04-29

### Fixed
- **GUI widgets**
  - `LcdGfxMenu` / `LcdGfxCheckboxMenu` — eliminate flicker and scroll
    artifacts; do not invert checkbox mark itself when row is highlighted
    (only label text is inverted); checkbox toggle now reflects on screen
    immediately without needing to move selection.
  - `LcdGfxSlider` — page-aligned 3-fillRect rendering on SSD1306: knob
    band lives in full pages strictly between the top/bottom border rows,
    so border lines are never disturbed and the slider does not flicker.
  - `LcdGfxSpinbox` — central text band cleared explicitly each frame so
    value updates do not leave artifacts; border + arrows redrawn cleanly.

## [1.3.0] - 2026-04-28

### Added
- **GUI widgets**
  - `LcdGfxSlider` — horizontal value slider with bounded range and
    keyboard navigation (`up`/`down`). See `examples/gui/slider_demo`.
  - `LcdGfxSpinbox` — bounded integer spinbox with configurable step.
    See `examples/gui/spinbox_demo`.
  - `LcdGfxTextEntry` — single-line text-entry widget with cursor and
    cycleable charset (ASCII default; UTF-8 charset works with a
    secondary font). See `examples/gui/text_entry_demo`.
- **Touch input**
  - `LcdGfxXpt2046` — templated SPI driver for XPT2046 resistive touch
    controllers. Median-of-3 sampling, pressure detection, swappable
    SPI transport. See `examples/touch_demo`.
  - `TouchCalibration` — per-axis affine calibration helper with
    swap/invert and clamping.
  - `LcdGfxMenu::onTouch()` and `itemAtPoint()` — touch-driving the
    existing menu widget without changing its key API. See
    `examples/gui/menu_touch_demo`.
- **Core API**
  - `lcdgfx::color` namespace — header-only, `constexpr`,
    `to_rgb565` / `to_rgb332` / `from_rgb` / `gray` (BT.601 weights).
    Bit-identical to the existing `RGB_COLOR16` / `RGB_COLOR8` macros.
  - `nano_utf8_decode()` — stateless UTF-8 decoder in
    `canvas/font_utf8.h`. Rejects overlongs, surrogates and
    codepoints above `U+10FFFF`.
  - `ssd1306xled_font6x8_Cyrillic` — proof secondary font covering
    `U+0410..U+042F` (А..Я). See `examples/utf8_demo`.
- **Build / CI**
  - Opt-in `CXXSTD` knob in `Makefile` (defaults to `c++11`).
  - CI matrix now builds the library and runs the unit tests under
    `c++11`, `c++14` and `c++17`.
- **Documentation**
  - `docs/controller_matrix.md` — capability matrix for every
    supported display controller.
  - `docs/migration_v1_v2.md` — migration guide from the v1 API to
    the v2 API.

### Fixed
- `SSD1325::fillRect` 4-bit nibble-packing now respects `setColor()`
  for odd columns.
- Linux sysfs GPIO fallback no longer leaks the export handle on
  early-return error paths.

### Tests
- Unit-test count: **363 → 470** (107 new tests across 7 new test
  files), all green under `make ARCH=linux SDL_EMULATION=y check`.

## [1.2.0] - prior release

- ILI9341 SDL emulator 90° CW rotation fix.
- Comprehensive Doxygen documentation overhaul.
- Checkbox menu widget and scroll indicators.
- libgpiod line-ownership bug fix (issue #134).
- Other fixes covering issues #28, #74, #92, #96, #98, #100, #125, #137.
