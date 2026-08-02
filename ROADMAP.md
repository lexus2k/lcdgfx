# lcdgfx roadmap

This roadmap captures optimization and feature work to keep lcdgfx the most
resource-efficient display library while closing the gaps that make developers
reach for competitors (U8g2, TFT_eSPI, Adafruit GFX, LVGL).

The guiding principle is unchanged: **smallest possible SRAM/flash footprint,
compile-time polymorphism, no mandatory framebuffer.** Every item below must
preserve that on 8-bit AVR while scaling up on ESP32/STM32/RP2040.

## Where lcdgfx already wins

- Interface is a **template parameter** (`NanoDisplayBase<I>`), not a virtual —
  only `begin()`/`end()` are virtual. No vtable dispatch on the draw hot path.
- **Bufferless by default**: `putPixel` writes straight to the controller, so a
  128x64 OLED needs ~25-56 bytes SRAM (vs ~1 KB for a U8g2 full buffer).
- Optional `NanoCanvas<W,H,BPP>` when a RAM buffer is actually wanted.
- Deliberate flash control (`__attribute__((noinline))` on big primitives).
- Code generator keeps 14 controllers consistent and small.

## Optimization / footprint

- [ ] **Batch bus transactions in bufferless mode.** Today `putPixel` and each
      `drawHLine`/`drawVLine` open a fresh `startBlock`/`endBlock` (set-window
      command sequence); `drawLine` addresses the controller *per pixel* over
      I2C. Add a span abstraction that coalesces consecutive same-page writes
      and a page-grouped diagonal path. Biggest real-world speedup, zero RAM
      cost. *(A self-contained first step: rework `drawLine` to emit runs.)*
- [ ] **Reduce template instantiation bloat (flash).** Each concrete
      `Display<CTRL>_<res>_<iface>` re-instantiates the interface-independent
      geometry/text logic. Factor the non-`I` logic into a base compiled once,
      leaving only `send`/`startBlock` shims templated. Helps multi-display and
      `DisplayAny` sketches.
- [ ] **Dirty-rectangle partial refresh** for the buffered `NanoCanvas` path, so
      GUI widgets (menu/slider/spinbox) flush only what changed — less flicker,
      less bus traffic. Pairs naturally with the 1.3.x GUI work.
- [ ] **Font linking guidance + trimming.** `fonts.c` is ~100 KB source. Document
      `-ffunction-sections -fdata-sections -Wl,--gc-sections` prominently and add
      per-glyph-range trimming to `tools/fontgenerator.py` (UTF-8/Cyrillic pulls
      large tables).

## Features to beat competitors (priority order)

- [ ] **1. DMA-accelerated SPI** (ESP32 / RP2040 / STM32). The main reason people
      pick TFT_eSPI. Add async `sendBufferDMA` to the HAL; drivers stay untouched
      thanks to the template interface. Highest strategic impact.
- [x] **2. Adafruit_GFX-compatible facade.** ✅ *Shipped* — see
      `src/canvas/gfx_compat.h` (`GfxCompat<D>` / `makeGfxCompat`) and
      [docs/gfx_compat.md](docs/gfx_compat.md). Lets existing Adafruit GFX
      sketches run on any lcdgfx display with no external library. Now includes
      `fillCircle`, `drawTriangle`/`fillTriangle`, `draw`/`fillRoundRect`,
      `Print`-style overloads, and Adafruit `GFXfont` custom-font rendering.
      Remaining follow-ups: anti-aliased/smooth GFX fonts, `drawXBitmap`,
      colour (`drawRGBBitmap`) bitmaps.
- [ ] **3. E-paper support** (SSD1680 / IL0373, Waveshare). A popular display
      class currently owned by GxEPD/U8g2. The bufferless/partial-refresh model
      fits e-paper well.
- [ ] **4. Capacitive touch** (FT6236 / GT911 / CST816). Only XPT2046 resistive
      exists today (`src/touch/`); modern round IPS and ESP32 boards are all
      capacitive. Reuse the existing menu/touch integration.
- [ ] **5. Published benchmarks + packaging.** Measure footprint/speed vs
      TFT_eSPI and U8g2 and publish them (the advantage is currently invisible).
      First-class PlatformIO and ESP-IDF v5 CMake component support.

## Polish (table stakes)

- [ ] Anti-aliased / smooth fonts and lines for color TFTs.
- [ ] JPEG/PNG image-decode helpers for TFT displays.
- [ ] RGB parallel / i80 8-bit bus for larger panels (ILI9488 / ST7796).
- [ ] RP2040 PIO-based drivers for parallel displays.

## Recommendation

To move from "respected" to "popular", the two strategic items are **DMA SPI**
(removes the "slower than TFT_eSPI" objection) and the **GFX facade + published
benchmarks** (removes migration cost and makes the footprint win visible). The
GFX facade is done; DMA SPI is the next highest-leverage investment.
