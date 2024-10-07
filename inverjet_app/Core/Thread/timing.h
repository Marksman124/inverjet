/**
******************************************************************************
* @file    		timing.h
* @brief   		timing function implementation
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMING_H__
#define __TIMING_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "display.h"			// 显示模块
#include "data.h"

#include "fault.h"				// 故障 菜单
#include "macro_definition.h"				// 统一宏定义

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	TIMING_STATE_SUSPEND,				//	暂停
	TIMING_STATE_STARTING,			//	软启动
	TIMING_STATE_RUNNING,				//	运行
	
} SYSTEM_STATE_TIMING_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#ifndef __MACRO_DEFINITION_H__
#define TIMING_THREAD_LIFECYCLE				500				// ms  1000

//******************  调试模式 **************************
#ifdef SYSTEM_DEBUG_MODE
//配网时长
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(6*(1000/TIMING_THREAD_LIFECYCLE))				// 6 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(6*(1000/TIMING_THREAD_LIFECYCLE))				// 6 s
//故障自恢复
#define SYSTEM_FAULT_TIME_CALLOUT							(20*(1000/TIMING_THREAD_LIFECYCLE))				// 20 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 次故障
#define SYSTEM_FAULT_RECOVERY_TIME						(60*(1000/TIMING_THREAD_LIFECYCLE))				// 1 分钟  60 s
//自动关机
#define AUTOMATIC_SHUTDOWN_TIME								(600*(1000/TIMING_THREAD_LIFECYCLE))				// 10 min

#else
//配网时长
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(60*(1000/TIMING_THREAD_LIFECYCLE))				// 60 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(60*(1000/TIMING_THREAD_LIFECYCLE))				// 60 s
//故障自恢复
#define SYSTEM_FAULT_TIME_CALLOUT							(120*(1000/TIMING_THREAD_LIFECYCLE))				// 120 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 次故障
#define SYSTEM_FAULT_RECOVERY_TIME						(3600*(1000/TIMING_THREAD_LIFECYCLE))				// 1 小时内  3600 s
//自动关机
#define AUTOMATIC_SHUTDOWN_TIME								(3600*(1000/TIMING_THREAD_LIFECYCLE))				// 1 小时内  3600 s

#endif
//自动关机 时间

//-------------- 降速检查时间 -------------------
#define TIME_SLOW_DOWN_TIME													120		//2 min  120 sec
//-------------- 降速 档位 -------------------
#define TIME_SLOW_DOWN_SPEED_01											10		//第一档 降速
#define TIME_SLOW_DOWN_SPEED_02											5			//第二档 降速
//-------------- 降速 最低速度 -------------------
#define TIME_SLOW_DOWN_SPEED_MIX										20		//最低降到 20%
#define TIME_SLOW_DOWN_SPEED_MAX										100		//恢复速度最高恢复到 100%
//*******************************************************
#endif
/* Exported functions prototypes ---------------------------------------------*/

extern void App_Timing_Init(void);

extern void Clean_Timing_Timer_Cnt(void);
// 获取刷新标志
extern void LCD_Refresh_Restore(void);
// 定时任务主线程
extern void App_Timing_Handler(void);

//-------------------- 设置高温降速模式 0:关   1开 ----------------------------
void Set_Temp_Slow_Down_State(uint8_t vaule);
//-------------------- 获取高温降速模式 ----------------------------
uint8_t Get_Temp_Slow_Down_State(void);
//-------------------- 设置高温降速 速度 下降的速度 ----------------------------
void Set_Temp_Slow_Down_Speed(uint8_t vaule);
//-------------------- 获取高温降速 速度 ----------------------------
uint8_t Get_Temp_Slow_Down_Speed(void);

void CallOut_Fault_State(void);
//-------------------- 清除故障恢复计数器 ----------------------------
void Clean_Fault_Recovery_Cnt(void);
//-------------------- 累计故障恢复计数器 ----------------------------
void Add_Fault_Recovery_Cnt(void);
//-------------------- 超过最大次数 ----------------------------
uint8_t If_Fault_Recovery_Max(void);

//-------------------- 清除 自动关机计时器 ----------------------------
void Clean_Automatic_Shutdown_Timer(void);
//-------------------- 清除 更改速度计时器 ----------------------------
void Clean_Change_Speed_Timer(void);


/* Private defines -----------------------------------------------------------*/
//电机电流低
extern uint16_t Check_Motor_Current_Cnt;
//电机转速不符
extern uint16_t Check_Motor_Speed_Cnt;

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

