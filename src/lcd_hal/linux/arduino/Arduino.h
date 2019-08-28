#ifndef _ARDUINO_H_
#define _ARDUINO_H_

#define ARDUINO_FAKE
#if defined(__MINGW32__)
#include "../../mingw/io.h"
#include "../../Print_internal.h"
#else
#include "../../linux/io.h"
#include "../../Print_internal.h"
#endif

#ifndef boolean
    typedef uint8_t boolean;
#endif

#endif

