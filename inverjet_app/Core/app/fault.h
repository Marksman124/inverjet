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
	FAULT_BIT_03_CURRENT_BIAS,									// 电流传感器偏置故障				E003
	FAULT_BIT_04_SHORT_CIRCUIT,									// 短路											E004
	FAULT_BIT_05_LACK_PHASE,										// 缺相											E005
	FAULT_BIT_06_LOCK_ROTOR,										// 堵转											E006
	
	FAULT_BIT_07_TEMPERATURE_MOS,								// 温度 MOS									E101
	FAULT_BIT_08_TEMPERATURE_AMBIENT,						// 温度 环境									E102
		
	FAULT_BIT_09_TEMPERATURE_SENSOR,						// 温度 传感器								E201
	FAULT_BIT_10_DRIVE_BOARD,										// 驱动板 异常								E202
	FAULT_BIT_11_DRIVE_LOSS,										// 驱动板 通信故障						E203
	FAULT_BIT_12_VOLTAGE_AMBIENT,								// 电压 传感器								E205
	
} FAULT_STATE_BIT_E;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#define FAULT_STATE_MAX										(1<<(FAULT_BIT_12_VOLTAGE_AMBIENT+1))

#define E001_BUS_VOLTAGE_ABNORMAL					(1 << FAULT_BIT_01_VOLTAGE_ABNORMAL)
#define E002_BUS_CURRENT_ABNORMAL					(1 << FAULT_BIT_02_CURRENT_ABNORMAL)
#define E003_BUS_CURRENT_BIAS							(1 << FAULT_BIT_03_CURRENT_BIAS)
#define E004_ABNORMAL_SHORT_CIRCUIT				(1 << FAULT_BIT_04_SHORT_CIRCUIT)
#define E005_LACK_PHASE										(1 << FAULT_BIT_05_LACK_PHASE)
#define E006_LOCK_ROTOR										(1 << FAULT_BIT_06_LOCK_ROTOR)
#define E101_TEMPERATURE_MOS							(1 << FAULT_BIT_07_TEMPERATURE_MOS)
#define E102_TEMPERATURE_AMBIENT					(1 << FAULT_BIT_08_TEMPERATURE_AMBIENT)
#define E201_TEMPERATURE_SENSOR						(1 << FAULT_BIT_09_TEMPERATURE_SENSOR)
#define E202_MOTOR_DRIVER									(1 << FAULT_BIT_10_DRIVE_BOARD)
#define E203_MOTOR_LOSS										(1 << FAULT_BIT_11_DRIVE_LOSS)
#define E205_VOLTAGE_AMBIENT							(1 << FAULT_BIT_12_VOLTAGE_AMBIENT)

//#define FAULT_WIFI_TEST_ERROR								0x200
//-------------- 按键组合响应 总数 -------------------
#define CALL_OUT_NUMBER_MAX						8


// 恢复故障后等待时间
#define RECOVERY_ATTEMPT_TIME							30



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

