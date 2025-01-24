/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "macro_definition.h"				// 统一宏定义
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

//#define SYSTEM_HARDWARE_DEBUG
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

// 串口1 --> 中控Modbus 	(485)
// 串口2 --> wifi 			(ttl)
// 串口3 --> 驱动板 			(ttl)
// 串口4 --> 调试 debug 	(ttl)
// 串口5 --> 蓝牙				(ttl)

#define MODBUS_USART								MACRO_MODBUS_USART
#define	WIFI_USART									MACRO_WIFI_USART
#define	DRIVER_USART								MACRO_DRIVER_USART
#define	DEBUG_USART									MACRO_DEBUG_USART
#define	BLUETOOTH_USART							MACRO_BLUETOOTH_USART

#define	SYSTEM_USER_USART_MAX				MACRO_SYSTEM_USER_USART_MAX

#define THREAD_PERIOD_RS485_MODBUS_TASK					MODBUS_THREAD_LIFECYCLE
#define THREAD_PERIOD_MAIN_TASK									TIMING_THREAD_LIFECYCLE
#define	THREAD_PERIOD_BREATH_LIGHT_TASK					BREATH_LIGHT_THREAD_LIFECYCLE
#define	THREAD_PERIOD_KEY_BUTTON_TASK						KEY_THREAD_LIFECYCLE
#define	THREAD_PERIOD_MOTOR_TASK								MOTOR_THREAD_LIFECYCLE
#define	THREAD_PERIOD_WIFI_TASK									WIFI_THREAD_LIFECYCLE
#define	THREAD_PERIOD_BT_TASK										BT_THREAD_LIFECYCLE

#define POWER_ON_WAITE_TIME_TASK								MACRO_POWER_ON_WAITE_TIME_TASK

//*******************************************************


// 产品机型码
#define	SYSTEM_PRODUCT_MODEL_CODE								MACRO_SYSTEM_PRODUCT_MODEL_CODE		//

// 软件版本
#define	SOFTWARE_VERSION_UINT32									MACRO_SOFTWARE_VERSION_UINT32


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Led_Power_Pin GPIO_PIN_0
#define Led_Power_GPIO_Port GPIOC
#define Led_Mode_Pin GPIO_PIN_1
#define Led_Mode_GPIO_Port GPIOC
#define Led_Time_Pin GPIO_PIN_2
#define Led_Time_GPIO_Port GPIOC
#define Led_Speed_Pin GPIO_PIN_3
#define Led_Speed_GPIO_Port GPIOC
#define RS485_A_Pin GPIO_PIN_2
#define RS485_A_GPIO_Port GPIOA
#define RS485_B_Pin GPIO_PIN_3
#define RS485_B_GPIO_Port GPIOA
#define SW_3_Pin GPIO_PIN_4
#define SW_3_GPIO_Port GPIOA
#define ADC_CN6_Pin GPIO_PIN_5
#define ADC_CN6_GPIO_Port GPIOA
#define ADC_CN6A6_Pin GPIO_PIN_6
#define ADC_CN6A6_GPIO_Port GPIOA
#define SW_4_Pin GPIO_PIN_7
#define SW_4_GPIO_Port GPIOA
#define SW_1_Pin GPIO_PIN_4
#define SW_1_GPIO_Port GPIOC
#define SW_2_Pin GPIO_PIN_5
#define SW_2_GPIO_Port GPIOC
#define BZ_PWM_Pin GPIO_PIN_0
#define BZ_PWM_GPIO_Port GPIOB
#define ADC_TEMP_Pin GPIO_PIN_1
#define ADC_TEMP_GPIO_Port GPIOB
#define SPI1_CS_Pin GPIO_PIN_2
#define SPI1_CS_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_12
#define CS_GPIO_Port GPIOB
#define DI_Key_Power_Pin GPIO_PIN_13
#define DI_Key_Power_GPIO_Port GPIOB
#define WR_Pin GPIO_PIN_14
#define WR_GPIO_Port GPIOB
#define DATA_Pin GPIO_PIN_15
#define DATA_GPIO_Port GPIOB
#define RS485_EN_Pin GPIO_PIN_6
#define RS485_EN_GPIO_Port GPIOC
#define Key_Speed_Pin GPIO_PIN_7
#define Key_Speed_GPIO_Port GPIOC
#define Key_Time_Pin GPIO_PIN_8
#define Key_Time_GPIO_Port GPIOC
#define Key_Mode_Pin GPIO_PIN_9
#define Key_Mode_GPIO_Port GPIOC
#define Key_Power_Pin GPIO_PIN_8
#define Key_Power_GPIO_Port GPIOA
#define DI_Key_Up_Pin GPIO_PIN_11
#define DI_Key_Up_GPIO_Port GPIOA
#define DI_Key_Down_Pin GPIO_PIN_12
#define DI_Key_Down_GPIO_Port GPIOA
#define LCD_BackLight_Pin GPIO_PIN_15
#define LCD_BackLight_GPIO_Port GPIOA
#define Main_RS485_EN_Pin GPIO_PIN_3
#define Main_RS485_EN_GPIO_Port GPIOB
#define Debug_Led_02_Pin GPIO_PIN_4
#define Debug_Led_02_GPIO_Port GPIOB
#define Debug_Led_01_Pin GPIO_PIN_5
#define Debug_Led_01_GPIO_Port GPIOB
#define Debug_IO_01_Pin GPIO_PIN_8
#define Debug_IO_01_GPIO_Port GPIOB
#define Debug_IO_02_Pin GPIO_PIN_9
#define Debug_IO_02_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
