/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dev.h
  * @brief   This file contains all the function prototypes for
  *          the dev.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEV_H__
#define __DEV_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define __IO    volatile 
typedef __IO uint16_t vu16;

#define USER_FLASH_ADDR_START				FLASH_APP_PARAM_ADDR
#define USER_FLASH_ADDR_END					FLASH_APP_PROGRAM_ADDR

#define MODBUS_LOCAL_ADDRESS_MAX			(254)
#define MODBUS_LOCAL_ADDRESS_MIX			(1)


#define MODBUS_LOCAL_ADDRESS			(21)
#define MODBUS_BAUDRATE_DEFAULT			2

extern uint16_t* p_Local_Address;			//	���ص�ַ
extern uint16_t* p_Baud_Rate;					//	������
extern uint16_t* p_Software_Version_high;		//	����汾
extern uint16_t* p_Software_Version_low;		//	����汾 ��

uint32_t Dev_BaudRate_Get(uint8_t usart_num);
extern void Dev_Information_Init(void);
extern uint16_t Read_Local_Address(void);
extern uint16_t Read_Baud_Rate(void);
extern uint32_t Read_Software_Version(void);
extern void Set_Local_Address(uint16_t addr);
extern void Set_Baud_Rate(uint16_t rate);
extern void Set_Software_Version(void);



#ifdef __cplusplus
}
#endif

#endif /* __DEV_H__ */

