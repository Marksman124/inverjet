/**
******************************************************************************
* @file    		subsystem.c
* @brief   		子系统查询
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SUBSYSTEM_H__
#define __SUBSYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


extern void Subsystem_Init(void);
extern void Subsystem_RxData(uint8_t len);
extern void Subsystem_Handler_Task(void);



#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

