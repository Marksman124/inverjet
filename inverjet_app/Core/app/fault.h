/**
******************************************************************************
* @file    		fault.h
* @brief   		����ģ��s
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
#include "macro_definition.h"				// ͳһ�궨��
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	NO_FAULT_STATE = 0,			//	�����˵�
	ss
} FAULT_STATE_MODE_E;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#define FAULT_BUS_VOLTAGE_ABNORMAL					0x1
#define FAULT_BUS_CURRENT_ABNORMAL					0x2
#define FAULT_BUS_CURRENT_BIAS							0x4
#define FAULT_ABNORMAL_OUTPUT_VOLTAGE				0x8
#define FAULT_LACK_PHASE										0x10	//ȱ��
#define FAULT_TEMPERATURE_AMBIENT						0x20
#define FAULT_TEMPERATURE_SENSOR						0x40
#define FAULT_MOTOR_DRIVER									0x80
#define FAULT_MOTOR_LOSS										0x100

#define FAULT_WIFI_TEST_ERROR								0x200
//-------------- ���������Ӧ ���� -------------------
#define CALL_OUT_NUMBER_MAX						8


// �ָ����Ϻ�ȴ�ʱ��
#define RECOVERY_ATTEMPT_TIME							30


//extern uint8_t Fault_Recovery_Attempt_cnt;				//

/* Exported functions prototypes ---------------------------------------------*/
// ��ʼ��
extern void App_Fault_Init(void);
// ������
extern uint8_t If_System_Is_Error(void);
// ���ù���ֵ
void Set_Fault_Data(uint16_t type);

//-------------------   ----------------------------

// ��ȡ����
extern uint8_t Get_Fault_Number_Sum(uint16_t para);
// ��ȡ���Ϻ�
extern uint8_t Get_Fault_Number_Now(uint16_t para, uint8_t num);
// ��������˵�
extern void To_Fault_Menu(void);
// ���Ͻ��� ����
void Update_Fault_Menu(void);
// �������״̬
extern void Clean_Fault_State(void);
// ���� ��ʾ
extern void Lcd_Fault_Display(uint8_t sum, uint8_t now, uint16_t type);

/* Private defines -----------------------------------------------------------*/

// �̰� �ۺ���
extern void (*p_Fault_Button[CALL_OUT_NUMBER_MAX])(void);

// ���� �ۺ���
extern void (*p_Fault_Long_Press[CALL_OUT_NUMBER_MAX])(void);



#ifdef __cplusplus
}
#endif

#endif /* __FAULT_H__ */

