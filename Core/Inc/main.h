/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FLY_OUT_I_ADC_Pin GPIO_PIN_1
#define FLY_OUT_I_ADC_GPIO_Port GPIOC
#define MCU_SW_MOS2_Pin GPIO_PIN_2
#define MCU_SW_MOS2_GPIO_Port GPIOC
#define MCU_SW_MOS3_Pin GPIO_PIN_3
#define MCU_SW_MOS3_GPIO_Port GPIOC
#define IC_WAKEUP_Pin GPIO_PIN_0
#define IC_WAKEUP_GPIO_Port GPIOA
#define LPUART1_TX_Pin GPIO_PIN_2
#define LPUART1_TX_GPIO_Port GPIOA
#define LPUART1_RX_Pin GPIO_PIN_3
#define LPUART1_RX_GPIO_Port GPIOA
#define PACK_ENABLE_Pin GPIO_PIN_4
#define PACK_ENABLE_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define MCU_SW_MOS7_Pin GPIO_PIN_6
#define MCU_SW_MOS7_GPIO_Port GPIOA
#define MCU_SW_MOS8_Pin GPIO_PIN_7
#define MCU_SW_MOS8_GPIO_Port GPIOA
#define MCU_SW_MOS9_Pin GPIO_PIN_0
#define MCU_SW_MOS9_GPIO_Port GPIOB
#define MCU_SW_MOS10_Pin GPIO_PIN_1
#define MCU_SW_MOS10_GPIO_Port GPIOB
#define MCU_SW_MOS5_Pin GPIO_PIN_10
#define MCU_SW_MOS5_GPIO_Port GPIOB
#define MCU_SW_MOS6_Pin GPIO_PIN_11
#define MCU_SW_MOS6_GPIO_Port GPIOB
#define PACK_I_ADC_Pin GPIO_PIN_14
#define PACK_I_ADC_GPIO_Port GPIOB
#define MCU_FLY_PWM_Pin GPIO_PIN_8
#define MCU_FLY_PWM_GPIO_Port GPIOA
#define MCU_SW_MOS1_Pin GPIO_PIN_9
#define MCU_SW_MOS1_GPIO_Port GPIOA
#define MCU_SW_MOS11_Pin GPIO_PIN_11
#define MCU_SW_MOS11_GPIO_Port GPIOA
#define T_SWDIO_Pin GPIO_PIN_13
#define T_SWDIO_GPIO_Port GPIOA
#define T_SWCLK_Pin GPIO_PIN_14
#define T_SWCLK_GPIO_Port GPIOA
#define MCU_SW_MOS4_Pin GPIO_PIN_15
#define MCU_SW_MOS4_GPIO_Port GPIOA
#define T_SWO_Pin GPIO_PIN_3
#define T_SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
