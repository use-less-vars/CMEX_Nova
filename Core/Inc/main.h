/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define FIRST_Pin GPIO_PIN_1
#define FIRST_GPIO_Port GPIOA
#define CONVST_Pin GPIO_PIN_2
#define CONVST_GPIO_Port GPIOA
#define ADC_RST_Pin GPIO_PIN_3
#define ADC_RST_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_4
#define CS_GPIO_Port GPIOA
#define OS0_Pin GPIO_PIN_7
#define OS0_GPIO_Port GPIOA
#define AWAGS_SEL_Pin GPIO_PIN_12
#define AWAGS_SEL_GPIO_Port GPIOB
#define AWAGS_CLK_Pin GPIO_PIN_13
#define AWAGS_CLK_GPIO_Port GPIOB
#define AWAGS_MISO_Pin GPIO_PIN_14
#define AWAGS_MISO_GPIO_Port GPIOB
#define AWAGS_MOSI_Pin GPIO_PIN_15
#define AWAGS_MOSI_GPIO_Port GPIOB
#define AWAGS_Reset_Pin GPIO_PIN_6
#define AWAGS_Reset_GPIO_Port GPIOC
#define OS1_Pin GPIO_PIN_8
#define OS1_GPIO_Port GPIOA
#define OS2_Pin GPIO_PIN_9
#define OS2_GPIO_Port GPIOA
#define BUSY_Pin GPIO_PIN_10
#define BUSY_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
void main_timer_callback(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
