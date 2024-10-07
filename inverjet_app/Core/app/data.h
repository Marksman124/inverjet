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
#ifndef __DATA_H__
#define __DATA_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include <string.h>
#include "state_machine.h"
#include "tm1621.h"
#include "motor.h"
//#include "modbus.h"

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef struct Operating_Parameters
{
	uint16_t speed;
	uint16_t time;
}Operating_Parameters;

typedef enum 
{
	CTRL_FROM_KEY = 0,				//	����
	CTRL_FROM_WIFI,						//	wifi
	CTRL_FROM_BT,							//	����
	CTRL_FROM_RS485,					//	modbus 485	
} System_Ctrl_Mode_Type_enum;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- ѵ��ģʽ ���ֵ -------------------

#define TRAINING_MODE_NUMBER_MAX						5
#define TRAINING_MODE_PERIOD_MAX						50
//����ģʽ --�� P5
#define SURFING_MODE_NUMBER_ID							5
//------------------- �Ϸ���Χ ----------------------------
#define SPEED_LEGAL_MIN						20
#define SPEED_LEGAL_MAX						100

#define TIME_LEGAL_MIN						20
#define TIME_LEGAL_MAX						6000

#define	MOTOR_RPM_NUMBER_OF_POLES								5		//������� Ĭ��ֵ
#define	MOTOR_RPM_MIX_OF_POLES									1		//������� �Ϸ���Сֵ
#define	MOTOR_RPM_MAX_OF_POLES									8		//������� �Ϸ����ֵ

/* Exported functions prototypes ---------------------------------------------*/

extern void Check_Data_Init(void);

extern void App_Data_Init(void);

extern void App_Data_ReInit(void);
// �� flash
extern uint8_t Read_OPMode(void);
extern uint8_t Memset_OPMode(void);

//�洢 �� �ٶ�
extern void Update_OP_Speed(void);
//�洢 �� ʱ��
extern void Update_OP_Time(void);


// ��������ֵ
extern uint8_t Set_Data_OPMode(Operating_Parameters* p_op);
//------------------- �ж� ģʽ �Ϸ� ----------------------------
uint8_t Is_Mode_Legal(uint8_t mode);
//------------------- �ж��ٶ� �Ϸ� ----------------------------
extern uint8_t Is_Speed_Legal(uint16_t speed);

extern uint8_t Is_Time_Legal(uint16_t time);

//------------------- ���� ��ǰ �ٶ� ----------------------------
extern void Data_Set_Current_Speed(uint8_t speed);
//------------------- ���� ��ǰ ʱ�� ----------------------------
extern void Data_Set_Current_Time(uint16_t time);
//------------------- ���� ѵ��ʱ�� ----------------------------
extern void Set_Pmode_Period_Now(uint16_t value);
//------------------- �Ƿ�����ⲿ���� ----------------------------
extern uint8_t If_Accept_External_Control(void);
//------------------- ��ȡ����汾��  �ַ���ת uint32 ----------------------------
extern uint32_t get_uint3_version(char * buffer);
//------------------- ���wifi��־ ----------------------------
extern void System_Wifi_State_Clean(void);

//------------------- ���ÿ��Ʒ�ʽ ----------------------------
extern void Set_Ctrl_Mode_Type(System_Ctrl_Mode_Type_enum type);
//------------------- ��ȡ���Ʒ�ʽ ----------------------------
extern System_Ctrl_Mode_Type_enum Get_Ctrl_Mode_Type(void);

//------------------- OTA �Զ��˳� 1���һ�� ----------------------------
extern void OTA_Time_Out(void);


/* Private defines -----------------------------------------------------------*/

extern Operating_Parameters OP_ShowNow;

extern Operating_Parameters* p_OP_ShowLater;


// ѵ��ģʽ ��ǰ״̬
extern uint8_t PMode_Now;

extern uint8_t Period_Now;

// ��ģʽ ���� ��ʼֵ
extern Operating_Parameters OP_Init_Free;

extern Operating_Parameters OP_Init_Timing;

extern Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX];

//--------------------------- ϵͳ����
extern uint16_t* p_System_State_Machine;			//״̬��
extern uint16_t* p_PMode_Now;									// ��ǰģʽ
extern uint16_t* p_OP_ShowNow_Speed;					// ��ǰ�ٶ�
extern uint16_t* p_OP_ShowNow_Time;						// ��ǰʱ��

extern System_Ctrl_Mode_Type_enum Ctrl_Mode_Type;				// ���Ʒ�ʽ  0:����   1:wifi 2:bt
// ��ģʽ ����
extern Operating_Parameters* p_OP_Free_Mode;

extern Operating_Parameters* p_OP_Timing_Mode;

extern Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX];
//==========================================================
//--------------------------- �������ȡ��Ϣ
//==========================================================
extern uint16_t* p_Driver_Software_Version;			// ����������汾
extern uint16_t* p_Motor_Fault_Static;						// ����״̬		������

extern uint32_t* p_Motor_Reality_Speed;					// ��� ʵ�� ת��
extern uint32_t* p_Motor_Reality_Power;					// ��� ʵ�� ����

extern uint16_t* p_Mos_Temperature;							// mos �¶�
extern uint32_t* p_Motor_Current;								// ��� ����		���
extern uint16_t* p_Motor_Bus_Voltage;						// ĸ�� ��ѹ		����
extern uint16_t* p_Motor_Bus_Current;						// ĸ�� ����  	����

//==========================================================
//--------------------------- ������Ϣ
//==========================================================
extern uint16_t* p_System_Fault_Static;					// ����״̬		����
extern uint16_t* p_Box_Temperature;							// ���� �¶�
extern uint32_t* p_Send_Reality_Speed;					// �·� ʵ�� ת��
extern uint32_t* p_Simulated_Swim_Distance;			// ģ����Ӿ����


extern uint16_t* p_Modbus_Node_Addr;						//��ַ
extern uint16_t* p_Modbus_Baud_Rate;						//������
extern uint16_t* p_Support_Control_Methods;			//���ο��Ʒ�ʽ
extern uint16_t* p_Motor_Pole_Number;						//�������
extern uint16_t* p_Breath_Light_Max;						//��Ȧ����  
	
extern uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX];	//���״̬

//================= ��ʱ����  ȫ�� ================================
extern uint16_t Temp_Data_P5_Acceleration;				//P5 ���ٶ�
extern uint16_t Temp_Data_P5_100_Time;						//P5 100% ʱ��	��
extern uint16_t Temp_Data_P5_0_Time;							//P5 0% 	ʱ��	��

extern uint8_t WIFI_Rssi;

extern uint16_t* p_Analog_key_Value;							// ���ⰴ��

extern uint8_t System_PowerUp_Finish;

extern uint8_t System_Self_Testing_State;

//================= ����ʹ��  ʱ�� ================================

extern uint32_t* p_System_Runing_Second_Cnt;			// ϵͳʱ��
extern uint32_t* p_No_Operation_Second_Cnt;			// ���˲���ʱ��
extern uint32_t* p_System_Sleeping_Second_Cnt;		// ����ʱ��


#ifdef __cplusplus
}
#endif

#endif /* __DATA_H__ */

