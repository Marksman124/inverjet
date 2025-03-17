/**
******************************************************************************
* @file    		Breath_light.h
* @brief   		呼吸灯模块   独立线程
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

#include "macro_definition.h"				// 统一宏定义

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

// 蜂鸣器长度
typedef enum
{
	NO_BUZZER_TIME = 0,				//	无
	CLICK_BUZZER_TIME,				//	点击
	LONG_BUZZER_TIME,					//	长按
	DOUBLE_LONG_BUZZER_TIME,	//	超长按
} Buzzer_Time_Type_enum;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#ifndef __MACRO_DEFINITION_H__

#define BREATH_LIGHT_THREAD_LIFECYCLE				20				// ms

#endif

/* Exported functions prototypes ---------------------------------------------*/

extern void App_Breath_light_Init(void);
extern void App_Breath_light_Handler(void);

//------------------- 模拟调节亮度 ----------------------------

void Breath_light_Max(void);

void Breath_light_Off(void);
//------------------- 蜂鸣器 ----------------------------

void Buzzer_Click_On(void);

void Buzzer_Click_Long_On(uint8_t type);

void Buzzer_Click_Handler(void);

/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

