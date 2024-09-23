/**
******************************************************************************
* @file    		dmx512.h
* @brief   		灯板 通讯协议
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-8
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DMX512_H__
#define __DMX512_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


/* Private define ------------------------------------------------------------*/

#if (DMX512_HUART == 1)
#define DMX512_RS485_EN_PORT		RS48501_RE_GPIO_Port
#define DMX512_RS485_EN_PIN			RS48501_RE_Pin

#define DMX512_RS485_TX_PORT		GPIOA
#define DMX512_RS485_TX_PIN			GPIO_PIN_9

#elif (DMX512_HUART == 3)
#define DMX512_RS485_TX_PORT		GPIOB
#define DMX512_RS485_TX_PIN			GPIO_PIN_10

#elif (DMX512_HUART == 4)
#define DMX512_RS485_EN_PORT		RS48504_RE_GPIO_Port
#define DMX512_RS485_EN_PIN			RS48504_RE_Pin

#define DMX512_RS485_TX_PORT		GPIOC
#define DMX512_RS485_TX_PIN			GPIO_PIN_10

#elif (DMX512_HUART == 5)
#define DMX512_RS485_TX_PORT		GPIOC
#define DMX512_RS485_TX_PIN			GPIO_PIN_12
#endif

#define DMX512_RS485_TX_BUFF_SIZE			512
//#define DMX512_RS485_RX_BUFF_SIZE			512




extern void Dmx512_Receive_Init(void);
extern void Dmx512_Protocol_Analysis(void);


extern void Dmx512_Hardware_Debug(void);


#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

