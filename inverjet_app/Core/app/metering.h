/**
******************************************************************************
* @file    		metering.h
* @brief   		计量模块通讯协议
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __METERING_H__
#define __METERING_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include "usart.h"
#include "modbus.h"
#include "mbcrc.h"
#include "cmsis_os.h"

/* Private define ------------------------------------------------------------*/

/*------------------- IO define ----------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

//#define RS48501_RE_Pin GPIO_PIN_8
//#define RS48501_RE_GPIO_Port GPIOA
//#define RS48504_RE_Pin GPIO_PIN_12
//#define RS48504_RE_GPIO_Port GPIOA


#if (METERING_MODULE_HUART == 1)
#define METERING_RS485_TX_EN_PORT		RS48501_RE_GPIO_Port
#define METERING_RS485_TX_EN_PIN			RS48501_RE_Pin
#elif (METERING_MODULE_HUART == 4)
#define METERING_RS485_TX_EN_PORT		RS48504_RE_GPIO_Port
#define METERING_RS485_TX_EN_PIN			RS48504_RE_Pin
#endif

#define METERING_RS485_TX_BUFF_SIZE			8
#define METERING_RS485_RX_BUFF_SIZE			100

/*------------------- modbus dataAddr  ---------------------------------------*/
#define METERING_ADDR_START 							( 0x0010 )
#define METERING_ADDR_END 								( 0x002F )

#define METERING_ADDR_BAUDRATE 						( 0x002D )


/*------------------- metering dataAddr  ---------------------------------------*/
#define METERING_ADDR_MAX 								( 247 )
#define MB_METERING_ADDR_DEFAULT					( 10 )
/*------------------- used type  ---------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
  NO_MODE = 0,
	SINGLE_PHASE,
  THREE_PHASE_THREE_WIRE,
	THREE_PHASE_FOUR_WIRE,
} CONNECT_MODE_E;



typedef union FloatUint
{
	float f;
	unsigned char c[4];
}FloatUint;


typedef union IntUint
{
	int i;
	uint32_t c;
}IntUint;

/* Exported constants --------------------------------------------------------*/


//**************** 收发缓冲区
extern uint8_t Metering_DMABuff[METERING_RS485_RX_BUFF_SIZE];//定义一个接收缓存区
//extern uint8_t Metering_RxBuff[METERING_RS485_RX_BUFF_SIZE];//定义一个正真的接收数组
extern uint8_t Metering_TxBuff[METERING_RS485_TX_BUFF_SIZE];//定义一个发送缓存区


extern osMessageQId MeteringQueueHandle;



extern void Metering_Receive_Init(void);

extern void Metering_RxData(uint8_t len);

extern void Metering_Protocol_Analysis(void);

extern uint8_t Check_Connection_Mode(uint16_t* p_mode);
extern uint8_t Check_Metering_Address(uint16_t* p_addr);


#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

