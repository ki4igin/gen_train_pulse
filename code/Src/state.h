#ifndef __STATE_H
#define __STATE_H

#include "stdint.h"
#include "joy.h"

enum state {
    STATE_NONE = 0,

    STATE_PULSE,
    STATE_PULSE_WAIT,
    STATE_PULSE_RUN_SINGLE,

    STATE_CONT,
    STATE_CONT_RUN,

    STATE_SETTINGS,
    STATE_SETTINGS_SAVE,

    STATE_SETTINGS_DELAY,
    STATE_SETTINGS_DELAY_SET,
    STATE_SETTINGS_POLARITY,
    STATE_SETTINGS_POLARITY_SET,
    STATE_SETTINGS_PULSE,
    STATE_SETTINGS_PULSE_WIDTH,
    STATE_SETTINGS_PULSE_WIDTH_SET,
    STATE_SETTINGS_TRAIN,
    STATE_SETTINGS_TRAIN_PERIOD,
    STATE_SETTINGS_TRAIN_PERIOD_SET,
    STATE_SETTINGS_CONT,
    STATE_SETTINGS_CONT_PERIOD,
    STATE_SETTINGS_CONT_PERIOD_SET,

    STATE_MAX,
};

extern uint32_t progTimerCnt;

enum state state_next(enum state cur, enum joy joy);
enum state state_action(enum state state, enum joy joy);
void ExtInt_Callback(void);


#endif
