# This is Makefile for ESP32 IDF

COMPONENT_ADD_INCLUDEDIRS := ./src \

COMPONENT_SRCDIRS := ./src \
                     ./src/lcd_hal \
                     ./src/lcd_hal/esp \
                     ./src/v2 \
                     ./src/v2/gui \
                     ./src/v2/lcd \
                     ./src/v2/lcd/lcdany \
                     ./src/v2/lcd/lcdwio \
                     ./src/v2/lcd/lcdttgo \
                     ./src/v2/lcd/pcd8544 \
                     ./src/v2/lcd/ssd1306 \
                     ./src/v2/lcd/sh1107 \
                     ./src/v2/lcd/sh1106 \
                     ./src/v2/lcd/ssd1325 \
                     ./src/v2/lcd/ssd1327 \
                     ./src/v2/lcd/ssd1331 \
                     ./src/v2/lcd/ssd1351 \
                     ./src/v2/lcd/st7735 \
                     ./src/v2/lcd/st7789 \
                     ./src/v2/lcd/il9163 \
                     ./src/v2/lcd/ili9341 \
                     ./src/canvas \
                     ./src/canvas/fonts \
                     ./src/v2/nano_engine \
