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

#pragma once

#include <stdint.h>

enum class Key: uint8_t
{
    BT_NONE   = 0,
    BT_RIGHT  = 1,
    BT_UP     = 2,
    BT_DOWN   = 3,
    BT_LEFT   = 4,
    BT_SELECT = 5,
};

Key getPressedButton(uint8_t analogPin);
