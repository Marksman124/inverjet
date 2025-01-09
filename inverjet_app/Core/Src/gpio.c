/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
//#define GEAR_MAX		4

//IO_Hardware_Info IO_LED[2]	= {{GPIOB,GPIO_PIN_4},{GPIOB,GPIO_PIN_5}};	//  PB5
/*
IO_Hardware_Info IO_Pump[2][4] = {
// PB12, PB13, PB14, PB15,
{	{GPIOB,GPIO_PIN_12},{GPIOB,GPIO_PIN_13},{GPIOB,GPIO_PIN_14},{GPIOB,GPIO_PIN_15}},
// PC6, PC7, PC8, PC9,
{	{GPIOC,GPIO_PIN_6},{GPIOC,GPIO_PIN_7},{GPIOC,GPIO_PIN_8},{GPIOC,GPIO_PIN_9}}
};
*/
//IO_Hardware_Info IO_Extend[6] = {	// PA6, PA7, PC4, PC5, PB0, PB1,
//{GPIOA,GPIO_PIN_6},{GPIOA,GPIO_PIN_7},
//{GPIOC,GPIO_PIN_4},{GPIOC,GPIO_PIN_5},
//{GPIOB,GPIO_PIN_0},{GPIOB,GPIO_PIN_1}
//};


//--------------- 拨码开关  ----------------------------------
//*************  位数
//#define DIAL_SWITCH_NUMBER		2
#define DIAL_SWITCH_NUMBER		4

//*************  IO 引脚
//IO_Hardware_Info IO_Dial_Switch[DIAL_SWITCH_NUMBER]	= {{GPIOC,GPIO_PIN_4},{GPIOC,GPIO_PIN_5}};	//  PB5
IO_Hardware_Info IO_Dial_Switch[DIAL_SWITCH_NUMBER]	= {{SW_1_GPIO_Port,SW_1_Pin},{SW_2_GPIO_Port,SW_2_Pin},{SW_3_GPIO_Port,SW_3_Pin},{SW_4_GPIO_Port,SW_4_Pin}};	//  PB5

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Led_Power_Pin|Led_Mode_Pin|Led_Time_Pin|Led_Speed_Pin
                          |RS485_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI1_CS_Pin|CS_Pin|WR_Pin|DATA_Pin
                          |Debug_Led_02_Pin|Debug_Led_01_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Fan_Switch_Pin|LCD_BackLight_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin
                           PCPin */
  GPIO_InitStruct.Pin = Led_Power_Pin|Led_Mode_Pin|Led_Time_Pin|Led_Speed_Pin
                          |RS485_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = SW_3_Pin|SW_4_Pin|Key_Power_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin
                           PCPin */
  GPIO_InitStruct.Pin = SW_1_Pin|SW_2_Pin|Key_Speed_Pin|Key_Time_Pin
                          |Key_Mode_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin
                           PBPin PBPin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin|CS_Pin|WR_Pin|DATA_Pin
                          |Debug_Led_02_Pin|Debug_Led_01_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin */
  GPIO_InitStruct.Pin = Fan_Switch_Pin|LCD_BackLight_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
//void led_show(uint16_t num, uint16_t time)
//{
//	uint16_t i;
//	
//	for(i=0; i<num; i++)
//	{
//		HAL_GPIO_WritePin(IO_LED.io_type, IO_LED.io_pin, GPIO_PIN_SET);
//		HAL_Delay(time);
//		HAL_GPIO_WritePin(IO_LED.io_type, IO_LED.io_pin, GPIO_PIN_RESET);
//		HAL_Delay(time);
//	}
//}

//void led_on(uint8_t num)
//{
//	HAL_GPIO_WritePin(IO_LED.io_type, IO_LED.io_pin, GPIO_PIN_RESET);
//}


//void led_off(uint8_t num)
//{
//	HAL_GPIO_WritePin(IO_LED.io_type, IO_LED.io_pin, GPIO_PIN_SET);
//}
	

//void StartUp_Pump(uint8_t num, uint16_t para)
//{
//	if( num > 1)
//		return;
//	
//	switch(para){
//		case 0:
//			HAL_GPIO_WritePin(IO_Pump[num][0].io_type, IO_Pump[num][0].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][1].io_type, IO_Pump[num][1].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][2].io_type, IO_Pump[num][2].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][3].io_type, IO_Pump[num][3].io_pin, GPIO_PIN_RESET);
//		break;
//		case 1:
//			HAL_GPIO_WritePin(IO_Pump[num][0].io_type, IO_Pump[num][0].io_pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(IO_Pump[num][1].io_type, IO_Pump[num][1].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][2].io_type, IO_Pump[num][2].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][3].io_type, IO_Pump[num][3].io_pin, GPIO_PIN_RESET);
//		break;
//		case 2:
//			HAL_GPIO_WritePin(IO_Pump[num][0].io_type, IO_Pump[num][0].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][1].io_type, IO_Pump[num][1].io_pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(IO_Pump[num][2].io_type, IO_Pump[num][2].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][3].io_type, IO_Pump[num][3].io_pin, GPIO_PIN_RESET);
//		break;
//		case 3:
//			HAL_GPIO_WritePin(IO_Pump[num][0].io_type, IO_Pump[num][0].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][1].io_type, IO_Pump[num][1].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][2].io_type, IO_Pump[num][2].io_pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(IO_Pump[num][3].io_type, IO_Pump[num][3].io_pin, GPIO_PIN_RESET);
//		break;
//		case 4:
//			HAL_GPIO_WritePin(IO_Pump[num][0].io_type, IO_Pump[num][0].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][1].io_type, IO_Pump[num][1].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][2].io_type, IO_Pump[num][2].io_pin, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(IO_Pump[num][3].io_type, IO_Pump[num][3].io_pin, GPIO_PIN_SET);
//		break;
//		default:
//#ifdef SYSTEM_HARDWARE_DEBUG
//		if(para == 0xFF)
//		{
//			HAL_GPIO_WritePin(IO_Pump[num][0].io_type, IO_Pump[num][0].io_pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(IO_Pump[num][1].io_type, IO_Pump[num][1].io_pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(IO_Pump[num][2].io_type, IO_Pump[num][2].io_pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(IO_Pump[num][3].io_type, IO_Pump[num][3].io_pin, GPIO_PIN_SET);
//		}
//#endif
//			break;
//	}
//}

uint8_t Gpio_Get_Dial_Switch(void)
{
	uint8_t rulse=0;
	uint8_t i;
	uint8_t read_io;
	
	for(i=0; i<DIAL_SWITCH_NUMBER; i++)
	{
		read_io = HAL_GPIO_ReadPin(IO_Dial_Switch[i].io_type, IO_Dial_Switch[i].io_pin);
		
		if(read_io == 0)//低有效
			rulse |= 1<<i;
	}
	
	return rulse;
}

void IO_Hardware_Ctrl_All(uint16_t para)
{
//	uint8_t i;

//	for(i=0; i<6; i++)
//	{
//		if(para & 1<<i)
//			HAL_GPIO_WritePin(IO_Extend[i].io_type, IO_Extend[i].io_pin, GPIO_PIN_RESET);
//		else
//			HAL_GPIO_WritePin(IO_Extend[i].io_type, IO_Extend[i].io_pin, GPIO_PIN_SET);
//	}
}


void IO_Hardware_Ctrl_One(uint8_t num, uint8_t value)
{
//	if(num > 5)
//		return;
//	
//	if(value == 0)
//		HAL_GPIO_WritePin(IO_Extend[num].io_type, IO_Extend[num].io_pin, GPIO_PIN_RESET);
//	else
//		HAL_GPIO_WritePin(IO_Extend[num].io_type, IO_Extend[num].io_pin, GPIO_PIN_SET);
}

/* USER CODE END 2 */
