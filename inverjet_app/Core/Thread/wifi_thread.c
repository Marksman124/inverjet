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
	static uint32_t Wifi_Motor_Current = 0;
	static uint32_t Wifi_Motor_Reality_Speed = 0;
	static uint32_t Wifi_Send_Reality_Speed = 0;

	static uint32_t Wifi_Simulated_Swim_Distance = 0;
	
	static uint16_t Wifi_Motor_Reality_Power = 0;
	static uint16_t Wifi_Motor_Bus_Voltage = 0;
	static uint16_t Wifi_Motor_Bus_Current = 0;
	
	static uint16_t Wifi_OP_Free_Mode_Speed = 0;
	static uint16_t Wifi_OP_Time_Mode_Speed = 0;
	static uint16_t Wifi_OP_Time_Mode_Time = 0;
	
	short int box_temperature = 0;
	short int mos_temperature = 0;
		
	static uint16_t State_Upload_Cnt=0;
	
	//------------------- �����ϴ� ----------------------------
	if(Wifi_Motor_Fault_Static != *p_Motor_Fault_Static)
	{
		mcu_dp_fault_update(DPID_DEVICE_ERROR_CODE,*p_Motor_Fault_Static);					// ���������
		Wifi_Motor_Fault_Static = *p_Motor_Fault_Static;
	}
	if(Wifi_System_Fault_Static != *p_System_Fault_Static)
	{
		mcu_dp_fault_update(DPID_GET_SYSTEM_FAULT_STATUS,*p_System_Fault_Static); 	// ϵͳ �����������ϱ�
		Wifi_System_Fault_Static = *p_System_Fault_Static;
	}
	//------------------- ϵͳ״̬ (��Ҫ)  ----------------------------
	if(Wifi_PMode_Now != *p_PMode_Now)
	{
		mcu_dp_value_update(DPID_SYSTEM_WORKING_MODE,*p_PMode_Now); 							// ����ģʽ
		Wifi_PMode_Now = *p_PMode_Now;
	}
	if(Wifi_System_State_Machine != *p_System_State_Machine)
	{
		mcu_dp_value_update(DPID_SYSTEM_WORKING_STATUS,*p_System_State_Machine); 	// ״̬��
		Wifi_System_State_Machine = *p_System_State_Machine;
	}
	if(Wifi_OP_ShowNow_Speed != *p_OP_ShowNow_Speed)
	{
		mcu_dp_value_update(DPID_MOTOR_CURRENT_SPEED,*p_OP_ShowNow_Speed); 				// ��ǰ�ٶ�
		Wifi_OP_ShowNow_Speed = *p_OP_ShowNow_Speed;
	}
	if(Wifi_OP_ShowNow_Time != *p_OP_ShowNow_Time)
	{
		mcu_dp_value_update(DPID_MOTOR_CURRENT_TIME,*p_OP_ShowNow_Time); 					// ��ǰʱ��
		Wifi_OP_ShowNow_Time = *p_OP_ShowNow_Time;
	}
	//*********************************
	//===== һ������ 1s �ϴ�   =========
	//*********************************
	if((State_Upload_Cnt % WIFI_DATE_UPLOAD_TIME_NORMAL)==0)
	{
		//------------------- Debug ����  ----------------------------
		if(Wifi_Simulated_Swim_Distance != *p_Simulated_Swim_Distance)
		{
			//mcu_dp_value_update(DPID_MOTOR_CURRENT_TIME,*p_Simulated_Swim_Distance); //��Ӿ����
			Wifi_Simulated_Swim_Distance = *p_Simulated_Swim_Distance;
		}
		memcpy(&box_temperature, p_Box_Temperature, 2);
		if(Wifi_Box_Temperature != box_temperature)
		{
			Wifi_Box_Temperature = box_temperature;
			mcu_dp_value_update(DPID_GET_BOX_TEMPERATURE,Wifi_Box_Temperature); 			// �����¶�
		}
		memcpy(&mos_temperature, p_Mos_Temperature, 2);
		if(Wifi_Mos_Temperature != mos_temperature)
		{
			Wifi_Mos_Temperature = mos_temperature;
			mcu_dp_value_update(DPID_GET_MOS_TEMPERATURE,Wifi_Mos_Temperature); 			// MOS�¶�
		}
		if(Wifi_Motor_Current != *p_Motor_Current)
		{
			mcu_dp_value_update(DPID_GET_MOTOR_CURRENT,*p_Motor_Current); 						// ������� �������
			Wifi_Motor_Current = *p_Motor_Current;
		}
		if(Wifi_Motor_Bus_Voltage != *p_Motor_Bus_Voltage)
		{
			mcu_dp_value_update(DPID_MOTOR_BUS_VOLTAGE,*p_Motor_Bus_Voltage); 				// �����ѹ ��ĸ�ߣ�
			Wifi_Motor_Bus_Voltage = *p_Motor_Bus_Voltage;
		}
		if(Wifi_Motor_Bus_Current != *p_Motor_Bus_Current)
		{
			mcu_dp_value_update(DPID_MOTOR_BUS_CURRENTE,*p_Motor_Bus_Current); 				// ���������ĸ�ߣ�
			Wifi_Motor_Bus_Current = *p_Motor_Bus_Current;
		}
		
		if(Wifi_Motor_Reality_Speed != *p_Motor_Reality_Speed)
		{
			mcu_dp_value_update(DPID_MOTOR_REALITY_SPEED,*p_Motor_Reality_Speed); 		// ʵ��ת��
			Wifi_Motor_Reality_Speed = *p_Motor_Reality_Speed;
		}
		if(Wifi_Send_Reality_Speed != *p_Send_Reality_Speed)
		{
			mcu_dp_value_update(DPID_SEND_REALITY_SPEED,*p_Send_Reality_Speed); 			// �·�ת��
			Wifi_Send_Reality_Speed = *p_Send_Reality_Speed;
		}
		if(Wifi_Motor_Reality_Power != *p_Motor_Reality_Power)
		{
			mcu_dp_value_update(DPID_MOTOR_POWER,*p_Motor_Reality_Power); 						// ��ǰ����
			Wifi_Motor_Reality_Power = *p_Motor_Reality_Power;
		}
		if(Wifi_Simulated_Swim_Distance != *p_Simulated_Swim_Distance)
		{
			mcu_dp_value_update(DPID_SWIMMING_DISTANCE,*p_Simulated_Swim_Distance); 	// ģ����Ӿ����
			Wifi_Simulated_Swim_Distance = *p_Simulated_Swim_Distance;
		}
		
		//------------------- Ĭ�� ϵͳ����  ----------------------------
		if(Wifi_OP_Free_Mode_Speed != p_OP_Free_Mode->speed)
		{
			mcu_dp_value_update(DPID_FREE_MODE_SPEEN,p_OP_Free_Mode->speed); 					// ����ģʽ �ٶ�
			Wifi_OP_Free_Mode_Speed = p_OP_Free_Mode->speed;
		}
		if(Wifi_OP_Time_Mode_Speed != p_OP_Timing_Mode->speed)
		{
			mcu_dp_value_update(DPID_TIMING_MODE_SPEEN,p_OP_Timing_Mode->speed); 			// ��ʱģʽ �ٶ�
			Wifi_OP_Time_Mode_Speed = p_OP_Timing_Mode->speed;
		}
		if(Wifi_OP_Time_Mode_Time != p_OP_Timing_Mode->time)
		{
			mcu_dp_value_update(DPID_TIMING_MODE_TIME,p_OP_Timing_Mode->time); 				// ��ʱģʽ ʱ��
			Wifi_OP_Time_Mode_Time = p_OP_Timing_Mode->time;
		}
	}
	//*********************************
	//===== ����Ҫ���� 10s �ϴ�   ======
	//*********************************
	if((State_Upload_Cnt % WIFI_DATE_UPLOAD_TIME)==0)
	{
		// debug ��
		mcu_dp_value_update(DPID_SYSTEM_RUNNING_TIME,		*p_System_Runing_Second_Cnt); 		// ����ʱ��;
		mcu_dp_value_update(DPID_NO_OPERATION_TIME,			*p_No_Operation_Second_Cnt); 			// �޲���ʱ��;
		mcu_dp_value_update(DPID_SYSTEM_STARTUP_TIME,	*p_System_Startup_Second_Cnt); 			// ����ʱ��;
		// �²�ͬ�� ����һ��
		//mcu_dp_value_update(DPID_SYSTEM_WORKING_MODE,*p_PMode_Now);  				// ����ģʽ;
		//mcu_dp_value_update(DPID_MOTOR_CURRENT_SPEED,*p_OP_ShowNow_Speed); 	// ��ǰ�ٶ�;
	}
	
	State_Upload_Cnt++;
	//State_Upload_Cnt = 0;
}

//------------------- wifi ״̬  ͼ�� ----------------------------
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


