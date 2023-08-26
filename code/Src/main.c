#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "stm32l476g_discovery_glass_lcd.h"
#include "stm32l4xx_ll_tim.h"
#include "tim.h"
#include "conventer.h"

typedef enum {
    JOY_UP = 0,
    JOY_DOWN = 1,
    JOY_SEL = 2,
    JOY_LEFT = 3,
    JOY_RIGHT = 4,
    JOY_NONE = 5,
    JOY_MAX
} JoyState_t;

typedef enum {
    // STATE_DISABLE = 0,
    // STATE_DISABLE_RESET,
    // STATE_DISABLE_RESET_SURE,
    // STATE_PULSE,
    STATE_PULSE_WAIT,
    STATE_PULSE_RUN,
    STATE_PULSE_RUN_SINGLE,
    STATE_PULSE_WIDTH,
    STATE_PULSE_WIDTH_SET,
    STATE_PULSE_PERIOD,
    STATE_PULSE_PERIOD_SET,
    STATE_PULSE_OFFSET,
    STATE_PULSE_OFFSET_SET,
    STATE_PULSE_POLARITY,
    STATE_PULSE_POLARITY_SET,
    STATE_CONTINUOUS,
    STATE_CONTINUOUS_RUN,
    STATE_CONTINUOUS_PERIOD,
    STATE_CONTINUOUS_PERIOD_SET,
    STATE_PERIOD,
    // STATE_CONTINUOUS_WIDTH,
    // STATE_CONTINUOUS_WIDTH_SET,
    STATE_MAX
} State_t;

// clang-format off
const State_t fsmTable[STATE_MAX][JOY_MAX] = {
    // [STATE_DISABLE][JOY_UP]    = STATE_PULSE,
    // [STATE_DISABLE][JOY_DOWN]  = STATE_CONTINUOUS,
    // [STATE_DISABLE][JOY_SEL]   = STATE_DISABLE,
    // [STATE_DISABLE][JOY_LEFT]  = STATE_DISABLE,
    // [STATE_DISABLE][JOY_RIGHT] = STATE_DISABLE_RESET,
    // [STATE_DISABLE][JOY_NONE]  = STATE_DISABLE,

    // [STATE_DISABLE_RESET][JOY_UP]    = STATE_DISABLE,
    // [STATE_DISABLE_RESET][JOY_DOWN]  = STATE_DISABLE,
    // [STATE_DISABLE_RESET][JOY_SEL]   = STATE_DISABLE,
    // [STATE_DISABLE_RESET][JOY_LEFT]  = STATE_DISABLE,
    // [STATE_DISABLE_RESET][JOY_RIGHT] = STATE_DISABLE_RESET_SURE,
    // [STATE_DISABLE_RESET][JOY_NONE]  = STATE_DISABLE,

    // [STATE_DISABLE_RESET_SURE][JOY_UP]    = STATE_DISABLE,
    // [STATE_DISABLE_RESET_SURE][JOY_DOWN]  = STATE_DISABLE,
    // [STATE_DISABLE_RESET_SURE][JOY_SEL]   = STATE_DISABLE,
    // [STATE_DISABLE_RESET_SURE][JOY_LEFT]  = STATE_DISABLE,
    // [STATE_DISABLE_RESET_SURE][JOY_RIGHT] = STATE_DISABLE,
    // [STATE_DISABLE_RESET_SURE][JOY_NONE]  = STATE_DISABLE,

    // [STATE_PULSE][JOY_UP]    = STATE_CONTINUOUS,
    // [STATE_PULSE][JOY_DOWN]  = STATE_DISABLE,
    // [STATE_PULSE][JOY_SEL]   = STATE_PULSE,
    // [STATE_PULSE][JOY_LEFT]  = STATE_PULSE,
    // [STATE_PULSE][JOY_RIGHT] = STATE_PULSE_PERIOD,
    // [STATE_PULSE][JOY_NONE]  = STATE_PULSE,

    [STATE_PULSE_RUN][JOY_UP]    = STATE_PULSE_POLARITY,
    [STATE_PULSE_RUN][JOY_DOWN]  = STATE_CONTINUOUS,
    [STATE_PULSE_RUN][JOY_SEL]   = STATE_PULSE_WAIT,
    [STATE_PULSE_RUN][JOY_LEFT]  = STATE_PULSE_RUN,
    [STATE_PULSE_RUN][JOY_RIGHT] = STATE_PULSE_WAIT,
    [STATE_PULSE_RUN][JOY_NONE]  = STATE_PULSE_RUN,

    [STATE_PULSE_WAIT][JOY_UP]    = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_WAIT][JOY_DOWN]  = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_WAIT][JOY_SEL]   = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_WAIT][JOY_LEFT]  = STATE_PULSE_RUN,
    [STATE_PULSE_WAIT][JOY_RIGHT] = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_WAIT][JOY_NONE]  = STATE_PULSE_RUN_SINGLE,

    [STATE_PULSE_RUN_SINGLE][JOY_UP]    = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_RUN_SINGLE][JOY_DOWN]  = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_RUN_SINGLE][JOY_SEL]   = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_RUN_SINGLE][JOY_LEFT]  = STATE_PULSE_WAIT,
    [STATE_PULSE_RUN_SINGLE][JOY_RIGHT] = STATE_PULSE_RUN_SINGLE,
    [STATE_PULSE_RUN_SINGLE][JOY_NONE]  = STATE_PULSE_RUN_SINGLE,

    [STATE_PULSE_WIDTH][JOY_UP]    = STATE_CONTINUOUS,
    [STATE_PULSE_WIDTH][JOY_DOWN]  = STATE_PERIOD,
    [STATE_PULSE_WIDTH][JOY_SEL]   = STATE_PULSE_WIDTH_SET,
    [STATE_PULSE_WIDTH][JOY_LEFT]  = STATE_PULSE_WIDTH,
    [STATE_PULSE_WIDTH][JOY_RIGHT] = STATE_PULSE_WIDTH_SET,
    [STATE_PULSE_WIDTH][JOY_NONE]  = STATE_PULSE_WIDTH,

    [STATE_PULSE_WIDTH_SET][JOY_UP]    = STATE_PULSE_WIDTH_SET,
    [STATE_PULSE_WIDTH_SET][JOY_DOWN]  = STATE_PULSE_WIDTH_SET,
    [STATE_PULSE_WIDTH_SET][JOY_SEL]   = STATE_PULSE_WIDTH,
    [STATE_PULSE_WIDTH_SET][JOY_LEFT]  = STATE_PULSE_WIDTH,
    [STATE_PULSE_WIDTH_SET][JOY_RIGHT] = STATE_PULSE_WIDTH,
    [STATE_PULSE_WIDTH_SET][JOY_NONE]  = STATE_PULSE_WIDTH_SET,

    [STATE_PERIOD][JOY_UP]    = STATE_PULSE_WIDTH,
    [STATE_PERIOD][JOY_DOWN]  = STATE_PULSE_OFFSET,
    [STATE_PERIOD][JOY_SEL]   = STATE_PULSE_PERIOD,
    [STATE_PERIOD][JOY_LEFT]  = STATE_PERIOD,
    [STATE_PERIOD][JOY_RIGHT] = STATE_PULSE_PERIOD,
    [STATE_PERIOD][JOY_NONE]  = STATE_PERIOD,

    [STATE_PULSE_PERIOD][JOY_UP]    = STATE_CONTINUOUS_PERIOD,
    [STATE_PULSE_PERIOD][JOY_DOWN]  = STATE_CONTINUOUS_PERIOD,
    [STATE_PULSE_PERIOD][JOY_SEL]   = STATE_PULSE_PERIOD_SET,
    [STATE_PULSE_PERIOD][JOY_LEFT]  = STATE_PERIOD,
    [STATE_PULSE_PERIOD][JOY_RIGHT] = STATE_PULSE_PERIOD_SET,
    [STATE_PULSE_PERIOD][JOY_NONE]  = STATE_PULSE_PERIOD,

    [STATE_PULSE_PERIOD_SET][JOY_UP]    = STATE_PULSE_PERIOD_SET,
    [STATE_PULSE_PERIOD_SET][JOY_DOWN]  = STATE_PULSE_PERIOD_SET,
    [STATE_PULSE_PERIOD_SET][JOY_SEL]   = STATE_PULSE_PERIOD,
    [STATE_PULSE_PERIOD_SET][JOY_LEFT]  = STATE_PULSE_PERIOD,
    [STATE_PULSE_PERIOD_SET][JOY_RIGHT] = STATE_PULSE_PERIOD,
    [STATE_PULSE_PERIOD_SET][JOY_NONE]  = STATE_PULSE_PERIOD_SET,

    [STATE_CONTINUOUS_PERIOD][JOY_UP]    = STATE_PULSE_PERIOD,
    [STATE_CONTINUOUS_PERIOD][JOY_DOWN]  = STATE_PULSE_PERIOD,
    [STATE_CONTINUOUS_PERIOD][JOY_SEL]   = STATE_CONTINUOUS_PERIOD_SET,
    [STATE_CONTINUOUS_PERIOD][JOY_LEFT]  = STATE_PERIOD,
    [STATE_CONTINUOUS_PERIOD][JOY_RIGHT] = STATE_CONTINUOUS_PERIOD_SET,
    [STATE_CONTINUOUS_PERIOD][JOY_NONE]  = STATE_CONTINUOUS_PERIOD,

    [STATE_CONTINUOUS_PERIOD_SET][JOY_UP]    = STATE_CONTINUOUS_PERIOD_SET,
    [STATE_CONTINUOUS_PERIOD_SET][JOY_DOWN]  = STATE_CONTINUOUS_PERIOD_SET,
    [STATE_CONTINUOUS_PERIOD_SET][JOY_SEL]   = STATE_CONTINUOUS_PERIOD,
    [STATE_CONTINUOUS_PERIOD_SET][JOY_LEFT]  = STATE_CONTINUOUS_PERIOD,
    [STATE_CONTINUOUS_PERIOD_SET][JOY_RIGHT] = STATE_CONTINUOUS_PERIOD,
    [STATE_CONTINUOUS_PERIOD_SET][JOY_NONE]  = STATE_CONTINUOUS_PERIOD_SET,

    [STATE_PULSE_OFFSET][JOY_UP]    = STATE_PERIOD,
    [STATE_PULSE_OFFSET][JOY_DOWN]  = STATE_PULSE_POLARITY,
    [STATE_PULSE_OFFSET][JOY_SEL]   = STATE_PULSE_OFFSET_SET,
    [STATE_PULSE_OFFSET][JOY_LEFT]  = STATE_PULSE_OFFSET,
    [STATE_PULSE_OFFSET][JOY_RIGHT] = STATE_PULSE_OFFSET_SET,
    [STATE_PULSE_OFFSET][JOY_NONE]  = STATE_PULSE_OFFSET,

    [STATE_PULSE_OFFSET_SET][JOY_UP]    = STATE_PULSE_OFFSET_SET,
    [STATE_PULSE_OFFSET_SET][JOY_DOWN]  = STATE_PULSE_OFFSET_SET,
    [STATE_PULSE_OFFSET_SET][JOY_SEL]   = STATE_PULSE_OFFSET,
    [STATE_PULSE_OFFSET_SET][JOY_LEFT]  = STATE_PULSE_OFFSET,
    [STATE_PULSE_OFFSET_SET][JOY_RIGHT] = STATE_PULSE_OFFSET,
    [STATE_PULSE_OFFSET_SET][JOY_NONE]  = STATE_PULSE_OFFSET_SET,

    [STATE_PULSE_POLARITY][JOY_UP]    = STATE_PULSE_OFFSET,
    [STATE_PULSE_POLARITY][JOY_DOWN]  = STATE_PULSE_RUN,
    [STATE_PULSE_POLARITY][JOY_SEL]   = STATE_PULSE_POLARITY_SET,
    [STATE_PULSE_POLARITY][JOY_LEFT]  = STATE_PULSE_POLARITY,
    [STATE_PULSE_POLARITY][JOY_RIGHT] = STATE_PULSE_POLARITY_SET,
    [STATE_PULSE_POLARITY][JOY_NONE]  = STATE_PULSE_POLARITY,

    [STATE_PULSE_POLARITY_SET][JOY_UP]    = STATE_PULSE_POLARITY_SET,
    [STATE_PULSE_POLARITY_SET][JOY_DOWN]  = STATE_PULSE_POLARITY_SET,
    [STATE_PULSE_POLARITY_SET][JOY_SEL]   = STATE_PULSE_POLARITY,
    [STATE_PULSE_POLARITY_SET][JOY_LEFT]  = STATE_PULSE_POLARITY,
    [STATE_PULSE_POLARITY_SET][JOY_RIGHT] = STATE_PULSE_POLARITY,
    [STATE_PULSE_POLARITY_SET][JOY_NONE]  = STATE_PULSE_POLARITY_SET,

    [STATE_CONTINUOUS][JOY_UP]    = STATE_PULSE_RUN,
    [STATE_CONTINUOUS][JOY_DOWN]  = STATE_PULSE_WIDTH,
    [STATE_CONTINUOUS][JOY_SEL]   = STATE_CONTINUOUS_RUN,
    [STATE_CONTINUOUS][JOY_LEFT]  = STATE_CONTINUOUS,
    [STATE_CONTINUOUS][JOY_RIGHT] = STATE_CONTINUOUS_RUN,
    [STATE_CONTINUOUS][JOY_NONE]  = STATE_CONTINUOUS,

    [STATE_CONTINUOUS_RUN][JOY_UP]    = STATE_CONTINUOUS_RUN,
    [STATE_CONTINUOUS_RUN][JOY_DOWN]  = STATE_CONTINUOUS_RUN,
    [STATE_CONTINUOUS_RUN][JOY_SEL]   = STATE_CONTINUOUS_RUN,
    [STATE_CONTINUOUS_RUN][JOY_LEFT]  = STATE_CONTINUOUS,
    [STATE_CONTINUOUS_RUN][JOY_RIGHT] = STATE_CONTINUOUS_RUN,
    [STATE_CONTINUOUS_RUN][JOY_NONE]  = STATE_CONTINUOUS_RUN,


    // [STATE_CONTINUOUS][JOY_UP]    = STATE_DISABLE,
    // [STATE_CONTINUOUS][JOY_DOWN]  = STATE_PULSE,
    // [STATE_CONTINUOUS][JOY_SEL]   = STATE_CONTINUOUS,
    // [STATE_CONTINUOUS][JOY_LEFT]  = STATE_CONTINUOUS,
    // [STATE_CONTINUOUS][JOY_RIGHT] = STATE_CONTINUOUS_PERIOD,
    // [STATE_CONTINUOUS][JOY_NONE]  = STATE_CONTINUOUS,

    // [STATE_CONTINUOUS_PERIOD][JOY_UP]    = STATE_CONTINUOUS_WIDTH,
    // [STATE_CONTINUOUS_PERIOD][JOY_DOWN]  = STATE_CONTINUOUS_WIDTH,
    // [STATE_CONTINUOUS_PERIOD][JOY_SEL]   = STATE_CONTINUOUS_PERIOD,
    // [STATE_CONTINUOUS_PERIOD][JOY_LEFT]  = STATE_CONTINUOUS,
    // [STATE_CONTINUOUS_PERIOD][JOY_RIGHT] = STATE_CONTINUOUS_PERIOD_SET,
    // [STATE_CONTINUOUS_PERIOD][JOY_NONE]  = STATE_CONTINUOUS_PERIOD,

    // [STATE_CONTINUOUS_PERIOD_SET][JOY_UP]    = STATE_CONTINUOUS_PERIOD_SET,
    // [STATE_CONTINUOUS_PERIOD_SET][JOY_DOWN]  = STATE_CONTINUOUS_PERIOD_SET,
    // [STATE_CONTINUOUS_PERIOD_SET][JOY_SEL]   = STATE_CONTINUOUS_PERIOD_SET,
    // [STATE_CONTINUOUS_PERIOD_SET][JOY_LEFT]  = STATE_CONTINUOUS_PERIOD,
    // [STATE_CONTINUOUS_PERIOD_SET][JOY_RIGHT] = STATE_CONTINUOUS_PERIOD_SET,
    // [STATE_CONTINUOUS_PERIOD_SET][JOY_NONE]  = STATE_CONTINUOUS_PERIOD_SET,

    // [STATE_CONTINUOUS_WIDTH][JOY_UP]    = STATE_CONTINUOUS_PERIOD,
    // [STATE_CONTINUOUS_WIDTH][JOY_DOWN]  = STATE_CONTINUOUS_PERIOD,
    // [STATE_CONTINUOUS_WIDTH][JOY_SEL]   = STATE_CONTINUOUS_WIDTH,
    // [STATE_CONTINUOUS_WIDTH][JOY_LEFT]  = STATE_CONTINUOUS,
    // [STATE_CONTINUOUS_WIDTH][JOY_RIGHT] = STATE_CONTINUOUS_WIDTH_SET,
    // [STATE_CONTINUOUS_WIDTH][JOY_NONE]  = STATE_CONTINUOUS_WIDTH,

    // [STATE_CONTINUOUS_WIDTH_SET][JOY_UP]    = STATE_CONTINUOUS_WIDTH_SET,
    // [STATE_CONTINUOUS_WIDTH_SET][JOY_DOWN]  = STATE_CONTINUOUS_WIDTH_SET,
    // [STATE_CONTINUOUS_WIDTH_SET][JOY_SEL]   = STATE_CONTINUOUS_WIDTH_SET,
    // [STATE_CONTINUOUS_WIDTH_SET][JOY_LEFT]  = STATE_CONTINUOUS_WIDTH,
    // [STATE_CONTINUOUS_WIDTH_SET][JOY_RIGHT] = STATE_CONTINUOUS_WIDTH_SET,
    // [STATE_CONTINUOUS_WIDTH_SET][JOY_NONE]  = STATE_CONTINUOUS_WIDTH_SET
};

// clang-format on
typedef struct
{
    uint8_t btnOn :1;
} Flags_t;

Flags_t flags = {0};
volatile uint32_t progTimerCnt = 0;
volatile State_t state;

static uint16_t pulseOffset = 10500;
static uint16_t pulseWidth = 200;
static uint16_t pulsePeriod = 402;
// период для непрерывного режима в нс
static uint32_t continue_period = 1000;

void SystemClock_Config(void);
void ButtonHandler(uint32_t joyPort);
uint16_t PulseOffsetCalc(JoyState_t joyState, uint16_t offset);
uint16_t PulseWidthCalc(JoyState_t joyState, uint16_t count);
uint16_t PulsePeriodCalc(JoyState_t joyState, uint16_t period);
uint16_t ContinWidthCalc(JoyState_t joyState, uint16_t width);
uint16_t ContinPeriodCalc(JoyState_t joyState, uint16_t period);
uint32_t PulsePolarityChange(JoyState_t joyState, uint32_t pol_old);

inline static void start_pulse()
{
    if (LL_TIM_IsEnabledCounter(TIM2)) {
        return;
    }

    // LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_14);
    LL_TIM_SetCounter(TIM2, UINT32_MAX - pulseOffset);
    LL_TIM_SetCounter(TIM3, TIM3_PERIOD - 1);
    LL_TIM_OC_SetCompareCH1(TIM3, 8 * pulseWidth / 100);
    LL_TIM_SetAutoReload(TIM2, pulsePeriod - 1);
    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableCounter(TIM3);
    LL_TIM_ClearFlag_CC1(TIM2);
}

inline static void start_continue()
{
    uint32_t period = (continue_period * 80) / 1000;
    LL_TIM_SetAutoReload(TIM3, period - 1);
    LL_TIM_OC_SetCompareCH1(TIM3, period / 2);
    LL_TIM_SetSlaveMode(TIM3, LL_TIM_SLAVEMODE_DISABLED);
    LL_TIM_EnableCounter(TIM3);
}

inline static void stop_continue()
{
    LL_TIM_DisableCounter(TIM3);
    LL_TIM_SetAutoReload(TIM3, TIM3_PERIOD - 1);
    LL_TIM_OC_SetCompareCH1(TIM3, 16);
    LL_TIM_SetSlaveMode(TIM3, LL_TIM_SLAVEMODE_GATED);
}

int main(void)
{
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

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

    // LL_TIM_EnableCounter(TIM3);
    // LL_TIM_EnableCounter(TIM2);
    LL_TIM_DisableCounter(TIM3);
    LL_TIM_DisableCounter(TIM2);
    // LL_TIM_SetCounter(TIM3, 80 - 1);
    // LL_TIM_SetCounter(TIM2, 0);
    BSP_LCD_GLASS_DisplayString((uint8_t *)"PULSE");
    state = STATE_PULSE_RUN;

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

void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4) {
    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while (LL_RCC_HSE_IsReady() != 1) {
    }
    LL_RCC_LSI_Enable();

    /* Wait till LSI is ready */
    while (LL_RCC_LSI_IsReady() != 1) {
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
    LL_RCC_EnableRTC();
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_1, 20, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1) {
    }
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_SetSystemCoreClock(80000000);

    /* Update the time base */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
        Error_Handler();
    }
    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
}

void ExtInt_Callback()
{
    start_pulse();
}

#define MAX_TIME_SEC 20

void Tim6Update_Callback()
{
    uint32_t joyPort = LL_GPIO_ReadInputPort(JOY_GPIO_Port) & JOY_ALL_Pin;
    if (joyPort) {
        if (!flags.btnOn) {
            flags.btnOn = 1;
            ButtonHandler(joyPort);
        }
    } else {
        flags.btnOn = 0;
    }
    if (state == STATE_PULSE_RUN_SINGLE) {
        uint32_t time_s = progTimerCnt / 100;
        static uint32_t old_time_s = 0;
        if (old_time_s != time_s) {
            old_time_s = time_s;
            uint8_t buf[6] = {' ', ' ', ' ', 'S', 'e', 'c'};
            Bin2Str((MAX_TIME_SEC - time_s) * 1000, buf, 5);
            buf[2] = (uint8_t)' ';
            buf[3] = (uint8_t)'S';
            buf[4] = (uint8_t)'e';
            buf[5] = (uint8_t)'c';
            BSP_LCD_GLASS_DisplayString(buf);
        }

        if (time_s > MAX_TIME_SEC) {
            ButtonHandler(JOY_LEFT_Pin);
            RelayOff();
        };
    }

    progTimerCnt++;
}

void ButtonHandler(uint32_t joyPort)
{
    JoyState_t joyState = JOY_NONE;
    if ((joyPort & JOY_CENTER_Pin) == JOY_CENTER_Pin) {
        joyState = JOY_SEL;
    } else if ((joyPort & JOY_DOWN_Pin) == JOY_DOWN_Pin) {
        joyState = JOY_DOWN;
    } else if ((joyPort & JOY_UP_Pin) == JOY_UP_Pin) {
        joyState = JOY_UP;
    } else if ((joyPort & JOY_LEFT_Pin) == JOY_LEFT_Pin) {
        joyState = JOY_LEFT;
    } else if ((joyPort & JOY_RIGHT_Pin) == JOY_RIGHT_Pin) {
        joyState = JOY_RIGHT;
    } else {
        return;
    }

    State_t stateOld = state;
    state = fsmTable[stateOld][joyState];

    switch (state) {
    case STATE_PULSE_RUN:
        LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_14);
        LL_TIM_DisableCounter(TIM3);
        LL_TIM_DisableCounter(TIM2);
        LL_TIM_SetCounter(TIM3, TIM3_PERIOD - 1);
        LL_TIM_SetCounter(TIM2, 0);
        BSP_LCD_GLASS_DisplayString((uint8_t *)"PULSE");
        break;
    case STATE_PULSE_WAIT:

        if (LL_TIM_IsEnabledCounter(TIM2)) {
            while (LL_TIM_IsActiveFlag_CC1(TIM2) == 0) {
                ;
            }
        }
        RelayOff();
        LL_TIM_DisableCounter(TIM3);
        LL_TIM_DisableCounter(TIM2);
        LL_TIM_SetCounter(TIM3, TIM3_PERIOD - 1);
        LL_TIM_SetCounter(TIM2, 0);
        BSP_LCD_GLASS_DisplayString((uint8_t *)"WAIT");
        LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_14);
        break;
    case STATE_PULSE_RUN_SINGLE:
        RelayOn();
        LL_mDelay(50);
        start_pulse();
        BSP_LCD_GLASS_DisplayString((uint8_t *)"RUN S");
        progTimerCnt = 0;
        // state = STATE_PULSE_WAIT;
        break;
    case STATE_CONTINUOUS:
        stop_continue();
        BSP_LCD_GLASS_DisplayString((uint8_t *)"CONTIN");
        break;
    case STATE_CONTINUOUS_RUN:
        start_continue();
        BSP_LCD_GLASS_DisplayString((uint8_t *)"RUNING");
        break;
    case STATE_PULSE_OFFSET:
        BSP_LCD_GLASS_DisplayString((uint8_t *)"OFFSET");
        break;
    case STATE_PULSE_OFFSET_SET: {
        pulseOffset = PulseOffsetCalc(joyState, pulseOffset);
        break;
    }
    case STATE_PULSE_WIDTH:
        BSP_LCD_GLASS_DisplayString((uint8_t *)"WIDTH");
        break;
    case STATE_PULSE_WIDTH_SET: {
        pulseWidth = PulseWidthCalc(joyState, pulseWidth);
        break;
    }
    case STATE_PERIOD:
        BSP_LCD_GLASS_DisplayString((uint8_t *)"PERIOD");
        break;
    case STATE_PULSE_PERIOD:
        BSP_LCD_GLASS_DisplayString((uint8_t *)"PULSE");
        break;
    case STATE_CONTINUOUS_PERIOD:
        BSP_LCD_GLASS_DisplayString((uint8_t *)"CONTIN");
        break;
    case STATE_PULSE_PERIOD_SET:
        pulsePeriod = PulsePeriodCalc(joyState, pulsePeriod);
        break;
    case STATE_CONTINUOUS_PERIOD_SET:
        continue_period = ContinPeriodCalc(joyState, continue_period);
        break;
    case STATE_PULSE_POLARITY:
        BSP_LCD_GLASS_DisplayString((uint8_t *)"POLAR");
        break;
    case STATE_PULSE_POLARITY_SET: {
        uint32_t pol =
            LL_TIM_OC_GetPolarity(TIM3, LL_TIM_CHANNEL_CH1) == LL_TIM_OCPOLARITY_HIGH
                ? 1
                : 0;
        pol = PulsePolarityChange(joyState, pol);

        if (pol) {
            LL_TIM_OC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
        } else {
            LL_TIM_OC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_LOW);
        }
    }

    break;
    default:
        break;
    }
}

uint16_t PulseOffsetCalc(JoyState_t joyState, uint16_t offset)
{
    switch (joyState) {
    case JOY_SEL:
        break;
    case JOY_UP:
        offset = offset + 100;
        break;
    case JOY_DOWN:
        offset = offset - 100;
        break;
    case JOY_RIGHT:
        break;
    case JOY_LEFT:
        break;
    default:
        break;
    }
    if (offset > 15000) {
        offset = 15000;
    } else if (offset < 8000) {
        offset = 8000;
    } else {
        uint8_t buf[6] = {0};
        Bin2Str((uint16_t)(offset), buf, 5);
        buf[3] = ' ';
        buf[4] = 'm';
        buf[5] = 's';
        BSP_LCD_GLASS_DisplayStringPoint(buf, 1);
    }
    return offset;
}

uint16_t PulseWidthCalc(JoyState_t joyState, uint16_t width)
{
    switch (joyState) {
    case JOY_SEL:
        break;
    case JOY_UP:
        width = width + 50;
        break;
    case JOY_DOWN:
        width = width - 50;
        break;
    case JOY_RIGHT:
        break;
    case JOY_LEFT:
        break;
    default:
        break;
    }
    if (width > 500) {
        width = 500;
    } else if (width < 200) {
        width = 200;
    } else {
        uint8_t buf[6] = {0};
        Bin2Str((uint16_t)(width * 100), buf, 5);
        buf[3] = ' ';
        buf[4] = 'n';
        buf[5] = 's';
        BSP_LCD_GLASS_DisplayString(buf);
    }

    return width;
}

uint16_t PulsePeriodCalc(JoyState_t joyState, uint16_t period)
{
    switch (joyState) {
    case JOY_SEL:
        break;
    case JOY_UP:
        period += 1;
        break;
    case JOY_DOWN:
        period -= 1;
        break;
    case JOY_RIGHT:
        break;
    case JOY_LEFT:
        break;
    default:
        break;
    }

    if (period > 410) {
        period = 410;
    } else if (period < 400) {
        period = 400;
    } else {
        uint8_t buf[6] = {0};
        Bin2Str((uint16_t)((period - 200) * 100), buf, 5);
        buf[3] = ' ';
        buf[4] = 'u';
        buf[5] = 's';
        BSP_LCD_GLASS_DisplayString(buf);
    }
    return period;
}

uint16_t ContinPeriodCalc(JoyState_t joyState, uint16_t period)
{
    switch (joyState) {
    case JOY_SEL:
        break;
    case JOY_UP:
        period += 500;
        break;
    case JOY_DOWN:
        period -= 500;
        break;
    case JOY_RIGHT:
        break;
    case JOY_LEFT:
        break;
    default:
        break;
    }

    if (period > 5000) {
        period = 5000;
    } else if (period < 1000) {
        period = 1000;
    } else {
        uint8_t buf[6] = {0};
        Bin2Str((uint16_t)(period), buf, 5);
        buf[0] = ' ';
        buf[3] = ' ';
        buf[4] = 'u';
        buf[5] = 's';
        BSP_LCD_GLASS_DisplayStringPoint(buf, 1);
    }
    return period;
}

uint32_t PulsePolarityChange(JoyState_t joyState, uint32_t pol_old)
{
    switch (joyState) {
    case JOY_SEL:
        break;
    case JOY_UP:
        pol_old = (pol_old + 1) & 0x01;
        break;
    case JOY_DOWN:
        pol_old = (pol_old - 1) & 0x01;
        break;
    case JOY_RIGHT:
        break;
    case JOY_LEFT:
        break;
    default:
        break;
    }

    if (pol_old) {
        uint8_t buf[6] = {'H', 'I', 'G', 'H'};
        BSP_LCD_GLASS_DisplayString(buf);
    } else {
        uint8_t buf[6] = {'L', 'O', 'W'};
        BSP_LCD_GLASS_DisplayString(buf);
    }

    return pol_old;
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
