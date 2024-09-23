/**
******************************************************************************
* @file    		display.h
* @brief   		��ʾģ��  ��ʾģ�鱾��ռ���߳�,�������߳��������
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

//-------------- ͼ����ʾ��׼λ -------------------
#define STATUS_BIT_BLUETOOTH			1
#define STATUS_BIT_PERCENTAGE			2
#define STATUS_BIT_WIFI						4
#define STATUS_BIT_COLON					8
#define STATUS_BIT_POINT					16

//#define STATUS_BIT_P1							32
//#define STATUS_BIT_P2							64
//#define STATUS_BIT_P3							128


/* Exported functions prototypes ---------------------------------------------*/

//------------------- Ӳ�� & ���� ----------------------------
// ��ʼ��
extern void App_Display_Init(void);
//------------------- ��ʾ�� & �ӿ� ----------------------------
/*
******************************************************************************
Display_Show_Speed	

��ʾ�ٶȣ� 0-100
******************************************************************************
*/  
extern void Display_Show_Speed(uint8_t speed);
/*
******************************************************************************
Display_Show_Min	

��ʾ���ӣ� 0-99
******************************************************************************
*/  
extern void Display_Show_Min(uint8_t min);
/*
******************************************************************************
Display_Show_Speed	

��ʾ�ٶȣ� 0-100
******************************************************************************
*/  
extern void Display_Show_Sec(uint8_t sec);
/*
******************************************************************************
Display_Show_Mode	

��ʾģʽ�� P1~P3
******************************************************************************
*/  
extern void Display_Show_Mode(uint8_t mode);

void Display_Hide_Speed(uint8_t para);
void Display_Hide_Min(uint8_t para);
void Display_Hide_Sec(uint8_t para);
void Display_Hide_Mode(uint8_t para);
	
//------------------- ��ʾ & ���� ----------------------------

extern void Lcd_Display_Symbol(uint8_t status_para);
extern void Lcd_Test(uint8_t num);
extern void Lcd_Display(uint16_t speed, uint16_t time, uint8_t status_para, uint8_t mode);
// Ϣ��
extern void Lcd_Off(void);
// �ٶ� Ϩ��
extern void Lcd_No_Speed(uint16_t time, uint8_t status_para, uint8_t mode);
//------------------- �ⲿ�ӿ�  ----------------------------
//��ʾ
extern void Lcd_Show(void);

extern void Lcd_Show_Upgradation(uint8_t sum, uint8_t num);

// ������ & ����
extern void Lcd_System_Information(void);
extern void Lcd_Speed_Off(void);
// ���� ���� 2��1ˢ
void Lcd_Show_Slow_Down(uint8_t value);
//------------------- �л�ģʽ  ----------------------------
// �л�ģʽ
extern void Fun_Change_Mode(void);
// �ػ�
extern void To_Power_Off(void);
// ����ģʽ
extern void To_Free_Mode(uint8_t mode);
// ��ʱģʽ
extern void To_Timing_Mode(void);
//	ѵ��ģʽ  num:0-2
extern void To_Train_Mode(uint8_t num);
//	�Բ�
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

