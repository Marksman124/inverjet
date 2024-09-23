/**
******************************************************************************
* @file    		Breath_light.h
* @brief   		������ģ��   �����߳�
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BREATH_LIGHT_H__
#define __BREATH_LIGHT_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "tim.h"
#include "display.h"

#include "macro_definition.h"				// ͳһ�궨��

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#ifndef __MACRO_DEFINITION_H__

#define BREATH_LIGHT_THREAD_LIFECYCLE				20				// ms

#endif

/* Exported functions prototypes ---------------------------------------------*/

extern void App_Breath_light_Init(void);
extern void App_Breath_light_Handler(void);

//------------------- ģ��������� ----------------------------

void Breath_light_Max(void);

void Breath_light_Off(void);

/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

