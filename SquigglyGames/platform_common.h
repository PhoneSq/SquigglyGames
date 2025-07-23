#ifndef PLATFORM_COMMON_H
#define PLATFORM_COMMON_H

#include "utils.h"

// Button state structure
struct Button_State {
    bool is_down;
    bool changed;
};

// Button enumeration
enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_W,
    BUTTON_S,
    BUTTON_A,
    BUTTON_D,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_ENTER,

    BUTTON_COUNT, // Should be the last item
};

// Input structure
struct Input {
    Button_State buttons[BUTTON_COUNT];
};

#endif