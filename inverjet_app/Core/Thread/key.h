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
#include "display.h"			// ��ʾģ��
#include "gpio.h"
#include "usart.h"				// ����
#include "timing.h"				// ��̨��ʱ��

#include "operation.h"		// ���� �˵�
#include "fault.h"				// ���� �˵�

#include "wifi_thread.h"				// wifi ģ��
#include "bluetooth.h"				// bluetooth ģ��
#include "macro_definition.h"				// ͳһ�궨��
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

#define KEY_THREAD_LIFECYCLE								20				// ������������ 200ms

#define KEY_LONG_PRESS_TIME									(3000/KEY_THREAD_LIFECYCLE)			//����ʱ�� 3s
#define KEY_LONG_PRESS_TIME_SHORT						(1000/KEY_THREAD_LIFECYCLE)			//��һ��� ����ʱ��  1s

#define KEY_FOR_SLEEP_TIME_SHORT						(3000/KEY_THREAD_LIFECYCLE)			//????  5 min  300000

//-------------- ������ ���� -------------------
//******************  ����ģʽ **************************
//-------------- ������ ���� -------------------
#define KEY_BUZZER_TIME								(8)					//����  KEY_THREAD_LIFECYCLE ����
#define KEY_BUZZER_TIME_LONG					(16)					//����  KEY_THREAD_LIFECYCLE ����


#endif

//-------------- ���������Ӧ ���� -------------------
#define KEY_CALL_OUT_NUMBER_MAX						8

#define KEY_VALUE_BIT_BUTTON_1						0x01
#define KEY_VALUE_BIT_BUTTON_2						0x02
#define KEY_VALUE_BIT_BUTTON_3						0x04
#define KEY_VALUE_BIT_BUTTON_4						0x08

/* Exported functions prototypes ---------------------------------------------*/

//------------------- �����ص� ----------------------------
//--------------- �̰� -----------------------
// �� ��λ��
extern void on_pushButton_clicked(void);
// �� ʱ���
extern void on_pushButton_2_clicked(void);
// �� ģʽ��
extern void on_pushButton_3_clicked(void);
// �� ������  �̰�
extern void on_pushButton_4_Short_Press(void);
// �� + ��  ��ϼ�  ���� ���ù���
extern void on_pushButton_1_2_Short_Press(void);
// �� + ��  ��ϼ�  �̰�   �л���λ 80�� or 5��
extern void on_pushButton_1_3_Short_Press(void);
// �� + ��  ��ϼ�  �̰�
extern void on_pushButton_2_3_Short_Press(void);
// �� + ��  ��ϼ�  �̰�
extern void on_pushButton_2_4_Short_Press(void);
//--------------- ���� -----------------------
// ����
extern void on_pushButton_1_Long_Press(void);
extern void on_pushButton_2_Long_Press(void);
extern void on_pushButton_3_Long_Press(void);
extern void on_pushButton_4_Long_Press(void);
extern void on_pushButton_1_2_Long_Press(void);
extern void on_pushButton_1_3_Long_Press(void);
extern void on_pushButton_2_3_Long_Press(void);
extern void on_pushButton_2_4_Long_Press(void);

//------------------- Ӳ�� & ���� ----------------------------
// ��ʼ��
extern void App_Key_Init(void);
// ������
extern void Led_Button_On(uint8_t para);
// ������ѭ������
extern void App_Key_Handler(void);
// ��ȡ����
extern uint8_t Key_Get_IO_Input(void);
//------------------- ���ܽӿ� ----------------------------

extern void Buzzer_Click_On(void);

extern void Buzzer_Click_Long_On(uint8_t type);

//	���� ��������ģʽ
extern void System_Power_On(void);
//	�ػ�
extern void System_Power_Off(void);
//	��������
extern void System_Boot_Screens(void);
//	�ָ���������
extern void Restore_Factory_Settings(void);

/* Private defines -----------------------------------------------------------*/



#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

