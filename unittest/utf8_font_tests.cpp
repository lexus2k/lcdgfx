/*
    MIT License

    Copyright (c) 2025, Alexey Dynda

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
#include <string.h>
#include "canvas/font_utf8.h"
#include "canvas/font.h"
#include "canvas/fonts/fonts.h"

TEST_GROUP(Utf8DecoderTests)
{
};

TEST(Utf8DecoderTests, AsciiPassesThrough)
{
    const char *p = "ABC";
    CHECK_EQUAL((uint32_t)'A', nano_utf8_decode(&p, NULL));
    CHECK_EQUAL((uint32_t)'B', nano_utf8_decode(&p, NULL));
    CHECK_EQUAL((uint32_t)'C', nano_utf8_decode(&p, NULL));
    CHECK_EQUAL(0, *p);
}

TEST(Utf8DecoderTests, TwoByteSequenceLatin1)
{
    // U+00E4 (ä) = 0xC3 0xA4
    const char buf[] = "\xC3\xA4";
    const char *p = buf;
    CHECK_EQUAL(0x00E4u, nano_utf8_decode(&p, NULL));
    CHECK_EQUAL(buf + 2, p);
}

TEST(Utf8DecoderTests, TwoByteSequenceCyrillic)
{
    // U+0410 (А) = 0xD0 0x90
    const char buf[] = "\xD0\x90";
    const char *p = buf;
    CHECK_EQUAL(0x0410u, nano_utf8_decode(&p, NULL));
    CHECK_EQUAL(buf + 2, p);
}

TEST(Utf8DecoderTests, ThreeByteSequenceCJK)
{
    // U+4E2D (中) = 0xE4 0xB8 0xAD
    const char buf[] = "\xE4\xB8\xAD";
    const char *p = buf;
    CHECK_EQUAL(0x4E2Du, nano_utf8_decode(&p, NULL));
    CHECK_EQUAL(buf + 3, p);
}

TEST(Utf8DecoderTests, FourByteSequenceEmoji)
{
    // U+1F600 (😀) = 0xF0 0x9F 0x98 0x80
    const char buf[] = "\xF0\x9F\x98\x80";
    const char *p = buf;
    CHECK_EQUAL(0x1F600u, nano_utf8_decode(&p, NULL));
    CHECK_EQUAL(buf + 4, p);
}

TEST(Utf8DecoderTests, MixedAsciiAndCyrillic)
{
    // "Hi Привет" = 48 69 20 D0 9F D1 80 D0 B8 D0 B2 D0 B5 D1 82
    const char buf[] = "Hi \xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82";
    const char *p = buf;
    CHECK_EQUAL((uint32_t)'H', nano_utf8_decode(&p, NULL));
    CHECK_EQUAL((uint32_t)'i', nano_utf8_decode(&p, NULL));
    CHECK_EQUAL((uint32_t)' ', nano_utf8_decode(&p, NULL));
    CHECK_EQUAL(0x041Fu, nano_utf8_decode(&p, NULL)); // П
    CHECK_EQUAL(0x0440u, nano_utf8_decode(&p, NULL)); // р
    CHECK_EQUAL(0x0438u, nano_utf8_decode(&p, NULL)); // и
    CHECK_EQUAL(0x0432u, nano_utf8_decode(&p, NULL)); // в
    CHECK_EQUAL(0x0435u, nano_utf8_decode(&p, NULL)); // е
    CHECK_EQUAL(0x0442u, nano_utf8_decode(&p, NULL)); // т
    CHECK_EQUAL(0, *p);
}

TEST(Utf8DecoderTests, RejectsLoneContinuationByte)
{
    const char buf[] = "\x80z";
    const char *p = buf;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p, NULL));
    // Decoder must resync to the next byte so the caller doesn't loop.
    CHECK_EQUAL(buf + 1, p);
    CHECK_EQUAL((uint32_t)'z', nano_utf8_decode(&p, NULL));
}

TEST(Utf8DecoderTests, RejectsTruncatedTwoByteAtNul)
{
    const char buf[2] = {(char)0xC3, 0};
    const char *p = buf;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p, NULL));
}

TEST(Utf8DecoderTests, RejectsTruncatedTwoByteAtEnd)
{
    const char buf[1] = {(char)0xC3};
    const char *p = buf;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p, buf + 1));
}

TEST(Utf8DecoderTests, RejectsOverlongCAndC1)
{
    const char buf1[] = "\xC0\x80";
    const char *p1 = buf1;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p1, NULL));
    const char buf2[] = "\xC1\xBF";
    const char *p2 = buf2;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p2, NULL));
}

TEST(Utf8DecoderTests, RejectsSurrogateCodepoint)
{
    // U+D800 would encode as ED A0 80 — must be rejected.
    const char buf[] = "\xED\xA0\x80";
    const char *p = buf;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p, NULL));
}

TEST(Utf8DecoderTests, RejectsCodepointAboveMax)
{
    // 0xF5 leads only to codepoints > U+10FFFF.
    const char buf[] = "\xF5\x80\x80\x80";
    const char *p = buf;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p, NULL));
}

TEST(Utf8DecoderTests, RejectsBareInvalidLeadByte)
{
    const char buf[] = "\xFF";
    const char *p = buf;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p, NULL));
}

TEST(Utf8DecoderTests, RejectsContinuationByteMissingHighBit)
{
    // E4 in a 3-byte sequence should be followed by 10xxxxxx; here we feed it 'A'.
    const char buf[] = "\xE4""AA";
    const char *p = buf;
    CHECK_EQUAL(NANO_UTF8_INVALID, nano_utf8_decode(&p, NULL));
    // After resync we should still be able to read the 'A'.
    CHECK_EQUAL((uint32_t)'A', nano_utf8_decode(&p, NULL));
}

// ---------------------------------------------------------------------------
// Range-based glyph lookup: secondary Cyrillic font shipped with this PR.
// ---------------------------------------------------------------------------

TEST_GROUP(CyrillicFontTests)
{
};

TEST(CyrillicFontTests, AsciiStillRendersFromPrimaryFont)
{
    NanoFont font;
    font.loadFixedFont(ssd1306xled_font6x8);
    font.loadSecondaryFont(ssd1306xled_font6x8_Cyrillic);
    SCharInfo info;
    info.glyph = NULL;
    font.getCharBitmap('A', &info);
    CHECK(info.glyph != NULL);
    CHECK_EQUAL(6, info.width);
    CHECK_EQUAL(8, info.height);
}

TEST(CyrillicFontTests, CyrillicCodepointResolvesViaSecondaryTable)
{
    NanoFont font;
    font.loadFixedFont(ssd1306xled_font6x8);
    font.loadSecondaryFont(ssd1306xled_font6x8_Cyrillic);
    SCharInfo info;
    info.glyph = NULL;
    font.getCharBitmap(0x0410, &info); // А
    CHECK(info.glyph != NULL);
    // The А glyph in the Cyrillic table is byte-identical to Latin A.
    SCharInfo latin;
    font.getCharBitmap('A', &latin);
    CHECK_EQUAL(0, memcmp(info.glyph, latin.glyph, 6));
}

TEST(CyrillicFontTests, AllCyrillicUppercaseLettersHaveGlyphs)
{
    NanoFont font;
    font.loadFixedFont(ssd1306xled_font6x8);
    font.loadSecondaryFont(ssd1306xled_font6x8_Cyrillic);
    for ( uint16_t cp = 0x0410; cp <= 0x042F; cp++ )
    {
        SCharInfo info;
        info.glyph = NULL;
        font.getCharBitmap(cp, &info);
        CHECK_TEXT(info.glyph != NULL, "missing Cyrillic glyph");
    }
}

TEST(CyrillicFontTests, MissingCyrillicCodepointFallsBackGracefully)
{
    NanoFont font;
    font.loadFixedFont(ssd1306xled_font6x8);
    font.loadSecondaryFont(ssd1306xled_font6x8_Cyrillic);
    SCharInfo info;
    info.glyph = NULL;
    // U+0500 is outside the shipped block; library falls back to ascii_offset
    // glyph rather than crashing.
    font.getCharBitmap(0x0500, &info);
    CHECK(info.glyph != NULL);
}

TEST(CyrillicFontTests, GetTextSizeWalksUtf8MultiByteSequence)
{
    NanoFont font;
    font.loadFixedFont(ssd1306xled_font6x8);
    font.loadSecondaryFont(ssd1306xled_font6x8_Cyrillic);
    // "АБ" is two glyphs of width 6, encoded in 4 bytes UTF-8.
    lcduint_t height = 0;
    lcduint_t width = font.getTextSize("\xD0\x90\xD0\x91", &height);
    CHECK_EQUAL(12, width);
    CHECK_EQUAL(8, height);
}
