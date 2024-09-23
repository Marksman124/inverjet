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
#define USART_REC_LEN  			100  	//�����������ֽ��� 200
#define RXBUFFERSIZE   1 //�����С

#define MODBUS_RS485_TX_EN_PORT			GPIOC
#define MODBUS_RS485_TX_EN_PIN			GPIO_PIN_6

//����1
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����  	
extern uint8_t  USART1_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART1_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef huart1; //UART���
extern uint8_t aRxBuffer1[RXBUFFERSIZE];//HAL��USART����Buffer

//����2
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
extern uint8_t  USART2_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART2_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef huart2; //UART���
extern uint8_t aRxBuffer2[RXBUFFERSIZE];//HAL��USART����Buffer

//����3
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
extern uint8_t  USART3_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART3_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef huart3; //UART���
extern uint8_t aRxBuffer3[RXBUFFERSIZE];//HAL��USART����Buffer

//����4
#define EN_USART4_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
extern uint8_t  USART4_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART4_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef huart4; //UART���
extern uint8_t aRxBuffer4[RXBUFFERSIZE];     //HAL��USART����Buffer

//����4
#define EN_USART5_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
extern uint8_t  USART5_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern uint16_t USART5_RX_STA;         		//����״̬���	
extern UART_HandleTypeDef huart5; //UART���
extern uint8_t aRxBuffer5[RXBUFFERSIZE];     //HAL��USART����Buffer

/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart5;

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
//****************  ӳ�� ����
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

