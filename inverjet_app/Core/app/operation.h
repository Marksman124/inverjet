/**
******************************************************************************
* @file    		operation.h
* @brief   		����ģ�� �������ز��� ����������������\ɾ��
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OPERATION_H__
#define __OPERATION_H__


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
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

//typedef enum 
//{

//#define	OPERATION_P5_ACCELERATION				(1)			//	���ٶ�
//#define	OPERATION_P5_100_TIME						(OPERATION_P5_ACCELERATION+1)			//	100% ʱ��
//#define	OPERATION_P5_0_TIME							(OPERATION_P5_100_TIME+1)			//	0% ʱ��


#define OPERATION_ADDR_SET							(1)																//	��ַ����
#define	OPERATION_BAUD_RATE							(OPERATION_ADDR_SET+1)						//	������
//#define	OPERATION_SPEED_MODE					(OPERATION_BAUD_RATE+1)						//	ת�ٷ�ʽ�� 0��ת��  1������
//#define	OPERATION_MOTOR_POLES						(OPERATION_BAUD_RATE+1)					//	�������

#define	OPERATION_SHIELD_MENU						(OPERATION_BAUD_RATE+1)						//	���ο��Ʒ�ʽ
#define	OPERATION_DISPLAY_VERSION				(OPERATION_SHIELD_MENU+1)					//	��ʾ�� �汾��

//#define	OPERATION_BREATH_LIGHT_MAX			(OPERATION_DISPLAY_VERSION+1)			//	��Ȧ����

#define	OPERATION_DEIVES_VERSION				(OPERATION_DISPLAY_VERSION+1)			//	������ �汾��
#define	OPERATION_STATE_END							(OPERATION_DEIVES_VERSION+1)			//	����
//} OPERATION_STATE_MODE_E;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- ���������Ӧ ���� -------------------
#define CALL_OUT_NUMBER_MAX						8


#define OPERATION_BAUD_MAX						4
#define OPERATION_SHIELD_MAX					7
#define OPERATION_POLES_MAX						MOTOR_RPM_MAX_OF_POLES


#define OPERATION_POLES_MIX						MOTOR_RPM_MIX_OF_POLES
/* Exported functions prototypes ---------------------------------------------*/

extern void App_Operation_Init(void);
	
// ��������˵�
extern void To_Operation_Menu(void);

/* Private defines -----------------------------------------------------------*/

// �̰� �ۺ���
extern void (*p_Operation_Button[CALL_OUT_NUMBER_MAX])(void);

// ���� �ۺ���
extern void (*p_Operation_Long_Press[CALL_OUT_NUMBER_MAX])(void);



#ifdef __cplusplus
}
#endif

#endif /* __OPERATION_H__ */

