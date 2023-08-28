#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "stm32l476g_discovery_glass_lcd.h"
#include "stm32l4xx_ll_tim.h"
#include "tim.h"
#include "conventer.h"
#include "rcc.h"
#include "settings.h"
#include "state.h"
#include "joy.h"

enum state state = STATE_PULSE;

static void button_handler(enum joy joy)
{
    enum state new_state = state_next(state, joy);
    state = state_action(new_state, joy);
}

int main(void)
{
    HAL_Init();

    /* Configure the system clock */
    rcc_config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_TIM6_Init();
    BSP_LCD_GLASS_Init();

    LL_TIM_EnableCounter(TIM6);
    LL_TIM_EnableIT_UPDATE(TIM6);

    LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableIT_CC1(TIM2);

    LL_TIM_DisableCounter(TIM3);
    LL_TIM_DisableCounter(TIM2);
    settings_init();
    state_action(state, JOY_LEFT);

    LL_EXTI_EnableEvent_0_31(LL_EXTI_LINE_14);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_14);

    /* Infinite loop */

    while (1) {
        LL_GPIO_SetOutputPin(LD_G_GPIO_Port, LD_G_Pin);
        LL_GPIO_SetOutputPin(LD_R_GPIO_Port, LD_R_Pin);
        LL_mDelay(1000);
        LL_GPIO_ResetOutputPin(LD_G_GPIO_Port, LD_G_Pin);
        LL_GPIO_ResetOutputPin(LD_R_GPIO_Port, LD_R_Pin);
        LL_mDelay(1000);
    }
}

#define MAX_TIME_SEC 20

void Tim6Update_Callback()
{
    static uint32_t btn_on = 0;

    uint32_t joy_pin = LL_GPIO_ReadInputPort(JOY_GPIO_Port) & JOY_ALL_Pin;
    if (joy_pin) {
        if (btn_on == 0) {
            btn_on = 1;
            enum joy joy = joy_get(joy_pin);
            if (joy != JOY_NONE) {
                button_handler(joy);
            }
        }
    } else {
        btn_on = 0;
    }
    if (state == STATE_PULSE_RUN_SINGLE) {
        uint32_t time_s = progTimerCnt / 100;
        static uint32_t old_time_s = 0;
        if (old_time_s != time_s) {
            old_time_s = time_s;
            uint8_t buf[6] = {' ', ' ', ' ', 'S', 'e', 'c'};
            bin2str((MAX_TIME_SEC - time_s) * 1000, buf);
            buf[2] = (uint8_t)' ';
            buf[3] = (uint8_t)'S';
            buf[4] = (uint8_t)'e';
            buf[5] = (uint8_t)'c';
            BSP_LCD_GLASS_DisplayString(buf);
        }

        if (time_s > MAX_TIME_SEC) {
            button_handler(JOY_LEFT_Pin);
            relay_off();
        };
    }

    progTimerCnt++;
}

void Error_Handler(void)
{
    while (1) {
        LL_GPIO_SetOutputPin(LD_R_GPIO_Port, LD_R_Pin);
        LL_mDelay(300);
        LL_GPIO_ResetOutputPin(LD_R_GPIO_Port, LD_R_Pin);
        LL_mDelay(300);
    }
}
