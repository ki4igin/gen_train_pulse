#ifndef __JOY_H
#define __JOY_H

#include <stdint.h>
#include "main.h"

enum joy {
    JOY_NONE = 0,
    JOY_UP,
    JOY_DOWN,
    JOY_SEL,
    JOY_LEFT,
    JOY_RIGHT,
    JOY_MAX
};

inline static enum joy joy_get(uint32_t pin)
{
    enum joy joy = JOY_NONE;

    if ((pin & JOY_CENTER_Pin) == JOY_CENTER_Pin) {
        joy = JOY_SEL;
    } else if ((pin & JOY_DOWN_Pin) == JOY_DOWN_Pin) {
        joy = JOY_DOWN;
    } else if ((pin & JOY_UP_Pin) == JOY_UP_Pin) {
        joy = JOY_UP;
    } else if ((pin & JOY_LEFT_Pin) == JOY_LEFT_Pin) {
        joy = JOY_LEFT;
    } else if ((pin & JOY_RIGHT_Pin) == JOY_RIGHT_Pin) {
        joy = JOY_RIGHT;
    }

    return joy;
}

#endif
