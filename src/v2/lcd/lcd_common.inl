/*
    MIT License

    Copyright (c) 2019, 2021 Alexey Dynda

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

#define CMD_DELAY 0xFF

template <class I> void _configureSpiDisplay(I &intf, const uint8_t *config, uint8_t configSize)
{
    uint8_t command = 1;
    int8_t args = -1;
    intf.commandStart();
    for ( uint8_t i = 0; i < configSize; i++ )
    {
        uint8_t data = pgm_read_byte(&config[i]);
        if ( command )
        {
            if ( command == CMD_DELAY )
            {
                command = 1;
                intf.stop(); // Force communication layer to send the data to LCD display - stop prevents buffering
                lcd_delay(data == 0xFF ? 500 : data);
                intf.commandStart();
            }
            else
            {
                intf.send(data);
                command = 0;
                args = -1;
            }
        }
        else
        {
            if ( args < 0 )
            {
                if ( data >= 128 )
                {
                    command = data;
                }
                else if ( data > 0 )
                {
                    args = data;
                    intf.setDataMode(1);
                }
                else
                {
                    command = 1;
                }
            }
            else
            {
                args--;
                intf.send(data);
                if ( !args )
                {
                    command = 1;
                    intf.setDataMode(0);
                }
            }
        }
    }
    intf.stop();
}

template <class I> void _configureSpiDisplayCmdModeOnly(I &intf, const uint8_t *config, uint8_t configSize)
{
    uint8_t command = 1;
    int8_t args = -1;
    intf.commandStart();
    for ( uint8_t i = 0; i < configSize; i++ )
    {
        uint8_t data = pgm_read_byte(&config[i]);
        if ( command )
        {
            if ( command == CMD_DELAY )
            {
                command = 1;
                lcd_delay(data == 0xFF ? 500 : data);
            }
            else
            {
                intf.send(data);
                command = 0;
                args = -1;
            }
        }
        else
        {
            if ( args < 0 )
            {
                if ( data >= 128 )
                {
                    command = data;
                }
                else if ( data > 0 )
                {
                    args = data;
                }
                else
                {
                    command = 1;
                }
            }
            else
            {
                args--;
                intf.send(data);
                if ( !args )
                {
                    command = 1;
                }
            }
        }
    }
    intf.stop();
}
