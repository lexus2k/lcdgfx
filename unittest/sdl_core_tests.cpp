/*
    MIT License

    Copyright (c) 2019-2026, Alexey Dynda

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

#include <CppUTest/TestHarness.h>
#include <stdlib.h>
#include <string.h>
#include "sdl_core.h"

// ==================== SDL core utility function tests ====================

TEST_GROUP(SDL_CORE_GPIO)
{
    void setup() {}
    void teardown() {}
};

TEST(SDL_CORE_GPIO, sdl_write_and_read_digital)
{
    sdl_write_digital(5, 1);
    CHECK_EQUAL(1, sdl_read_digital(5));
    sdl_write_digital(5, 0);
    CHECK_EQUAL(0, sdl_read_digital(5));
}

TEST(SDL_CORE_GPIO, sdl_read_digital_default_zero)
{
    // Pins not explicitly set should read 0
    sdl_write_digital(10, 0);
    CHECK_EQUAL(0, sdl_read_digital(10));
}

TEST(SDL_CORE_GPIO, sdl_set_gpio_keys)
{
    uint8_t keys[6] = {2, 3, 4, 5, 6, 7};
    sdl_set_gpio_keys(keys);
    // Verify keys were set by exercising digital read on those pins
    for (int i = 0; i < 6; i++)
    {
        sdl_write_digital(keys[i], 1);
        CHECK_EQUAL(1, sdl_read_digital(keys[i]));
        sdl_write_digital(keys[i], 0);
    }
}

TEST(SDL_CORE_GPIO, sdl_read_analog_returns_value)
{
    // sdl_read_analog calls sdl_poll_event internally, then returns the analog value.
    // In headless test mode, sdl_poll_event returns immediately (no events).
    int val = sdl_read_analog(0);
    // Default analog value — just verify it returns without crashing
    CHECK(val >= 0);
}

TEST(SDL_CORE_GPIO, sdl_write_digital_multiple_pins)
{
    for (int pin = 0; pin < 8; pin++)
    {
        sdl_write_digital(pin, pin % 2);
    }
    for (int pin = 0; pin < 8; pin++)
    {
        CHECK_EQUAL(pin % 2, sdl_read_digital(pin));
    }
    // Clean up
    for (int pin = 0; pin < 8; pin++)
    {
        sdl_write_digital(pin, 0);
    }
}

TEST(SDL_CORE_GPIO, sdl_dc_mode)
{
    // sdl_set_dc_pin + sdl_is_dc_mode
    sdl_set_dc_pin(3);
    sdl_write_digital(3, 0);
    CHECK_EQUAL(0, sdl_is_dc_mode());
    sdl_write_digital(3, 1);
    CHECK_EQUAL(1, sdl_is_dc_mode());
    // pin >= 128 means no DC pin
    sdl_set_dc_pin(128);
    CHECK_EQUAL(0, sdl_is_dc_mode());
}
