/**
******************************************************************************
* @file    		state_machine.h
* @brief   		״̬��
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
#include "macro_definition.h"				// ͳһ�궨��
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	POWER_OFF_STATUS = 0,			//	�ػ�
	//����ģʽ
  FREE_MODE_INITIAL,				//	1		��ʼ״̬	
	FREE_MODE_STARTING,				//			������
  FREE_MODE_RUNNING,				//			������
	FREE_MODE_SUSPEND,				//			��ͣ
	FREE_MODE_STOP,						//	5		����
	//��ʱģʽ
  TIMING_MODE_INITIAL,			//	6		��ʼ״̬
	TIMING_MODE_STARTING,			//			������
  TIMING_MODE_RUNNING,			//			������
	TIMING_MODE_SUSPEND,			//			��ͣ
	TIMING_MODE_STOP,					//	10	����
	//ѵ��ģʽ
  TRAINING_MODE_INITIAL,		//	11	��ʼ״̬
	TRAINING_MODE_STARTING,		//			������
  TRAINING_MODE_RUNNING,		//			������
	TRAINING_MODE_SUSPEND,		//			��ͣ
	TRAINING_MODE_STOP,				//	15	����
	
	// �쳣״̬
  OPERATION_MENU_STATUS,		//			�����˵�
	ERROR_DISPLAY_STATUS,			//			���Ͻ���
	SYSTEM_STATE_END,					//	����
} SYSTEM_STATE_MODE_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/


//-------------- �ر�״̬ λ -------------------
#define SPECIAL_BIT_SKIP_INITIAL				0x01		//���� ��ʼ��   ���Զ�����3s
#define SPECIAL_BIT_SKIP_STARTING				0x02		//���� ������  ֱ�ӽ��� ����
#define SPECIAL_BIT_SPEED_CHANGE				0x04		//�����и���ת��

#define SPECIAL_BIT_SPEED_100_GEAR			0x10		//�ٶ� 100 ��λ
#define SPECIAL_BIT_TIME_100_GEAR				0x20		//��ʱ 100 ��λ


/* Exported functions prototypes ---------------------------------------------*/

//------------------- Ӳ�� & ���� ----------------------------
// ��ʼ��
extern void App_State_Machine_Init(void);
//------------------- ����״̬��  ----------------------------
extern uint8_t Set_System_State_Machine(uint8_t val);
//------------------- ��ȡ״̬��  ----------------------------
extern uint8_t Get_System_State_Machine(void);

//------------------- ���ٻ�ȡ״̬  ----------------------------
// �������״̬
extern uint8_t Motor_is_Start(void);
// ��������״̬
extern uint8_t System_is_Normal_Operation(void);
// ��ʼ״̬
extern uint8_t System_is_Initial(void);
// ����״̬
extern uint8_t System_is_Starting(void);
// ����״̬
extern uint8_t System_is_Running(void);
// ��ͣ״̬
extern uint8_t System_is_Pause(void);
// ����״̬
extern uint8_t System_is_Stop(void);
// �����˵�
extern uint8_t System_is_Operation(void);
// ���Ͻ���
extern uint8_t System_is_Error(void);
// �ػ�
extern uint8_t System_is_Power_Off(void);
// ���� ģʽ
extern uint8_t System_Mode_Free(void);
// ��ʱ ģʽ
extern uint8_t System_Mode_Time(void);
// ѵ�� ģʽ
extern uint8_t System_Mode_Train(void);
	
//------------------- ����״̬ ���ı�ģʽ  ----------------------------

// --> ��ʼ״̬ ��������ģʽ��
extern void Arbitrarily_To_Initial(void);
	
// --> ����״̬ ��������ģʽ��
extern void Arbitrarily_To_Starting(void);

// --> ����״̬ ��������ģʽ��
extern void Arbitrarily_To_Running(void);

// --> ��ͣ״̬ ��������ģʽ��
extern void Arbitrarily_To_Pause(void);

// --> ����״̬ ��������ģʽ��
extern void Arbitrarily_To_Stop(void);

//------------------- �ر�״̬ ��  ----------------------------

// ��� �ر�״̬ ��־
extern void Special_Status_Add(uint8_t num);

// ɾ�� �ر�״̬ ��־
extern void Special_Status_Delete(uint8_t num);
	
// ��ȡ �ر�״̬ ��־
extern uint8_t Special_Status_Get(uint8_t num);

/* Private defines -----------------------------------------------------------*/
extern uint8_t Special_Status_Bit;

// ���뿪��
extern uint8_t System_Dial_Switch;

extern uint16_t *p_PMode_Now;

extern uint8_t Period_Now;

#ifdef __cplusplus
}
#endif

#endif /* __STATE_MACHINE_H__ */

