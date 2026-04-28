# Changelog

All notable changes to **lcdgfx** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
