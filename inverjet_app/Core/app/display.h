/**
******************************************************************************
* @file    		display.h
* @brief   		显示模块  显示模块本身不占用线程,由其它线程任务调用
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISPLAY_H__
#define __DISPLAY_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "usart.h"
#include "state_machine.h"
#include "data.h"
#include "modbus.h"
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- 图标显示标准位 -------------------
#define STATUS_BIT_BLUETOOTH			1
#define STATUS_BIT_PERCENTAGE			2
#define STATUS_BIT_WIFI						4
#define STATUS_BIT_COLON					8
#define STATUS_BIT_POINT					16

//#define STATUS_BIT_P1							32
//#define STATUS_BIT_P2							64
//#define STATUS_BIT_P3							128

#define LCD_SYMBOL_FOT_FAULT								(STATUS_BIT_BLUETOOTH + STATUS_BIT_WIFI)

/* Exported functions prototypes ---------------------------------------------*/

//------------------- 硬件 & 驱动 ----------------------------
// 初始化
extern void App_Display_Init(void);
//------------------- 显示屏 & 接口 ----------------------------
/*
******************************************************************************
Display_Show_Speed	

显示速度， 0-100
******************************************************************************
*/  
extern void Display_Show_Speed(uint8_t speed);
/*
******************************************************************************
Display_Show_Min	

显示分钟， 0-99
******************************************************************************
*/  
extern void Display_Show_Min(uint8_t min);
/*
******************************************************************************
Display_Show_Speed	

显示速度， 0-100
******************************************************************************
*/  
extern void Display_Show_Sec(uint8_t sec);
/*
******************************************************************************
Display_Show_Mode	

显示模式， P1~P3
******************************************************************************
*/  
extern void Display_Show_Mode(uint8_t mode);

void Display_Hide_Speed(uint8_t para);
void Display_Hide_Min(uint8_t para);
void Display_Hide_Sec(uint8_t para);
void Display_Hide_Mode(uint8_t para);
	
//------------------- 显示 & 界面 ----------------------------

extern void Lcd_Display_Symbol(uint8_t status_para);
extern void Lcd_Test(uint8_t num);
extern void Lcd_Display(uint16_t speed, uint16_t time, uint8_t status_para, uint8_t mode);
// 息屏
extern void Lcd_Off(void);
// 速度 熄灭
extern void Lcd_No_Speed(uint16_t time, uint8_t status_para, uint8_t mode);
//------------------- 外部接口  ----------------------------
//显示
extern void Lcd_Show(void);

extern void Lcd_Show_Upgradation(uint8_t sum, uint8_t num);

// 机型码 & 拨码
extern void Lcd_System_Information(void);
extern void Lcd_Speed_Off(void);
// 降速 界面 2秒1刷
void Lcd_Show_Slow_Down(uint8_t value);

// 关机
extern void To_Power_Off(void);
// 自由模式
extern void To_Free_Mode(uint8_t mode);
// 定时模式
extern void To_Timing_Mode(void);
//	训练模式  num:0-2
extern void To_Train_Mode(uint8_t num);
//	自测
extern void System_Self_Testing_Porgram(void);


extern void Freertos_TaskSuspend_Wifi(void);

extern void Freertos_TaskResume_All(void);

extern void Freertos_TaskSuspend_RS485(void);
/* Private defines -----------------------------------------------------------*/

extern uint8_t LCD_Show_Bit;


#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */

