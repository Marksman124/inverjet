/**
******************************************************************************
* @file    		control_interface.h
* @brief   		control_interface 控制接口, wifi,蓝牙直接控制
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONTROL_INTERFACE_H__
#define __CONTROL_INTERFACE_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "state_machine.h"

#include "wifi_thread.h"							// wifi 模组
#include "bluetooth.h"				// bluetooth 模组
#include "display.h"					// 显示 模块

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/


//------------------- 配网控制 遥控 & wifi ----------------------------
extern void Ctrl_GetIn_Distribution(uint16_t para);
//------------------- 设置系统工作 模式 ----------------------------
//		高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
extern void Ctrl_Set_System_Mode(uint16_t para);
//------------------- 设置系统工作 状态 ----------------------------
//		0:暂停,   1:暂停恢复,   2:重新开始,  3:结束
extern void Ctrl_Set_System_Status(uint16_t para);
//------------------- 设置系统 电源  ----------------------------
//		0：关机  1：开机
extern void Ctrl_Set_System_PowerOn(uint16_t para);
//------------------- 设置 当前转速 (临时有效)----------------------------
extern void Ctrl_Set_Motor_Current_Speed(uint16_t para);
//------------------- 设置 当前时间 (临时有效)----------------------------
extern void Ctrl_Set_Motor_Current_Time(uint16_t para);
/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __CONTROL_INTERFACE_H__ */

