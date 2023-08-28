#include "state.h"
#include "joy.h"
#include "settings.h"
#include "tim.h"
#include "stm32l476g_discovery_glass_lcd.h"
#include "conventer.h"
#include "gpio.h"

#define DELAY_STEP_US       1000
#define DELAY_MAX_US        70000
#define DELAY_MIX_US        8000
#define PULSE_WIDTH_STEP_NS 50
#define PULSE_WIDTH_MAX_NS  500
#define PULSE_WIDTH_MIX_NS  200
// TRAIN PULSES - пачка импульсов
#define TRAIN_PERIOD_STEP_US  1
#define TRAIN_PERIOD_MAX_US   410
#define TRAIN_PERIOD_MIX_US   400

#define CONTIN_PERIOD_STEP_US 500
#define CONTIN_PERIOD_MAX_US  5000
#define CONTIN_PERIOD_MIX_US  1000

const enum state fsm_table[STATE_MAX][JOY_MAX] = {
    [STATE_PULSE][JOY_UP] = STATE_SETTINGS,
    [STATE_PULSE][JOY_DOWN] = STATE_CONT,
    [STATE_PULSE][JOY_SEL] = STATE_PULSE_WAIT,
    [STATE_PULSE][JOY_RIGHT] = STATE_PULSE_WAIT,

    [STATE_PULSE_WAIT][JOY_SEL] = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_WAIT][JOY_LEFT] = STATE_PULSE,
    [STATE_PULSE_WAIT][JOY_RIGHT] = STATE_PULSE_RUN_SINGLE,

    [STATE_PULSE_RUN_SINGLE][JOY_LEFT] = STATE_PULSE_WAIT,

    [STATE_CONT][JOY_UP] = STATE_PULSE,
    [STATE_CONT][JOY_DOWN] = STATE_SETTINGS,
    [STATE_CONT][JOY_SEL] = STATE_CONT_RUN,
    [STATE_CONT][JOY_RIGHT] = STATE_CONT_RUN,

    [STATE_CONT_RUN][JOY_LEFT] = STATE_CONT,

    [STATE_SETTINGS][JOY_UP] = STATE_CONT,
    [STATE_SETTINGS][JOY_DOWN] = STATE_PULSE,
    [STATE_SETTINGS][JOY_SEL] = STATE_SETTINGS_DELAY,
    [STATE_SETTINGS][JOY_RIGHT] = STATE_SETTINGS_DELAY,

    [STATE_SETTINGS_DELAY][JOY_UP] = STATE_SETTINGS_CONT,
    [STATE_SETTINGS_DELAY][JOY_DOWN] = STATE_SETTINGS_POLARITY,
    [STATE_SETTINGS_DELAY][JOY_SEL] = STATE_SETTINGS_DELAY_SET,
    [STATE_SETTINGS_DELAY][JOY_LEFT] = STATE_SETTINGS_SAVE,
    [STATE_SETTINGS_DELAY][JOY_RIGHT] = STATE_SETTINGS_DELAY_SET,

    [STATE_SETTINGS_DELAY_SET][JOY_SEL] = STATE_SETTINGS_DELAY,
    [STATE_SETTINGS_DELAY_SET][JOY_LEFT] = STATE_SETTINGS_DELAY,
    [STATE_SETTINGS_DELAY_SET][JOY_RIGHT] = STATE_SETTINGS_DELAY,

    [STATE_SETTINGS_POLARITY][JOY_UP] = STATE_SETTINGS_DELAY,
    [STATE_SETTINGS_POLARITY][JOY_DOWN] = STATE_SETTINGS_PULSE,
    [STATE_SETTINGS_POLARITY][JOY_SEL] = STATE_SETTINGS_POLARITY_SET,
    [STATE_SETTINGS_POLARITY][JOY_LEFT] = STATE_SETTINGS_SAVE,
    [STATE_SETTINGS_POLARITY][JOY_RIGHT] = STATE_SETTINGS_POLARITY_SET,

    [STATE_SETTINGS_POLARITY_SET][JOY_SEL] = STATE_SETTINGS_POLARITY,
    [STATE_SETTINGS_POLARITY_SET][JOY_LEFT] = STATE_SETTINGS_POLARITY,
    [STATE_SETTINGS_POLARITY_SET][JOY_RIGHT] = STATE_SETTINGS_POLARITY,

    [STATE_SETTINGS_PULSE][JOY_UP] = STATE_SETTINGS_POLARITY,
    [STATE_SETTINGS_PULSE][JOY_DOWN] = STATE_SETTINGS_TRAIN,
    [STATE_SETTINGS_PULSE][JOY_SEL] = STATE_SETTINGS_PULSE_WIDTH,
    [STATE_SETTINGS_PULSE][JOY_LEFT] = STATE_SETTINGS_SAVE,
    [STATE_SETTINGS_PULSE][JOY_RIGHT] = STATE_SETTINGS_PULSE_WIDTH,

    [STATE_SETTINGS_PULSE_WIDTH][JOY_SEL] = STATE_SETTINGS_PULSE_WIDTH_SET,
    [STATE_SETTINGS_PULSE_WIDTH][JOY_LEFT] = STATE_SETTINGS_PULSE_WIDTH_SET,
    [STATE_SETTINGS_PULSE_WIDTH][JOY_RIGHT] = STATE_SETTINGS_PULSE_WIDTH_SET,

    [STATE_SETTINGS_PULSE_WIDTH_SET][JOY_SEL] = STATE_SETTINGS_PULSE_WIDTH,
    [STATE_SETTINGS_PULSE_WIDTH_SET][JOY_LEFT] = STATE_SETTINGS_PULSE_WIDTH,
    [STATE_SETTINGS_PULSE_WIDTH_SET][JOY_RIGHT] = STATE_SETTINGS_PULSE_WIDTH,

    [STATE_SETTINGS_TRAIN][JOY_UP] = STATE_SETTINGS_PULSE,
    [STATE_SETTINGS_TRAIN][JOY_DOWN] = STATE_SETTINGS_CONT,
    [STATE_SETTINGS_TRAIN][JOY_SEL] = STATE_SETTINGS_TRAIN_PERIOD,
    [STATE_SETTINGS_TRAIN][JOY_LEFT] = STATE_SETTINGS_SAVE,
    [STATE_SETTINGS_TRAIN][JOY_RIGHT] = STATE_SETTINGS_TRAIN_PERIOD,

    [STATE_SETTINGS_TRAIN_PERIOD][JOY_SEL] = STATE_SETTINGS_TRAIN_PERIOD_SET,
    [STATE_SETTINGS_TRAIN_PERIOD][JOY_LEFT] = STATE_SETTINGS_TRAIN,
    [STATE_SETTINGS_TRAIN_PERIOD][JOY_RIGHT] = STATE_SETTINGS_TRAIN_PERIOD_SET,

    [STATE_SETTINGS_TRAIN_PERIOD_SET][JOY_SEL] = STATE_SETTINGS_TRAIN_PERIOD,
    [STATE_SETTINGS_TRAIN_PERIOD_SET][JOY_LEFT] = STATE_SETTINGS_TRAIN_PERIOD,
    [STATE_SETTINGS_TRAIN_PERIOD_SET][JOY_RIGHT] = STATE_SETTINGS_TRAIN_PERIOD,

    [STATE_SETTINGS_CONT][JOY_UP] = STATE_SETTINGS_TRAIN,
    [STATE_SETTINGS_CONT][JOY_DOWN] = STATE_SETTINGS_DELAY,
    [STATE_SETTINGS_CONT][JOY_SEL] = STATE_SETTINGS_CONT_PERIOD,
    [STATE_SETTINGS_CONT][JOY_LEFT] = STATE_SETTINGS_SAVE,
    [STATE_SETTINGS_CONT][JOY_RIGHT] = STATE_SETTINGS_CONT_PERIOD,

    [STATE_SETTINGS_CONT_PERIOD][JOY_SEL] = STATE_SETTINGS_CONT_PERIOD_SET,
    [STATE_SETTINGS_CONT_PERIOD][JOY_LEFT] = STATE_SETTINGS_CONT,
    [STATE_SETTINGS_CONT_PERIOD][JOY_RIGHT] = STATE_SETTINGS_CONT_PERIOD_SET,

    [STATE_SETTINGS_CONT_PERIOD_SET][JOY_SEL] = STATE_SETTINGS_CONT_PERIOD,
    [STATE_SETTINGS_CONT_PERIOD_SET][JOY_LEFT] = STATE_SETTINGS_CONT_PERIOD,
    [STATE_SETTINGS_CONT_PERIOD_SET][JOY_RIGHT] = STATE_SETTINGS_CONT_PERIOD,
};

const uint8_t state_strs[STATE_MAX][6] = {
    [STATE_PULSE] = "PULSE",
    [STATE_PULSE_WAIT] = "WAIT",
    [STATE_PULSE_RUN_SINGLE] = "RUN S",

    [STATE_CONT] = "CONTIN",
    [STATE_CONT_RUN] = "RUN C",

    [STATE_SETTINGS] = "SETTIN",

    [STATE_SETTINGS_DELAY] = "DELAY",
    [STATE_SETTINGS_POLARITY] = "POLAR",
    [STATE_SETTINGS_PULSE] = "PULSE",
    [STATE_SETTINGS_PULSE_WIDTH] = "WIDTH",
    [STATE_SETTINGS_TRAIN] = "TRAIN",
    [STATE_SETTINGS_TRAIN_PERIOD] = "PERIOD",
    [STATE_SETTINGS_CONT] = "CONTIN",
    [STATE_SETTINGS_CONT_PERIOD] = "PERIOD",
};

uint32_t progTimerCnt = 0;

static void start_pulse(void)
{
    if (LL_TIM_IsEnabledCounter(TIM2)) {
        return;
    }
    LL_TIM_SetCounter(TIM2, UINT32_MAX - settings.delay_us);
    LL_TIM_SetCounter(TIM3, TIM3_PERIOD - 1);
    LL_TIM_OC_SetCompareCH1(TIM3, 8 * settings.pulse_width_ns / 100);
    LL_TIM_SetAutoReload(TIM2, settings.train_period_us - 1);
    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableCounter(TIM3);
    LL_TIM_ClearFlag_CC1(TIM2);
    progTimerCnt = 0;
}

static void stop_pulse(void)
{
    LL_TIM_DisableCounter(TIM3);
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetCounter(TIM3, TIM3_PERIOD - 1);
    LL_TIM_SetCounter(TIM2, 0);
}

static void start_continue(void)
{
    uint32_t period_us = (settings.cont_period_ns * 80) / 1000;
    LL_TIM_SetAutoReload(TIM3, period_us - 1);
    LL_TIM_OC_SetCompareCH1(TIM3, period_us / 2);
    LL_TIM_SetSlaveMode(TIM3, LL_TIM_SLAVEMODE_DISABLED);
    LL_TIM_EnableCounter(TIM3);
}

static void stop_continue(void)
{
    LL_TIM_DisableCounter(TIM3);
    LL_TIM_SetAutoReload(TIM3, TIM3_PERIOD - 1);
    LL_TIM_OC_SetCompareCH1(TIM3, 16);
    LL_TIM_SetSlaveMode(TIM3, LL_TIM_SLAVEMODE_GATED);
}

static uint32_t polarity_calc(enum joy joy, uint32_t pol_old)
{
    switch (joy) {
    case JOY_UP:
        pol_old = (pol_old + 1) & 0x01;
        break;
    case JOY_DOWN:
        pol_old = (pol_old - 1) & 0x01;
        break;
    default:
        break;
    }

    if (pol_old) {
        uint8_t buf[6] = "HIGH";
        BSP_LCD_GLASS_DisplayString(buf);
    } else {
        uint8_t buf[6] = "LOW";
        BSP_LCD_GLASS_DisplayString(buf);
    }

    return pol_old;
}

static void polarity_change(enum joy joy)
{
    settings.pol = polarity_calc(joy, settings.pol);

    if (settings.pol) {
        LL_TIM_OC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
    } else {
        LL_TIM_OC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_LOW);
    }
}

static uint32_t pulse_delay_calc(enum joy joy, uint32_t delay_us)
{
    switch (joy) {
    case JOY_UP:
        delay_us += DELAY_STEP_US;
        break;
    case JOY_DOWN:
        delay_us -= DELAY_STEP_US;
        break;
    default:
        break;
    }
    if (delay_us > DELAY_MAX_US) {
        delay_us = DELAY_MAX_US;
    } else if (delay_us < DELAY_MIX_US) {
        delay_us = DELAY_MIX_US;
    } else {
        uint8_t buf[6] = {0};
        bin2str((uint16_t)(delay_us), buf);
        buf[3] = ' ';
        buf[4] = 'm';
        buf[5] = 's';
        BSP_LCD_GLASS_DisplayStringPoint(buf, 1);
    }
    return delay_us;
}

static uint32_t pulse_width_calc(enum joy joy, uint32_t width_ns)
{
    switch (joy) {
    case JOY_UP:
        width_ns += PULSE_WIDTH_STEP_NS;
        break;
    case JOY_DOWN:
        width_ns -= PULSE_WIDTH_STEP_NS;
        break;
    default:
        break;
    }
    if (width_ns > PULSE_WIDTH_MAX_NS) {
        width_ns = PULSE_WIDTH_MAX_NS;
    } else if (width_ns < PULSE_WIDTH_MIX_NS) {
        width_ns = PULSE_WIDTH_MIX_NS;
    } else {
        uint8_t buf[6] = {0};
        bin2str((uint16_t)(width_ns * 100), buf);
        buf[3] = ' ';
        buf[4] = 'n';
        buf[5] = 's';
        BSP_LCD_GLASS_DisplayString(buf);
    }

    return width_ns;
}

static uint32_t train_period_calc(enum joy joy, uint32_t period_us)
{
    switch (joy) {
    case JOY_UP:
        period_us += TRAIN_PERIOD_STEP_US;
        break;
    case JOY_DOWN:
        period_us -= TRAIN_PERIOD_STEP_US;
        break;
    default:
        break;
    }

    if (period_us > TRAIN_PERIOD_MAX_US) {
        period_us = TRAIN_PERIOD_MAX_US;
    } else if (period_us < TRAIN_PERIOD_MIX_US) {
        period_us = TRAIN_PERIOD_MIX_US;
    } else {
        uint8_t buf[6] = {0};
        // выводим интервал между крайними импульсами в пачках
        bin2str((uint16_t)((period_us - 200) * 100), buf);
        buf[3] = ' ';
        buf[4] = 'u';
        buf[5] = 's';
        BSP_LCD_GLASS_DisplayString(buf);
    }
    return period_us;
}

static uint32_t cont_period_calc(enum joy joy, uint32_t period_us)
{
    switch (joy) {
    case JOY_UP:
        period_us += CONTIN_PERIOD_STEP_US;
        break;
    case JOY_DOWN:
        period_us -= CONTIN_PERIOD_STEP_US;
        break;
    default:
        break;
    }

    if (period_us > CONTIN_PERIOD_MAX_US) {
        period_us = CONTIN_PERIOD_MAX_US;
    } else if (period_us < CONTIN_PERIOD_MIX_US) {
        period_us = CONTIN_PERIOD_MIX_US;
    } else {
        uint8_t buf[6] = {0};
        bin2str((uint16_t)(period_us), buf);
        buf[0] = ' ';
        buf[3] = ' ';
        buf[4] = 'u';
        buf[5] = 's';
        BSP_LCD_GLASS_DisplayStringPoint(buf, 1);
    }
    return period_us;
}

void ExtInt_Callback()
{
    start_pulse();
}

enum state state_next(enum state cur, enum joy joy)
{
    enum state new_state = fsm_table[cur][joy];
    if (new_state == STATE_NONE) {
        new_state = cur;
    }
    return new_state;
}

enum state state_action(enum state state, enum joy joy)
{
    switch (state) {
    case STATE_PULSE: {
        LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_14);
        stop_pulse();
    } break;
    case STATE_PULSE_WAIT: {
        if (LL_TIM_IsEnabledCounter(TIM2)) {
            while (LL_TIM_IsActiveFlag_CC1(TIM2) == 0) {
                ;
            }
        }
        stop_pulse();
        relay_off();
        LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_14);
    } break;
    case STATE_PULSE_RUN_SINGLE: {
        relay_on();
        start_pulse();
    } break;
    case STATE_CONT: {
        stop_continue();
    } break;
    case STATE_CONT_RUN: {
        start_continue();
    } break;
    case STATE_SETTINGS_DELAY_SET: {
        settings.delay_us = pulse_delay_calc(joy, settings.delay_us);
    } break;
    case STATE_SETTINGS_PULSE_WIDTH_SET: {
        settings.pulse_width_ns = pulse_width_calc(joy, settings.pulse_width_ns);
    } break;
    case STATE_SETTINGS_TRAIN_PERIOD_SET: {
        settings.train_period_us = train_period_calc(joy, settings.train_period_us);
    } break;
    case STATE_SETTINGS_CONT_PERIOD_SET: {
        settings.cont_period_ns = cont_period_calc(joy, settings.cont_period_ns);
    } break;
    case STATE_SETTINGS_POLARITY_SET: {
        polarity_change(joy);
    } break;
    case STATE_SETTINGS_SAVE: {
        settings_write_to_flash();
        state = STATE_SETTINGS;
    } break;
    default:
        break;
    }
    const uint8_t *menu_str = state_strs[state];
    if (menu_str[0] != 0) {
        BSP_LCD_GLASS_DisplayString((uint8_t *)menu_str);
    }
    return state;
}
