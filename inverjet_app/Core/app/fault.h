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
	FAULT_BIT_VOLTAGE_ABNORMAL = 0,					// ��ѹ �쳣
	FAULT_BIT_CURRENT_ABNORMAL,							// ���� �쳣
	FAULT_BIT_CURRENT_BIAS,									// ����������ƫ�ù���
	FAULT_BIT_SHORT_CIRCUIT,								// ��·
	FAULT_BIT_LACK_PHASE,										// ȱ��
	FAULT_BIT_LOCK_ROTOR,										// ��ת
	
	FAULT_BIT_TEMPERATURE_MOS,							// �¶� MOS
	FAULT_BIT_TEMPERATURE_AMBIENT,					// �¶� ����
	FAULT_BIT_TEMPERATURE_SENSOR,						// �¶� ������
	
	FAULT_BIT_DRIVE_BOARD,									// ������ �쳣
	FAULT_BIT_DRIVE_LOSS,										// ������ ͨ�Ź���
	FAULT_BIT_VOLTAGE_AMBIENT,							// ��ѹ ������
	
} FAULT_STATE_BIT_E;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#define FAULT_STATE_MAX									(1<<11)

#define FAULT_BUS_VOLTAGE_ABNORMAL					(1 << FAULT_BIT_VOLTAGE_ABNORMAL)
#define FAULT_BUS_CURRENT_ABNORMAL					(1 << FAULT_BIT_CURRENT_ABNORMAL)
#define FAULT_BUS_CURRENT_BIAS							(1 << FAULT_BIT_CURRENT_BIAS)
#define FAULT_ABNORMAL_OUTPUT_VOLTAGE				(1 << FAULT_BIT_SHORT_CIRCUIT)
#define FAULT_LACK_PHASE										(1 << FAULT_BIT_LACK_PHASE)
#define FAULT_LOCK_ROTOR										(1 << FAULT_BIT_LOCK_ROTOR)
#define FAULT_TEMPERATURE_MOS								(1 << FAULT_BIT_TEMPERATURE_MOS)
#define FAULT_TEMPERATURE_AMBIENT						(1 << FAULT_BIT_TEMPERATURE_AMBIENT)
#define FAULT_TEMPERATURE_SENSOR						(1 << FAULT_BIT_TEMPERATURE_SENSOR)
#define FAULT_MOTOR_DRIVER									(1 << FAULT_BIT_DRIVE_BOARD)
#define FAULT_MOTOR_LOSS										(1 << FAULT_BIT_DRIVE_LOSS)
#define FAULT_VOLTAGE_AMBIENT								(1 << FAULT_BIT_VOLTAGE_AMBIENT)

//#define FAULT_WIFI_TEST_ERROR								0x200
//-------------- ���������Ӧ ���� -------------------
#define CALL_OUT_NUMBER_MAX						8


// �ָ����Ϻ�ȴ�ʱ��
#define RECOVERY_ATTEMPT_TIME							30


//extern uint8_t Fault_Recovery_Attempt_cnt;				//

/* Exported functions prototypes ---------------------------------------------*/
// ��ʼ��
extern void App_Fault_Init(void);
// ��� ����״̬ �Ƿ�Ϸ�
extern uint8_t Fault_Check_Status_Legal(uint16_t parameter);
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

