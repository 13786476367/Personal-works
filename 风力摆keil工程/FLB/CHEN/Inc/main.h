/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FLASH_CLK_Pin GPIO_PIN_5
#define FLASH_CLK_GPIO_Port GPIOA
#define FLASH_D0_Pin GPIO_PIN_6
#define FLASH_D0_GPIO_Port GPIOA
#define FLASH_DI_Pin GPIO_PIN_7
#define FLASH_DI_GPIO_Port GPIOA
#define MPU_SCL_Pin GPIO_PIN_10
#define MPU_SCL_GPIO_Port GPIOB
#define MPU_SDA_Pin GPIO_PIN_11
#define MPU_SDA_GPIO_Port GPIOB
#define FLASH_CS_Pin GPIO_PIN_14
#define FLASH_CS_GPIO_Port GPIOB
#define TEST_Pin GPIO_PIN_15
#define TEST_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_12
#define OLED_CS_GPIO_Port GPIOC
#define OLED_DC_Pin GPIO_PIN_2
#define OLED_DC_GPIO_Port GPIOD
#define OLED_RES_Pin GPIO_PIN_3
#define OLED_RES_GPIO_Port GPIOB
#define OLED_D1_Pin GPIO_PIN_4
#define OLED_D1_GPIO_Port GPIOB
#define OLED_D0_Pin GPIO_PIN_5
#define OLED_D0_GPIO_Port GPIOB
#define KEY_SCL_Pin GPIO_PIN_6
#define KEY_SCL_GPIO_Port GPIOB
#define KEY_SDA_Pin GPIO_PIN_7
#define KEY_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
