/**
******************************************************************************
* @file				wifi.c
* @brief			wifi ģ��
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "wifi_thread.h"
#include "wifi.h"
#include "data.h"
#include "debug_protocol.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static WIFI_STATE_MODE_E WIFI_State_Machine = WIFI_NO_CONNECT;

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

//------------------- ����wifi״̬�� ----------------------------
void WIFI_Set_Machine_State(WIFI_STATE_MODE_E para)
{
	if(para <= WIFI_ERROR)
	{
		WIFI_State_Machine = para;
	}
}
	
//------------------- ��ȡwifi״̬�� ----------------------------
WIFI_STATE_MODE_E WIFI_Get_Machine_State(void)
{
	return WIFI_State_Machine;
}


//------------------- ���մ����� ----------------------------
void WIFI_Read_Handler(void)
{

}

//------------------- �������� ----------------------------
void WIFI_Get_In_Distribution(void)
{
	if(WIFI_Get_Machine_State() != WIFI_DISTRIBUTION)
	{
		WIFI_Set_Machine_State( WIFI_DISTRIBUTION );
		
		
		// ��λģ��
		mcu_reset_wifi();
		
		// ����wifiģʽ
		mcu_set_wifi_mode(SMART_CONFIG);
	}
}

//------------------- ������� ----------------------------
void WIFI_Get_In_Error(void)
{
	// ����ͼ��
	WIFI_Set_Machine_State( WIFI_ERROR );
	
}


//------------------- �ϴ�״̬���� ----------------------------
void WIFI_Update_State_Upload(void)
{

	static uint16_t Wifi_Motor_Fault_Static = 0;
	static uint16_t Wifi_System_Fault_Static = 0;
	static uint16_t Wifi_PMode_Now = 0;
	static uint16_t Wifi_System_State_Machine = 0;
	static uint16_t Wifi_OP_ShowNow_Speed = 0;
	static uint16_t Wifi_OP_ShowNow_Time = 0;
	static int	 Wifi_Box_Temperature = 0;
	static int	 Wifi_Mos_Temperature = 0;
	static uint16_t Wifi_Motor_Current = 0;
	static uint16_t Wifi_Motor_Reality_Speed = 0;
	static uint16_t Wifi_Send_Reality_Speed = 0;
	
	static uint16_t Wifi_Motor_Bus_Voltage = 0;
	
	static uint16_t Wifi_OP_Free_Mode_Speed = 0;
	static uint16_t Wifi_OP_Time_Mode_Speed = 0;
	static uint16_t Wifi_OP_Time_Mode_Time = 0;
	
	short int box_temperature = 0;
	short int mos_temperature = 0;
		
	static uint16_t State_Upload_Cnt=0;
	
	//�����ϴ�
	if(Wifi_Motor_Fault_Static != *p_Motor_Fault_Static)
	{
		mcu_dp_fault_update(DPID_DEVICE_ERROR_CODE,*p_Motor_Fault_Static); //�����������ϱ�;
		Wifi_Motor_Fault_Static = *p_Motor_Fault_Static;
	}
	
	//�����ϴ�
	if(Wifi_System_Fault_Static != *p_System_Fault_Static)
	{
		mcu_dp_fault_update(DPID_GET_SYSTEM_FAULT_STATUS,*p_System_Fault_Static); //�����������ϱ�;
		Wifi_System_Fault_Static = *p_System_Fault_Static;
	}
	// ϵͳ״̬ (��Ҫ)
	if(Wifi_PMode_Now != *p_PMode_Now)
	{
		mcu_dp_value_update(DPID_SYSTEM_WORKING_MODE,*p_PMode_Now); //VALUE�������ϱ�;
		Wifi_PMode_Now = *p_PMode_Now;
	}
	if(Wifi_System_State_Machine != *p_System_State_Machine)
	{
		mcu_dp_value_update(DPID_SYSTEM_WORKING_STATUS,*p_System_State_Machine); //VALUE�������ϱ�;
		Wifi_System_State_Machine = *p_System_State_Machine;
	}
	
	if(Wifi_OP_ShowNow_Speed != *p_OP_ShowNow_Speed)
	{
		mcu_dp_value_update(DPID_MOTOR_CURRENT_SPEED,*p_OP_ShowNow_Speed); //VALUE�������ϱ�;
		Wifi_OP_ShowNow_Speed = *p_OP_ShowNow_Speed;
	}
	
	if(Wifi_OP_ShowNow_Time != *p_OP_ShowNow_Time)
	{
		mcu_dp_value_update(DPID_MOTOR_CURRENT_TIME,*p_OP_ShowNow_Time); //VALUE�������ϱ�;
		Wifi_OP_ShowNow_Time = *p_OP_ShowNow_Time;
	}
	// �¶�  
	memcpy(&box_temperature, p_Box_Temperature, 2);
	if(Wifi_Box_Temperature != box_temperature)
	{
		Wifi_Box_Temperature = box_temperature;
		mcu_dp_value_update(DPID_GET_BOX_TEMPERATURE,Wifi_Box_Temperature); //VALUE�������ϱ�;
	}
	
	memcpy(&mos_temperature, p_Mos_Temperature, 2);
	if(Wifi_Mos_Temperature != mos_temperature)
	{
		Wifi_Mos_Temperature = mos_temperature;
		mcu_dp_value_update(DPID_GET_MOS_TEMPERATURE,Wifi_Mos_Temperature); //VALUE�������ϱ�;
	}
	// ��ѹ����
	if(Wifi_Motor_Current != *p_Motor_Current)
	{
		mcu_dp_value_update(DPID_GET_MOTOR_CURRENT,*p_Motor_Current); //VALUE�������ϱ�;
		Wifi_Motor_Current = *p_Motor_Current;
	}
	if(Wifi_Motor_Bus_Voltage != *p_Motor_Bus_Voltage)
	{
		mcu_dp_value_update(DPID_MOTOR_BUS_VOLTAGE,*p_Motor_Bus_Voltage); //VALUE�������ϱ�;
		Wifi_Motor_Bus_Voltage = *p_Motor_Bus_Voltage;
	}
	// ʵ��ת��
	if(Wifi_Motor_Reality_Speed != *p_Motor_Reality_Speed)
	{
		mcu_dp_value_update(DPID_MOTOR_REALITY_SPEED,*p_Motor_Reality_Speed); //VALUE�������ϱ�;
		Wifi_Motor_Reality_Speed = *p_Motor_Reality_Speed;
	}
	
	// �·� ʵ��ת��
	if(Wifi_Send_Reality_Speed != *p_Send_Reality_Speed)
	{
		mcu_dp_value_update(DPID_SEND_REALITY_SPEED,*p_Send_Reality_Speed); //VALUE�������ϱ�;
		Wifi_Send_Reality_Speed = *p_Send_Reality_Speed;
	}
	
	// Ĭ�ϲ���
	if(Wifi_OP_Free_Mode_Speed != p_OP_Free_Mode->speed)
	{
		mcu_dp_value_update(DPID_FREE_MODE_SPEEN,p_OP_Free_Mode->speed); //VALUE�������ϱ�;
		Wifi_OP_Free_Mode_Speed = p_OP_Free_Mode->speed;
	}
	if(Wifi_OP_Time_Mode_Speed != p_OP_Timing_Mode->speed)
	{
		mcu_dp_value_update(DPID_TIMING_MODE_SPEEN,p_OP_Timing_Mode->speed); //VALUE�������ϱ�;
		Wifi_OP_Time_Mode_Speed = p_OP_Timing_Mode->speed;
	}
	if(Wifi_OP_Time_Mode_Time != p_OP_Timing_Mode->time)
	{
		mcu_dp_value_update(DPID_TIMING_MODE_TIME,p_OP_Timing_Mode->time); //VALUE�������ϱ�;
		Wifi_OP_Time_Mode_Time = p_OP_Timing_Mode->time;
	}
	
	if(State_Upload_Cnt++ > WIFI_DATE_UPLOAD_TIME)
	{
		State_Upload_Cnt = 0;
		// debug ��
		mcu_dp_value_update(DPID_SYSTEM_RUNNING_TIME,		*p_System_Runing_Second_Cnt); //VALUE�������ϱ�;
		mcu_dp_value_update(DPID_NO_OPERATION_TIME,			*p_No_Operation_Second_Cnt); //VALUE�������ϱ�;
		mcu_dp_value_update(DPID_SYSTEM_SLEEPING_TIME,	*p_System_Sleeping_Second_Cnt); //VALUE�������ϱ�;
	}
}

//------------------- �ϴ�״̬���� ----------------------------
void WIFI_Get_Work_State(void)
{
	switch(mcu_get_wifi_work_state())
	{
		case SMART_CONFIG_STATE:
		//���� Smart ����״̬���� LED ����
		WIFI_Set_Machine_State( WIFI_DISTRIBUTION );
		break;
		case AP_STATE:
		//���� AP ����״̬���� LED ����
		WIFI_Set_Machine_State( WIFI_DISTRIBUTION );
		break;
		case WIFI_NOT_CONNECTED:
		//Wi-Fi ������ɣ���������·�������� LED ����
		//if(WIFI_Get_Machine_State() != WIFI_ERROR)
		WIFI_Set_Machine_State( WIFI_ERROR );
		break;
		case WIFI_CONNECTED:
		//·�������ӳɹ����� LED ����
		WIFI_Set_Machine_State(WIFI_WORKING);
		break;
		case WIFI_CONN_CLOUD:
		//������·���������ӵ��ƶˣ����������������ɿ��ƣ��� LED ����
		WIFI_Set_Machine_State(WIFI_WORKING);
		break;
		case WIFI_LOW_POWER:
		//�͹���ģʽ�� �� LED ����
		WIFI_Set_Machine_State(WIFI_WORKING);
		break;
		case SMART_AND_AP_STATE:
		//�������ȵ�ģʽ��������״̬�� �� LED ����
		WIFI_Set_Machine_State(WIFI_WORKING);
		break;
		default:
			//if(WIFI_Get_Machine_State() != WIFI_ERROR)
				WIFI_Set_Machine_State(WIFI_NO_CONNECT);
		break;
	}
}


// ��ʼ��
void wifi_Module_Init(void)
{
	wifi_protocol_init();
}

// ������ѭ������
//  20 ms
void Wifi_Module_Handler(void)
{
	
	wifi_uart_service();
	
	WIFI_Update_State_Upload();
	
	WIFI_Get_Work_State();
	
}


