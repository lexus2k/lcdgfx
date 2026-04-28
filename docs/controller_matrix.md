# Controller Capability Matrix

This page documents the per-controller capabilities of every display driver
shipped with lcdgfx. It is generated from the code-generator templates in
`tools/templates/lcd/<controller>/<controller>.json` (`functions.interface_list`
field) and verified against the auto-generated drivers in
`src/v2/lcd/<controller>/`.

## Legend

- **BPP**: native bits-per-pixel of the controller's frame data.
- **Resolutions**: panel resolutions for which an init sequence is shipped.
- **Bus**: data bus types supported by the HAL (`SPI`, `I2C`, `custom`).
- **Rotation**: software (`sw`) or hardware (`hw`) rotation. `sw` means the
  library rotates pixel coordinates before sending; `hw` means the controller
  has a memory-access-control register that handles the transform itself.
- **Invert**: `normalMode()` / `invertMode()` colour-inversion API available.
- **Contrast**: `setContrast()` API available.
- **HW scroll**: hardware vertical-scroll register exposed (`setStartLine`).
- **Flip H/V**: `flipHorizontal()` / `flipVertical()` API available.
- **FB needed**: a software framebuffer (NanoCanvas) is *required* to use this
  controller efficiently in some modes (e.g. SH1107 landscape).
- **Status**: `stable` (well-tested, no known correctness bugs);
  `limited` (works but has documented constraints);
  `buggy` (has open known issues — see notes).

## Monochrome OLED / LCD (1 bpp)

| Controller | BPP | Resolutions | Bus | Rotation | Invert | Contrast | HW scroll | Flip H/V | FB needed | Status | Notes |
|---|---|---|---|---|---|---|---|---|---|---|---|
| SSD1306 | 1 | 64x32, 64x48, 128x32, 128x64 | SPI, I2C, custom | sw | ✅ | ✅ | ✅ | ✅ | no | stable | The reference driver for this library. |
| SH1106  | 1 | 128x64                        | SPI, I2C         | sw | ✅ | ✅ | ✅ | ✅ | no | stable | SSD1306-compatible command set with column offset. |
| SH1107  | 1 | 128x64, 64x128                | SPI, I2C         | sw | ✅ | ✅ | ✅ | ✅ | yes (landscape) | limited | 64x128 landscape rotation requires a software framebuffer (memory layout is page-based on the long axis). |
| PCD8544 | 1 | 84x48                         | SPI              | sw | —  | —  | —  | —  | no | stable | Nokia 5110. No invert / contrast API. |

## Greyscale OLED (4 bpp)

| Controller | BPP | Resolutions | Bus | Rotation | Invert | Contrast | HW scroll | Flip H/V | FB needed | Status | Notes |
|---|---|---|---|---|---|---|---|---|---|---|---|
| SSD1325 | 4 | 128x64  | SPI, I2C | sw | — | ✅ | — | — | no | buggy   | `fillRect` has a known nibble-packing issue at odd column boundaries with `setColor(0x0F)` — see `unittest/canvas_tests.cpp`. |
| SSD1327 | 4 | 128x128 | SPI, I2C | sw | — | ✅ | — | — | no | limited | `printFixedN` with `BOLD` / `SIZE_2X` is not supported (architectural — the renderer assumes 1-bpp glyphs at this scale). |

## Colour OLED (8 / 16 bpp)

| Controller | BPP | Resolutions | Bus | Rotation | Invert | Contrast | HW scroll | Flip H/V | FB needed | Status | Notes |
|---|---|---|---|---|---|---|---|---|---|---|---|
| SSD1331 | 8, 16 | 96x64           | SPI | hw (`setRotation`) | — | ✅ | — | — | no | stable | Hardware-accelerated `drawLine` and `copyBlock`. |
| SSD1351 | 16    | 96x96, 128x128  | SPI | hw (`setRotation`) | — | ✅ | — | — | no | stable | `setOffset()` and `setRgbMode()` available. |

## Colour TFT (16 bpp, RGB565)

| Controller | BPP | Resolutions | Bus | Rotation | Invert | Contrast | HW scroll | Flip H/V | FB needed | Status | Notes |
|---|---|---|---|---|---|---|---|---|---|---|---|
| IL9163  | 16 | 128x128, 128x160          | SPI | hw (`setRotation`)                  | ✅ | — | — | — | no | stable  | Backlight control is the user's responsibility. |
| ST7735  | 16 | 80x160, 128x128, 128x160  | SPI | hw (`setRotation`)                  | —  | — | — | — | no | stable  | `setOffset()` and `setRgbMode()` available. |
| ST7789  | 16 | 135x240, 170x320, 240x240 | SPI | hw (`setRotation`)                  | ✅ | — | — | — | no | stable  | Many panels need `INVON` for correct colour — the init sequence sets it by default; toggle with `normalMode()` / `invertMode()`. |
| ILI9341 | 16 | 128x160, 240x320          | SPI | hw (`setRotation` + `rotateOutput`) | ✅ | — | — | — | no | stable  | SDL emulator rotation fixed in v1.2.0. |
| ILI9488 | 16 | 320x480                    | SPI | hw                                   | —  | — | — | — | no | limited | Shipped via custom interface — see `examples/`. |

## Caveats and conventions

- All controllers expose a 1-bit "pixel" API (`putPixel`, `drawLine`, etc.) as
  well as the native-depth API. On greyscale / colour controllers the
  monochrome API maps "black" to 0 and "white" to the maximum native value.
- Rotation: `sw` rotation costs CPU on every draw call; `hw` rotation costs
  one register write at setup. Mixed-mode rotation is not supported.
- "Bus" lists what the HAL templates emit. Adding a new bus to a stable
  controller is usually a matter of editing the JSON template — see
  `tools/templates/lcd/<controller>/<controller>.json`.
- Files in `src/v2/lcd/<controller>/` are auto-generated. Edit the templates
  in `tools/templates/lcd/<controller>/` and re-run
  `cd tools && python3 lcd_code_generator.py -c <controller>` to regenerate.

## How to read this matrix when picking a controller

- **Need rotation in firmware?** Pick a colour TFT or SSD1331 / SSD1351 — they
  have hardware rotation. Mono OLED rotation is software-only and slower.
- **Need contrast control?** All OLEDs support it. TFTs do not (use the
  backlight PWM instead).
- **Need hardware vertical scroll?** Only mono OLEDs (SSD1306, SH1106, SH1107)
  expose `setStartLine`.
- **Tightest flash / RAM budget (e.g. ATtiny)?** Stick with SSD1306 / SH1106.
  4-bpp and 16-bpp drivers carry larger init sequences and pixel paths.
