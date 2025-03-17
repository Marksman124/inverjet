/**
******************************************************************************
* @file    		key.h
* @brief   		Key function implementation
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEY_H__
#define __KEY_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "display.h"			// 显示模块
#include "gpio.h"
#include "usart.h"				// 串口
#include "timing.h"				// 后台定时器
#include "Breath_light.h"
#include "operation.h"		// 操作 菜单
#include "fault.h"				// 故障 菜单

#include "wifi_thread.h"				// wifi 模组
#include "bluetooth.h"				// bluetooth 模组
#include "macro_definition.h"				// 统一宏定义
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef struct IO_Hardware_Pin
{
	GPIO_TypeDef * port;
	uint16_t pin;
	
}IO_Hardware_Pin;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#ifndef __MACRO_DEFINITION_H__

#define KEY_THREAD_LIFECYCLE								20				// 任务生命周期 200ms

#define KEY_LONG_PRESS_TIME_3S									(2000/KEY_THREAD_LIFECYCLE)			//长按时间 3s
#define KEY_LONG_PRESS_TIME_2S									(2000/KEY_THREAD_LIFECYCLE)			//长按时间 2s
#define KEY_LONG_PRESS_TIME_1S									(1000/KEY_THREAD_LIFECYCLE)			//短一点的 长按时间  1s

#define KEY_FOR_SLEEP_TIME_SHORT						(3000/KEY_THREAD_LIFECYCLE)			//????  5 min  300000

//-------------- 蜂鸣器 长度 -------------------
//******************  调试模式 **************************
//-------------- 蜂鸣器 长度 -------------------
#define KEY_BUZZER_TIME								(200/KEY_THREAD_LIFECYCLE)					//周期  KEY_THREAD_LIFECYCLE 倍数
#define KEY_BUZZER_TIME_LONG					(400/KEY_THREAD_LIFECYCLE)					//周期  KEY_THREAD_LIFECYCLE 倍数
#define KEY_BUZZER_TIME_LONG_32				(800/KEY_THREAD_LIFECYCLE)					//周期  KEY_THREAD_LIFECYCLE 倍数


#endif

//-------------- 按键组合响应 总数 -------------------
#define KEY_CALL_OUT_NUMBER_MAX						11

#define KEY_VALUE_BIT_BUTTON_1						0x01
#define KEY_VALUE_BIT_BUTTON_2						0x02
#define KEY_VALUE_BIT_BUTTON_3						0x04
#define KEY_VALUE_BIT_BUTTON_4						0x08

#define KEY_VALUE_BIT_BUTTON_5						0x10
#define KEY_VALUE_BIT_BUTTON_6						0x20
#define KEY_VALUE_BIT_BUTTON_7						0x40
/* Exported functions prototypes ---------------------------------------------*/

//------------------- 按键回调 ----------------------------
//--------------- 短按 -----------------------
// ① 档位键
extern void on_pushButton_clicked(void);
// ② 时间键
extern void on_pushButton_2_clicked(void);
// ③ 模式键
extern void on_pushButton_3_clicked(void);
// ④ 开机键  短按
extern void on_pushButton_4_Short_Press(void);
// ① + ②  组合键  测试 设置故障
extern void on_pushButton_1_2_Short_Press(void);
// ① + ③  组合键  短按   切换档位 80级 or 5级
extern void on_pushButton_1_3_Short_Press(void);
// ② + ③  组合键  短按
extern void on_pushButton_2_3_Short_Press(void);
// ② + ④  组合键  短按
extern void on_pushButton_2_4_Short_Press(void);

/**********************************************************************************************
*
*						按键回调    短按  拓展键
*
**********************************************************************************************/
//==================================  +  键
void on_DiButton_Add_clicked(void);
//==================================   - 键
void on_DiButton_Minus_clicked(void);

//--------------- 长按 -----------------------
// 长按
extern void on_pushButton_1_Long_Press(void);
extern void on_pushButton_2_Long_Press(void);
extern void on_pushButton_3_Long_Press(void);
extern void on_pushButton_4_Long_Press(void);
extern void on_pushButton_1_2_Long_Press(void);
extern void on_pushButton_1_3_Long_Press(void);
extern void on_pushButton_2_3_Long_Press(void);
extern void on_pushButton_2_4_Long_Press(void);


extern void on_pushButton_NULL_Press(void);
//------------------- 硬件 & 驱动 ----------------------------
// 初始化
extern void App_Key_Init(void);
// 按键灯
extern void Led_Button_On(uint8_t para);
// 按键主循环任务
extern void App_Key_Handler(void);
// 获取按键
extern uint8_t Key_Get_IO_Input(void);
//------------------- 功能接口 ----------------------------
//	开机 进入自由模式
extern void System_Power_On(void);
//	关机
extern void System_Power_Off(void);
//	开机画面
extern void System_Boot_Screens(void);
//	恢复出厂设置
extern void Restore_Factory_Settings(void);
//	OTA
extern uint8_t System_To_OTA(void);
/* Private defines -----------------------------------------------------------*/



#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

