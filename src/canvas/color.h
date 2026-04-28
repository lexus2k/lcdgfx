/*
    MIT License

    Copyright (c) 2026, Alexey Dynda

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
/**
 * @file color.h Unified Color helper for lcdgfx.
 *
 * Header-only, C++11-compatible. All operations are constexpr.
 *
 * The helper is *additive*: existing RGB_COLOR8 / RGB_COLOR16 / RGB_COLOR4 /
 * GRAY_COLOR4 macros remain the canonical fast-path for embedded targets and
 * are not deprecated. The Color struct is intended for new code that wants:
 *
 *  - One typed value that can be converted to whatever the active display
 *    expects, instead of reaching for a different macro per controller.
 *  - Round-trip conversions for testing / SDL emulation.
 *  - Compile-time evaluation of palette tables.
 *
 * Conventions
 * -----------
 *  - Internal storage is 24-bit RGB888 in a uint32_t (0x00RRGGBB).
 *  - to_rgb565 produces the 5-6-5 layout that lcdgfx 16bpp drivers expect.
 *  - to_rgb332 produces the 3-3-2 layout that lcdgfx 8bpp drivers expect.
 *  - to_gray4 produces the 4-bit luminance value SSD1325 / SSD1327 use.
 *  - to_mono produces 0 or 1 using the same threshold as the legacy macros.
 *
 * @code{.cpp}
 * #include "canvas/color.h"
 *
 * constexpr auto red   = lcdgfx::Color::from_rgb(255, 0, 0);
 * constexpr auto red16 = red.to_rgb565();   // 0xF800
 * constexpr auto red8  = red.to_rgb332();   // 0xE0
 *
 * display.setColor(red.to_rgb565());
 * @endcode
 */

#pragma once

#include <stdint.h>

namespace lcdgfx
{

/**
 * 24-bit RGB colour with constexpr conversions to all native lcdgfx
 * pixel formats. See file docstring for conventions.
 */
struct Color
{
    /** 0x00RRGGBB (R in bits 23..16, G in bits 15..8, B in bits 7..0). */
    uint32_t rgb;

    /** Construct from 8-bit-per-channel RGB. Components are clamped via &0xFF. */
    static constexpr Color from_rgb(uint8_t r, uint8_t g, uint8_t b)
    {
        return Color{ (uint32_t)((uint32_t)r << 16) | (uint32_t)((uint32_t)g << 8) | (uint32_t)b };
    }

    /** Construct from a 24-bit packed value (e.g. 0xFF8800). */
    static constexpr Color from_rgb888(uint32_t v)
    {
        return Color{ v & 0x00FFFFFFu };
    }

    /** Construct from a 5-6-5 packed 16-bit value, expanding back to RGB888. */
    static constexpr Color from_rgb565(uint16_t v)
    {
        // 5-bit r/b expanded to 8 by replicating high bits into low.
        return from_rgb(
            (uint8_t)(((v >> 11) & 0x1F) * 255u / 31u),
            (uint8_t)(((v >>  5) & 0x3F) * 255u / 63u),
            (uint8_t)(( v        & 0x1F) * 255u / 31u));
    }

    /** Construct from a 3-3-2 packed 8-bit value, expanding back to RGB888. */
    static constexpr Color from_rgb332(uint8_t v)
    {
        return from_rgb(
            (uint8_t)(((v >> 5) & 0x07) * 255u / 7u),
            (uint8_t)(((v >> 2) & 0x07) * 255u / 7u),
            (uint8_t)(( v       & 0x03) * 255u / 3u));
    }

    /** Construct from a 4-bit luminance (0..15) — broadcasts to all channels. */
    static constexpr Color from_gray4(uint8_t v)
    {
        return from_rgb(
            (uint8_t)(((v & 0x0F) * 255u) / 15u),
            (uint8_t)(((v & 0x0F) * 255u) / 15u),
            (uint8_t)(((v & 0x0F) * 255u) / 15u));
    }

    /** Construct from a 1-bit value (0 = black, anything else = white). */
    static constexpr Color from_mono(uint8_t v)
    {
        return v ? from_rgb(0xFF, 0xFF, 0xFF) : from_rgb(0, 0, 0);
    }

    /** Red channel (0..255). */
    constexpr uint8_t r() const { return (uint8_t)((rgb >> 16) & 0xFFu); }
    /** Green channel (0..255). */
    constexpr uint8_t g() const { return (uint8_t)((rgb >> 8) & 0xFFu); }
    /** Blue channel (0..255). */
    constexpr uint8_t b() const { return (uint8_t)(rgb & 0xFFu); }

    /** Pack as 24-bit RGB888 (0x00RRGGBB). */
    constexpr uint32_t to_rgb888() const { return rgb & 0x00FFFFFFu; }

    /** Pack as 5-6-5 RGB565 (matches RGB_COLOR16 macro for matching inputs). */
    constexpr uint16_t to_rgb565() const
    {
        return (uint16_t)(((rgb >> 8)  & 0xF800u) |
                          ((rgb >> 5)  & 0x07E0u) |
                          ((rgb >> 3)  & 0x001Fu));
    }

    /** Pack as 3-3-2 RGB332 (matches RGB_COLOR8 macro for matching inputs). */
    constexpr uint8_t to_rgb332() const
    {
        return (uint8_t)(((rgb >> 16) & 0xE0u) |
                         ((rgb >>  11) & 0x1Cu) |
                         ((rgb >>  6)  & 0x03u));
    }

    /** Pack as 4-bit luminance using the standard 0.299R + 0.587G + 0.114B
     *  weighting, rounded to 4 bits. */
    constexpr uint8_t to_gray4() const
    {
        // Integer approximation of 0.299*R + 0.587*G + 0.114*B, then >>4.
        // 77*R + 150*G + 29*B fits in 16 bits and matches the standard
        // ITU-R BT.601 weighting to within one LSB.
        return (uint8_t)((((uint32_t)r() * 77u +
                           (uint32_t)g() * 150u +
                           (uint32_t)b() * 29u) >> 8) >> 4);
    }

    /** Reduce to 1 bit (0 or 1). Threshold = 128 on luminance. */
    constexpr uint8_t to_mono() const
    {
        return (((uint32_t)r() * 77u +
                 (uint32_t)g() * 150u +
                 (uint32_t)b() * 29u) >= (128u * 256u)) ? 1u : 0u;
    }

    /** Compile-time-selectable convert: to_native<bpp>(). Falls back to
     *  RGB888 for unrecognised depths so user code does not silently miscompile.
     *  Supported depths: 1, 4 (luminance), 8 (RGB332), 16 (RGB565), 24 (RGB888). */
    template <unsigned Bpp>
    constexpr uint32_t to_native() const;

    constexpr bool operator==(const Color &other) const { return to_rgb888() == other.to_rgb888(); }
    constexpr bool operator!=(const Color &other) const { return !(*this == other); }
};

template <> constexpr uint32_t Color::to_native<1>()  const { return to_mono(); }
template <> constexpr uint32_t Color::to_native<4>()  const { return to_gray4(); }
template <> constexpr uint32_t Color::to_native<8>()  const { return to_rgb332(); }
template <> constexpr uint32_t Color::to_native<16>() const { return to_rgb565(); }
template <> constexpr uint32_t Color::to_native<24>() const { return to_rgb888(); }

} // namespace lcdgfx
