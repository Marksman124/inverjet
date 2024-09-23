/**
******************************************************************************
* @file				wifi.c
* @brief			wifi 模组
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

//------------------- 设置wifi状态机 ----------------------------
void WIFI_Set_Machine_State(WIFI_STATE_MODE_E para)
{
	if(para <= WIFI_ERROR)
	{
		WIFI_State_Machine = para;
	}
}
	
//------------------- 获取wifi状态机 ----------------------------
WIFI_STATE_MODE_E WIFI_Get_Machine_State(void)
{
	return WIFI_State_Machine;
}


//------------------- 接收处理函数 ----------------------------
void WIFI_Read_Handler(void)
{

}

//------------------- 进入配网 ----------------------------
void WIFI_Get_In_Distribution(void)
{
	if(WIFI_Get_Machine_State() != WIFI_DISTRIBUTION)
	{
		WIFI_Set_Machine_State( WIFI_DISTRIBUTION );
		
		
		// 复位模组
		mcu_reset_wifi();
		
		// 设置wifi模式
		mcu_set_wifi_mode(SMART_CONFIG);
	}
}

//------------------- 进入故障 ----------------------------
void WIFI_Get_In_Error(void)
{
	// 设置图标
	WIFI_Set_Machine_State( WIFI_ERROR );
	
}


//------------------- 上传状态更新 ----------------------------
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
	
	//故障上传
	if(Wifi_Motor_Fault_Static != *p_Motor_Fault_Static)
	{
		mcu_dp_fault_update(DPID_DEVICE_ERROR_CODE,*p_Motor_Fault_Static); //故障型数据上报;
		Wifi_Motor_Fault_Static = *p_Motor_Fault_Static;
	}
	
	//故障上传
	if(Wifi_System_Fault_Static != *p_System_Fault_Static)
	{
		mcu_dp_fault_update(DPID_GET_SYSTEM_FAULT_STATUS,*p_System_Fault_Static); //故障型数据上报;
		Wifi_System_Fault_Static = *p_System_Fault_Static;
	}
	// 系统状态 (重要)
	if(Wifi_PMode_Now != *p_PMode_Now)
	{
		mcu_dp_value_update(DPID_SYSTEM_WORKING_MODE,*p_PMode_Now); //VALUE型数据上报;
		Wifi_PMode_Now = *p_PMode_Now;
	}
	if(Wifi_System_State_Machine != *p_System_State_Machine)
	{
		mcu_dp_value_update(DPID_SYSTEM_WORKING_STATUS,*p_System_State_Machine); //VALUE型数据上报;
		Wifi_System_State_Machine = *p_System_State_Machine;
	}
	
	if(Wifi_OP_ShowNow_Speed != *p_OP_ShowNow_Speed)
	{
		mcu_dp_value_update(DPID_MOTOR_CURRENT_SPEED,*p_OP_ShowNow_Speed); //VALUE型数据上报;
		Wifi_OP_ShowNow_Speed = *p_OP_ShowNow_Speed;
	}
	
	if(Wifi_OP_ShowNow_Time != *p_OP_ShowNow_Time)
	{
		mcu_dp_value_update(DPID_MOTOR_CURRENT_TIME,*p_OP_ShowNow_Time); //VALUE型数据上报;
		Wifi_OP_ShowNow_Time = *p_OP_ShowNow_Time;
	}
	// 温度  
	memcpy(&box_temperature, p_Box_Temperature, 2);
	if(Wifi_Box_Temperature != box_temperature)
	{
		Wifi_Box_Temperature = box_temperature;
		mcu_dp_value_update(DPID_GET_BOX_TEMPERATURE,Wifi_Box_Temperature); //VALUE型数据上报;
	}
	
	memcpy(&mos_temperature, p_Mos_Temperature, 2);
	if(Wifi_Mos_Temperature != mos_temperature)
	{
		Wifi_Mos_Temperature = mos_temperature;
		mcu_dp_value_update(DPID_GET_MOS_TEMPERATURE,Wifi_Mos_Temperature); //VALUE型数据上报;
	}
	// 电压电流
	if(Wifi_Motor_Current != *p_Motor_Current)
	{
		mcu_dp_value_update(DPID_GET_MOTOR_CURRENT,*p_Motor_Current); //VALUE型数据上报;
		Wifi_Motor_Current = *p_Motor_Current;
	}
	if(Wifi_Motor_Bus_Voltage != *p_Motor_Bus_Voltage)
	{
		mcu_dp_value_update(DPID_MOTOR_BUS_VOLTAGE,*p_Motor_Bus_Voltage); //VALUE型数据上报;
		Wifi_Motor_Bus_Voltage = *p_Motor_Bus_Voltage;
	}
	// 实际转速
	if(Wifi_Motor_Reality_Speed != *p_Motor_Reality_Speed)
	{
		mcu_dp_value_update(DPID_MOTOR_REALITY_SPEED,*p_Motor_Reality_Speed); //VALUE型数据上报;
		Wifi_Motor_Reality_Speed = *p_Motor_Reality_Speed;
	}
	
	// 下发 实际转速
	if(Wifi_Send_Reality_Speed != *p_Send_Reality_Speed)
	{
		mcu_dp_value_update(DPID_SEND_REALITY_SPEED,*p_Send_Reality_Speed); //VALUE型数据上报;
		Wifi_Send_Reality_Speed = *p_Send_Reality_Speed;
	}
	
	// 默认参数
	if(Wifi_OP_Free_Mode_Speed != p_OP_Free_Mode->speed)
	{
		mcu_dp_value_update(DPID_FREE_MODE_SPEEN,p_OP_Free_Mode->speed); //VALUE型数据上报;
		Wifi_OP_Free_Mode_Speed = p_OP_Free_Mode->speed;
	}
	if(Wifi_OP_Time_Mode_Speed != p_OP_Timing_Mode->speed)
	{
		mcu_dp_value_update(DPID_TIMING_MODE_SPEEN,p_OP_Timing_Mode->speed); //VALUE型数据上报;
		Wifi_OP_Time_Mode_Speed = p_OP_Timing_Mode->speed;
	}
	if(Wifi_OP_Time_Mode_Time != p_OP_Timing_Mode->time)
	{
		mcu_dp_value_update(DPID_TIMING_MODE_TIME,p_OP_Timing_Mode->time); //VALUE型数据上报;
		Wifi_OP_Time_Mode_Time = p_OP_Timing_Mode->time;
	}
	
	if(State_Upload_Cnt++ > WIFI_DATE_UPLOAD_TIME)
	{
		State_Upload_Cnt = 0;
		// debug 用
		mcu_dp_value_update(DPID_SYSTEM_RUNNING_TIME,		*p_System_Runing_Second_Cnt); //VALUE型数据上报;
		mcu_dp_value_update(DPID_NO_OPERATION_TIME,			*p_No_Operation_Second_Cnt); //VALUE型数据上报;
		mcu_dp_value_update(DPID_SYSTEM_SLEEPING_TIME,	*p_System_Sleeping_Second_Cnt); //VALUE型数据上报;
	}
}

//------------------- 上传状态更新 ----------------------------
void WIFI_Get_Work_State(void)
{
	switch(mcu_get_wifi_work_state())
	{
		case SMART_CONFIG_STATE:
		//处于 Smart 配置状态，即 LED 快闪
		WIFI_Set_Machine_State( WIFI_DISTRIBUTION );
		break;
		case AP_STATE:
		//处于 AP 配置状态，即 LED 慢闪
		WIFI_Set_Machine_State( WIFI_DISTRIBUTION );
		break;
		case WIFI_NOT_CONNECTED:
		//Wi-Fi 配置完成，正在连接路由器，即 LED 常暗
		//if(WIFI_Get_Machine_State() != WIFI_ERROR)
		WIFI_Set_Machine_State( WIFI_ERROR );
		break;
		case WIFI_CONNECTED:
		//路由器连接成功，即 LED 常亮
		WIFI_Set_Machine_State(WIFI_WORKING);
		break;
		case WIFI_CONN_CLOUD:
		//已连上路由器且连接到云端，局域网和外网均可控制，即 LED 常亮
		WIFI_Set_Machine_State(WIFI_WORKING);
		break;
		case WIFI_LOW_POWER:
		//低功耗模式， 即 LED 常暗
		WIFI_Set_Machine_State(WIFI_WORKING);
		break;
		case SMART_AND_AP_STATE:
		//快连和热点模式共存配置状态， 即 LED 常亮
		WIFI_Set_Machine_State(WIFI_WORKING);
		break;
		default:
			//if(WIFI_Get_Machine_State() != WIFI_ERROR)
				WIFI_Set_Machine_State(WIFI_NO_CONNECT);
		break;
	}
}


// 初始化
void wifi_Module_Init(void)
{
	wifi_protocol_init();
}

// 按键主循环任务
//  20 ms
void Wifi_Module_Handler(void)
{
	
	wifi_uart_service();
	
	WIFI_Update_State_Upload();
	
	WIFI_Get_Work_State();
	
}


