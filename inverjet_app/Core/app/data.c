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
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

uint32_t Product_Model_Ccode[MODEL_DIAL_SWITCH_NUMBER] = 
{
	PRODUCT_MODEL_CODE_SJ230,
	PRODUCT_MODEL_CODE_SJ200,
	PRODUCT_MODEL_CODE_SJ160,
	PRODUCT_MODEL_CODE_SJ230
};

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

System_Ctrl_Mode_Type_enum Ctrl_Mode_Type = CTRL_FROM_KEY;				// 控制方式

// 各模式 属性
Operating_Parameters* p_OP_Free_Mode;

Operating_Parameters* p_OP_Timing_Mode;

Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX] = OP_Init_PMode;

//==========================================================
//--------------------------- 驱动板读取信息
//==========================================================
uint16_t* p_Driver_Software_Version;			// 驱动板软件版本
uint16_t* p_Motor_Fault_Static;						// 故障状态		驱动板

uint32_t* p_Motor_Reality_Speed;					// 电机 实际 转速
uint32_t* p_Motor_Reality_Power;					// 电机 实际 功率

uint16_t* p_Mos_Temperature;							// mos 温度
uint32_t* p_Motor_Current;								// 电机 电流		输出
uint16_t* p_Motor_Bus_Voltage;						// 母线 电压		输入
uint16_t* p_Motor_Bus_Current;						// 母线 电流  	输入

//==========================================================
//--------------------------- 整机信息
//==========================================================
uint16_t* p_System_Fault_Static;					// 故障状态		整机
uint16_t* p_Box_Temperature;							// 电箱 温度
uint32_t* p_Send_Reality_Speed;						// 下发 实际 转速
uint16_t* p_Simulated_Swim_Distance;			// 模拟游泳距离


uint16_t* p_Support_Control_Methods;	//屏蔽控制方式
uint16_t* p_Motor_Pole_Number;				//电机极数
uint16_t* p_Breath_Light_Max;					//光圈亮度  
	
uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX]={0};//电机状态

//================= 临时变量  全局 ================================
uint16_t Temp_Data_P5_Acceleration = 2;				//P5 加速度
uint16_t Temp_Data_P5_100_Time = 15;					//P5 100% 时间	秒
uint16_t Temp_Data_P5_0_Time = 15;						//P5 0% 	时间	秒

uint8_t WIFI_Rssi = 0xFF;

uint16_t* p_Analog_key_Value;					// 虚拟按键

uint8_t System_PowerUp_Finish = 0;
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



/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- 硬件 & 驱动 ----------------------------

// 初始化 开机执行
void Check_Data_Init(void)
{
	static uint8_t x=0,y=0;
	
	*p_System_Runing_Second_Cnt = 0; 		//运行 时间
	*p_No_Operation_Second_Cnt = 0;			//无人操作 时间
	*p_System_Startup_Second_Cnt = 0;		// 启动 时间
	*p_Analog_key_Value = 0;						//虚拟按键
	
	if(Is_Speed_Legal(p_OP_Free_Mode->speed) == 0)
	{
		*p_OP_Free_Mode = OP_Init_Free;
	}
	
	if((Is_Speed_Legal(p_OP_Timing_Mode->speed) == 0) || (Is_Time_Legal(p_OP_Timing_Mode->time) == 0))
	{
		*p_OP_Timing_Mode = OP_Init_Timing;
	}

	if( ( *p_Motor_Pole_Number > MOTOR_RPM_MAX_OF_POLES) || ( *p_Motor_Pole_Number < MOTOR_RPM_MIX_OF_POLES))
	{
		*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	}
	
	for(x=0; x<TRAINING_MODE_NUMBER_MAX; x++)
	{
		for(y=0; y<TRAINING_MODE_PERIOD_MAX; y++)
		{
			if((Is_Speed_Legal(p_OP_PMode[x][y].speed) == 0) || (Is_Time_Legal(p_OP_PMode[x][y].time) == 0))
			{
				memcpy(p_OP_PMode, OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
				return;
			}
			if(y>0)
			{
				if(p_OP_PMode[x][y].time <= p_OP_PMode[x][y-1].time)
				{
					memcpy(p_OP_PMode, &OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
					return;
				}
			}
		}
	}
}


extern void Get_Mapping_Register(void);
// 初始化
void App_Data_Init(void)
{
	Read_OPMode();
	// 获取映射  flash已读
	Get_Mapping_Register();
	
	// 训练模式 当前状态
	*p_PMode_Now = 0;
	Period_Now = 0;
	
	// 各模式 属性
	Check_Data_Init();
	
	//存储  存一个 还是 扇区存
	//Memset_OPMode();
	
	// 屏幕初始化
	TM1621_LCD_Init();
	
	TM1621_Buzzer_Init();
	
	//test 测试关屏幕背光pwm 记得删  wuqingguang 2024-09-09
	TM1621_light_Off();
}


// 恢复 初始化
void App_Data_ReInit(void)
{
	
	*p_Local_Address = MODBUS_LOCAL_ADDRESS;
	*p_Baud_Rate = MODBUS_BAUDRATE_DEFAULT;
	*p_Support_Control_Methods = 0;
	
	
	// 训练模式 当前状态
	*p_PMode_Now = 0;
	Period_Now = 0;
	
	// 各模式 属性
	*p_OP_Free_Mode = OP_Init_Free;
	*p_OP_Timing_Mode = OP_Init_Timing;
	
	*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	
	memcpy(&p_OP_PMode[0][0], &OP_Init_PMode[0][0], sizeof(OP_Init_PMode));
	
	//存储  存一个 还是 扇区存
	Memset_OPMode();
}

// 读 flash
uint8_t Read_OPMode(void)
{
	MB_Flash_Buffer_Read();
	return 1;
}

// 存 flash
uint8_t Memset_OPMode(void)
{
	MB_Flash_Buffer_Write();
	return 1;
}

//存储 新 速度
void Update_OP_Speed(void)
{
	if(System_Mode_Free())	// 自由
	{
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
		Memset_OPMode();//存flash
	}
	else if(System_Mode_Time())	// 定时
	{
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		Memset_OPMode();//存flash
	}
}

//存储 新 时间
void Update_OP_Time(void)
{
	if(System_Mode_Time())	// 定时
	{
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
		Memset_OPMode();//存flash
	}
}

//存储 新 速度 & 时间
void Update_OP_All(void)
{
	if(System_Mode_Free())	// 自由
	{
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
		Memset_OPMode();//存flash
	}
	else if(System_Mode_Time())	// 定时
	{
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
		Memset_OPMode();//存flash
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
	
	*p_OP_ShowNow_Speed = speed;	
	Motor_Speed_Target_Set(speed);
	
	//if(System_is_Running())
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
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
uint32_t get_uint3_version(char * buffer)
{
	char str[32];
	char *tmp;
	uint32_t version = 0;
	
	strncpy(str, (const char *)buffer, strlen(buffer));
	
	tmp = strtok(str, ".");
	if (tmp != NULL) {
			version = (uint8_t)atoi(tmp)<<16;

			// 软件子版本号初始化
			tmp = strtok(NULL, ".");
			if (tmp != NULL) {
					tmp = strtok(NULL, ".");
					if (tmp != NULL) {
							version |= (uint8_t)atoi(tmp);
					}
			}
	}
	return version;
}

//------------------- 清除wifi标志 ----------------------------
void System_Wifi_State_Clean(void)
{
	WIFI_Rssi = 0xFF;
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


//------------------- 获取机型码 ----------------------------
uint32_t Get_Model_Code_Num(void)
{
	System_Dial_Switch = Gpio_Get_Dial_Switch();
	
	if(System_Dial_Switch < MODEL_DIAL_SWITCH_NUMBER)
		return Product_Model_Ccode[System_Dial_Switch];
	else
		return PRODUCT_MODEL_CODE_SJ230;//默认
}

// 每 %1 每秒 增加游泳距离 放大100倍
uint32_t Get_Every_1Percent_Distance_Per_Second(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
	else
	{
		return SJ230_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
}

//最大转速 100%
uint32_t Get_Motor_Rpm_Speed_Max(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_RPM_SPEED_MAX;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_RPM_SPEED_MAX;
	}
	else
	{
		return SJ230_MOTOR_RPM_SPEED_MAX;
	}
}


//最低转速 20%
uint32_t Get_Motor_Rpm_Speed_Mix(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_RPM_SPEED_MIX;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_RPM_SPEED_MIX;
	}
	else
	{
		return SJ230_MOTOR_RPM_SPEED_MIX;
	}
}

// 功率 降频
//*********************************************************************************************
//-------------- 电机功率 报警值  -------------------
uint32_t Get_Motor_Power_Alarm_Value(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_ALARM_VALUE;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_ALARM_VALUE;
	}
	else
	{
		return SJ230_MOTOR_POWER_ALARM_VALUE;
	}
}

//-------------- 电机功率 降速  -------------------
uint32_t Get_Motor_Power_Reduce_Speed(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_REDUCE_SPEED;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_REDUCE_SPEED;
	}
	else
	{
		return SJ230_MOTOR_POWER_REDUCE_SPEED;
	}
}

//-------------- 电机功率 恢复  -------------------
uint32_t Get_Motor_Power_Restore_Speed(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_RESTORE_SPEED;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_RESTORE_SPEED;
	}
	else
	{
		return SJ230_MOTOR_POWER_RESTORE_SPEED;
	}
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
	Finish_Statistics_Clean();
}












//---------------------------------------------------------------------------------------------------------------------------------------------------






