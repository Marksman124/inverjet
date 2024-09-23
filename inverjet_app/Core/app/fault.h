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
	NO_FAULT_STATE = 0,			//	操作菜单
	ss
} FAULT_STATE_MODE_E;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#define FAULT_BUS_VOLTAGE_ABNORMAL					0x1
#define FAULT_BUS_CURRENT_ABNORMAL					0x2
#define FAULT_BUS_CURRENT_BIAS							0x4
#define FAULT_ABNORMAL_OUTPUT_VOLTAGE				0x8
#define FAULT_LACK_PHASE										0x10	//缺相
#define FAULT_TEMPERATURE_AMBIENT						0x20
#define FAULT_TEMPERATURE_SENSOR						0x40
#define FAULT_MOTOR_DRIVER									0x80
#define FAULT_MOTOR_LOSS										0x100

#define FAULT_WIFI_TEST_ERROR								0x200
//-------------- 按键组合响应 总数 -------------------
#define CALL_OUT_NUMBER_MAX						8


// 恢复故障后等待时间
#define RECOVERY_ATTEMPT_TIME							30


//extern uint8_t Fault_Recovery_Attempt_cnt;				//

/* Exported functions prototypes ---------------------------------------------*/
// 初始化
extern void App_Fault_Init(void);
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

