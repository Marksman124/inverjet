/**
******************************************************************************
* @file    		down_conversion.h
* @brief   		降频控制
*
*
* @author			WQG
* @versions   v1.0
* @date   		2025-2-21
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DOWN_CONVESION_H__
#define __DOWN_CONVESION_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

//#include "display.h"			// 显示模块
#include "macro_definition.h"				// 统一宏定义

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

// 降频 类型
typedef enum
{
	Down_Conversion_Off = 0,					//	无
	Down_Conversion_Mos_Temper,				//	mos 高温
	Down_Conversion_Box_Temper,				//	机箱 高温
	Down_Conversion_Out_Current,			//	输出 电流
	Down_Conversion_Out_Power,				//	输出 功率
} Down_Conversion_Type_enum;


// 降频 频率
typedef enum
{
	Down_Conversion_Rate_Off = 0,						//	无
	Down_Conversion_Rate_Low,								//	慢速						1%/2min
	Down_Conversion_Rate_Normal,						//	正常速度					1%/min
	Down_Conversion_Rate_High,							//	快速						1%/30s
	Down_Conversion_Rate_Realtime,					//	异常快速					1%/10s
} Down_Conversion_Rate_enum;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------  降频频率 依据 -------------
// MOS 温度 ±1°C  steady
//			#define MOS_TEMPER_RATE_STABLE					(15 * 60)				//	15min 	稳定
#define MOS_TEMPER_RATE_LOW							(10 * 60)				//	10min 	慢速
#define MOS_TEMPER_RATE_NORMAL					( 5 * 60)				//	5min		正常速度
#define MOS_TEMPER_RATE_HIGH						( 2 * 60)				//	2min		快速
#define MOS_TEMPER_RATE_REALTIME				( 1 * 60)				//	1min		异常快速

// BOX 温度 ±1°C
//			#define BOX_TEMPER_RATE_STABLE					(15 * 60)				//	15min 	稳定
#define BOX_TEMPER_RATE_LOW							(10 * 60)				//	10min 	慢速
#define BOX_TEMPER_RATE_NORMAL					( 5 * 60)				//	5min		正常速度
#define BOX_TEMPER_RATE_HIGH						( 2 * 60)				//	2min		快速
#define BOX_TEMPER_RATE_REALTIME				( 1 * 60)				//	1min		异常快速

// 输出 电流 ±2A
//			#define OUT_CURRENT_RATE_STABLE					(15 * 60)				//	15min 	稳定
#define OUT_CURRENT_RATE_LOW						(2 * 60)				//	2min 	慢速
#define OUT_CURRENT_RATE_NORMAL					( 60 )					//	1min		正常速度
#define OUT_CURRENT_RATE_HIGH						( 30 )					//	30S		快速
#define OUT_CURRENT_RATE_REALTIME				( 10 )					//	10s		异常快速


// 降频 频率  N 秒每 1%
#define DOWN_CONVERSION_RATE_LOW						(120)
#define DOWN_CONVERSION_RATE_NORMAL					( 60)
#define DOWN_CONVERSION_RATE_HIGH						( 30)
#define DOWN_CONVERSION_RATE_REALTIME				( 10)


// 回升 速度 秒/1%
#define REBOUND_FREQUENCHY_TIME_LOW						(120)		//	2min 	慢速
#define REBOUND_FREQUENCHY_TIME_HIGH					(30)		//	30s 	快速


//公式
#define GET_MAX_FROM_3(x, y, z) ((x) > (y) ? ((x) > (z) ? (x) : (z)) : ((y) > (z) ? (y) : (z)))

/* Exported functions prototypes ---------------------------------------------*/
// 清除 所有 降频 状态
extern void Clean_All_Down_Conversion_Status(void);


//-------------------- 高温降速  mos ----------------------------
extern void Check_Down_Conversion_MOS_Temperature(short int Temperature);
//-------------------- 高温降频  机箱 ----------------------------
extern void Check_Down_Conversion_BOX_Temperature(short int Temperature);
//-------------------- 功率降频   ----------------------------
extern void Check_Down_Conversion_Motor_Power(uint16_t power);
//-------------------- 电流降频   ----------------------------
extern void Check_Down_Conversion_Motor_Current(uint32_t Current);

//-------------------- 获取降频次数 ----------------------------
uint8_t Get_Down_Conversion_Time(void);
//-------------------- 获取降频前速度 ----------------------------
uint8_t Get_Down_Conversion_Speed_Old(void);
//-------------------- 设置降频前速度 ----------------------------
void Set_Down_Conversion_Speed_Old(uint8_t vaule);
//-------------------- 获取降频现在速度 ----------------------------
uint8_t Get_Down_Conversion_Speed_Now(void);

//-------------------- 获取高温降速模式 ----------------------------
extern uint8_t Get_Temp_Slow_Down_State(void);


// 降频任务主线程 	每秒进一次
extern void Down_Conversion_Handler(void);

/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __DOWN_CONVESION_H__ */

