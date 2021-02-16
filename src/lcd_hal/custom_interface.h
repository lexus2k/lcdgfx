/*
    MIT License

    Copyright (c) 2017-2020, Alexey Dynda

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
 * @file lcd_hal/custom_interface.h LCD basic custom interface class.
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus

/**
 * This is basic class for custom user-defined interfaces
 * This is template, which accepts maximum number of bytes in interface buffer as template argument
 */
template <int N> class _ICustom
{
public:
    /**
     * Creates instance of custom basic interface
     */
    explicit _ICustom(int8_t dc = -1)
        : m_dc(dc)
    {
        if ( getDc() >= 0 )
        {
            lcd_registerGpioEvent(getDc(), onDcChange, this);
        }
    }

    /**
     * Destroys instance of custom basic interface
     */
    virtual ~_ICustom()
    {
        if ( getDc() >= 0 )
        {
            lcd_unregisterGpioEvent(getDc());
        }
    }

    /**
     * Initializes basic class for custom interface
     */
    virtual void begin()
    {
    }

    /**
     * Deinitializes basic class for custom interface
     */
    virtual void end()
    {
    }

    /**
     * Starts communication with the display over custom interface.
     */
    virtual void start()
    {
    }

    /**
     * Ends communication with the display over custom interface..
     */
    virtual void stop()
    {
    }

    /**
     * Sends byte to custom interface
     * @param data - byte to send
     */
    void send(uint8_t data)
    {
        m_buffer[m_data_size] = data;
        m_data_size++;
        if ( m_data_size == sizeof(m_buffer) )
        {
            forceTransfer();
        }
    }

    /**
     * @brief Sends bytes to custom interface
     *
     * Sends bytes to custom interface.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size)
    {
        while ( size )
        {
            send(*buffer);
            size--;
            buffer++;
        }
    }

protected:
    /**
     * This function must implement actual sending of data to hardware interface
     */
    virtual void transferToHw(const uint8_t *buffer, uint16_t size) = 0;

    /**
     * Call this method to transfer data to hardware. This method is called
     * automatically, when internal interface buffer is overflown.
     */
    void forceTransfer()
    {
        transferToHw(m_buffer, m_data_size);
        m_data_size = 0;
    }

    /**
     * Returns number of D/C pin if used, or -1
     */
    int8_t getDc()
    {
        return m_dc;
    }

private:
    uint8_t m_buffer[N];
    uint16_t m_data_size = 0;
    int8_t m_dc = -1;

    static void onDcChange(void *arg)
    {
        _ICustom *obj = reinterpret_cast<_ICustom *>(arg);
        obj->forceTransfer();
    }
};

#endif
