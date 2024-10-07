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
#include "display.h"			// ��ʾģ��
#include "data.h"

#include "fault.h"				// ���� �˵�
#include "macro_definition.h"				// ͳһ�궨��

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	TIMING_STATE_SUSPEND,				//	��ͣ
	TIMING_STATE_STARTING,			//	������
	TIMING_STATE_RUNNING,				//	����
	
} SYSTEM_STATE_TIMING_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#ifndef __MACRO_DEFINITION_H__
#define TIMING_THREAD_LIFECYCLE				500				// ms  1000

//******************  ����ģʽ **************************
#ifdef SYSTEM_DEBUG_MODE
//����ʱ��
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(6*(1000/TIMING_THREAD_LIFECYCLE))				// 6 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(6*(1000/TIMING_THREAD_LIFECYCLE))				// 6 s
//�����Իָ�
#define SYSTEM_FAULT_TIME_CALLOUT							(20*(1000/TIMING_THREAD_LIFECYCLE))				// 20 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 �ι���
#define SYSTEM_FAULT_RECOVERY_TIME						(60*(1000/TIMING_THREAD_LIFECYCLE))				// 1 ����  60 s
//�Զ��ػ�
#define AUTOMATIC_SHUTDOWN_TIME								(600*(1000/TIMING_THREAD_LIFECYCLE))				// 10 min

#else
//����ʱ��
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(60*(1000/TIMING_THREAD_LIFECYCLE))				// 60 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(60*(1000/TIMING_THREAD_LIFECYCLE))				// 60 s
//�����Իָ�
#define SYSTEM_FAULT_TIME_CALLOUT							(120*(1000/TIMING_THREAD_LIFECYCLE))				// 120 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 �ι���
#define SYSTEM_FAULT_RECOVERY_TIME						(3600*(1000/TIMING_THREAD_LIFECYCLE))				// 1 Сʱ��  3600 s
//�Զ��ػ�
#define AUTOMATIC_SHUTDOWN_TIME								(3600*(1000/TIMING_THREAD_LIFECYCLE))				// 1 Сʱ��  3600 s

#endif
//�Զ��ػ� ʱ��

//-------------- ���ټ��ʱ�� -------------------
#define TIME_SLOW_DOWN_TIME													120		//2 min  120 sec
//-------------- ���� ��λ -------------------
#define TIME_SLOW_DOWN_SPEED_01											10		//��һ�� ����
#define TIME_SLOW_DOWN_SPEED_02											5			//�ڶ��� ����
//-------------- ���� ����ٶ� -------------------
#define TIME_SLOW_DOWN_SPEED_MIX										20		//��ͽ��� 20%
#define TIME_SLOW_DOWN_SPEED_MAX										100		//�ָ��ٶ���߻ָ��� 100%
//*******************************************************
#endif
/* Exported functions prototypes ---------------------------------------------*/

extern void App_Timing_Init(void);

extern void Clean_Timing_Timer_Cnt(void);
// ��ȡˢ�±�־
extern void LCD_Refresh_Restore(void);
// ��ʱ�������߳�
extern void App_Timing_Handler(void);

//-------------------- ���ø��½���ģʽ 0:��   1�� ----------------------------
void Set_Temp_Slow_Down_State(uint8_t vaule);
//-------------------- ��ȡ���½���ģʽ ----------------------------
uint8_t Get_Temp_Slow_Down_State(void);
//-------------------- ���ø��½��� �ٶ� �½����ٶ� ----------------------------
void Set_Temp_Slow_Down_Speed(uint8_t vaule);
//-------------------- ��ȡ���½��� �ٶ� ----------------------------
uint8_t Get_Temp_Slow_Down_Speed(void);

void CallOut_Fault_State(void);
//-------------------- ������ϻָ������� ----------------------------
void Clean_Fault_Recovery_Cnt(void);
//-------------------- �ۼƹ��ϻָ������� ----------------------------
void Add_Fault_Recovery_Cnt(void);
//-------------------- ���������� ----------------------------
uint8_t If_Fault_Recovery_Max(void);

//-------------------- ��� �Զ��ػ���ʱ�� ----------------------------
void Clean_Automatic_Shutdown_Timer(void);
//-------------------- ��� �����ٶȼ�ʱ�� ----------------------------
void Clean_Change_Speed_Timer(void);


/* Private defines -----------------------------------------------------------*/
//���������
extern uint16_t Check_Motor_Current_Cnt;
//���ת�ٲ���
extern uint16_t Check_Motor_Speed_Cnt;

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

