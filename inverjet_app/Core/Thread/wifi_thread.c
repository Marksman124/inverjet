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
#include "fault.h"
#include "debug_protocol.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static WIFI_STATE_MODE_E WIFI_State_Machine = WIFI_NO_CONNECT;

static uint8_t Upload_Finish_Data=0;
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
	
	static uint16_t Wifi_Motor_Reality_Power = 0;
	static uint16_t Wifi_Motor_Bus_Voltage = 0;
	static uint16_t Wifi_Motor_Bus_Current = 0;
	
	static uint16_t Wifi_OP_Free_Mode_Speed = 0;
	static uint16_t Wifi_OP_Time_Mode_Speed = 0;
	static uint16_t Wifi_OP_Time_Mode_Time = 0;
	
	short int box_temperature = 0;
	short int mos_temperature = 0;
		
	static uint16_t Upload_Timer_Cnt = 0;						// �ϴ�ʱ�� ��ʱ��
	static uint8_t Upload_Second_Cnt = 0;						// �ϴ�ʱ�� ��
	//------------------- ͳ���ϴ�	----------------------------
	if(Upload_Finish_Data == 1)
	{
		Upload_Finish_Data = 0;
		Wifi_DP_Data_Update(DPID_FINISH_STATISTICS_TIME); 		//	���ͳ�� --> ʱ��
		Wifi_DP_Data_Update(DPID_FINISH_STATISTICS_SEED); 		//	���ͳ�� --> ǿ��
		Wifi_DP_Data_Update(DPID_FINISH_STATISTICS_DISTANCE); //	���ͳ�� --> ��Ӿ����
		Finish_Statistics_Clean();
	}
	//------------------- �����ϴ� ----------------------------
	if(Wifi_Motor_Fault_Static != *p_Motor_Fault_Static)
	{
		Wifi_DP_Data_Update(DPID_DEVICE_ERROR_CODE);					// ���������
		Wifi_Motor_Fault_Static = *p_Motor_Fault_Static;
	}
	if(Wifi_System_Fault_Static != *p_System_Fault_Static)
	{
		if(Fault_Check_Status_Legal(*p_System_Fault_Static))
		{
			Wifi_DP_Data_Update(DPID_GET_SYSTEM_FAULT_STATUS); 	// ϵͳ �����������ϱ�
			Wifi_System_Fault_Static = *p_System_Fault_Static;
		}
	}
	//------------------- ϵͳ״̬ (��Ҫ)  ----------------------------
	if((Wifi_PMode_Now != Get_System_State_Mode())||(Wifi_System_State_Machine != Get_System_State_Machine()))
	{
		Wifi_DP_Data_Update(DPID_SYSTEM_STATUS_MODE);			// �ϲ�����;
		if(Wifi_PMode_Now != Get_System_State_Mode())
		{
			Wifi_DP_Data_Update(DPID_SYSTEM_WORKING_MODE);			// ����ģʽ;
			Wifi_PMode_Now = Get_System_State_Mode();
		}
		if(Wifi_System_State_Machine != Get_System_State_Machine())
		{
			Wifi_DP_Data_Update(DPID_SYSTEM_WORKING_STATUS);			// ״̬��;
			Wifi_System_State_Machine = Get_System_State_Machine();
		}
	}
	if(Wifi_OP_ShowNow_Speed != *p_OP_ShowNow_Speed)
	{
		Wifi_DP_Data_Update(DPID_MOTOR_CURRENT_SPEED); 				// ��ǰ�ٶ�
		Wifi_OP_ShowNow_Speed = *p_OP_ShowNow_Speed;
	}
	if(Wifi_OP_ShowNow_Time != *p_OP_ShowNow_Time)
	{
		Wifi_DP_Data_Update(DPID_MOTOR_CURRENT_TIME); 					// ��ǰʱ��
		Wifi_OP_ShowNow_Time = *p_OP_ShowNow_Time;
	}
	//*********************************
	//===== һ������ 1s �ϴ�   =========
	//*********************************
	if((Upload_Timer_Cnt % WIFI_DATE_UPLOAD_TIME_NORMAL)==0)
	{
		mcu_get_system_time();	//��ʱ��
		//------------------- Debug ����  ----------------------------
		memcpy(&box_temperature, p_Box_Temperature, 2);
		if(Wifi_Box_Temperature != box_temperature)
		{
			Wifi_Box_Temperature = box_temperature;
			Wifi_DP_Data_Update(DPID_GET_BOX_TEMPERATURE); 			// �����¶�
		}
		memcpy(&mos_temperature, p_Mos_Temperature, 2);
		if(Wifi_Mos_Temperature != mos_temperature)
		{
			Wifi_Mos_Temperature = mos_temperature;
			Wifi_DP_Data_Update(DPID_GET_MOS_TEMPERATURE); 			// MOS�¶�
		}
		if(Wifi_Motor_Current != *p_Motor_Current)
		{
			Wifi_DP_Data_Update(DPID_GET_MOTOR_CURRENT); 				// ������� �������
			Wifi_Motor_Current = *p_Motor_Current;
		}
		if(Wifi_Motor_Bus_Voltage != *p_Motor_Bus_Voltage)
		{
			Wifi_DP_Data_Update(DPID_MOTOR_BUS_VOLTAGE); 				// �����ѹ ��ĸ�ߣ�
			Wifi_Motor_Bus_Voltage = *p_Motor_Bus_Voltage;
		}
		if(Wifi_Motor_Bus_Current != *p_Motor_Bus_Current)
		{
			Wifi_DP_Data_Update(DPID_MOTOR_BUS_CURRENTE); 			// ���������ĸ�ߣ�
			Wifi_Motor_Bus_Current = *p_Motor_Bus_Current;
		}
		
		if(Wifi_Motor_Reality_Speed != *p_Motor_Reality_Speed)
		{
			Wifi_DP_Data_Update(DPID_MOTOR_REALITY_SPEED); 			// ʵ��ת��
			Wifi_Motor_Reality_Speed = *p_Motor_Reality_Speed;
		}
		if(Wifi_Send_Reality_Speed != *p_Send_Reality_Speed)
		{
			Wifi_DP_Data_Update(DPID_SEND_REALITY_SPEED); 			// �·�ת��
			Wifi_Send_Reality_Speed = *p_Send_Reality_Speed;
		}
		if(Wifi_Motor_Reality_Power != *p_Motor_Reality_Power)
		{
			Wifi_DP_Data_Update(DPID_MOTOR_POWER); 							// ��ǰ����
			Wifi_Motor_Reality_Power = *p_Motor_Reality_Power;
		}
		
		//------------------- Ĭ�� ϵͳ����  ----------------------------
		if(Wifi_OP_Free_Mode_Speed != p_OP_Free_Mode->speed)
		{
			Wifi_DP_Data_Update(DPID_FREE_MODE_SPEEN); 					// ����ģʽ �ٶ�
			Wifi_OP_Free_Mode_Speed = p_OP_Free_Mode->speed;
		}
		if(Wifi_OP_Time_Mode_Speed != p_OP_Timing_Mode->speed)
		{
			Wifi_DP_Data_Update(DPID_TIMING_MODE_SPEEN); 				// ��ʱģʽ �ٶ�
			Wifi_OP_Time_Mode_Speed = p_OP_Timing_Mode->speed;
		}
		if(Wifi_OP_Time_Mode_Time != p_OP_Timing_Mode->time)
		{
			Wifi_DP_Data_Update(DPID_TIMING_MODE_TIME); 				// ��ʱģʽ ʱ��
			Wifi_OP_Time_Mode_Time = p_OP_Timing_Mode->time;
		}
		
		//*********************************
		//===== ����Ҫ���� 10s �ϴ�   ======
		//*********************************
		if((*p_Wifi_DP_Upload_Level & 0xFF) > 0x80)
		{
			if((Upload_Second_Cnt % (*p_Wifi_DP_Upload_Level & 0x7F))==0)
			{
				// debug ��
				Wifi_DP_Data_Update(DPID_SYSTEM_RUNNING_TIME); 		// ����ʱ��;
				Wifi_DP_Data_Update(DPID_NO_OPERATION_TIME); 			// �޲���ʱ��;
				Wifi_DP_Data_Update(DPID_SYSTEM_STARTUP_TIME); 		// ����ʱ��;
				Wifi_DP_Data_Update(DPID_THREAD_ACTIVITY_SIGN); 	// �̻߳��־;
				
				Wifi_DP_Data_Update(DPID_DRIVE_NTC_TEMP_01);			// ������ NTC �¶� 01
				Wifi_DP_Data_Update(DPID_DRIVE_NTC_TEMP_02);			// ������ NTC �¶� 01
				Wifi_DP_Data_Update(DPID_DRIVE_NTC_TEMP_03);			// ������ NTC �¶� 01
			}
		}
		
		if(Upload_Second_Cnt < 99)
			Upload_Second_Cnt ++;
		else
			Upload_Second_Cnt = 0;
	}
	
	if(Upload_Timer_Cnt < 9999)
		Upload_Timer_Cnt ++;
	else
		Upload_Timer_Cnt = 0;
}



//-------------- �ϴ� ���ͳ��  -------------------
void WIFI_Finish_Statistics_Upload( void )
{
	if(* p_Finish_Statistics_Time == 0)
		return;
	
	if(*p_Finish_Statistics_Time >= WIFI_STATISTICE_UPLOAD_MINIMUM_TIME)
	{
		DEBUG_PRINT("\n�ϴ�ͳ������:\tʱ��:\t%d\tǿ��:\t%d\t����:\t%d\t\n",*p_Finish_Statistics_Time,*p_Finish_Statistics_Speed,*p_Finish_Statistics_Distance);

	}
	else
	{
		DEBUG_PRINT("\nͳ��ʱ�����3���ӣ��ϴ� %d\n",* p_Finish_Statistics_Time);
		Finish_Statistics_Clean();
	}
	Upload_Finish_Data = 1;
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
	static uint32_t WIFI_Rx_Timer_cnt= 0;
	
	Thread_Activity_Sign_Set(THREAD_ACTIVITY_WIFI);
	
	wifi_uart_service();
	
	if(IS_CHECK_ERROR_MODE())
	{
		// ===================  ͨѶ����
		if(get_mcu_reset_state() == FALSE)
			WIFI_Rx_Timer_cnt++;  //ͨ�Ź��ϼ�����
		else
			WIFI_Rx_Timer_cnt = 0;
		
		if(WIFI_Rx_Timer_cnt > FAULT_WIFI_LOST_TIME)
		{
			Set_Motor_Fault_State(E301_WIFI_HARDWARE);
		}
		// =================== wifi �źŹ��� 
		if(*p_WIFI_Rssi < WIFI_RSSI_ERROR_VAULE)
		{
			DEBUG_PRINT("wifiģ�����: �ź�ǿ�� %d dBm   ( �ϸ�: %d dBm)\n",*p_WIFI_Rssi, WIFI_RSSI_ERROR_VAULE);
			Set_Motor_Fault_State(E301_WIFI_HARDWARE);
		}
	}
	
	if(System_is_OTA() == 0)
	{
		WIFI_Update_State_Upload();
		
		WIFI_Get_Work_State();
	}
	
}


//  �ϴ�dp�� (ͳһ�ӿ�)
void Wifi_DP_Data_Update(uint16_t id)
{
	switch(id)
	{
		//*********************************
		//========== ��̨����  =============
		//*********************************
		case DPID_PREPARATION_TIME:// ׼��ʱ��(APP����,����ֻ���𱣴�)
			mcu_dp_value_update(DPID_PREPARATION_TIME,				*p_Preparation_Time_BIT);
			break;
		case DPID_DEVICE_ERROR_CODE:// ���������
			mcu_dp_value_update(DPID_DEVICE_ERROR_CODE,				*p_Motor_Fault_Static);
			break;
		case DPID_GET_SYSTEM_FAULT_STATUS:// ϵͳ�����ϱ�
			
			mcu_dp_fault_update(DPID_GET_SYSTEM_FAULT_STATUS,	*p_System_Fault_Static);
			break;
		case DPID_GET_MOS_TEMPERATURE:// MOS�¶�
			mcu_dp_value_update(DPID_GET_MOS_TEMPERATURE,			*p_Mos_Temperature);
			break;
		case DPID_GET_BOX_TEMPERATURE:// �����¶�
			mcu_dp_value_update(DPID_GET_BOX_TEMPERATURE,			*p_Box_Temperature);
			break;
		case DPID_GET_MOTOR_CURRENT:// ������������)
			mcu_dp_value_update(DPID_GET_MOTOR_CURRENT,				*p_Motor_Current);
			break;
		case DPID_MOTOR_REALITY_SPEED:// ʵ��ת��
			mcu_dp_value_update(DPID_MOTOR_REALITY_SPEED,			*p_Motor_Reality_Speed/5);
			break;
		case DPID_MOTOR_BUS_VOLTAGE:// �����ѹ��ĸ�ߣ�
			mcu_dp_value_update(DPID_MOTOR_BUS_VOLTAGE,				*p_Motor_Bus_Voltage);
			break;
		case DPID_SEND_REALITY_SPEED:// �·�ת��
			mcu_dp_value_update(DPID_SEND_REALITY_SPEED,			*p_Send_Reality_Speed/5);
			break;
		case DPID_MOTOR_POWER:// ��ǰ����
			mcu_dp_value_update(DPID_MOTOR_POWER,							*p_Motor_Reality_Power);
			break;
		case DPID_MOTOR_BUS_CURRENTE:// ���������ĸ�ߣ�
			mcu_dp_value_update(DPID_MOTOR_BUS_CURRENTE,			*p_Motor_Bus_Current);
			break;
		case DPID_SYSTEM_RUNNING_TIME:// ����ʱ��
			mcu_dp_value_update(DPID_SYSTEM_RUNNING_TIME,			*p_System_Runing_Second_Cnt);
			break;
		case DPID_NO_OPERATION_TIME:// �޲���ʱ��
			mcu_dp_value_update(DPID_NO_OPERATION_TIME,				*p_No_Operation_Second_Cnt);
			break;
		case DPID_SYSTEM_STARTUP_TIME:// ���� ʱ��
			mcu_dp_value_update(DPID_SYSTEM_STARTUP_TIME,			*p_System_Startup_Second_Cnt);
			break;
		case DPID_THREAD_ACTIVITY_SIGN:// �̻߳��־
			mcu_dp_value_update(DPID_THREAD_ACTIVITY_SIGN,			*p_Thread_Activity_Sign);
			break;
			//*********************************
			//========== ��ǰ״̬��  ===========
			//*********************************
		case DPID_SYSTEM_WORKING_MODE:// ����ģʽ
			mcu_dp_enum_update(DPID_SYSTEM_WORKING_MODE,			Get_System_State_Mode());
			break;
		case DPID_SYSTEM_WORKING_STATUS:// ״̬��
			/*if(System_is_Error())	//�ǹ��� �ϴ�  wuqingguang
				mcu_dp_enum_update(DPID_SYSTEM_WORKING_STATUS,		*p_System_State_Machine_Memory);
			else*/
				mcu_dp_enum_update(DPID_SYSTEM_WORKING_STATUS,		Get_System_State_Machine());
			break;
		case DPID_MOTOR_CURRENT_SPEED:// ��ǰ�ٶ�
			mcu_dp_value_update(DPID_MOTOR_CURRENT_SPEED,			*p_OP_ShowNow_Speed);
			break;
		case DPID_MOTOR_CURRENT_TIME:// ��ǰʱ��
			mcu_dp_value_update(DPID_MOTOR_CURRENT_TIME,			*p_OP_ShowNow_Time);
			break;
		case DPID_SYSTEM_STATUS_MODE:// �ϲ��ϴ�
			/*if(System_is_Error())//�ǹ��� �ϴ�  wuqingguang
				mcu_dp_raw_update(DPID_SYSTEM_STATUS_MODE,(unsigned char *)p_PMode_Now_Memory,	4);
			else*/
				mcu_dp_raw_update(DPID_SYSTEM_STATUS_MODE,(unsigned char *)p_PMode_Now,	4);
			break;
		//*********************************
		//=========== ��ʼֵ  =============
		//*********************************
		case DPID_FREE_MODE_SPEEN:// ����ģʽ �ٶ�
			mcu_dp_value_update(DPID_FREE_MODE_SPEEN,					p_OP_Free_Mode->speed);
			break;
		case DPID_TIMING_MODE_SPEEN:// ��ʱģʽ �ٶ�
			mcu_dp_value_update(DPID_TIMING_MODE_SPEEN,				p_OP_Timing_Mode->speed);
			break;
		case DPID_TIMING_MODE_TIME:// ��ʱģʽ ʱ��
			mcu_dp_value_update(DPID_TIMING_MODE_TIME,				p_OP_Timing_Mode->time);
			break;
		//*********************************
		//========== ѵ���ƻ�  =============
		//*********************************
		case DPID_SET_TRAIN_PLAN_01:// ѵ���ƻ� P1
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_01,	(unsigned char *)p_OP_PMode[0],	TRAINING_MODE_PERIOD_MAX*4);
			break;
		case DPID_SET_TRAIN_PLAN_02:// ѵ���ƻ� P2
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_02,	(unsigned char *)p_OP_PMode[1],	TRAINING_MODE_PERIOD_MAX*4);
			break;
		case DPID_SET_TRAIN_PLAN_03:// ѵ���ƻ� P3
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_03,	(unsigned char *)p_OP_PMode[2],	TRAINING_MODE_PERIOD_MAX*4);
			break;
		case DPID_SET_TRAIN_PLAN_04:// ѵ���ƻ� P4
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_04,	(unsigned char *)p_OP_PMode[3],	TRAINING_MODE_PERIOD_MAX*4); 
			break;
		case DPID_SET_TRAIN_PLAN_05:// ѵ���ƻ� P5
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_05,	(unsigned char *)p_OP_PMode[4],	TRAINING_MODE_PERIOD_MAX*4);
			break;
		//*********************************
		//========== �ϴ�ͳ��  =============
		//*********************************
		case DPID_FINISH_STATISTICS_TIME:// ��ǰ���ͳ��_ʱ��
			mcu_dp_value_update(DPID_FINISH_STATISTICS_TIME,			*p_Finish_Statistics_Time);
			break;
		case DPID_FINISH_STATISTICS_SEED:// ��ǰ���ͳ��_��Ӿǿ��
			mcu_dp_value_update(DPID_FINISH_STATISTICS_SEED,			*p_Finish_Statistics_Speed);
			break;
		case DPID_FINISH_STATISTICS_DISTANCE:// ��ǰ���ͳ��_����
			mcu_dp_value_update(DPID_FINISH_STATISTICS_DISTANCE,	*p_Finish_Statistics_Distance);
			break;
		//*********************************
		//========== �Զ���ƻ�  =============
		//*********************************
		case DPID_CUSTOM_TRAIN_PLAN_01:// ��ǰ�Զ���ѵ���ƻ�_01
			mcu_dp_raw_update(DPID_CUSTOM_TRAIN_PLAN_01,	(unsigned char *)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TRAIN_MODE_SPEED_P6_1),	TRAINING_MODE_PERIOD_MAX*4);
			break;
		case DPID_CUSTOM_TRAIN_PLAN_02:// ��ǰ�Զ���ѵ���ƻ�_02
			mcu_dp_raw_update(DPID_CUSTOM_TRAIN_PLAN_02,	(unsigned char *)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TRAIN_MODE_SPEED_P7_1),	TRAINING_MODE_PERIOD_MAX*4);
			break;
		//case DPID_CUSTOM_TRAIN_PLAN_03:// ��ǰ�Զ���ѵ���ƻ�_03
			//mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_01,	(unsigned char *)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TRAIN_MODE_SPEED_P8_1),	TRAINING_MODE_PERIOD_MAX*4);
			//break;
		//*********************************
		//========== ��ϸ�¶Ȳ���  =============
		//*********************************
		case DPID_DRIVE_NTC_TEMP_01:// ������ NTC �¶ȡ���01
			mcu_dp_value_update(DPID_DRIVE_NTC_TEMP_01,Get_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_01));
			break;
		case DPID_DRIVE_NTC_TEMP_02:// ������ NTC �¶ȡ���02
			mcu_dp_value_update(DPID_DRIVE_NTC_TEMP_02,Get_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_02));
			break;
		case DPID_DRIVE_NTC_TEMP_03:// ������ NTC �¶ȡ���03
			mcu_dp_value_update(DPID_DRIVE_NTC_TEMP_03,Get_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_03));
			break;
		default:
			break;
	}
}




