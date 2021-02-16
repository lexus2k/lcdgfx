/*
    MIT License

    Copyright (c) 2020, Alexey Dynda

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

#include "yesno.h"

LcdGfxYesNo::LcdGfxYesNo(const char *text, const NanoRect &rect)
    : m_text(text)
    , m_rect(rect)
    , m_yes("yes", {})
    , m_no("no", {})
{
    swapToYes();
}

void LcdGfxYesNo::swapToYes()
{
    m_no.setFocus(false);
    m_yes.setFocus(true);
}

void LcdGfxYesNo::swapToNo()
{
    m_no.setFocus(true);
    m_yes.setFocus(false);
}

bool LcdGfxYesNo::isYes()
{
    return m_yes.isActive();
}
