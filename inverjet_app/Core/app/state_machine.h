/**
******************************************************************************
* @file    		state_machine.h
* @brief   		状态机
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "macro_definition.h"				// 统一宏定义

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

//*********************************
//========== 状态机 ===============
//*********************************
typedef enum 
{
	POWER_OFF_STATUS = 0,			//	关机
	//自由模式
  FREE_MODE_INITIAL,				//	1		初始状态	
	FREE_MODE_STARTING,				//			启动中
  FREE_MODE_RUNNING,				//			运行中
	FREE_MODE_SUSPEND,				//			暂停
	FREE_MODE_STOP,						//	5		结束
	//定时模式
  TIMING_MODE_INITIAL,			//	6		初始状态
	TIMING_MODE_STARTING,			//			启动中
  TIMING_MODE_RUNNING,			//			运行中
	TIMING_MODE_SUSPEND,			//			暂停
	TIMING_MODE_STOP,					//	10	结束
	//训练模式
  TRAINING_MODE_INITIAL,		//	11	初始状态
	TRAINING_MODE_STARTING,		//			启动中
  TRAINING_MODE_RUNNING,		//			运行中
	TRAINING_MODE_SUSPEND,		//			暂停
	TRAINING_MODE_STOP,				//	15	结束
	
	// 其它状态
  OPERATION_MENU_STATUS,		//			操作菜单
	ERROR_DISPLAY_STATUS,			//			故障界面
	OTA_UPGRADE_STATUS,				//			OTA 升级
	SYSTEM_STATE_END,					//	结束
} SYSTEM_STATE_MACHINE_E;


//*********************************
//========== 训练模式 =============
//*********************************
typedef enum 
{
	SYSTEM_MODE_FREE_0 = 0,			//	自由 & 定时 模式
  SYSTEM_MODE_TRAIN_P1,				//	P1
	SYSTEM_MODE_TRAIN_P2,				//	
  SYSTEM_MODE_TRAIN_P3,				//	
	SYSTEM_MODE_TRAIN_P4,				//	
	SYSTEM_MODE_TRAIN_P5,				//	冲浪
} SYSTEM_MODE_NUM_E;


//*********************************
//========== 控制源 =============
//*********************************
typedef enum 
{
	CTRL_FROM_KEY = 0,				//	按键
	CTRL_FROM_WIFI,						//	wifi
	CTRL_FROM_BT,							//	蓝牙
	CTRL_FROM_RS485,					//	modbus 485	
} System_Ctrl_Mode_Type_enum;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/


//-------------- 特别状态 位 -------------------
#define SPECIAL_BIT_SKIP_INITIAL				0x01		//跳过 初始化   不自动倒数3s
#define SPECIAL_BIT_SKIP_STARTING				0x02		//跳过 软启动  直接进入 运行
#define SPECIAL_BIT_SPEED_CHANGE				0x04		//运行中更改转速

#define SPECIAL_BIT_SPEED_100_GEAR			0x10		//速度 100 档位
#define SPECIAL_BIT_TIME_100_GEAR				0x20		//定时 100 档位


/* Exported functions prototypes ---------------------------------------------*/

//------------------- 硬件 & 驱动 ----------------------------
// 初始化
extern void App_State_Machine_Init(void);
//------------- 模式发生切换 ------------------------------------
extern void Check_Mode_Change(uint16_t machine);
//------------------- 设置状态机  ----------------------------
extern uint8_t Set_System_State_Machine(uint8_t val);
//------------------- 获取状态机  ----------------------------
extern uint8_t Get_System_State_Machine(void);

//------------------- 设置 模式  ----------------------------
extern uint8_t Set_System_State_Mode(uint8_t val);
//------------------- 获取 模式  ----------------------------
extern uint8_t Get_System_State_Mode(void);

//------------------- 快速获取状态  ----------------------------
// 电机启动状态
extern uint8_t Motor_is_Start(void);
// 正常工作状态
extern uint8_t System_is_Normal_Operation(void);
// 初始状态
extern uint8_t System_is_Initial(void);
// 启动状态
extern uint8_t System_is_Starting(void);
// 运行状态
extern uint8_t System_is_Running(void);
// 暂停状态
extern uint8_t System_is_Pause(void);
// 结束状态
extern uint8_t System_is_Stop(void);
// 操作菜单
extern uint8_t System_is_Operation(void);
// 故障界面
extern uint8_t System_is_Error(void);
// 关机
extern uint8_t System_is_Power_Off(void);
// OTA
extern uint8_t System_is_OTA(void);
// 自由 模式
extern uint8_t System_Mode_Free(void);
// 定时 模式
extern uint8_t System_Mode_Time(void);
// 训练 模式
extern uint8_t System_Mode_Train(void);
// 冲浪 模式
extern uint8_t System_Mode_Surf(void);
//------------------- 更改状态 不改变模式  ----------------------------

// --> 初始状态 （不更改模式）
extern void Arbitrarily_To_Initial(void);
	
// --> 启动状态 （不更改模式）
extern void Arbitrarily_To_Starting(void);

// --> 运行状态 （不更改模式）
extern void Arbitrarily_To_Running(void);

// --> 暂停状态 （不更改模式）
extern void Arbitrarily_To_Pause(void);

// --> 结束状态 （不更改模式）
extern void Arbitrarily_To_Stop(void);

//------------------- 判断 状态 & 模式  ----------------------------
// 	模式 是否发生变化 
//	1:是  0:否
extern uint8_t Is_Change_System_Mode(uint16_t machine);

//------------------- 特别状态 机  ----------------------------

// 添加 特别状态 标志
extern void Special_Status_Add(uint8_t num);

// 删除 特别状态 标志
extern void Special_Status_Delete(uint8_t num);
	
// 获取 特别状态 标志
extern uint8_t Special_Status_Get(uint8_t num);

//------------------- 运行参数设置接口 ----------------------------
// 模式
extern void System_Para_Set_PMode(uint16_t pmode, System_Ctrl_Mode_Type_enum ctrler);
// 状态机
extern void System_Para_Set_Status(uint16_t status, System_Ctrl_Mode_Type_enum ctrler);
// 速度
extern void System_Para_Set_Speed(uint16_t speed, System_Ctrl_Mode_Type_enum ctrler);
// 时间
extern void System_Para_Set_Time(uint16_t time, System_Ctrl_Mode_Type_enum ctrler);
	
/* Private defines -----------------------------------------------------------*/
extern uint8_t Special_Status_Bit;

extern uint16_t *p_PMode_Now;

extern uint8_t Period_Now;

#ifdef __cplusplus
}
#endif

#endif /* __STATE_MACHINE_H__ */

