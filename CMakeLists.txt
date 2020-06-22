idf_component_register(
    SRCS
        ./src/canvas/fonts/fonts.c
        ./src/canvas/canvas.cpp
        ./src/canvas/font.cpp
        ./src/lcd_hal/esp/esp32_spi.cpp
        ./src/lcd_hal/esp/esp8266_spi.cpp
        ./src/lcd_hal/esp/esp32_i2c.cpp
        ./src/lcd_hal/esp/esp8266_i2c.cpp
        ./src/lcd_hal/esp/platform.cpp
        ./src/lcd_hal/interface.c
        ./src/v2/lcd/ili9341/lcd_ili9341.cpp
        ./src/v2/lcd/lcd_common.cpp
        ./src/v2//nano_engine/core.cpp
        ./src/v2/ssd1306_utf8.c
    INCLUDE_DIRS
        ./src
)

# This is Makefile for ESP32 IDF

COMPONENT_ADD_INCLUDEDIRS := ./src \

COMPONENT_SRCDIRS := ./src \
                     ./src/lcd_hal \
                     ./src/lcd_hal/esp \
                     ./src/v2 \
                     ./src/v2/lcd \
                     ./src/v2/lcd/ili9341 \
                     ./src/canvas \
                     ./src/canvas/fonts \
                     ./src/v2/nano_engine \
