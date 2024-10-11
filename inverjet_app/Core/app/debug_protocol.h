/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    debug_protocol.h
  * @brief   This file contains all the function prototypes for
  *          the debug_protocol.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEBUG_PROTOCOL_H__
#define __DEBUG_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "modbus.h"
#include <stdio.h>
#include "macro_definition.h"				// 统一宏定义

/* Exported macro ------------------------------------------------------------*/
#define DEBUG_PROTOCOL_RX_MAX			16
#define DEBUG_PROTOCOL_TX_MAX			512

extern uint8_t debug_buffer[DEBUG_PROTOCOL_TX_MAX];

extern uint8_t Debug_Read_Buffer[DEBUG_PROTOCOL_RX_MAX];

#ifdef UART_DEBUG_SEND_CTRL
extern uint8_t Chassis_Temperature_Debug;
#endif
	
extern uint32_t Print_Flag_Position;

/* Exported functions prototypes ---------------------------------------------*/

void Add_Ctrl_Log(void);

void UART_Send_Debug(uint8_t * p_buff, uint8_t len);

void To_Debug_Protocol_Analysis(uint8_t len);

void Debug_Protocol_Init(void);
// 重启
void Debug_Usart_Restar(void);
	
	
	
	
// 定义DEBUG_PRINT宏，用于条件编译调试信息
#ifdef UART_PRINTF_LOG
  #define DEBUG_PRINT(format, ...) 	sprintf((char *)debug_buffer,format, __VA_ARGS__);UART_Send_Debug(debug_buffer,strlen((char *)debug_buffer))
#else
  #define DEBUG_PRINT(format, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_PROTOCOL_H__ */

