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
*/

#include <CppUTest/TestHarness.h>
#include "touch/xpt2046.h"
#include <vector>
#include <cstdint>

namespace
{

/**
 * Mock SPI that returns canned bytes for read() and records the bytes sent
 * by the driver. Each transfer() pops a byte from the response queue.
 */
class MockSpi
{
public:
    std::vector<uint8_t> sent;
    std::vector<uint8_t> responses;
    size_t responseIndex = 0;
    int startCount = 0;
    int stopCount = 0;
    int beginCount = 0;
    int endCount = 0;

    void begin() { beginCount++; }
    void end() { endCount++; }
    void start() { startCount++; }
    void stop() { stopCount++; }

    uint8_t transfer(uint8_t b)
    {
        sent.push_back(b);
        if ( responseIndex < responses.size() )
        {
            return responses[responseIndex++];
        }
        return 0;
    }

    /**
     * Queues two response bytes representing a 12-bit ADC reading.
     * The XPT2046 returns the result MSB first across two bytes:
     *   hi = bits 11..5 of value, in bits 6..0 of hi (bit 7 is the busy bit, 0)
     *   lo = bits 4..0 of value in bits 7..3 of lo (bits 2..0 are 0)
     * Composed by the driver as ((hi << 5) | (lo >> 3)) & 0x0FFF.
     */
    void queue12bit(uint16_t value)
    {
        value &= 0x0FFF;
        uint8_t hi = (uint8_t)((value >> 5) & 0x7F);
        uint8_t lo = (uint8_t)((value & 0x1F) << 3);
        responses.push_back(hi);
        responses.push_back(lo);
    }

    void queueChannel(uint16_t value)
    {
        // The driver clocks out one command byte (response ignored), then
        // two data bytes carrying the ADC result.
        responses.push_back(0x00);
        queue12bit(value);
    }
};

} // namespace

TEST_GROUP(TouchCalibrationTests)
{
};

TEST(TouchCalibrationTests, IdentityMappingForCenter)
{
    TouchCalibration cal;
    cal.xMin = 0; cal.xMax = 4095;
    cal.yMin = 0; cal.yMax = 4095;
    cal.screenWidth = 320; cal.screenHeight = 240;
    int16_t x, y;
    cal.apply(2048, 2048, x, y);
    CHECK(x > 150 && x < 170);
    CHECK(y > 110 && y < 130);
}

TEST(TouchCalibrationTests, ClampsBelowMin)
{
    TouchCalibration cal;
    cal.xMin = 200; cal.xMax = 3900;
    cal.yMin = 200; cal.yMax = 3900;
    cal.screenWidth = 320; cal.screenHeight = 240;
    int16_t x, y;
    cal.apply(0, 0, x, y);
    CHECK_EQUAL(0, x);
    CHECK_EQUAL(0, y);
}

TEST(TouchCalibrationTests, ClampsAboveMax)
{
    TouchCalibration cal;
    cal.xMin = 200; cal.xMax = 3900;
    cal.yMin = 200; cal.yMax = 3900;
    cal.screenWidth = 320; cal.screenHeight = 240;
    int16_t x, y;
    cal.apply(4095, 4095, x, y);
    CHECK_EQUAL(319, x);
    CHECK_EQUAL(239, y);
}

TEST(TouchCalibrationTests, InvertX)
{
    TouchCalibration cal;
    cal.xMin = 0; cal.xMax = 4095;
    cal.yMin = 0; cal.yMax = 4095;
    cal.screenWidth = 320; cal.screenHeight = 240;
    cal.invertX = true;
    int16_t x, y;
    cal.apply(0, 0, x, y);
    CHECK_EQUAL(319, x);
    CHECK_EQUAL(0, y);
}

TEST(TouchCalibrationTests, InvertY)
{
    TouchCalibration cal;
    cal.xMin = 0; cal.xMax = 4095;
    cal.yMin = 0; cal.yMax = 4095;
    cal.screenWidth = 320; cal.screenHeight = 240;
    cal.invertY = true;
    int16_t x, y;
    cal.apply(0, 0, x, y);
    CHECK_EQUAL(0, x);
    CHECK_EQUAL(239, y);
}

TEST(TouchCalibrationTests, SwapXY)
{
    TouchCalibration cal;
    cal.xMin = 0; cal.xMax = 4095;
    cal.yMin = 0; cal.yMax = 4095;
    cal.screenWidth = 320; cal.screenHeight = 240;
    cal.swapXY = true;
    int16_t x, y;
    cal.apply(4095, 0, x, y);
    // After swap, rawX=0, rawY=4095 -> x=0, y=239
    CHECK_EQUAL(0, x);
    CHECK_EQUAL(239, y);
}

TEST(TouchCalibrationTests, ZeroSpanDoesNotDivideByZero)
{
    TouchCalibration cal;
    cal.xMin = 100; cal.xMax = 100; // degenerate
    cal.yMin = 0; cal.yMax = 4095;
    cal.screenWidth = 320; cal.screenHeight = 240;
    int16_t x, y;
    cal.apply(50, 2048, x, y);
    CHECK_EQUAL(0, x);
}

TEST_GROUP(Xpt2046Tests)
{
};

TEST(Xpt2046Tests, ReadRawDecodesValuesCorrectly)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    // readRaw clocks Z1, Z2, then 3*X, 3*Y. Each sample = 1 cmd + 2 data bytes.
    spi.queueChannel(1000); // Z1 (above threshold 200)
    spi.queueChannel(500);  // Z2
    spi.queueChannel(2000); // X1
    spi.queueChannel(1500); // Y1
    spi.queueChannel(2050); // X2
    spi.queueChannel(1550); // Y2
    spi.queueChannel(2100); // X3
    spi.queueChannel(1600); // Y3

    int16_t rawX, rawY, rawZ;
    bool pressed = touch.readRaw(rawX, rawY, rawZ);
    CHECK_TRUE(pressed);
    CHECK_EQUAL(2050, rawX); // median of 2000/2050/2100
    CHECK_EQUAL(1550, rawY); // median of 1500/1550/1600
    CHECK_EQUAL(1000 + (4095 - 500), rawZ);
}

TEST(Xpt2046Tests, ReadRawReturnsFalseWhenZBelowThreshold)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    spi.queueChannel(50);  // Z1 - below default threshold of 200
    spi.queueChannel(2000);
    spi.queueChannel(2000);
    spi.queueChannel(1500);
    spi.queueChannel(2000);
    spi.queueChannel(1500);
    spi.queueChannel(2000);
    spi.queueChannel(1500);

    int16_t rawX, rawY, rawZ;
    bool pressed = touch.readRaw(rawX, rawY, rawZ);
    CHECK_FALSE(pressed);
}

TEST(Xpt2046Tests, ReadRawSendsCorrectCommandBytes)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    for ( int i = 0; i < 8; i++ ) spi.queueChannel(1000);

    int16_t rawX, rawY, rawZ;
    touch.readRaw(rawX, rawY, rawZ);

    // 8 samples * 3 bytes each = 24 bytes
    CHECK_EQUAL(24u, spi.sent.size());
    // Sample order: Z1, Z2, X, Y, X, Y, X, Y
    CHECK_EQUAL(lcdgfx_xpt2046::CMD_Z1, spi.sent[0]);
    CHECK_EQUAL(lcdgfx_xpt2046::CMD_Z2, spi.sent[3]);
    CHECK_EQUAL(lcdgfx_xpt2046::CMD_X, spi.sent[6]);
    CHECK_EQUAL(lcdgfx_xpt2046::CMD_Y, spi.sent[9]);
    CHECK_EQUAL(lcdgfx_xpt2046::CMD_X, spi.sent[12]);
}

TEST(Xpt2046Tests, ReadRawAssertsAndReleasesCs)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    for ( int i = 0; i < 8; i++ ) spi.queueChannel(1000);

    int16_t rawX, rawY, rawZ;
    touch.readRaw(rawX, rawY, rawZ);

    CHECK_EQUAL(1, spi.startCount);
    CHECK_EQUAL(1, spi.stopCount);
}

TEST(Xpt2046Tests, ReadAppliesCalibration)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    TouchCalibration cal;
    cal.xMin = 0; cal.xMax = 4095;
    cal.yMin = 0; cal.yMax = 4095;
    cal.screenWidth = 320; cal.screenHeight = 240;
    touch.setCalibration(cal);

    spi.queueChannel(1000);  // Z1 pressed
    spi.queueChannel(500);   // Z2
    for ( int i = 0; i < 3; i++ )
    {
        spi.queueChannel(2048); // X
        spi.queueChannel(2048); // Y
    }

    int16_t x, y;
    bool pressed = touch.read(x, y);
    CHECK_TRUE(pressed);
    CHECK(x > 150 && x < 170);
    CHECK(y > 110 && y < 130);
}

TEST(Xpt2046Tests, ReadReturnsFalseWhenNotPressed)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    spi.queueChannel(10); // Z1 - far below threshold
    for ( int i = 0; i < 7; i++ ) spi.queueChannel(2000);

    int16_t x, y;
    bool pressed = touch.read(x, y);
    CHECK_FALSE(pressed);
}

TEST(Xpt2046Tests, PressureThresholdIsConfigurable)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    touch.setPressureThreshold(2000);

    spi.queueChannel(1500); // Z1 - above default 200, below new 2000
    for ( int i = 0; i < 7; i++ ) spi.queueChannel(2000);

    int16_t rawX, rawY, rawZ;
    bool pressed = touch.readRaw(rawX, rawY, rawZ);
    CHECK_FALSE(pressed);
}

TEST(Xpt2046Tests, BeginAndEndForwardToSpi)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    touch.begin();
    touch.end();
    CHECK_EQUAL(1, spi.beginCount);
    CHECK_EQUAL(1, spi.endCount);
}

TEST(Xpt2046Tests, ReadWithPressureReturnsZ)
{
    MockSpi spi;
    LcdGfxXpt2046<MockSpi> touch(spi);
    spi.queueChannel(1000); // Z1
    spi.queueChannel(500);  // Z2
    for ( int i = 0; i < 3; i++ )
    {
        spi.queueChannel(2048);
        spi.queueChannel(2048);
    }

    int16_t x, y, pressure;
    bool pressed = touch.read(x, y, pressure);
    CHECK_TRUE(pressed);
    CHECK_EQUAL(1000 + (4095 - 500), pressure);
}
