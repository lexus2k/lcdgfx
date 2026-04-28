# Hardware abstraction layer

This directory contain platform specific implementation of hardware abstraction layer.

  * arduino dir: for all Arduino platforms (if you use Arduino IDE)
  * avr dir: for plain avr-gcc environment
  * esp dir: for plain esp8266/esp32 environment
  * linux dir: for linux and mac platforms including raspberry pi
  * mingw dir: for running under windows
  * stm32 dir: for plain stm32 support (not implemented)

Edit UserSettings.h header file, if you want to disable some parts of ssd1306 library to reduce memory consumption in your project

## Linux GPIO backend selection

The Linux HAL (`linux/platform.cpp`) auto-detects which GPIO API to use:

  * If `<gpiod.h>` is available at compile time (libgpiod), the modern
    character-device path (`/dev/gpiochip0`) is used. **This is the only
    path that works on Linux kernels >= 6.6**, where the legacy
    `/sys/class/gpio/...` interface is no longer present.
  * Otherwise, the deprecated sysfs fallback is compiled in. This path
    still works on older kernels (< 6.6) and on minimal images that do
    not ship libgpiod.

Selection is automatic via `__has_include(<gpiod.h>)`. To force the
modern path on a Raspberry Pi running a recent OS, install:

```
sudo apt install libgpiod-dev
```

and rebuild lcdgfx. If neither path works at runtime, lcdgfx prints a
diagnostic indicating which fallback was attempted and what to install.

## Permission notes

Both backends typically require either root privileges or membership of
the `gpio` group (and, for SPI/I²C, the `spi` / `i2c` groups). On
Raspberry Pi OS the default `pi` user is already in these groups; on
custom distributions you may need to add the user manually.
  
