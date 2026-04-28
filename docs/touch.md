# Touch input (XPT2046)

`lcdgfx` v1.3.0 ships a templated XPT2046 resistive-touch driver in
`src/touch/xpt2046.h`. The driver is transport-agnostic: any class
exposing `begin()`, `end()`, and `transfer(uint8_t)` works.

## Wiring

| XPT2046 | MCU pin (typical) |
|---------|-------------------|
| `T_CLK` | SPI SCK           |
| `T_DIN` | SPI MOSI          |
| `T_DO`  | SPI MISO          |
| `T_CS`  | any GPIO          |
| `T_IRQ` | any GPIO (optional, falling-edge) |

`T_CS` may safely be the same physical CS as a non-XPT slave on the
same bus as long as you select only one device at a time. The driver
uses 2.5 MHz SPI mode 0.

## Minimal example

```cpp
#include "lcdgfx_xpt2046.h"   // pulled in by lcdgfx_gui.h

LcdGfxXpt2046<SPI> touch(/*cs*/ 8, /*irq*/ 9);

void setup() {
    touch.begin();
    touch.setCalibration({ /*ax*/ 0.072f, /*bx*/ -16.0f,
                           /*ay*/ 0.085f, /*by*/ -12.0f,
                           /*swap*/ false, /*invert_x*/ false,
                           /*invert_y*/ false,
                           /*w*/ 240, /*h*/ 320 });
}

void loop() {
    if (touch.isPressed()) {
        int16_t x, y;
        if (touch.read(x, y)) menu.onTouch(x, y);
    }
}
```

`TouchCalibration` performs a per-axis affine transform (`x' =
ax * raw + bx`) followed by optional axis swap, axis invert, and
output clamping. A 3-point or 5-point calibration helper is *not*
shipped — pick three on-screen targets, record the raw samples, and
solve for `ax/bx`.

See `examples/gui/touch_demo` for a full sketch.
