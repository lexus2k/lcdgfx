/*
    MIT License

    Copyright (c) 2018-2021, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "core.h"
#include "lcd_hal/io.h"

#ifdef SDL_EMULATION
#include "sdl_core.h"
#endif
///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE INPUTS CLASS ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** Callback to call if buttons state needs to be updated */
TNanoEngineGetButtons NanoEngineInputs::m_onButtons = nullptr;

uint8_t NanoEngineInputs::m_lastButtons = 0;

uint8_t NanoEngineInputs::m_newButtons = 0;

void NanoEngineInputs::resetButtonsCache()
{
    m_lastButtons = m_newButtons;
    m_newButtons = m_onButtons ? m_onButtons() : 0;
}

bool NanoEngineInputs::pressed(uint8_t buttons)
{
    return (m_onButtons() & buttons) == buttons;
}

bool NanoEngineInputs::notPressed(uint8_t buttons)
{
    return (m_onButtons() & buttons) == 0;
}

bool NanoEngineInputs::clicked(uint8_t buttons)
{
    return ((~m_newButtons & buttons) & (m_lastButtons & buttons)) == buttons;
}

void NanoEngineInputs::connectCustomKeys(TNanoEngineGetButtons handler)
{
    m_onButtons = handler;
}

void NanoEngineInputs::connectArduboyKeys()
{
    m_onButtons = arduboyButtons;
}

uint8_t NanoEngineInputs::s_zkeypadPin;

uint8_t NanoEngineInputs::zkeypadButtons()
{
    int buttonValue = lcd_adcRead(s_zkeypadPin);
    if ( buttonValue < 100 )
        return BUTTON_RIGHT;
    if ( buttonValue < 200 )
        return BUTTON_UP;
    if ( buttonValue < 400 )
        return BUTTON_DOWN;
    if ( buttonValue < 600 )
        return BUTTON_LEFT;
    if ( buttonValue < 800 )
        return BUTTON_A;
    /** Z-keypad has only 5 analog buttons: no button B */
    return BUTTON_NONE;
}

void NanoEngineInputs::connectZKeypad(uint8_t analogPin)
{
    NanoEngineInputs::s_zkeypadPin = analogPin;
    m_onButtons = zkeypadButtons;
}

uint8_t NanoEngineInputs::arduboyButtons()
{
    uint8_t buttons;
/* Arduboy buttons available only for Atmega32U4 platform */
#if defined(__AVR_ATmega32U4__)
    // down, up, left right
    buttons = (((~PINF) & 0B11110000) >> 4);
    // A (left)
    buttons |= (((~PINE) & 0B01000000) >> 2);
    // B (right)
    buttons |= (((~PINB) & 0B00010000) << 1);
#else
    buttons = 0;
#endif
    return buttons;
}

const uint8_t *NanoEngineInputs::s_gpioKeypadPins;

void NanoEngineInputs::connectGpioKeypad(const uint8_t *gpioKeys)
{
    NanoEngineInputs::s_gpioKeypadPins = gpioKeys;
#ifdef SDL_EMULATION
    sdl_set_gpio_keys(gpioKeys);
#endif
    m_onButtons = gpioButtons;
}

uint8_t NanoEngineInputs::gpioButtons()
{
    uint8_t buttons = BUTTON_NONE;
    if ( (s_gpioKeypadPins[0]) && (lcd_gpioRead(s_gpioKeypadPins[0]) == LCD_HIGH) )
        buttons |= BUTTON_DOWN;
    if ( (s_gpioKeypadPins[1]) && (lcd_gpioRead(s_gpioKeypadPins[1]) == LCD_HIGH) )
        buttons |= BUTTON_LEFT;
    if ( (s_gpioKeypadPins[2]) && (lcd_gpioRead(s_gpioKeypadPins[2]) == LCD_HIGH) )
        buttons |= BUTTON_RIGHT;
    if ( (s_gpioKeypadPins[3]) && (lcd_gpioRead(s_gpioKeypadPins[3]) == LCD_HIGH) )
        buttons |= BUTTON_UP;
    if ( (s_gpioKeypadPins[4]) && (lcd_gpioRead(s_gpioKeypadPins[4]) == LCD_HIGH) )
        buttons |= BUTTON_A;
    if ( (s_gpioKeypadPins[5]) && (lcd_gpioRead(s_gpioKeypadPins[5]) == LCD_HIGH) )
        buttons |= BUTTON_B;
    return buttons;
}

void NanoEngineInputs::connectWioKeypad()
{
#ifdef SDL_EMULATION
    static uint8_t keys[] = {31ul, 32ul, 33ul, 34ul, 35ul, 36ul, 37ul, 0};
    connectGpioKeypad(keys);
#else
    lcd_gpioMode(31Ul, LCD_GPIO_INPUT_PULLUP);
    lcd_gpioMode(32Ul, LCD_GPIO_INPUT_PULLUP);
    lcd_gpioMode(33Ul, LCD_GPIO_INPUT_PULLUP);
    lcd_gpioMode(34Ul, LCD_GPIO_INPUT_PULLUP);
    lcd_gpioMode(35Ul, LCD_GPIO_INPUT_PULLUP);
    lcd_gpioMode(28Ul, LCD_GPIO_INPUT_PULLUP);
    lcd_gpioMode(29Ul, LCD_GPIO_INPUT_PULLUP);
    lcd_gpioMode(30Ul, LCD_GPIO_INPUT_PULLUP);
    m_onButtons = wioButtons;
#endif
}

#ifndef SDL_EMULATION
uint8_t NanoEngineInputs::wioButtons()
{
    uint8_t buttons = BUTTON_NONE;
    if ( lcd_gpioRead(34ul) == LCD_LOW )
        buttons |= BUTTON_DOWN;
    if ( lcd_gpioRead(32ul) == LCD_LOW )
        buttons |= BUTTON_LEFT;
    if ( lcd_gpioRead(33ul) == LCD_LOW )
        buttons |= BUTTON_RIGHT;
    if ( lcd_gpioRead(31ul) == LCD_LOW )
        buttons |= BUTTON_UP;
    if ( lcd_gpioRead(35ul) == LCD_LOW )
        buttons |= BUTTON_CENTER;
    if ( lcd_gpioRead(28ul) == LCD_LOW )
        buttons |= BUTTON_A;
    if ( lcd_gpioRead(29ul) == LCD_LOW )
        buttons |= BUTTON_B;
    if ( lcd_gpioRead(30ul) == LCD_LOW )
        buttons |= BUTTON_C;
    return buttons;
}
#endif

uint8_t NanoEngineInputs::s_ky40_clk;
uint8_t NanoEngineInputs::s_ky40_dt;
uint8_t NanoEngineInputs::s_ky40_sw;

uint8_t NanoEngineInputs::ky40Buttons()
{
    static uint8_t last_clk = lcd_gpioRead(s_ky40_clk);
    uint8_t buttons = BUTTON_NONE;
    uint8_t clk = lcd_gpioRead(s_ky40_clk);
    if ( clk != last_clk )
    {
        if ( clk == LCD_HIGH )
        {
            buttons = lcd_gpioRead(s_ky40_dt) == LCD_LOW ? BUTTON_DOWN : BUTTON_UP;
        }
        else
        {
            buttons = lcd_gpioRead(s_ky40_dt) == LCD_HIGH ? BUTTON_DOWN : BUTTON_UP;
        }
    }
    last_clk = clk;
    if ( s_ky40_sw > 0 && lcd_gpioRead(s_ky40_sw) == LCD_LOW )
    {
        buttons |= BUTTON_A;
    }
    return buttons;
}

void NanoEngineInputs::connectKY40encoder(uint8_t pina_clk, uint8_t pinb_dt, int8_t pinc_sw)
{
    s_ky40_clk = pina_clk;
    s_ky40_dt = pinb_dt;
    s_ky40_sw = pinc_sw;
    m_onButtons = NanoEngineInputs::ky40Buttons;
}

///////////////////////////////////////////////////////////////////////////////
////// NANO ENGINE CORE CLASS /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void NanoEngineCore::begin()
{
    m_lastFrameTs = lcd_millis();
}

void NanoEngineCore::setFrameRate(uint8_t fps)
{
    if ( fps > 0 )
    {
        m_fps = fps;
        m_frameDurationMs = 1000 / fps;
    }
}

bool NanoEngineCore::nextFrame()
{
    // We do not need 32-bit data here, since uin16_t allows to address 64 seconds
    bool needUpdate = (uint16_t)(lcd_millis() - m_lastFrameTs) >= m_frameDurationMs;
    if ( needUpdate && m_loop )
        m_loop();
    return needUpdate;
}
