/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32u5xx_hal.h"

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
#define LCD_WR_Pin GPIO_PIN_2
#define LCD_WR_GPIO_Port GPIOE
#define LCD_RD_Pin GPIO_PIN_3
#define LCD_RD_GPIO_Port GPIOE
#define LCD_RST_Pin GPIO_PIN_4
#define LCD_RST_GPIO_Port GPIOE
#define GT911_SDA_Pin GPIO_PIN_6
#define GT911_SDA_GPIO_Port GPIOF
#define GT911_SCL_Pin GPIO_PIN_7
#define GT911_SCL_GPIO_Port GPIOF
#define IIC_GC0308_SDA_Pin GPIO_PIN_2
#define IIC_GC0308_SDA_GPIO_Port GPIOC
#define IIC_GC0308_SCL_Pin GPIO_PIN_3
#define IIC_GC0308_SCL_GPIO_Port GPIOC
#define PWDN_Pin GPIO_PIN_7
#define PWDN_GPIO_Port GPIOA
#define D8_Pin GPIO_PIN_8
#define D8_GPIO_Port GPIOD
#define D9_Pin GPIO_PIN_9
#define D9_GPIO_Port GPIOD
#define D10_Pin GPIO_PIN_10
#define D10_GPIO_Port GPIOD
#define D11_Pin GPIO_PIN_11
#define D11_GPIO_Port GPIOD
#define D12_Pin GPIO_PIN_12
#define D12_GPIO_Port GPIOD
#define D13_Pin GPIO_PIN_13
#define D13_GPIO_Port GPIOD
#define D14_Pin GPIO_PIN_14
#define D14_GPIO_Port GPIOD
#define D15_Pin GPIO_PIN_15
#define D15_GPIO_Port GPIOD
#define RST_Pin GPIO_PIN_15
#define RST_GPIO_Port GPIOA
#define GT911_INT_Pin GPIO_PIN_10
#define GT911_INT_GPIO_Port GPIOC
#define GT911_RST_Pin GPIO_PIN_12
#define GT911_RST_GPIO_Port GPIOC
#define D0_Pin GPIO_PIN_0
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_1
#define D1_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_2
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_3
#define D3_GPIO_Port GPIOD
#define D4_Pin GPIO_PIN_4
#define D4_GPIO_Port GPIOD
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOD
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOD
#define D7_Pin GPIO_PIN_7
#define D7_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOE
#define LCD_RS_Pin GPIO_PIN_1
#define LCD_RS_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */
void MX_DCMI_Init(void);
void MX_GPDMA1_Init(void);
void MX_CRC_Init(void);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
