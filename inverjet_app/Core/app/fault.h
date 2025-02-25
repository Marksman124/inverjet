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
#ifdef SYSTEM_DRIVER_BOARD_TOOL
typedef enum 
{
	FAULT_BIT_01_VOLTAGE_ABNORMAL = 0,					// ��ѹ �쳣									E001
	FAULT_BIT_02_CURRENT_ABNORMAL,							// ���� �쳣									E002
	FAULT_BIT_03_CURRENT_BIAS,									// ����������ƫ�ù���				E003
	FAULT_BIT_04_SHORT_CIRCUIT,									// ��·											E004
	FAULT_BIT_05_LACK_PHASE,										// ȱ��											E005
	FAULT_BIT_06_LOCK_ROTOR,										// ��ת											E006
	
	FAULT_BIT_07_TEMPERATURE_MOS,								// �¶� MOS									E101
	FAULT_BIT_08_TEMPERATURE_AMBIENT,						// �¶� ����									E102
		
	FAULT_BIT_09_TEMPERATURE_HARDWARE,					// �¶� (Ӳ������)						E201
	FAULT_BIT_10_DRIVE_BOARD,										// ������ �쳣								E202
	FAULT_BIT_11_DRIVE_LOSS,										// ������ ͨ�Ź���						E203
	
	FAULT_BIT_12_WIFI_HARDWARE,									// WIFI ģ�� (Ӳ������)			E301
	FAULT_BIT_13_BT_HARDWARE,										// BT ģ�� (Ӳ������)				E302
	FAULT_BIT_14_RS485_HARDWARE,								// RS485 ģ�� (Ӳ������)			E303
	FAULT_BIT_15_MOTOR_MODEL_HARDWARE,					// ������������						E401
	
} FAULT_STATE_BIT_E;
#else
typedef enum 
{
	FAULT_BIT_01_VOLTAGE_ABNORMAL = 0,					// ��ѹ �쳣									E001
	FAULT_BIT_02_CURRENT_ABNORMAL,							// ���� �쳣									E002
	FAULT_BIT_03_CURRENT_BIAS,									// ����������ƫ�ù���				E003
	FAULT_BIT_04_SHORT_CIRCUIT,									// ��·											E004
	FAULT_BIT_05_LACK_PHASE,										// ȱ��											E005
	FAULT_BIT_06_LOCK_ROTOR,										// ��ת											E006
	
	FAULT_BIT_07_TEMPERATURE_MOS,								// �¶� MOS									E101
	FAULT_BIT_08_TEMPERATURE_AMBIENT,						// �¶� ����									E102
		
	FAULT_BIT_09_TEMPERATURE_HARDWARE,					// �¶� (Ӳ������)						E201
	FAULT_BIT_10_DRIVE_BOARD,										// ������ �쳣								E202
	FAULT_BIT_11_DRIVE_LOSS,										// ������ ͨ�Ź���						E203
	
	FAULT_BIT_12_WIFI_HARDWARE,									// WIFI ģ�� (Ӳ������)			E301
	FAULT_BIT_13_BT_HARDWARE,										// BT ģ�� (Ӳ������)				E302
	FAULT_BIT_14_RS485_HARDWARE,								// RS485 ģ�� (Ӳ������)			E303
	//FAULT_BIT_15_TTL_SEND_HARDWARE,									// ����	(Ӳ������)						E304
	
} FAULT_STATE_BIT_E;

#endif

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#ifdef SYSTEM_DRIVER_BOARD_TOOL
#define FAULT_STATE_MAX										(1<<(FAULT_BIT_15_MOTOR_MODEL_HARDWARE+1))

#define E001_BUS_VOLTAGE_ABNORMAL					(1 << FAULT_BIT_01_VOLTAGE_ABNORMAL)
#define E002_BUS_CURRENT_ABNORMAL					(1 << FAULT_BIT_02_CURRENT_ABNORMAL)
#define E003_BUS_CURRENT_BIAS							(1 << FAULT_BIT_03_CURRENT_BIAS)
#define E004_ABNORMAL_SHORT_CIRCUIT				(1 << FAULT_BIT_04_SHORT_CIRCUIT)
#define E005_LACK_PHASE										(1 << FAULT_BIT_05_LACK_PHASE)
#define E006_LOCK_ROTOR										(1 << FAULT_BIT_06_LOCK_ROTOR)
#define E101_TEMPERATURE_MOS							(1 << FAULT_BIT_07_TEMPERATURE_MOS)
#define E102_TEMPERATURE_AMBIENT					(1 << FAULT_BIT_08_TEMPERATURE_AMBIENT)
#define E201_TEMPERATURE_HARDWARE					(1 << FAULT_BIT_09_TEMPERATURE_HARDWARE)
#define E202_MOTOR_DRIVER									(1 << FAULT_BIT_10_DRIVE_BOARD)
#define E203_MOTOR_LOSS										(1 << FAULT_BIT_11_DRIVE_LOSS)
#define E301_WIFI_HARDWARE								(1 << FAULT_BIT_12_WIFI_HARDWARE)
#define E302_BT_HARDWARE									(1 << FAULT_BIT_13_BT_HARDWARE)
#define E303_RS485_HARDWARE								(1 << FAULT_BIT_14_RS485_HARDWARE)
#define E401_MOTOR_MODEL_HARDWARE					(1 << FAULT_BIT_15_MOTOR_MODEL_HARDWARE)

#else
#define FAULT_STATE_MAX										(1<<(FAULT_BIT_14_RS485_HARDWARE+1))

#define E001_BUS_VOLTAGE_ABNORMAL					(1 << FAULT_BIT_01_VOLTAGE_ABNORMAL)
#define E002_BUS_CURRENT_ABNORMAL					(1 << FAULT_BIT_02_CURRENT_ABNORMAL)
#define E003_BUS_CURRENT_BIAS							(1 << FAULT_BIT_03_CURRENT_BIAS)
#define E004_ABNORMAL_SHORT_CIRCUIT				(1 << FAULT_BIT_04_SHORT_CIRCUIT)
#define E005_LACK_PHASE										(1 << FAULT_BIT_05_LACK_PHASE)
#define E006_LOCK_ROTOR										(1 << FAULT_BIT_06_LOCK_ROTOR)
#define E101_TEMPERATURE_MOS							(1 << FAULT_BIT_07_TEMPERATURE_MOS)
#define E102_TEMPERATURE_AMBIENT					(1 << FAULT_BIT_08_TEMPERATURE_AMBIENT)
#define E201_TEMPERATURE_HARDWARE					(1 << FAULT_BIT_09_TEMPERATURE_HARDWARE)
#define E202_MOTOR_DRIVER									(1 << FAULT_BIT_10_DRIVE_BOARD)
#define E203_MOTOR_LOSS										(1 << FAULT_BIT_11_DRIVE_LOSS)
#define E301_WIFI_HARDWARE								(1 << FAULT_BIT_12_WIFI_HARDWARE)
#define E302_BT_HARDWARE									(1 << FAULT_BIT_13_BT_HARDWARE)
#define E303_RS485_HARDWARE								(1 << FAULT_BIT_14_RS485_HARDWARE)
//#define E304_TTL_SEND_HARDWARE						(1 << FAULT_BIT_15_TTL_SEND_HARDWARE)

#endif

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

// �ӿڼ��
void Clean_Comm_Test(void);
void Self_Testing_Check_Comm(void);
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

