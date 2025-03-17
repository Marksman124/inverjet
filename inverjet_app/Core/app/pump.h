/**
******************************************************************************
* @file    		pump.h
* @brief   		水泵控制模块
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PUMP_H__
#define __PUMP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* USER CODE BEGIN Private defines */
extern uint16_t* p_Gear_Pump_1;
extern uint16_t* p_Gear_Pump_2;

extern uint16_t* p_Current_Pump_1;
extern uint16_t* p_Current_Pump_2;

extern uint16_t* p_Voltage_Pump_1;
extern uint16_t* p_Voltage_Pump_2;

extern uint16_t Gear_Pump_1;
extern uint16_t Gear_Pump_2;

extern uint16_t Current_Pump_1;
extern uint16_t Current_Pump_2;

extern uint16_t Voltage_Pump_1;
extern uint16_t Voltage_Pump_2;


/* USER CODE END Private defines */
#define AGREEMENT_GEAR_PUMP_MAX							( 4 )
#define AGREEMENT_CURRENT_PUMP_MAX					( 100 )
#define AGREEMENT_VOLTAGE_PUMP_MAX					( 4095 )
/* USER CODE BEGIN Prototypes */


/************************************************/
extern void Call_PUMP_Handle_Init(void);
extern void Call_PUMP_Handle_Task(void);

/************* 测试用 ***************************/
extern void PUMP_Hardware_Debug(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __PUMP_H__ */

