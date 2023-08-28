#ifndef __MAIN_H
#define __MAIN_H

#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_crs.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_gpio.h"

void Error_Handler(void);
void Tim6Update_Callback(void);

#define SAI1_MCK_Pin                   LL_GPIO_PIN_2
#define SAI1_MCK_GPIO_Port             GPIOE
#define PWM_OUT_Pin                    LL_GPIO_PIN_3
#define PWM_OUT_GPIO_Port              GPIOE
#define SAI1_FS_Pin                    LL_GPIO_PIN_4
#define SAI1_FS_GPIO_Port              GPIOE
#define SAI1_SCK_Pin                   LL_GPIO_PIN_5
#define SAI1_SCK_GPIO_Port             GPIOE
#define SAI1_SD_Pin                    LL_GPIO_PIN_6
#define SAI1_SD_GPIO_Port              GPIOE
#define MFX_IRQ_OUT_Pin                LL_GPIO_PIN_13
#define MFX_IRQ_OUT_GPIO_Port          GPIOC
#define MAG_INT_Pin                    LL_GPIO_PIN_1
#define MAG_INT_GPIO_Port              GPIOC
#define MAG_DRDY_Pin                   LL_GPIO_PIN_2
#define MAG_DRDY_GPIO_Port             GPIOC
#define VLCD_Pin                       LL_GPIO_PIN_3
#define VLCD_GPIO_Port                 GPIOC

#define JOY_CENTER_Pin                 LL_GPIO_PIN_0
#define JOY_LEFT_Pin                   LL_GPIO_PIN_1
#define JOY_RIGHT_Pin                  LL_GPIO_PIN_2
#define JOY_UP_Pin                     LL_GPIO_PIN_3
#define JOY_DOWN_Pin                   LL_GPIO_PIN_5
#define JOY_ALL_Pin                    (JOY_CENTER_Pin | JOY_LEFT_Pin | JOY_RIGHT_Pin | JOY_UP_Pin | JOY_DOWN_Pin)
#define JOY_GPIO_Port                  GPIOA

#define MFX_WAKEUP_Pin                 LL_GPIO_PIN_4
#define MFX_WAKEUP_GPIO_Port           GPIOA
#define SEG23_Pin                      LL_GPIO_PIN_6
#define SEG23_GPIO_Port                GPIOA
#define SEG0_Pin                       LL_GPIO_PIN_7
#define SEG0_GPIO_Port                 GPIOA
#define SEG22_Pin                      LL_GPIO_PIN_4
#define SEG22_GPIO_Port                GPIOC
#define SEG1_Pin                       LL_GPIO_PIN_5
#define SEG1_GPIO_Port                 GPIOC
#define SEG21_Pin                      LL_GPIO_PIN_0
#define SEG21_GPIO_Port                GPIOB
#define SEG2_Pin                       LL_GPIO_PIN_1
#define SEG2_GPIO_Port                 GPIOB
#define LD_R_Pin                       LL_GPIO_PIN_2
#define LD_R_GPIO_Port                 GPIOB
#define AUDIO_DIN_Pin                  LL_GPIO_PIN_7
#define AUDIO_DIN_GPIO_Port            GPIOE
#define LD_G_Pin                       LL_GPIO_PIN_8
#define LD_G_GPIO_Port                 GPIOE
#define AUDIO_CLK_Pin                  LL_GPIO_PIN_9
#define AUDIO_CLK_GPIO_Port            GPIOE
#define QSPI_CLK_Pin                   LL_GPIO_PIN_10
#define QSPI_CLK_GPIO_Port             GPIOE
#define QSPI_CS_Pin                    LL_GPIO_PIN_11
#define QSPI_CS_GPIO_Port              GPIOE
#define QSPI_D0_Pin                    LL_GPIO_PIN_12
#define QSPI_D0_GPIO_Port              GPIOE
#define QSPI_D1_Pin                    LL_GPIO_PIN_13

#define RELAY_GPIO_Port                GPIOE
#define RELAY_Pin                      LL_GPIO_PIN_13
#define RELAY_DIRECT_Pin               LL_GPIO_PIN_15

#define QSPI_D3_GPIO_Port              GPIOE
#define MFX_I2C_SLC_Pin                LL_GPIO_PIN_10
#define MFX_I2C_SLC_GPIO_Port          GPIOB
#define MFX_I2C_SDA_Pin                LL_GPIO_PIN_11
#define MFX_I2C_SDA_GPIO_Port          GPIOB
#define SEG20_Pin                      LL_GPIO_PIN_12
#define SEG20_GPIO_Port                GPIOB
#define SEG3_Pin                       LL_GPIO_PIN_13
#define SEG3_GPIO_Port                 GPIOB
#define SEG19_Pin                      LL_GPIO_PIN_14
#define SEG19_GPIO_Port                GPIOB
#define SEG4_Pin                       LL_GPIO_PIN_15
#define SEG4_GPIO_Port                 GPIOB
#define SEG18_Pin                      LL_GPIO_PIN_8
#define SEG18_GPIO_Port                GPIOD
#define SEG5_Pin                       LL_GPIO_PIN_9
#define SEG5_GPIO_Port                 GPIOD
#define SEG17_Pin                      LL_GPIO_PIN_10
#define SEG17_GPIO_Port                GPIOD
#define SEG6_Pin                       LL_GPIO_PIN_11
#define SEG6_GPIO_Port                 GPIOD
#define SEG16_Pin                      LL_GPIO_PIN_12
#define SEG16_GPIO_Port                GPIOD
#define SEG7_Pin                       LL_GPIO_PIN_13
#define SEG7_GPIO_Port                 GPIOD
#define SEG15_Pin                      LL_GPIO_PIN_14
#define SEG15_GPIO_Port                GPIOD
#define SEG8_Pin                       LL_GPIO_PIN_15
#define SEG8_GPIO_Port                 GPIOD
#define SEG14_Pin                      LL_GPIO_PIN_6
#define SEG14_GPIO_Port                GPIOC
#define SEG9_Pin                       LL_GPIO_PIN_7
#define SEG9_GPIO_Port                 GPIOC
#define SEG13_Pin                      LL_GPIO_PIN_8
#define SEG13_GPIO_Port                GPIOC
#define OTG_FS_PowerSwitchOn_Pin       LL_GPIO_PIN_9
#define OTG_FS_PowerSwitchOn_GPIO_Port GPIOC
#define COM0_Pin                       LL_GPIO_PIN_8
#define COM0_GPIO_Port                 GPIOA
#define COM1_Pin                       LL_GPIO_PIN_9
#define COM1_GPIO_Port                 GPIOA
#define COM2_Pin                       LL_GPIO_PIN_10
#define COM2_GPIO_Port                 GPIOA
#define OTG_FS_DM_Pin                  LL_GPIO_PIN_11
#define OTG_FS_DM_GPIO_Port            GPIOA
#define OTG_FS_DP_Pin                  LL_GPIO_PIN_12
#define OTG_FS_DP_GPIO_Port            GPIOA
#define SWDIO_Pin                      LL_GPIO_PIN_13
#define SWDIO_GPIO_Port                GPIOA
#define SWCLK_Pin                      LL_GPIO_PIN_14
#define SWCLK_GPIO_Port                GPIOA
#define SEG10_Pin                      LL_GPIO_PIN_15
#define SEG10_GPIO_Port                GPIOA
#define OTG_FS_OverCurrent_Pin         LL_GPIO_PIN_10
#define OTG_FS_OverCurrent_GPIO_Port   GPIOC
#define OTG_FS_VBUS_Pin                LL_GPIO_PIN_11
#define OTG_FS_VBUS_GPIO_Port          GPIOC
#define EXT_RST_Pin                    LL_GPIO_PIN_0
#define EXT_RST_GPIO_Port              GPIOD
#define MEMS_SCK_Pin                   LL_GPIO_PIN_1
#define MEMS_SCK_GPIO_Port             GPIOD
#define GYRO_INT1_Pin                  LL_GPIO_PIN_2
#define GYRO_INT1_GPIO_Port            GPIOD
#define MEMS_MISO_Pin                  LL_GPIO_PIN_3
#define MEMS_MISO_GPIO_Port            GPIOD
#define MEMS_MOSI_Pin                  LL_GPIO_PIN_4
#define MEMS_MOSI_GPIO_Port            GPIOD
#define USART_TX_Pin                   LL_GPIO_PIN_5
#define USART_TX_GPIO_Port             GPIOD
#define USART_RX_Pin                   LL_GPIO_PIN_6
#define USART_RX_GPIO_Port             GPIOD
#define GYRO_CS_Pin                    LL_GPIO_PIN_7
#define GYRO_CS_GPIO_Port              GPIOD
#define M3V3_REG_ON_Pin                LL_GPIO_PIN_3
#define M3V3_REG_ON_GPIO_Port          GPIOB
#define SEG11_Pin                      LL_GPIO_PIN_4
#define SEG11_GPIO_Port                GPIOB
#define SEG12_Pin                      LL_GPIO_PIN_5
#define SEG12_GPIO_Port                GPIOB
#define I2C1_SCL_Pin                   LL_GPIO_PIN_6
#define I2C1_SCL_GPIO_Port             GPIOB
#define I2C1_SDA_Pin                   LL_GPIO_PIN_7
#define I2C1_SDA_GPIO_Port             GPIOB
#define GYRO_INT2_Pin                  LL_GPIO_PIN_8
#define GYRO_INT2_GPIO_Port            GPIOB
#define COM3_Pin                       LL_GPIO_PIN_9
#define COM3_GPIO_Port                 GPIOB
#define XL_CS_Pin                      LL_GPIO_PIN_0
#define XL_CS_GPIO_Port                GPIOE
#define XL_INT_Pin                     LL_GPIO_PIN_1
#define XL_INT_GPIO_Port               GPIOE
#define EXT_INT_Pin                    LL_GPIO_PIN_14
#define EXT_INT_GPIO_Port              GPIOC
#define EXT_INT_EXTI_IRQn              EXTI15_10_IRQn

#endif
