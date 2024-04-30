/* This is an example for the microcontroller Raspberry Pi Pico
 * Simply create a C/C++ project with "New Project" wizzard to create a:
    * - CMakeLists.txt
    * - MyProject.cpp
    * - pico_sdk_import.cmake
 * Now clone the lcdgfx library into the project folder.
 * Modify the CMakeLists.txt to include the lcdgfx library (check the last 4 commands of this example)
 * And use the library like for Arduino.
 * 
 * When the Pico project is build, CMake will also automatically compile and link the lcdgfx library.
 */
#include <stdio.h>
#include "pico/stdlib.h"

// Pin definitions for OLED display
// NOTE: SCK & MOSI pins must be one of the hardware peripheral SPI pins
#define OLED_RST 3
#define OLED_DC 2
#define OLED_CS 5
#define OLED_SCK 18
#define OLED_MOSI 19
#define SPI_FREQ 0 // 0 means default frequency

// spi0 is used by default, to use spi1 uncomment the following line:
//#define PICO_USE_SPI1

// Include lcdgfx and use like for Arduino
#include "lcdgfx.h"
DisplaySSD1306_128x64_SPI oled(OLED_RST, {-1, OLED_CS, OLED_DC, SPI_FREQ, OLED_SCK, OLED_MOSI});


int main()
{
    stdio_init_all();

    oled.begin();
    oled.setFixedFont( ssd1306xled_font6x8 );
    oled.clear();
    oled.printFixed(0,  0, "Normal text", STYLE_NORMAL);
    
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
