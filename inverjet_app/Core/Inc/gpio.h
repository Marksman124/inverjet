/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

typedef struct IO_Hardware_Info
{
	GPIO_TypeDef * io_type;
	uint16_t io_pin;
}IO_Hardware_Info;

/* Private macro -------------------------------------------------------------*/

#define DEBUG_LED1_ON()				HAL_GPIO_WritePin(Debug_Led_01_GPIO_Port, Debug_Led_01_Pin, GPIO_PIN_RESET)
#define DEBUG_LED1_OFF()			HAL_GPIO_WritePin(Debug_Led_01_GPIO_Port, Debug_Led_01_Pin, GPIO_PIN_SET)

#define DEBUG_LED2_ON()				HAL_GPIO_WritePin(Debug_Led_02_GPIO_Port, Debug_Led_02_Pin, GPIO_PIN_RESET)
#define DEBUG_LED2_OFF()			HAL_GPIO_WritePin(Debug_Led_02_GPIO_Port, Debug_Led_02_Pin, GPIO_PIN_SET)

#define DEBUG_IO_01_ON()			HAL_GPIO_WritePin(Debug_IO_01_GPIO_Port, Debug_IO_01_Pin, GPIO_PIN_RESET)
#define DEBUG_IO_01_OFF()			HAL_GPIO_WritePin(Debug_IO_01_GPIO_Port, Debug_IO_01_Pin, GPIO_PIN_SET)

#define DEBUG_IO_02_ON()			HAL_GPIO_WritePin(Debug_IO_02_GPIO_Port, Debug_IO_02_Pin, GPIO_PIN_RESET)
#define DEBUG_IO_02_OFF()			HAL_GPIO_WritePin(Debug_IO_02_GPIO_Port, Debug_IO_02_Pin, GPIO_PIN_SET)

//#define FAN_SWITCH_ON()				HAL_GPIO_WritePin(Fan_Switch_GPIO_Port, Fan_Switch_Pin, GPIO_PIN_RESET)
//#define FAN_SWITCH_OFF()			HAL_GPIO_WritePin(Fan_Switch_GPIO_Port, Fan_Switch_Pin, GPIO_PIN_SET)

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
//extern void led_show(uint16_t num, uint16_t time);
//extern void led_on(void);
//extern void led_off(void);
	
extern void StartUp_Pump(uint8_t num, uint16_t para);
extern uint8_t Gpio_Get_Dial_Switch(void);
extern void IO_Hardware_Ctrl_All(uint16_t para);
extern void IO_Hardware_Ctrl_One(uint8_t num, uint8_t value);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

