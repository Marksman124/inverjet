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
	
	//------------------- 故障上传 ----------------------------
	if(Wifi_Motor_Fault_Static != *p_Motor_Fault_Static)
	{
		mcu_dp_fault_update(DPID_DEVICE_ERROR_CODE,*p_Motor_Fault_Static);					// 驱动板故障
		Wifi_Motor_Fault_Static = *p_Motor_Fault_Static;
	}
	if(Wifi_System_Fault_Static != *p_System_Fault_Static)
	{
		mcu_dp_fault_update(DPID_GET_SYSTEM_FAULT_STATUS,*p_System_Fault_Static); 	// 系统 故障型数据上报
		Wifi_System_Fault_Static = *p_System_Fault_Static;
	}
	//------------------- 系统状态 (重要)  ----------------------------
	if(Wifi_PMode_Now != *p_PMode_Now)
	{
		mcu_dp_value_update(DPID_SYSTEM_WORKING_MODE,*p_PMode_Now); 							// 工作模式
		Wifi_PMode_Now = *p_PMode_Now;
	}
	if(Wifi_System_State_Machine != *p_System_State_Machine)
	{
		mcu_dp_value_update(DPID_SYSTEM_WORKING_STATUS,*p_System_State_Machine); 	// 状态机
		Wifi_System_State_Machine = *p_System_State_Machine;
	}
	if(Wifi_OP_ShowNow_Speed != *p_OP_ShowNow_Speed)
	{
		mcu_dp_value_update(DPID_MOTOR_CURRENT_SPEED,*p_OP_ShowNow_Speed); 				// 当前速度
		Wifi_OP_ShowNow_Speed = *p_OP_ShowNow_Speed;
	}
	if(Wifi_OP_ShowNow_Time != *p_OP_ShowNow_Time)
	{
		mcu_dp_value_update(DPID_MOTOR_CURRENT_TIME,*p_OP_ShowNow_Time); 					// 当前时间
		Wifi_OP_ShowNow_Time = *p_OP_ShowNow_Time;
	}
	//*********************************
	//===== 一般数据 1s 上传   =========
	//*********************************
	if((State_Upload_Cnt % WIFI_DATE_UPLOAD_TIME_NORMAL)==0)
	{
		//------------------- Debug 数据  ----------------------------
		if(Wifi_Simulated_Swim_Distance != *p_Simulated_Swim_Distance)
		{
			//mcu_dp_value_update(DPID_MOTOR_CURRENT_TIME,*p_Simulated_Swim_Distance); //游泳距离
			Wifi_Simulated_Swim_Distance = *p_Simulated_Swim_Distance;
		}
		memcpy(&box_temperature, p_Box_Temperature, 2);
		if(Wifi_Box_Temperature != box_temperature)
		{
			Wifi_Box_Temperature = box_temperature;
			mcu_dp_value_update(DPID_GET_BOX_TEMPERATURE,Wifi_Box_Temperature); 			// 机箱温度
		}
		memcpy(&mos_temperature, p_Mos_Temperature, 2);
		if(Wifi_Mos_Temperature != mos_temperature)
		{
			Wifi_Mos_Temperature = mos_temperature;
			mcu_dp_value_update(DPID_GET_MOS_TEMPERATURE,Wifi_Mos_Temperature); 			// MOS温度
		}
		if(Wifi_Motor_Current != *p_Motor_Current)
		{
			mcu_dp_value_update(DPID_GET_MOTOR_CURRENT,*p_Motor_Current); 						// 输出电流 （电机）
			Wifi_Motor_Current = *p_Motor_Current;
		}
		if(Wifi_Motor_Bus_Voltage != *p_Motor_Bus_Voltage)
		{
			mcu_dp_value_update(DPID_MOTOR_BUS_VOLTAGE,*p_Motor_Bus_Voltage); 				// 输入电压 （母线）
			Wifi_Motor_Bus_Voltage = *p_Motor_Bus_Voltage;
		}
		if(Wifi_Motor_Bus_Current != *p_Motor_Bus_Current)
		{
			mcu_dp_value_update(DPID_MOTOR_BUS_CURRENTE,*p_Motor_Bus_Current); 				// 输入电流（母线）
			Wifi_Motor_Bus_Current = *p_Motor_Bus_Current;
		}
		
		if(Wifi_Motor_Reality_Speed != *p_Motor_Reality_Speed)
		{
			mcu_dp_value_update(DPID_MOTOR_REALITY_SPEED,*p_Motor_Reality_Speed); 		// 实际转速
			Wifi_Motor_Reality_Speed = *p_Motor_Reality_Speed;
		}
		if(Wifi_Send_Reality_Speed != *p_Send_Reality_Speed)
		{
			mcu_dp_value_update(DPID_SEND_REALITY_SPEED,*p_Send_Reality_Speed); 			// 下发转速
			Wifi_Send_Reality_Speed = *p_Send_Reality_Speed;
		}
		if(Wifi_Motor_Reality_Power != *p_Motor_Reality_Power)
		{
			mcu_dp_value_update(DPID_MOTOR_POWER,*p_Motor_Reality_Power); 						// 当前功率
			Wifi_Motor_Reality_Power = *p_Motor_Reality_Power;
		}
		if(Wifi_Simulated_Swim_Distance != *p_Simulated_Swim_Distance)
		{
			mcu_dp_value_update(DPID_SWIMMING_DISTANCE,*p_Simulated_Swim_Distance); 	// 模拟游泳距离
			Wifi_Simulated_Swim_Distance = *p_Simulated_Swim_Distance;
		}
		
		//------------------- 默认 系统参数  ----------------------------
		if(Wifi_OP_Free_Mode_Speed != p_OP_Free_Mode->speed)
		{
			mcu_dp_value_update(DPID_FREE_MODE_SPEEN,p_OP_Free_Mode->speed); 					// 自由模式 速度
			Wifi_OP_Free_Mode_Speed = p_OP_Free_Mode->speed;
		}
		if(Wifi_OP_Time_Mode_Speed != p_OP_Timing_Mode->speed)
		{
			mcu_dp_value_update(DPID_TIMING_MODE_SPEEN,p_OP_Timing_Mode->speed); 			// 定时模式 速度
			Wifi_OP_Time_Mode_Speed = p_OP_Timing_Mode->speed;
		}
		if(Wifi_OP_Time_Mode_Time != p_OP_Timing_Mode->time)
		{
			mcu_dp_value_update(DPID_TIMING_MODE_TIME,p_OP_Timing_Mode->time); 				// 定时模式 时间
			Wifi_OP_Time_Mode_Time = p_OP_Timing_Mode->time;
		}
	}
	//*********************************
	//===== 不重要数据 10s 上传   ======
	//*********************************
	if((State_Upload_Cnt % WIFI_DATE_UPLOAD_TIME)==0)
	{
		// debug 用
		mcu_dp_value_update(DPID_SYSTEM_RUNNING_TIME,		*p_System_Runing_Second_Cnt); 		// 运行时间;
		mcu_dp_value_update(DPID_NO_OPERATION_TIME,			*p_No_Operation_Second_Cnt); 			// 无操作时间;
		mcu_dp_value_update(DPID_SYSTEM_STARTUP_TIME,	*p_System_Startup_Second_Cnt); 			// 启动时间;
		// 怕不同步 补发一次
		//mcu_dp_value_update(DPID_SYSTEM_WORKING_MODE,*p_PMode_Now);  				// 工作模式;
		//mcu_dp_value_update(DPID_MOTOR_CURRENT_SPEED,*p_OP_ShowNow_Speed); 	// 当前速度;
	}
	
	State_Upload_Cnt++;
	//State_Upload_Cnt = 0;
}

//------------------- wifi 状态  图标 ----------------------------
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


