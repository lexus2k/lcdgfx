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
/**
 * @file touch/xpt2046.h XPT2046 / ADS7843-compatible resistive touch driver
 */

#ifndef _LCDGFX_TOUCH_XPT2046_H_
#define _LCDGFX_TOUCH_XPT2046_H_

#include "touch.h"

/**
 * @ingroup LCD_GENERIC_API
 * @{
 */

/**
 * XPT2046 control byte definitions.
 *
 * Bit layout (MSB first): S A2 A1 A0 MODE SER/DFR PD1 PD0
 *  - S = 1 (start bit)
 *  - A2..A0 = channel (X=5, Y=1, Z1=3, Z2=4)
 *  - MODE = 0 (12-bit)
 *  - SER/DFR = 0 (differential)
 *  - PD1..PD0 = 1 1 (PENIRQ disabled, internal ref off, ADC on between samples)
 */
namespace lcdgfx_xpt2046
{
constexpr uint8_t CMD_X  = 0xD3; ///< 1 101 0 0 11 - read X (differential, 12-bit)
constexpr uint8_t CMD_Y  = 0x93; ///< 1 001 0 0 11 - read Y
constexpr uint8_t CMD_Z1 = 0xB3; ///< 1 011 0 0 11 - read Z1
constexpr uint8_t CMD_Z2 = 0xC3; ///< 1 100 0 0 11 - read Z2
} // namespace lcdgfx_xpt2046

/**
 * Driver for the XPT2046 resistive touch controller (also compatible with
 * ADS7843 / TSC2046 silicon).
 *
 * The driver is parameterized on a SPI transport type so it can be unit-tested
 * with a mock and used on any platform without coupling to lcdgfx's display
 * SPI configuration.
 *
 * @tparam SpiT SPI transport class. It must provide:
 *   - void begin();
 *   - void start();             // assert CS
 *   - void stop();              // de-assert CS
 *   - uint8_t transfer(uint8_t outByte); // full-duplex transfer of one byte
 *   - void end();
 */
template <typename SpiT> class LcdGfxXpt2046
{
public:
    /**
     * Constructs the driver around an SPI transport instance owned by the caller.
     */
    explicit LcdGfxXpt2046(SpiT &spi)
        : m_spi(spi)
    {
    }

    /**
     * Initialises the SPI transport. The user is expected to have configured
     * any platform-specific pins (CS, IRQ, MISO/MOSI/CLK) before calling.
     */
    void begin()
    {
        m_spi.begin();
    }

    /**
     * Releases the SPI transport.
     */
    void end()
    {
        m_spi.end();
    }

    /**
     * Sets the calibration used by read(). Defaults are reasonable for a
     * typical 320x240 panel but most boards will need per-unit calibration.
     */
    void setCalibration(const TouchCalibration &cal)
    {
        m_calibration = cal;
    }

    /**
     * Returns the current calibration (mutable reference).
     */
    TouchCalibration &calibration()
    {
        return m_calibration;
    }

    /**
     * Sets the minimum pressure (Z1 reading) considered an active touch.
     * Default is 200; lower the threshold if your panel is less sensitive.
     */
    void setPressureThreshold(uint16_t threshold)
    {
        m_pressureThreshold = threshold;
    }

    /**
     * Reads raw 12-bit ADC samples from the controller without applying any
     * calibration. Returns true if pressure is above the threshold.
     *
     * Three samples per axis are averaged with the smallest and largest
     * discarded to suppress occasional spikes.
     */
    bool readRaw(int16_t &rawX, int16_t &rawY, int16_t &rawZ)
    {
        m_spi.start();
        uint16_t z1 = sampleChannel(lcdgfx_xpt2046::CMD_Z1);
        uint16_t z2 = sampleChannel(lcdgfx_xpt2046::CMD_Z2);
        rawZ = (int16_t)(z1 + (4095 - z2));

        uint16_t xs[3];
        uint16_t ys[3];
        for ( int i = 0; i < 3; i++ )
        {
            xs[i] = sampleChannel(lcdgfx_xpt2046::CMD_X);
            ys[i] = sampleChannel(lcdgfx_xpt2046::CMD_Y);
        }
        m_spi.stop();

        rawX = (int16_t)median3(xs[0], xs[1], xs[2]);
        rawY = (int16_t)median3(ys[0], ys[1], ys[2]);
        return z1 > m_pressureThreshold;
    }

    /**
     * Reads calibrated screen-pixel coordinates. Returns true if a touch is
     * detected (pressure above the threshold).
     */
    bool read(int16_t &x, int16_t &y)
    {
        int16_t rawX, rawY, rawZ;
        bool pressed = readRaw(rawX, rawY, rawZ);
        if ( !pressed )
        {
            return false;
        }
        m_calibration.apply(rawX, rawY, x, y);
        return true;
    }

    /**
     * Reads calibrated coordinates plus raw pressure. Pressure is a
     * dimensionless metric (higher = harder press) computed as Z1 + (4095-Z2).
     */
    bool read(int16_t &x, int16_t &y, int16_t &pressure)
    {
        int16_t rawX, rawY;
        bool pressed = readRaw(rawX, rawY, pressure);
        if ( !pressed )
        {
            return false;
        }
        m_calibration.apply(rawX, rawY, x, y);
        return true;
    }

private:
    SpiT &m_spi;
    TouchCalibration m_calibration{};
    uint16_t m_pressureThreshold = 200;

    uint16_t sampleChannel(uint8_t cmd)
    {
        m_spi.transfer(cmd);
        uint8_t hi = m_spi.transfer(0x00);
        uint8_t lo = m_spi.transfer(0x00);
        // 16 bits clocked out: result is 12-bit, MSB first, with 3 leading
        // status/zero bits and 1 trailing zero bit. Compose the 12-bit value:
        return (uint16_t)(((uint16_t)hi << 5) | (lo >> 3)) & 0x0FFF;
    }

    static uint16_t median3(uint16_t a, uint16_t b, uint16_t c)
    {
        if ( a > b )
        {
            uint16_t t = a;
            a = b;
            b = t;
        }
        if ( b > c )
        {
            uint16_t t = b;
            b = c;
            c = t;
        }
        if ( a > b )
        {
            uint16_t t = a;
            a = b;
            b = t;
        }
        return b;
    }
};

/**
 * @}
 */

#endif
