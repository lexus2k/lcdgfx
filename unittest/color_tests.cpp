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

#include <CppUTest/TestHarness.h>
#include "canvas/color.h"
#include "canvas/canvas_types.h"

using lcdgfx::Color;

TEST_GROUP(Color_basic)
{
};

TEST(Color_basic, from_rgb_packs_components)
{
    auto c = Color::from_rgb(0x12, 0x34, 0x56);
    CHECK_EQUAL(0x12u, c.r());
    CHECK_EQUAL(0x34u, c.g());
    CHECK_EQUAL(0x56u, c.b());
    CHECK_EQUAL(0x00123456u, c.to_rgb888());
}

TEST(Color_basic, from_rgb888_masks_alpha)
{
    auto c = Color::from_rgb888(0xFF112233u);
    CHECK_EQUAL(0x00112233u, c.to_rgb888());
}

TEST(Color_basic, equality)
{
    auto a = Color::from_rgb(10, 20, 30);
    auto b = Color::from_rgb(10, 20, 30);
    auto c = Color::from_rgb(11, 20, 30);
    CHECK_TRUE(a == b);
    CHECK_TRUE(a != c);
}

TEST_GROUP(Color_rgb565)
{
};

TEST(Color_rgb565, matches_legacy_macro_for_pure_red)
{
    // Pure red: RGB_COLOR16(255, 0, 0)
    uint16_t legacy = RGB_COLOR16(255, 0, 0);
    auto via_helper = Color::from_rgb(255, 0, 0).to_rgb565();
    CHECK_EQUAL(legacy, via_helper);
    CHECK_EQUAL((uint16_t)0xF800, via_helper);
}

TEST(Color_rgb565, matches_legacy_macro_for_pure_green)
{
    uint16_t legacy = RGB_COLOR16(0, 255, 0);
    auto via_helper = Color::from_rgb(0, 255, 0).to_rgb565();
    CHECK_EQUAL(legacy, via_helper);
    CHECK_EQUAL((uint16_t)0x07E0, via_helper);
}

TEST(Color_rgb565, matches_legacy_macro_for_pure_blue)
{
    uint16_t legacy = RGB_COLOR16(0, 0, 255);
    auto via_helper = Color::from_rgb(0, 0, 255).to_rgb565();
    CHECK_EQUAL(legacy, via_helper);
    CHECK_EQUAL((uint16_t)0x001F, via_helper);
}

TEST(Color_rgb565, matches_legacy_macro_for_assorted_values)
{
    const uint8_t cases[][3] = {
        { 0x00, 0x00, 0x00 },
        { 0xFF, 0xFF, 0xFF },
        { 0x10, 0x20, 0x30 },
        { 0x80, 0x40, 0xC0 },
        { 0xAB, 0xCD, 0xEF },
    };
    for ( auto &v : cases )
    {
        uint16_t legacy = RGB_COLOR16(v[0], v[1], v[2]);
        auto via_helper = Color::from_rgb(v[0], v[1], v[2]).to_rgb565();
        CHECK_EQUAL(legacy, via_helper);
    }
}

TEST(Color_rgb565, round_trip_preserves_565_bits)
{
    // from_rgb888 → to_rgb565 → from_rgb565 → to_rgb565 must be a fixed point.
    auto orig = Color::from_rgb(0xAB, 0xCD, 0xEF);
    uint16_t pack = orig.to_rgb565();
    auto reconstructed = Color::from_rgb565(pack);
    CHECK_EQUAL(pack, reconstructed.to_rgb565());
}

TEST_GROUP(Color_rgb332)
{
};

TEST(Color_rgb332, matches_legacy_macro_for_assorted_values)
{
    const uint8_t cases[][3] = {
        { 0x00, 0x00, 0x00 },
        { 0xFF, 0xFF, 0xFF },
        { 0xE0, 0x40, 0x80 },
        { 0xAB, 0xCD, 0xEF },
        { 0x12, 0x34, 0x56 },
    };
    for ( auto &v : cases )
    {
        uint8_t legacy = RGB_COLOR8(v[0], v[1], v[2]);
        auto via_helper = Color::from_rgb(v[0], v[1], v[2]).to_rgb332();
        CHECK_EQUAL((unsigned)legacy, (unsigned)via_helper);
    }
}

TEST(Color_rgb332, round_trip_preserves_332_bits)
{
    auto orig = Color::from_rgb(0x80, 0x40, 0xC0);
    uint8_t pack = orig.to_rgb332();
    auto reconstructed = Color::from_rgb332(pack);
    CHECK_EQUAL((unsigned)pack, (unsigned)reconstructed.to_rgb332());
}

TEST_GROUP(Color_gray)
{
};

TEST(Color_gray, gray4_white_is_15)
{
    CHECK_EQUAL(15u, Color::from_rgb(0xFF, 0xFF, 0xFF).to_gray4());
}

TEST(Color_gray, gray4_black_is_0)
{
    CHECK_EQUAL(0u, Color::from_rgb(0x00, 0x00, 0x00).to_gray4());
}

TEST(Color_gray, gray4_from_gray4_round_trip)
{
    for ( uint8_t v = 0; v <= 0x0F; v++ )
    {
        auto c = Color::from_gray4(v);
        CHECK_EQUAL((unsigned)v, (unsigned)c.to_gray4());
    }
}

TEST(Color_gray, gray4_green_brighter_than_blue)
{
    // BT.601 weights: green ~ 5x blue. Same numeric channel value should
    // produce a higher gray4 for green than for blue.
    auto green_only = Color::from_rgb(0, 0xFF, 0).to_gray4();
    auto blue_only  = Color::from_rgb(0, 0, 0xFF).to_gray4();
    CHECK_TRUE(green_only > blue_only);
}

TEST_GROUP(Color_mono)
{
};

TEST(Color_mono, white_is_1)
{
    CHECK_EQUAL(1u, Color::from_rgb(0xFF, 0xFF, 0xFF).to_mono());
}

TEST(Color_mono, black_is_0)
{
    CHECK_EQUAL(0u, Color::from_rgb(0, 0, 0).to_mono());
}

TEST(Color_mono, from_mono_round_trip)
{
    CHECK_EQUAL(0u, Color::from_mono(0).to_mono());
    CHECK_EQUAL(1u, Color::from_mono(1).to_mono());
    CHECK_EQUAL(1u, Color::from_mono(0xAA).to_mono());  // any non-zero -> 1
}

TEST_GROUP(Color_to_native)
{
};

TEST(Color_to_native, dispatches_by_bpp)
{
    auto c = Color::from_rgb(0xFF, 0x00, 0x00);
    CHECK_EQUAL((uint32_t)c.to_mono(),    c.to_native<1>());
    CHECK_EQUAL((uint32_t)c.to_gray4(),   c.to_native<4>());
    CHECK_EQUAL((uint32_t)c.to_rgb332(),  c.to_native<8>());
    CHECK_EQUAL((uint32_t)c.to_rgb565(),  c.to_native<16>());
    CHECK_EQUAL((uint32_t)c.to_rgb888(),  c.to_native<24>());
}

TEST_GROUP(Color_constexpr)
{
};

TEST(Color_constexpr, evaluates_at_compile_time)
{
    // If these are not constexpr, the static_assert below will not even
    // compile, so this test failing means the feature regressed.
    constexpr auto red = Color::from_rgb(0xFF, 0, 0);
    constexpr uint16_t r565 = red.to_rgb565();
    constexpr uint8_t r332 = red.to_rgb332();
    static_assert(r565 == 0xF800, "rgb565 of pure red must be constexpr 0xF800");
    static_assert(r332 == 0xE0,   "rgb332 of pure red must be constexpr 0xE0");
    CHECK_EQUAL((uint16_t)0xF800, r565);
    CHECK_EQUAL((uint8_t)0xE0, r332);
}
