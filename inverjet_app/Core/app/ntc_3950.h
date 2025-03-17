/**
******************************************************************************
* @file    		ntc_3950.h
* @brief   		ntc_3950 温度采集
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-7-23
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NTC_3950_H__
#define __NTC_3950_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

	
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/


//-------------- NTC 传感器故障 ADC -------------------
#define FAULT_NTC_ADC_MIX										(124)
#define FAULT_NTC_ADC_MAX										(4000)


/* Exported functions prototypes ---------------------------------------------*/


float Get_Internal_Temp(void);
float Get_External_Temp(void);

/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __NTC_3950_H__ */

