/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define USART_REC_LEN  			100  	//定义最大接收字节数 200
#define RXBUFFERSIZE   1 //缓存大小

#define MODBUS_RS485_TX_EN_PORT			GPIOC
#define MODBUS_RS485_TX_EN_PIN			GPIO_PIN_6

//串口1
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收  	
extern uint8_t  USART1_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART1_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef huart1; //UART句柄
extern uint8_t aRxBuffer1[RXBUFFERSIZE];//HAL库USART接收Buffer

//串口2
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
extern uint8_t  USART2_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART2_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef huart2; //UART句柄
extern uint8_t aRxBuffer2[RXBUFFERSIZE];//HAL库USART接收Buffer

//串口3
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收
extern uint8_t  USART3_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART3_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef huart3; //UART句柄
extern uint8_t aRxBuffer3[RXBUFFERSIZE];//HAL库USART接收Buffer

//串口4
#define EN_USART4_RX 			1		//使能（1）/禁止（0）串口1接收
extern uint8_t  USART4_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART4_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef huart4; //UART句柄
extern uint8_t aRxBuffer4[RXBUFFERSIZE];     //HAL库USART接收Buffer

//串口4
#define EN_USART5_RX 			1		//使能（1）/禁止（0）串口1接收
extern uint8_t  USART5_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART5_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef huart5; //UART句柄
extern uint8_t aRxBuffer5[RXBUFFERSIZE];     //HAL库USART接收Buffer

/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart5;

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
//****************  映射 参数
extern uint16_t* p_Modbus_BaudRate;
extern uint32_t Modbus_BaudRate_Table[];

/* USER CODE END Private defines */

void MX_UART4_Init(void);
void MX_UART5_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

