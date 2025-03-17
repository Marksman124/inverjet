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
	
	static uint16_t Wifi_Motor_Reality_Power = 0;
	static uint16_t Wifi_Motor_Bus_Voltage = 0;
	static uint16_t Wifi_Motor_Bus_Current = 0;
	
	static uint16_t Wifi_OP_Free_Mode_Speed = 0;
	static uint16_t Wifi_OP_Time_Mode_Speed = 0;
	static uint16_t Wifi_OP_Time_Mode_Time = 0;
	
	short int box_temperature = 0;
	short int mos_temperature = 0;
		
	static uint16_t Upload_Timer_Cnt = 0;						// 上传时间 计时器
	static uint8_t Upload_Second_Cnt = 0;						// 上传时间 秒
	//------------------- 统计上传	----------------------------
	if(Upload_Finish_Data == 1)
	{
		Upload_Finish_Data = 0;
		Wifi_DP_Data_Update(DPID_FINISH_STATISTICS_TIME); 		//	完成统计 --> 时长
		Wifi_DP_Data_Update(DPID_FINISH_STATISTICS_SEED); 		//	完成统计 --> 强度
		Wifi_DP_Data_Update(DPID_FINISH_STATISTICS_DISTANCE); //	完成统计 --> 游泳距离
		Finish_Statistics_Clean();
	}
	//------------------- 故障上传 ----------------------------
	if(Wifi_Motor_Fault_Static != *p_Motor_Fault_Static)
	{
		Wifi_DP_Data_Update(DPID_DEVICE_ERROR_CODE);					// 驱动板故障
		Wifi_Motor_Fault_Static = *p_Motor_Fault_Static;
	}
	if(Wifi_System_Fault_Static != *p_System_Fault_Static)
	{
		if(Fault_Check_Status_Legal(*p_System_Fault_Static))
		{
			Wifi_DP_Data_Update(DPID_GET_SYSTEM_FAULT_STATUS); 	// 系统 故障型数据上报
			Wifi_System_Fault_Static = *p_System_Fault_Static;
		}
	}
	//------------------- 系统状态 (重要)  ----------------------------
	if((Wifi_PMode_Now != Get_System_State_Mode())||(Wifi_System_State_Machine != Get_System_State_Machine()))
	{
		Wifi_DP_Data_Update(DPID_SYSTEM_STATUS_MODE);			// 合并发生;
		if(Wifi_PMode_Now != Get_System_State_Mode())
		{
			Wifi_DP_Data_Update(DPID_SYSTEM_WORKING_MODE);			// 工作模式;
			Wifi_PMode_Now = Get_System_State_Mode();
		}
		if(Wifi_System_State_Machine != Get_System_State_Machine())
		{
			Wifi_DP_Data_Update(DPID_SYSTEM_WORKING_STATUS);			// 状态机;
			Wifi_System_State_Machine = Get_System_State_Machine();
		}
	}
	if(Wifi_OP_ShowNow_Speed != *p_OP_ShowNow_Speed)
	{
		Wifi_DP_Data_Update(DPID_MOTOR_CURRENT_SPEED); 				// 当前速度
		Wifi_OP_ShowNow_Speed = *p_OP_ShowNow_Speed;
	}
	if(Wifi_OP_ShowNow_Time != *p_OP_ShowNow_Time)
	{
		Wifi_DP_Data_Update(DPID_MOTOR_CURRENT_TIME); 					// 当前时间
		Wifi_OP_ShowNow_Time = *p_OP_ShowNow_Time;
	}
	//*********************************
	//===== 一般数据 1s 上传   =========
	//*********************************
	if((Upload_Timer_Cnt % WIFI_DATE_UPLOAD_TIME_NORMAL)==0)
	{
		mcu_get_system_time();	//读时间
		//------------------- Debug 数据  ----------------------------
		memcpy(&box_temperature, p_Box_Temperature, 2);
		if(Wifi_Box_Temperature != box_temperature)
		{
			Wifi_Box_Temperature = box_temperature;
			Wifi_DP_Data_Update(DPID_GET_BOX_TEMPERATURE); 			// 机箱温度
		}
		memcpy(&mos_temperature, p_Mos_Temperature, 2);
		if(Wifi_Mos_Temperature != mos_temperature)
		{
			Wifi_Mos_Temperature = mos_temperature;
			Wifi_DP_Data_Update(DPID_GET_MOS_TEMPERATURE); 			// MOS温度
		}
		if(Wifi_Motor_Current != *p_Motor_Current)
		{
			Wifi_DP_Data_Update(DPID_GET_MOTOR_CURRENT); 				// 输出电流 （电机）
			Wifi_Motor_Current = *p_Motor_Current;
		}
		if(Wifi_Motor_Bus_Voltage != *p_Motor_Bus_Voltage)
		{
			Wifi_DP_Data_Update(DPID_MOTOR_BUS_VOLTAGE); 				// 输入电压 （母线）
			Wifi_Motor_Bus_Voltage = *p_Motor_Bus_Voltage;
		}
		if(Wifi_Motor_Bus_Current != *p_Motor_Bus_Current)
		{
			Wifi_DP_Data_Update(DPID_MOTOR_BUS_CURRENTE); 			// 输入电流（母线）
			Wifi_Motor_Bus_Current = *p_Motor_Bus_Current;
		}
		
		if(Wifi_Motor_Reality_Speed != *p_Motor_Reality_Speed)
		{
			Wifi_DP_Data_Update(DPID_MOTOR_REALITY_SPEED); 			// 实际转速
			Wifi_Motor_Reality_Speed = *p_Motor_Reality_Speed;
		}
		if(Wifi_Send_Reality_Speed != *p_Send_Reality_Speed)
		{
			Wifi_DP_Data_Update(DPID_SEND_REALITY_SPEED); 			// 下发转速
			Wifi_Send_Reality_Speed = *p_Send_Reality_Speed;
		}
		if(Wifi_Motor_Reality_Power != *p_Motor_Reality_Power)
		{
			Wifi_DP_Data_Update(DPID_MOTOR_POWER); 							// 当前功率
			Wifi_Motor_Reality_Power = *p_Motor_Reality_Power;
		}
		
		//------------------- 默认 系统参数  ----------------------------
		if(Wifi_OP_Free_Mode_Speed != p_OP_Free_Mode->speed)
		{
			Wifi_DP_Data_Update(DPID_FREE_MODE_SPEEN); 					// 自由模式 速度
			Wifi_OP_Free_Mode_Speed = p_OP_Free_Mode->speed;
		}
		if(Wifi_OP_Time_Mode_Speed != p_OP_Timing_Mode->speed)
		{
			Wifi_DP_Data_Update(DPID_TIMING_MODE_SPEEN); 				// 定时模式 速度
			Wifi_OP_Time_Mode_Speed = p_OP_Timing_Mode->speed;
		}
		if(Wifi_OP_Time_Mode_Time != p_OP_Timing_Mode->time)
		{
			Wifi_DP_Data_Update(DPID_TIMING_MODE_TIME); 				// 定时模式 时间
			Wifi_OP_Time_Mode_Time = p_OP_Timing_Mode->time;
		}
		
		//*********************************
		//===== 不重要数据 10s 上传   ======
		//*********************************
		if((*p_Wifi_DP_Upload_Level & 0xFF) > 0x80)
		{
			if((Upload_Second_Cnt % (*p_Wifi_DP_Upload_Level & 0x7F))==0)
			{
				// debug 用
				Wifi_DP_Data_Update(DPID_SYSTEM_RUNNING_TIME); 		// 运行时间;
				Wifi_DP_Data_Update(DPID_NO_OPERATION_TIME); 			// 无操作时间;
				Wifi_DP_Data_Update(DPID_SYSTEM_STARTUP_TIME); 		// 启动时间;
				Wifi_DP_Data_Update(DPID_THREAD_ACTIVITY_SIGN); 	// 线程活动标志;
				
				Wifi_DP_Data_Update(DPID_DRIVE_NTC_TEMP_01);			// 驱动板 NTC 温度 01
				Wifi_DP_Data_Update(DPID_DRIVE_NTC_TEMP_02);			// 驱动板 NTC 温度 01
				Wifi_DP_Data_Update(DPID_DRIVE_NTC_TEMP_03);			// 驱动板 NTC 温度 01
			}
		}
		
		if(Upload_Second_Cnt < 0x7F)
			Upload_Second_Cnt ++;
		else
			Upload_Second_Cnt = 0;
	}
	
	if(Upload_Timer_Cnt < 9999)
		Upload_Timer_Cnt ++;
	else
		Upload_Timer_Cnt = 0;
}



//-------------- 上传 完成统计  -------------------
void WIFI_Finish_Statistics_Upload( void )
{
	if(* p_Finish_Statistics_Time == 0)
		return;
	
	if(*p_Finish_Statistics_Time >= WIFI_STATISTICE_UPLOAD_MINIMUM_TIME)
	{
		DEBUG_PRINT("\n上传统计数据:\t时长:\t%d\t强度:\t%d\t距离:\t%d\t\n",*p_Finish_Statistics_Time,*p_Finish_Statistics_Speed,*p_Finish_Statistics_Distance);

	}
	else
	{
		DEBUG_PRINT("\n统计时间低于3分钟，上传 %d\n",* p_Finish_Statistics_Time);
		Finish_Statistics_Clean();
	}
	Upload_Finish_Data = 1;
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
	
	all_data_update();		// wifi 上传
}

// 按键主循环任务
//  20 ms
void Wifi_Module_Handler(void)
{
	static uint32_t WIFI_Rx_Timer_cnt= 0;
	
	Thread_Activity_Sign_Set(THREAD_ACTIVITY_WIFI);
	
	wifi_uart_service();
	
	if(IS_CHECK_ERROR_MODE())
	{
		// ===================  通讯故障
		if(get_mcu_reset_state() == FALSE)
			WIFI_Rx_Timer_cnt++;  //通信故障计数器
		else
			WIFI_Rx_Timer_cnt = 0;
		
		if(WIFI_Rx_Timer_cnt > FAULT_WIFI_LOST_TIME)
		{
			Set_Motor_Fault_State(E301_WIFI_HARDWARE);
		}
		// =================== wifi 信号故障 
		if(*p_WIFI_Rssi < WIFI_RSSI_ERROR_VAULE)
		{
			DEBUG_PRINT("wifi模组故障: 信号强度 %d dBm   ( 合格: %d dBm)\n",*p_WIFI_Rssi, WIFI_RSSI_ERROR_VAULE);
			Set_Motor_Fault_State(E301_WIFI_HARDWARE);
		}
		else
			ReSet_Motor_Fault_State(E301_WIFI_HARDWARE);
	}
	
	if(System_is_OTA() == 0)
	{
		WIFI_Update_State_Upload();
		
		WIFI_Get_Work_State();
	}
	
}


//  上传dp点 (统一接口)
void Wifi_DP_Data_Update(uint16_t id)
{
	switch(id)
	{
		//*********************************
		//========== 后台参数  =============
		//*********************************
		case DPID_PREPARATION_TIME:// 准备时间(APP管理,本地只负责保存)
			mcu_dp_value_update(DPID_PREPARATION_TIME,				*p_Preparation_Time_BIT);
			break;
		case DPID_DEVICE_ERROR_CODE:// 驱动板故障
			mcu_dp_value_update(DPID_DEVICE_ERROR_CODE,				*p_Motor_Fault_Static);
			break;
		case DPID_GET_SYSTEM_FAULT_STATUS:// 系统故障上报
			
			mcu_dp_fault_update(DPID_GET_SYSTEM_FAULT_STATUS,	*p_System_Fault_Static);
			break;
		case DPID_GET_MOS_TEMPERATURE:// MOS温度
			mcu_dp_value_update(DPID_GET_MOS_TEMPERATURE,			*p_Mos_Temperature);
			break;
		case DPID_GET_BOX_TEMPERATURE:// 机箱温度
			mcu_dp_value_update(DPID_GET_BOX_TEMPERATURE,			*p_Box_Temperature);
			break;
		case DPID_GET_MOTOR_CURRENT:// 输出电流（电机)
			mcu_dp_value_update(DPID_GET_MOTOR_CURRENT,				*p_Motor_Current);
			break;
		case DPID_MOTOR_REALITY_SPEED:// 实际转速
			mcu_dp_value_update(DPID_MOTOR_REALITY_SPEED,			*p_Motor_Reality_Speed);
			break;
		case DPID_MOTOR_BUS_VOLTAGE:// 输入电压（母线）
			mcu_dp_value_update(DPID_MOTOR_BUS_VOLTAGE,				*p_Motor_Bus_Voltage);
			break;
		case DPID_SEND_REALITY_SPEED:// 下发转速
			mcu_dp_value_update(DPID_SEND_REALITY_SPEED,			*p_Send_Reality_Speed);
			break;
		case DPID_MOTOR_POWER:// 当前功率
			mcu_dp_value_update(DPID_MOTOR_POWER,							*p_Motor_Reality_Power);
			break;
		case DPID_MOTOR_BUS_CURRENTE:// 输入电流（母线）
			mcu_dp_value_update(DPID_MOTOR_BUS_CURRENTE,			*p_Motor_Bus_Current);
			break;
		case DPID_SYSTEM_RUNNING_TIME:// 运行时间
			mcu_dp_value_update(DPID_SYSTEM_RUNNING_TIME,			*p_System_Runing_Second_Cnt);
			break;
		case DPID_NO_OPERATION_TIME:// 无操作时间
			mcu_dp_value_update(DPID_NO_OPERATION_TIME,				*p_No_Operation_Second_Cnt);
			break;
		case DPID_SYSTEM_STARTUP_TIME:// 启动 时间
			mcu_dp_value_update(DPID_SYSTEM_STARTUP_TIME,			*p_System_Startup_Second_Cnt);
			break;
		case DPID_THREAD_ACTIVITY_SIGN:// 线程活动标志
			mcu_dp_value_update(DPID_THREAD_ACTIVITY_SIGN,			*p_Thread_Activity_Sign);
			break;
			//*********************************
			//========== 当前状态机  ===========
			//*********************************
		case DPID_SYSTEM_WORKING_MODE:// 工作模式
			mcu_dp_enum_update(DPID_SYSTEM_WORKING_MODE,			Get_System_State_Mode());
			break;
		case DPID_SYSTEM_WORKING_STATUS:// 状态机
			/*if(System_is_Error())	//非故障 上传  wuqingguang
				mcu_dp_enum_update(DPID_SYSTEM_WORKING_STATUS,		*p_System_State_Machine_Memory);
			else*/
				mcu_dp_enum_update(DPID_SYSTEM_WORKING_STATUS,		Get_System_State_Machine());
			break;
		case DPID_MOTOR_CURRENT_SPEED:// 当前速度
			mcu_dp_value_update(DPID_MOTOR_CURRENT_SPEED,			*p_OP_ShowNow_Speed);
			break;
		case DPID_MOTOR_CURRENT_TIME:// 当前时间
			mcu_dp_value_update(DPID_MOTOR_CURRENT_TIME,			*p_OP_ShowNow_Time);
			break;
		case DPID_SYSTEM_STATUS_MODE:// 合并上传
			/*if(System_is_Error())//非故障 上传  wuqingguang
				mcu_dp_raw_update(DPID_SYSTEM_STATUS_MODE,(unsigned char *)p_PMode_Now_Memory,	4);
			else*/
				mcu_dp_raw_update(DPID_SYSTEM_STATUS_MODE,(unsigned char *)p_PMode_Now,	4);
			break;
		//*********************************
		//=========== 初始值  =============
		//*********************************
		case DPID_FREE_MODE_SPEEN:// 自由模式 速度
			mcu_dp_value_update(DPID_FREE_MODE_SPEEN,					p_OP_Free_Mode->speed);
			break;
		case DPID_TIMING_MODE_SPEEN:// 定时模式 速度
			mcu_dp_value_update(DPID_TIMING_MODE_SPEEN,				p_OP_Timing_Mode->speed);
			break;
		case DPID_TIMING_MODE_TIME:// 定时模式 时间
			mcu_dp_value_update(DPID_TIMING_MODE_TIME,				p_OP_Timing_Mode->time);
			break;
		//*********************************
		//========== 训练计划  =============
		//*********************************
		case DPID_SET_TRAIN_PLAN_01:// 训练计划 P1
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_01,	(unsigned char *)p_OP_PMode[0],	TRAINING_MODE_PERIOD_MAX*4);
			break;
		case DPID_SET_TRAIN_PLAN_02:// 训练计划 P2
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_02,	(unsigned char *)p_OP_PMode[1],	TRAINING_MODE_PERIOD_MAX*4);
			break;
		case DPID_SET_TRAIN_PLAN_03:// 训练计划 P3
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_03,	(unsigned char *)p_OP_PMode[2],	TRAINING_MODE_PERIOD_MAX*4);
			break;
		case DPID_SET_TRAIN_PLAN_04:// 训练计划 P4
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_04,	(unsigned char *)p_OP_PMode[3],	TRAINING_MODE_PERIOD_MAX*4); 
			break;
		case DPID_SET_TRAIN_PLAN_05:// 训练计划 P5
			mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_05,	(unsigned char *)p_OP_PMode[4],	TRAINING_MODE_PERIOD_MAX*4);
			break;
		//*********************************
		//========== 上传统计  =============
		//*********************************
		case DPID_FINISH_STATISTICS_TIME:// 当前完成统计_时长
			mcu_dp_value_update(DPID_FINISH_STATISTICS_TIME,			*p_Finish_Statistics_Time);
			break;
		case DPID_FINISH_STATISTICS_SEED:// 当前完成统计_游泳强度
			mcu_dp_value_update(DPID_FINISH_STATISTICS_SEED,			*p_Finish_Statistics_Speed);
			break;
		case DPID_FINISH_STATISTICS_DISTANCE:// 当前完成统计_距离
			mcu_dp_value_update(DPID_FINISH_STATISTICS_DISTANCE,	*p_Finish_Statistics_Distance);
			break;
		//*********************************
		//========== 自定义计划  =============
		//*********************************
		case DPID_CUSTOM_TRAIN_PLAN_01:// 当前自定义训练计划_01
			mcu_dp_raw_update(DPID_CUSTOM_TRAIN_PLAN_01,	(unsigned char *)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TRAIN_MODE_SPEED_P6_1),	TRAINING_MODE_PERIOD_MAX*4);
			break;
		case DPID_CUSTOM_TRAIN_PLAN_02:// 当前自定义训练计划_02
			mcu_dp_raw_update(DPID_CUSTOM_TRAIN_PLAN_02,	(unsigned char *)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TRAIN_MODE_SPEED_P7_1),	TRAINING_MODE_PERIOD_MAX*4);
			break;
		//case DPID_CUSTOM_TRAIN_PLAN_03:// 当前自定义训练计划_03
			//mcu_dp_raw_update(DPID_SET_TRAIN_PLAN_01,	(unsigned char *)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TRAIN_MODE_SPEED_P8_1),	TRAINING_MODE_PERIOD_MAX*4);
			//break;
		//*********************************
		//========== 详细温度参数  =============
		//*********************************
		case DPID_DRIVE_NTC_TEMP_01:// 驱动板 NTC 温度——01
			mcu_dp_value_update(DPID_DRIVE_NTC_TEMP_01,Get_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_01));
			break;
		case DPID_DRIVE_NTC_TEMP_02:// 驱动板 NTC 温度——02
			mcu_dp_value_update(DPID_DRIVE_NTC_TEMP_02,Get_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_02));
			break;
		case DPID_DRIVE_NTC_TEMP_03:// 驱动板 NTC 温度——03
			mcu_dp_value_update(DPID_DRIVE_NTC_TEMP_03,Get_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_03));
			break;
		default:
			break;
	}
}




