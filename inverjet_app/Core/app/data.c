/**
******************************************************************************
* @file				data.c
* @brief			数据管理模块
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "data.h"
#include "modbus.h"
#include <stdlib.h>
#include <string.h>
#include "dev.h"
#include "down_conversion.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

Operating_Parameters OP_ShowNow;

Operating_Parameters* p_OP_ShowLater;

// 训练模式 当前状态
uint8_t PMode_Now = 0;

uint8_t Period_Now = 0;

// 各模式 属性 初始值
Operating_Parameters OP_Init_Free = { 40 , 0};

Operating_Parameters OP_Init_Timing = { 40 , 1800};

Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX] = {
{{20,120},{30,300}, {20,360},{35,540},{20,600},{30,780}, {20,900} },
{{45,180},{55,360}, {45,480},{70,720},{45,780},{55,1020},{45,1200}},
{{70,300},{80,540}, {70,600},{85,840},{70,900},{80,1200},{70,1500}},
{{45,420},{65,1440},{45,1800}},
{{30,00}},
};

//--------------------------- 系统属性
uint16_t* p_System_State_Machine;			// 状态机
uint16_t* p_PMode_Now;								// 当前模式
uint16_t* p_OP_ShowNow_Speed;					// 当前速度
uint16_t* p_OP_ShowNow_Time;					// 当前时间

//--------------------------- 临时 用于故障等界面记录返回值
uint16_t* p_System_State_Machine_Memory;			// 状态机
uint16_t* p_PMode_Now_Memory;									// 当前模式
uint16_t* p_OP_ShowNow_Speed_Memory;					// 当前速度
uint16_t* p_OP_ShowNow_Time_Memory;						// 当前时间

System_Ctrl_Mode_Type_enum Ctrl_Mode_Type = CTRL_FROM_KEY;				// 控制方式

// 各模式 属性
Operating_Parameters* p_OP_Free_Mode;

Operating_Parameters* p_OP_Timing_Mode;

Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX] = OP_Init_PMode;

//==========================================================
//--------------------------- 驱动板读取信息
//==========================================================

uint16_t Driver_Software_Version_Read;		// 驱动板软件版本	 读上来的原始值

uint16_t* p_Motor_Fault_Static;						// 故障状态		驱动板

uint32_t* p_Motor_Reality_Speed;					// 电机 实际 转速
uint32_t* p_Motor_Reality_Power;					// 电机 实际 功率

int16_t* p_Mos_Temperature;							// mos 温度
uint32_t* p_Motor_Current;								// 电机 电流		输出
uint16_t* p_Motor_Bus_Voltage;						// 母线 电压		输入
uint16_t* p_Motor_Bus_Current;						// 母线 电流  	输入

//==========================================================
//--------------------------- 整机信息
//==========================================================
uint16_t* p_System_Fault_Static;					// 故障状态		整机
int16_t* p_Box_Temperature;							// 电箱 温度
uint32_t* p_Send_Reality_Speed;						// 下发 实际 转速


uint16_t* p_Support_Control_Methods;	//屏蔽控制方式
uint16_t* p_Motor_Pole_Number;				//电机极数
uint16_t* p_Breath_Light_Max;					//光圈亮度  
	
uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX]={0};//电机状态

//================= 冲浪模式 全局 参数 ================================
// ----------------------------------------------------------------------------------------------
uint16_t* p_Surf_Mode_Info_Acceleration;  		//	冲浪模式 -- 加速度
uint16_t* p_Surf_Mode_Info_Prepare_Time;  		//	冲浪模式 -- 准备时间
uint16_t* p_Surf_Mode_Info_Low_Speed;  				//	冲浪模式 -- 低速档 -- 速度
uint16_t* p_Surf_Mode_Info_Low_Time;					//	冲浪模式 -- 低速档 -- 时间
uint16_t* p_Surf_Mode_Info_High_Speed;  			//	冲浪模式 -- 高速档 -- 速度
uint16_t* p_Surf_Mode_Info_High_Time;  				//	冲浪模式 -- 高速档 -- 时间
// ----------------------------------------------------------------------------------------------

uint16_t *p_WIFI_Rssi;
uint16_t *p_BLE_Rssi;

uint16_t* p_Analog_key_Value;					// 虚拟按键

uint8_t System_PowerUp_Finish = 0;

uint16_t MB_Buffer_Write_Timer = 0;

//================= 调试使用  时间 ================================

uint32_t* p_System_Runing_Second_Cnt;			// 系统时间
uint32_t* p_No_Operation_Second_Cnt;			// 无人操作时间
uint32_t* p_System_Startup_Second_Cnt;		// 启动 时间


//==========================================================
//--------------------------- 完成统计 (APP要)
//==========================================================
uint16_t* p_Finish_Statistics_Time;					//	完成统计 --> 时长
uint16_t* p_Finish_Statistics_Speed;				//	完成统计 --> 强度
uint32_t* p_Finish_Statistics_Distance;			//	完成统计 --> 游泳距离
uint16_t* p_Preparation_Time_BIT;						//	准备时间 Bit: 定时模式 P1-P6


uint16_t* p_Thread_Activity_Sign;					//	线程 活动 标志位

uint32_t* p_Wifi_Timing_Value;						// wifi 系统时间	
uint32_t* p_Wifi_Timing_Value_Old;				// 上一次时间

uint16_t* p_Check_Timing_Add_More;				// 走慢了,补时

uint16_t* p_Check_Timing_Minus_More;			// 走块了, 减时

uint16_t* p_Check_Timing_Error_Cnt;				// wifi模块 校时错误计数器

uint16_t* p_Wifi_DP_Upload_Level;					// wifi模块 dp点上报等级
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- 硬件 & 驱动 ----------------------------

// 初始化 冲浪模式 参数
void Surf_Mode_Info_Data_Init(void)
{
	//================= 冲浪模式 全局 参数 ================================
	// ----------------------------------------------------------------------------------------------
	*p_Surf_Mode_Info_Acceleration	=	2;  			//	冲浪模式 -- 加速度
	*p_Surf_Mode_Info_Prepare_Time	=	10;  			//	冲浪模式 -- 准备时间
	*p_Surf_Mode_Info_Low_Speed			=	30;  			//	冲浪模式 -- 低速档 -- 速度
	*p_Surf_Mode_Info_Low_Time			=	15;				//	冲浪模式 -- 低速档 -- 时间
	*p_Surf_Mode_Info_High_Speed		=	100;  		//	冲浪模式 -- 高速档 -- 速度
	*p_Surf_Mode_Info_High_Time			=	15;  			//	冲浪模式 -- 高速档 -- 时间
	// ----------------------------------------------------------------------------------------------
}

// 初始化 开机执行
uint8_t Check_Data_Init(void)
{
	uint8_t x=0,y=0;
	uint8_t result = 0;
	
	//MB_HoldBuffer_Temp_Clean();
	
	if(Is_Speed_Legal(p_OP_Free_Mode->speed) == 0)
	{
		*p_OP_Free_Mode = OP_Init_Free;
		result = 1;
	}
	
	if((Is_Speed_Legal(p_OP_Timing_Mode->speed) == 0) || (Is_Time_Legal(p_OP_Timing_Mode->time) == 0))
	{
		*p_OP_Timing_Mode = OP_Init_Timing;
		result = 1;
	}

	if( ( *p_Motor_Pole_Number > MOTOR_RPM_MAX_OF_POLES) || ( *p_Motor_Pole_Number < MOTOR_RPM_MIX_OF_POLES))
	{
		*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
		result = 1;
	}
	
	if(result == 1)
	{
		*p_Local_Address 					= MODBUS_LOCAL_ADDRESS;
		//*p_Baud_Rate 							= MODBUS_BAUDRATE_DEFAULT;
		*p_Motor_Pole_Number 			= MOTOR_RPM_NUMBER_OF_POLES;				// 电机级数		5
		*p_Support_Control_Methods 	= 0;			//	控制方式	
	}
	
	//================= 冲浪模式 全局 参数 ================================
	Surf_Mode_Info_Data_Init();
	
	for(x=0; x<TRAINING_MODE_NUMBER_MAX; x++)
	{
		for(y=0; y<TRAINING_MODE_PERIOD_MAX; y++)
		{
			if((Is_Speed_Legal(p_OP_PMode[x][y].speed) == 0) || (Is_Time_Legal(p_OP_PMode[x][y].time) == 0))
			{
				memcpy(p_OP_PMode, OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
				result = 1;
				break;
			}
			if(y>0)
			{
				if(p_OP_PMode[x][y].time <= p_OP_PMode[x][y-1].time)
				{
					memcpy(p_OP_PMode, &OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
					result = 1;
					break;
				}
			}
		}
	}
	
	return result;
}


// 初始化
void App_Data_Init(void)
{
	Read_OPMode();
	// 获取映射  flash已读
	MB_Get_Mapping_Register();
	//
	MB_InputBuffer_Init();
	// 检查各模式 属性
	if(Check_Data_Init())
	{
		//App_Data_ReInit();
		Write_MbBuffer_Now();
	}
	// 屏幕初始化
	TM1621_LCD_Init();
	
	TM1621_Buzzer_Init();
	
	//test 测试关屏幕背光pwm 记得删  wuqingguang 2024-09-09
	TM1621_light_Off();
}


// 恢复 初始化
void App_Data_ReInit(void)
{
	memset(p_Local_Address,0,REG_HOLDING_NREGS*2);
	
	*p_Local_Address 					= MODBUS_LOCAL_ADDRESS;
	*p_Baud_Rate 							= MODBUS_BAUDRATE_DEFAULT;
	*p_Motor_Pole_Number 			= MOTOR_RPM_NUMBER_OF_POLES;				// 电机级数		5
	//*p_Preparation_Time_BIT 	= 0;				// 预备时间
	
	// 训练模式 当前状态
//	*p_System_State_Machine 	= 0;			// 状态机
//	*p_PMode_Now 							= 0;			// 当前模式
//	*p_OP_ShowNow_Speed 			= 0;			// 当前速度
//	*p_OP_ShowNow_Time 				= 0;			// 当前时间
	Set_Pmode_Period_Now(0);
	
	// 各模式 属性
	*p_OP_Free_Mode = OP_Init_Free;
	*p_OP_Timing_Mode = OP_Init_Timing;

	//================= 冲浪模式 全局 参数 ================================
	Surf_Mode_Info_Data_Init();
	//================= 电机 参数 ================================
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER , MB_MOTOR_DRIVE_MODE, MOTOR_DRIVE_MODE_POLES);	// 厂内模式
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER , MB_MOTOR_MODEL_CODE, MOTOR_MODEL_CODE_POLES);	// 电机型号
	
	memcpy(&p_OP_PMode[0][0], &OP_Init_PMode[0][0], sizeof(OP_Init_PMode[0]));
	memcpy(&p_OP_PMode[1][0], &OP_Init_PMode[1][0], sizeof(OP_Init_PMode[1]));
	memcpy(&p_OP_PMode[2][0], &OP_Init_PMode[2][0], sizeof(OP_Init_PMode[2]));
	memcpy(&p_OP_PMode[3][0], &OP_Init_PMode[3][0], sizeof(OP_Init_PMode[3]));
	memcpy(&p_OP_PMode[4][0], &OP_Init_PMode[4][0], sizeof(OP_Init_PMode[4]));
	
	//存储  存一个 还是 扇区存
	Write_MbBuffer_Now();
}

// 读 flash
uint8_t Read_OPMode(void)
{
	MB_Flash_Buffer_Read();
	return 1;
}

void MB_Write_Timer_CallOut(void)
{
	if(MB_Buffer_Write_Timer > 0)
	{
		MB_Buffer_Write_Timer ++;
		if(MB_Buffer_Write_Timer > 20) // 20:5s   60000:25min
		{
			MB_Flash_Buffer_Write();
			MB_Buffer_Write_Timer = 0;
		}
	}
}


// 存 flash
uint8_t Write_MbBuffer_Later(void)
{
	MB_Buffer_Write_Timer = 1;
	return 1;
}


// 立即存 flash
uint8_t Write_MbBuffer_Now(void)
{
	MB_Flash_Buffer_Write();
	MB_Buffer_Write_Timer = 0;

	return 1;
}

//存储 新 速度
void Update_OP_Speed(void)
{
	if(System_is_Pause())	// 暂停
		return;
	
	if(System_Mode_Free())	// 自由
	{
		if(*p_OP_ShowNow_Speed < MOTOR_PERCENT_SPEED_MIX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MIX;
		else if(*p_OP_ShowNow_Speed > MOTOR_PERCENT_SPEED_MAX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MAX;
			
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
		Write_MbBuffer_Later();//存flash
	}
	else if(System_Mode_Time())	// 定时
	{
		if(*p_OP_ShowNow_Speed < MOTOR_PERCENT_SPEED_MIX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MIX;
		else if(*p_OP_ShowNow_Speed > MOTOR_PERCENT_SPEED_MAX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MAX;
		
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		Write_MbBuffer_Later();//存flash
	}
}

//存储 新 时间
void Update_OP_Time(void)
{
	if(System_Mode_Time())	// 定时
	{
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
		Write_MbBuffer_Later();//存flash
	}
}

//存储 新 速度 & 时间
void Update_OP_All(void)
{
	if(System_Mode_Free())	// 自由
	{
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
		Write_MbBuffer_Later();//存flash
	}
	else if(System_Mode_Time())	// 定时
	{
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
		Write_MbBuffer_Later();//存flash
	}
}

extern void System_Power_Off(void);
extern void Clean_Timing_Timer_Cnt(void);
extern void Clean_Automatic_Shutdown_Timer(void);

void OP_Update_Mode(void)
{
	if(System_is_Power_Off())
		System_Power_Off();
	else if(System_is_Stop())
	{
		if(System_Mode_Train())
			*p_OP_ShowNow_Speed = p_OP_PMode[Get_System_State_Mode()-1][0].speed;
		else if(System_Mode_Time())
			*p_OP_ShowNow_Time = p_OP_Timing_Mode->time - *p_OP_ShowNow_Time;
				
		//Motor_Speed_Target_Set(0);
		Clean_Automatic_Shutdown_Timer();
		Clean_Timing_Timer_Cnt();
	}
	else if(System_is_Pause())
	{
		p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
		*p_OP_ShowNow_Speed = 0;
		Data_Set_Current_Speed(0);//注意,需要在切完运行状态后再设置速度,如"启动"
	}
	else
	{
		// 速度
		if(System_Mode_Free())
		{
			*p_OP_ShowNow_Speed = p_OP_Free_Mode->speed;
		}
		else if(System_Mode_Time())
		{
			*p_OP_ShowNow_Speed = p_OP_Timing_Mode->speed;
		}
		else if(System_Mode_Train())
		{
			//if((p_OP_ShowLater->speed < MOTOR_PERCENT_SPEED_MIX)||(p_OP_ShowLater->speed > MOTOR_PERCENT_SPEED_MAX))
				*p_OP_ShowNow_Speed = p_OP_PMode[Get_System_State_Mode()-1][Period_Now].speed;
			//else
				//*p_OP_ShowNow_Speed = p_OP_ShowLater->speed ;
		}
	}
	
	if(Motor_is_Start())
	{
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//光圈自动判断
		Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
	}
	else
		Motor_Speed_Target_Set(0);
}

//检查 新 速度 & 时间  防止溢出
void Check_OP_All(void)
{
	if(System_is_Power_Off())
	{
		System_Power_Off();
	}
	else if(System_is_Pause())
	{
		p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
		Data_Set_Current_Speed(0);//注意,需要在切完运行状态后再设置速度,如"启动"
	}
	else
	{
		// 速度
		if(System_Mode_Free())
		{
			*p_OP_ShowNow_Speed = p_OP_Free_Mode->speed;
			*p_OP_ShowNow_Time = 0;
		}
		else if(System_Mode_Time())
		{
			*p_OP_ShowNow_Speed = p_OP_Timing_Mode->speed;
			*p_OP_ShowNow_Time = p_OP_Timing_Mode->time;
		}
		else if(System_Mode_Train())
		{
			if(Is_Mode_Legal(Get_System_State_Mode()) == 0)
				Set_System_State_Mode(SYSTEM_MODE_TRAIN_P1);
			Set_Pmode_Period_Now(0);
			*p_OP_ShowNow_Speed = p_OP_PMode[Get_System_State_Mode()-1][0].speed;
			
			if(System_is_Stop() == 0)
				*p_OP_ShowNow_Time = 0;
		}
		
		if(Motor_is_Start())
		{
			Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//光圈自动判断
			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
		}
		else
			Motor_Speed_Target_Set(0);
	}
}

//------------------- 判断 模式 合法 ----------------------------
uint8_t Is_Mode_Legal(uint8_t mode)
{
	if((mode > 0) && (mode <= TRAINING_MODE_NUMBER_MAX) )
	{
		return 1;
	}
	else
		return 0;
}
//------------------- 判断速度 合法 ----------------------------
uint8_t Is_Speed_Legal(uint16_t speed)
{
	if((speed >= SPEED_LEGAL_MIN) && (speed <= SPEED_LEGAL_MAX))
	{
		return 1;
	}
	else
		return 0;
}

//------------------- 判断时间 合法 ----------------------------
uint8_t Is_Time_Legal(uint16_t time)
{
	if((time >= TIME_LEGAL_MIN) && (time <= TIME_LEGAL_MAX))
	{
		return 1;
	}
	else
		return 0;
}

//------------------- 设置 当前 速度 ----------------------------
void Data_Set_Current_Speed(uint8_t speed)
{
	//if(System_is_Starting())
		//return;
	if(Special_Status_Get( SPECIAL_BIT_SKIP_INITIAL))// 跳过 自动启动
		return;
	
	*p_OP_ShowNow_Speed = speed;	
	
	/*if(Get_Temp_Slow_Down_State())
	{
		if(speed > Get_Down_Conversion_Speed_Now())
		{
			*p_OP_ShowNow_Speed = Get_Down_Conversion_Speed_Now();
		}
	}*/
	
	Motor_Speed_Target_Set(speed);
}

//------------------- 设置 当前 时间 ----------------------------
void Data_Set_Current_Time(uint16_t time)
{
	//if(System_is_Starting())
		//return;
	
	*p_OP_ShowNow_Time = time;
}

//------------------- 设置 训练时段 ----------------------------
void Set_Pmode_Period_Now(uint16_t value)
{
	Period_Now = value;
}


//------------------- 是否接收外部控制 ----------------------------
uint8_t If_Accept_External_Control(uint8_t mode)
{
	if(ERROR_DISPLAY_STATUS == Get_System_State_Machine())
		return 0;
	
	//if((*p_Support_Control_Methods & mode) != 0)
		//return 0;
	

	return 1;
}

//------------------- 获取软件版本号  字符串转 uint32 ----------------------------
void get_uint3_version(char * buffer)
{
	char str[32];
	char *tmp;

	strncpy(str, (const char *)buffer, strlen(buffer));
	
	tmp = strtok(str, ".");
	if (tmp != NULL) {
			*p_Software_Version_high = (uint8_t)atoi(tmp);

			// 软件子版本号初始化
			tmp = strtok(NULL, ".");
			if (tmp != NULL) {
				*p_Software_Version_low = (uint8_t)atoi(tmp)<<8;
					tmp = strtok(NULL, ".");
					if (tmp != NULL) {
							*p_Software_Version_low |= (uint8_t)atoi(tmp);
					}
			}
	}
}

//------------------- 清除wifi标志 ----------------------------
void System_Wifi_State_Clean(void)
{
	*p_WIFI_Rssi = 0xFF;
}
//------------------- 清除蓝牙标志 ----------------------------
void System_BT_State_Clean(void)
{
	*p_BLE_Rssi = 0;
}
extern void LCD_Refresh_Restore(void);

//------------------- 设置控制方式 ----------------------------
void Set_Ctrl_Mode_Type(System_Ctrl_Mode_Type_enum type)
{
	LCD_Refresh_Restore();//恢复刷新
	Ctrl_Mode_Type = type;
}
	
//------------------- 获取控制方式 ----------------------------
System_Ctrl_Mode_Type_enum Get_Ctrl_Mode_Type(void)
{
	return Ctrl_Mode_Type;
}

//------------------- OTA 自动退出 1秒进一次 ----------------------------
void OTA_Time_Out(void)
{
	static uint32_t time_out_cnt=0;
	
	time_out_cnt ++;
	if(time_out_cnt > OTA_SHUTDOWN_TIME_OUT)
	{
		SysSoftReset();// 软件复位
	}
}

//------------------- 显示内容映射 ----------------------------
void LCD_TO_Mapping(void)
{
	
}

// 完成数据统计
//*********************************************************************************************
//-------------- 计算 完成统计  -------------------
//	count 秒
void Finish_Statistics_Count(uint8_t count)
{

	* p_Finish_Statistics_Time += count;								//	完成统计 --> 时长
	if(*p_OP_ShowNow_Speed >= MOTOR_PERCENT_SPEED_MIX)
		* p_Finish_Statistics_Speed = *p_OP_ShowNow_Speed;	//	完成统计 --> 强度
	* p_Finish_Statistics_Distance += (Motor_Speed_Now * EVERY_1PERCENT_DISTANCE_PER_SECOND / 100);			//	完成统计 --> 游泳距离
}


//-------------- 清除 完成统计  -------------------
void Finish_Statistics_Clean( void )
{
	* p_Finish_Statistics_Time = 0;				//	完成统计 --> 时长
	* p_Finish_Statistics_Speed = 0;			//	完成统计 --> 强度
	* p_Finish_Statistics_Distance = 0;		//	完成统计 --> 游泳距离
}


//-------------- 上传 完成统计  -------------------
void Finish_Statistics_Upload( void )
{
	WIFI_Finish_Statistics_Upload();
}


//-------------- 线程 活动 标志清零  -------------------
void Thread_Activity_Sign_Clean( void )
{
	*p_Thread_Activity_Sign = 0;
}

//-------------- 线程 活动 设置  -------------------
void Thread_Activity_Sign_Set( uint16_t val )
{
	*p_Thread_Activity_Sign |= val;
}







//---------------------------------------------------------------------------------------------------------------------------------------------------






