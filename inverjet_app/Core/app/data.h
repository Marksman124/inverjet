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


// ��Ƶ ����
typedef enum 
{
	MOTOR_DOWN_CONVERSION_NO = 0,						//	��
	MOTOR_DOWN_CONVERSION_MOS_TEMPER,				//	mos ����
	MOTOR_DOWN_CONVERSION_BOX_TEMPER,				//	���� ����
	MOTOR_DOWN_CONVERSION_OUT_CURRENT,			//	��� ����
	MOTOR_DOWN_CONVERSION_OUT_POWER,				//	��� ����
} Motor_Down_Conversion_Type_enum;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- ѵ��ģʽ ���ֵ -------------------

#define TRAINING_MODE_NUMBER_MAX						SYSTEM_MODE_TRAIN_P5
#define TRAINING_MODE_PERIOD_MAX						50
//����ģʽ --�� P5
#define SURFING_MODE_NUMBER_ID							SYSTEM_MODE_TRAIN_P5
//------------------- �Ϸ���Χ ----------------------------
#define SPEED_LEGAL_MIN						MOTOR_PERCENT_SPEED_MIX
#define SPEED_LEGAL_MAX						MOTOR_PERCENT_SPEED_MAX

#define TIME_LEGAL_MIN						20
#define TIME_LEGAL_MAX						MOTOR_TIME_SHOW_MAX

#define	MOTOR_RPM_NUMBER_OF_POLES								5		//������� Ĭ��ֵ
#define	MOTOR_RPM_MIX_OF_POLES									1		//������� �Ϸ���Сֵ
#define	MOTOR_RPM_MAX_OF_POLES									8		//������� �Ϸ����ֵ

//------------------- ���ο��� ----------------------------
#define	BLOCK_BLUETOOTH_CONTROL									1		// ���� ���� ����
#define	BLOCK_MODBUS_CONTROL										2		// ���� MODBUS ����
#define	BLOCK_WIFI_CONTROL											4		// ���� WIFI ����

// �߳� �
#define	THREAD_ACTIVITY_BREATH_LIGHT						( 1<<0 )
#define	THREAD_ACTIVITY_RS485_MODBUS						( 1<<1 )
#define	THREAD_ACTIVITY_MAIN										( 1<<2 )
#define	THREAD_ACTIVITY_KEY_BUTTON							( 1<<3 )
#define	THREAD_ACTIVITY_MOTOR										( 1<<4 )
#define	THREAD_ACTIVITY_WIFI										( 1<<5 )


/* Exported functions prototypes ---------------------------------------------*/
//================= ����ģʽ ȫ�� ���� ================================
extern void Surf_Mode_Info_Data_Init(void);

extern uint8_t Check_Data_Init(void);

extern void App_Data_Init(void);

extern void App_Data_ReInit(void);
// �� flash
extern uint8_t Read_OPMode(void);
extern uint8_t Memset_OPMode(void);

//�洢 �� �ٶ�
extern void Update_OP_Speed(void);
//�洢 �� ʱ��
extern void Update_OP_Time(void);
//�洢 �� �ٶ� & ʱ��
extern void Update_OP_All(void);

extern void OP_Update_Mode(void);
//��� �� �ٶ� & ʱ��  ��ֹ���
void Check_OP_All(void);

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
extern uint8_t If_Accept_External_Control(uint8_t mode);
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


//------------------- ��ȡ������ ----------------------------
extern uint32_t Get_Model_Code_Num(void);
// ÿ %1 ÿ�� ������Ӿ���� �Ŵ�100��
extern uint32_t Get_Every_1Percent_Distance_Per_Second(void);
//���ת�� 100%
extern uint32_t Get_Motor_Rpm_Speed_Max(void);
//���ת�� 20%
extern uint32_t Get_Motor_Rpm_Speed_Mix(void);
// ���� ��Ƶ
//*********************************************************************************************
//-------------- ������� ����ֵ  -------------------
extern uint32_t Get_Motor_Power_Alarm_Value(void);
//-------------- ������� ����  -------------------
extern uint32_t Get_Motor_Power_Reduce_Speed(void);
//-------------- ������� �ָ�  -------------------
extern uint32_t Get_Motor_Power_Restore_Speed(void);
//*********************************************************************************************
// ���� ��Ƶ
//*********************************************************************************************
//-------------- ������� ����ֵ  -------------------
extern uint32_t Get_Motor_Current_Alarm_Value(void);
//-------------- ������� ����  -------------------
extern uint32_t Get_Motor_Current_Reduce_Speed(void);
//-------------- ������� �ָ�  -------------------
extern uint32_t Get_Motor_Current_Restore_Speed(void);
//*********************************************************************************************

// �������ͳ��
//*********************************************************************************************
//-------------- ���� ���ͳ��  -------------------
extern void Finish_Statistics_Count(uint8_t count);
//-------------- ��� ���ͳ��  -------------------
extern void Finish_Statistics_Clean( void );
//-------------- �ϴ� ���ͳ��  -------------------
extern void Finish_Statistics_Upload( void );
	

//-------------- �߳� � ��־����  -------------------
extern void Thread_Activity_Sign_Clean( void );
//-------------- �߳� � ����  -------------------
extern void Thread_Activity_Sign_Set( uint16_t val );
	

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


extern uint16_t* p_Support_Control_Methods;			//���ο��Ʒ�ʽ
extern uint16_t* p_Motor_Pole_Number;						//�������
extern uint16_t* p_Breath_Light_Max;						//��Ȧ����  
	
extern uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX];	//���״̬

//================= ��ʱ����  ȫ�� ================================
// ----------------------------------------------------------------------------------------------
extern uint16_t* p_Surf_Mode_Info_Acceleration;  			//	����ģʽ -- ���ٶ�
extern uint16_t* p_Surf_Mode_Info_Prepare_Time;  			//	����ģʽ -- ׼��ʱ��
extern uint16_t* p_Surf_Mode_Info_Low_Speed;  				//	����ģʽ -- ���ٵ� -- �ٶ�
extern uint16_t* p_Surf_Mode_Info_Low_Time;						//	����ģʽ -- ���ٵ� -- ʱ��
extern uint16_t* p_Surf_Mode_Info_High_Speed;  				//	����ģʽ -- ���ٵ� -- �ٶ�
extern uint16_t* p_Surf_Mode_Info_High_Time;  				//	����ģʽ -- ���ٵ� -- ʱ��
// ----------------------------------------------------------------------------------------------

extern uint8_t WIFI_Rssi;

extern uint16_t* p_Analog_key_Value;							// ���ⰴ��

extern uint8_t System_PowerUp_Finish;

extern uint8_t System_Self_Testing_State;

//================= ����ʹ��  ʱ�� ================================

extern uint32_t* p_System_Runing_Second_Cnt;			// ϵͳʱ��
extern uint32_t* p_No_Operation_Second_Cnt;				// ���˲���ʱ��
extern uint32_t* p_System_Startup_Second_Cnt;			// ����ʱ��

//==========================================================
//--------------------------- ���ͳ�� (APPҪ)
//==========================================================
extern uint16_t* p_Finish_Statistics_Time;					//	���ͳ�� --> ʱ��
extern uint16_t* p_Finish_Statistics_Speed;					//	���ͳ�� --> ǿ��
extern uint32_t* p_Finish_Statistics_Distance;			//	���ͳ�� --> ��Ӿ����
extern uint16_t* p_Preparation_Time_BIT;						//	׼��ʱ�� Bit: ��ʱģʽ P1-P6

extern uint16_t* p_Thread_Activity_Sign;					//	�߳� � ��־λ

#ifdef __cplusplus
}
#endif

#endif /* __DATA_H__ */

