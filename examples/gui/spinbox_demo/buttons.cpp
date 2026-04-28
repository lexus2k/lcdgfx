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

#include "buttons.h"
#include "lcdgfx.h"

Key getPressedButton(uint8_t analogPin)
{
  int buttonValue = lcd_adcRead(analogPin);
  if (buttonValue < 100) {
    return Key::BT_RIGHT;
  }
  else if (buttonValue < 200) {
    return Key::BT_UP;
  }
  else if (buttonValue < 400){
    return Key::BT_DOWN;
  }
  else if (buttonValue < 600){
    return Key::BT_LEFT;
  }
  else if (buttonValue < 800){
    return Key::BT_SELECT;
  }
  return Key::BT_NONE;
}
