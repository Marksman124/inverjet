/**
******************************************************************************
* @file    		fault.h
* @brief   		故障模块s
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FAULT_H__
#define __FAULT_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "usart.h"
#include "state_machine.h"
#include "data.h"
#include "dev.h"
#include "modbus.h"
#include "macro_definition.h"				// 统一宏定义
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	FAULT_BIT_01_VOLTAGE_ABNORMAL = 0,					// 电压 异常									E001
	FAULT_BIT_02_CURRENT_ABNORMAL,							// 电流 异常									E002
	FAULT_BIT_03_CURRENT_BIAS,									// 电流传感器偏置故障					E003
	FAULT_BIT_04_SHORT_CIRCUIT,									// 短路												E004
	FAULT_BIT_05_LACK_PHASE,										// 缺相												E005
	FAULT_BIT_06_LOCK_ROTOR,										// 堵转												E006
	
	FAULT_BIT_07_TEMPERATURE_MOS,								// 温度 MOS										E101
	FAULT_BIT_08_TEMPERATURE_AMBIENT,						// 温度 环境									E102
		
	FAULT_BIT_09_TEMPERATURE_HARDWARE,					// 温度 (硬件故障)						E201
	FAULT_BIT_10_DRIVE_BOARD,										// 驱动板 异常								E202
	FAULT_BIT_11_DRIVE_LOSS,										// 驱动板 通信故障						E203
	FAULT_BIT_12_IDLING_ERROR,									// 空转 故障									E204
	
	FAULT_BIT_13_WIFI_HARDWARE,									// WIFI 模组 (硬件故障)				E301
	FAULT_BIT_14_BT_HARDWARE,										// BT 模组 (硬件故障)					E302
	FAULT_BIT_15_RS485_HARDWARE,								// RS485 模组 (硬件故障)			E303
	
} FAULT_STATE_BIT_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define FAULT_STATE_MAX										(1<<(FAULT_BIT_15_RS485_HARDWARE+1))

#define E001_BUS_VOLTAGE_ABNORMAL					(1 << FAULT_BIT_01_VOLTAGE_ABNORMAL)			// 母线电压异常
#define E002_BUS_CURRENT_ABNORMAL					(1 << FAULT_BIT_02_CURRENT_ABNORMAL)			// 过流故障
#define E003_BUS_CURRENT_BIAS							(1 << FAULT_BIT_03_CURRENT_BIAS)					// 电流不平衡
#define E004_ABNORMAL_SHORT_CIRCUIT				(1 << FAULT_BIT_04_SHORT_CIRCUIT)					// 输出短路
#define E005_LACK_PHASE										(1 << FAULT_BIT_05_LACK_PHASE)						// 缺相
#define E006_LOCK_ROTOR										(1 << FAULT_BIT_06_LOCK_ROTOR)						// 堵转
#define E101_TEMPERATURE_MOS							(1 << FAULT_BIT_07_TEMPERATURE_MOS)				// MOS温度过高
#define E102_TEMPERATURE_AMBIENT					(1 << FAULT_BIT_08_TEMPERATURE_AMBIENT)		// 机箱温度过高
#define E201_TEMPERATURE_HARDWARE					(1 << FAULT_BIT_09_TEMPERATURE_HARDWARE)	// 温度传感器故障
#define E202_MOTOR_DRIVER									(1 << FAULT_BIT_10_DRIVE_BOARD)						// 电机驱动故障 (其它故障)
#define E203_MOTOR_LOSS										(1 << FAULT_BIT_11_DRIVE_LOSS)						// 驱动板通讯故障
#define E204_IDLING_ERROR									(1 << FAULT_BIT_12_IDLING_ERROR)					// 空转 故障
#define E301_WIFI_HARDWARE								(1 << FAULT_BIT_13_WIFI_HARDWARE)
#define E302_BT_HARDWARE									(1 << FAULT_BIT_14_BT_HARDWARE)
#define E303_RS485_HARDWARE								(1 << FAULT_BIT_15_RS485_HARDWARE)


//-------------- 按键组合响应 总数 -------------------
#define CALL_OUT_NUMBER_MAX						8


// 恢复故障后等待时间
#define RECOVERY_ATTEMPT_TIME							30


#define ORDINARY_FAULT_BIT  				(E001_BUS_VOLTAGE_ABNORMAL | E201_TEMPERATURE_HARDWARE | E203_MOTOR_LOSS)

#define TEMP001_RESTAR_FAULT_BIT  	(TEMP001_MOTOR_FAULT_BUS_VOLTAGE_ERROR | TEMP001_MOTOR_FAULT_BUS_CIRCUIT_BIAS | TEMP001_MOTOR_FAULT_COMM_FAULT| TEMP001_MOTOR_FAULT_IDLING_ERROR| TEMP001_MOTOR_FAULT_MOSFET_TEMP_HARDWARE| TEMP001_MOTOR_FAULT_BRIDGE_HARDWARE)
//extern uint8_t Fault_Recovery_Attempt_cnt;				//

/* Exported functions prototypes ---------------------------------------------*/
// 初始化
extern void App_Fault_Init(void);
// 检查 故障状态 是否合法
extern uint8_t Fault_Check_Status_Legal(uint16_t parameter);
// 检测故障
extern uint8_t If_System_Is_Error(void);
// 设置故障值
void Set_Fault_Data(uint16_t type);

// 接口检测
void Clean_Comm_Test(void);
void Self_Testing_Check_Comm(void);
//-------------------   ----------------------------

// 获取总数
extern uint8_t Get_Fault_Number_Sum(uint16_t para);
// 获取故障号
extern uint8_t Get_Fault_Number_Now(uint16_t para, uint8_t num);
// 进入操作菜单
extern void To_Fault_Menu(void);
// 故障界面 更新
void Update_Fault_Menu(void);
// 清除故障状态
extern void Clean_Fault_State(void);
// 故障 显示
extern void Lcd_Fault_Display(uint8_t sum, uint8_t now, uint16_t type);

/* Private defines -----------------------------------------------------------*/

// 短按 槽函数
extern void (*p_Fault_Button[CALL_OUT_NUMBER_MAX])(void);

// 长按 槽函数
extern void (*p_Fault_Long_Press[CALL_OUT_NUMBER_MAX])(void);



#ifdef __cplusplus
}
#endif

#endif /* __FAULT_H__ */

