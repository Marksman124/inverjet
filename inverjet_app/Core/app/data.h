/**
******************************************************************************
* @file    		display.h
* @brief   		显示模块  显示模块本身不占用线程,由其它线程任务调用
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
	CTRL_FROM_KEY = 0,				//	按键
	CTRL_FROM_WIFI,						//	wifi
	CTRL_FROM_BT,							//	蓝牙
	CTRL_FROM_RS485,					//	modbus 485	
} System_Ctrl_Mode_Type_enum;


// 降频 类型
typedef enum 
{
	MOTOR_DOWN_CONVERSION_NO = 0,						//	无
	MOTOR_DOWN_CONVERSION_MOS_TEMPER,				//	mos 高温
	MOTOR_DOWN_CONVERSION_BOX_TEMPER,				//	机箱 高温
	MOTOR_DOWN_CONVERSION_OUT_CURRENT,			//	输出 电流
	MOTOR_DOWN_CONVERSION_OUT_POWER,				//	输出 功率
} Motor_Down_Conversion_Type_enum;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------- 训练模式 最大值 -------------------

#define TRAINING_MODE_NUMBER_MAX						SYSTEM_MODE_TRAIN_P5
#define TRAINING_MODE_PERIOD_MAX						50
//冲浪模式 --》 P5
#define SURFING_MODE_NUMBER_ID							SYSTEM_MODE_TRAIN_P5
//------------------- 合法范围 ----------------------------
#define SPEED_LEGAL_MIN						MOTOR_PERCENT_SPEED_MIX
#define SPEED_LEGAL_MAX						MOTOR_PERCENT_SPEED_MAX

#define TIME_LEGAL_MIN						20
#define TIME_LEGAL_MAX						MOTOR_TIME_SHOW_MAX

#define	MOTOR_RPM_NUMBER_OF_POLES								5		//电机级数 默认值
#define	MOTOR_RPM_MIX_OF_POLES									1		//电机级数 合法最小值
#define	MOTOR_RPM_MAX_OF_POLES									8		//电机级数 合法最大值

//------------------- 屏蔽控制 ----------------------------
#define	BLOCK_BLUETOOTH_CONTROL									1		// 屏蔽 蓝牙 控制
#define	BLOCK_MODBUS_CONTROL										2		// 屏蔽 MODBUS 控制
#define	BLOCK_WIFI_CONTROL											4		// 屏蔽 WIFI 控制

// 线程 活动
#define	THREAD_ACTIVITY_BREATH_LIGHT						( 1<<0 )
#define	THREAD_ACTIVITY_RS485_MODBUS						( 1<<1 )
#define	THREAD_ACTIVITY_MAIN										( 1<<2 )
#define	THREAD_ACTIVITY_KEY_BUTTON							( 1<<3 )
#define	THREAD_ACTIVITY_MOTOR										( 1<<4 )
#define	THREAD_ACTIVITY_WIFI										( 1<<5 )


/* Exported functions prototypes ---------------------------------------------*/
//================= 冲浪模式 全局 参数 ================================
extern void Surf_Mode_Info_Data_Init(void);

extern uint8_t Check_Data_Init(void);

extern void App_Data_Init(void);

extern void App_Data_ReInit(void);
// 读 flash
extern uint8_t Read_OPMode(void);
extern uint8_t Memset_OPMode(void);

//存储 新 速度
extern void Update_OP_Speed(void);
//存储 新 时间
extern void Update_OP_Time(void);
//存储 新 速度 & 时间
extern void Update_OP_All(void);

extern void OP_Update_Mode(void);
//检查 新 速度 & 时间  防止溢出
void Check_OP_All(void);

// 更改属性值
extern uint8_t Set_Data_OPMode(Operating_Parameters* p_op);
//------------------- 判断 模式 合法 ----------------------------
uint8_t Is_Mode_Legal(uint8_t mode);
//------------------- 判断速度 合法 ----------------------------
extern uint8_t Is_Speed_Legal(uint16_t speed);

extern uint8_t Is_Time_Legal(uint16_t time);

//------------------- 设置 当前 速度 ----------------------------
extern void Data_Set_Current_Speed(uint8_t speed);
//------------------- 设置 当前 时间 ----------------------------
extern void Data_Set_Current_Time(uint16_t time);
//------------------- 设置 训练时段 ----------------------------
extern void Set_Pmode_Period_Now(uint16_t value);
//------------------- 是否接收外部控制 ----------------------------
extern uint8_t If_Accept_External_Control(uint8_t mode);
//------------------- 获取软件版本号  字符串转 uint32 ----------------------------
extern uint32_t get_uint3_version(char * buffer);
//------------------- 清除wifi标志 ----------------------------
extern void System_Wifi_State_Clean(void);

//------------------- 设置控制方式 ----------------------------
extern void Set_Ctrl_Mode_Type(System_Ctrl_Mode_Type_enum type);
//------------------- 获取控制方式 ----------------------------
extern System_Ctrl_Mode_Type_enum Get_Ctrl_Mode_Type(void);

//------------------- OTA 自动退出 1秒进一次 ----------------------------
extern void OTA_Time_Out(void);


//------------------- 获取机型码 ----------------------------
extern uint32_t Get_Model_Code_Num(void);
// 每 %1 每秒 增加游泳距离 放大100倍
extern uint32_t Get_Every_1Percent_Distance_Per_Second(void);
//最大转速 100%
extern uint32_t Get_Motor_Rpm_Speed_Max(void);
//最低转速 20%
extern uint32_t Get_Motor_Rpm_Speed_Mix(void);
// 功率 降频
//*********************************************************************************************
//-------------- 电机功率 报警值  -------------------
extern uint32_t Get_Motor_Power_Alarm_Value(void);
//-------------- 电机功率 降速  -------------------
extern uint32_t Get_Motor_Power_Reduce_Speed(void);
//-------------- 电机功率 恢复  -------------------
extern uint32_t Get_Motor_Power_Restore_Speed(void);
//*********************************************************************************************
// 电流 降频
//*********************************************************************************************
//-------------- 输出电流 报警值  -------------------
extern uint32_t Get_Motor_Current_Alarm_Value(void);
//-------------- 输出电流 降速  -------------------
extern uint32_t Get_Motor_Current_Reduce_Speed(void);
//-------------- 输出电流 恢复  -------------------
extern uint32_t Get_Motor_Current_Restore_Speed(void);
//*********************************************************************************************

// 完成数据统计
//*********************************************************************************************
//-------------- 计算 完成统计  -------------------
extern void Finish_Statistics_Count(uint8_t count);
//-------------- 清除 完成统计  -------------------
extern void Finish_Statistics_Clean( void );
//-------------- 上传 完成统计  -------------------
extern void Finish_Statistics_Upload( void );
	

//-------------- 线程 活动 标志清零  -------------------
extern void Thread_Activity_Sign_Clean( void );
//-------------- 线程 活动 设置  -------------------
extern void Thread_Activity_Sign_Set( uint16_t val );
	

/* Private defines -----------------------------------------------------------*/

extern Operating_Parameters OP_ShowNow;

extern Operating_Parameters* p_OP_ShowLater;


// 训练模式 当前状态
extern uint8_t PMode_Now;

extern uint8_t Period_Now;

// 各模式 属性 初始值
extern Operating_Parameters OP_Init_Free;

extern Operating_Parameters OP_Init_Timing;

extern Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX];

//--------------------------- 系统属性
extern uint16_t* p_System_State_Machine;			//状态机
extern uint16_t* p_PMode_Now;									// 当前模式
extern uint16_t* p_OP_ShowNow_Speed;					// 当前速度
extern uint16_t* p_OP_ShowNow_Time;						// 当前时间

extern System_Ctrl_Mode_Type_enum Ctrl_Mode_Type;				// 控制方式  0:按键   1:wifi 2:bt
// 各模式 属性
extern Operating_Parameters* p_OP_Free_Mode;

extern Operating_Parameters* p_OP_Timing_Mode;

extern Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX];
//==========================================================
//--------------------------- 驱动板读取信息
//==========================================================
extern uint16_t* p_Driver_Software_Version;			// 驱动板软件版本
extern uint16_t* p_Motor_Fault_Static;						// 故障状态		驱动板

extern uint32_t* p_Motor_Reality_Speed;					// 电机 实际 转速
extern uint32_t* p_Motor_Reality_Power;					// 电机 实际 功率

extern uint16_t* p_Mos_Temperature;							// mos 温度
extern uint32_t* p_Motor_Current;								// 电机 电流		输出
extern uint16_t* p_Motor_Bus_Voltage;						// 母线 电压		输入
extern uint16_t* p_Motor_Bus_Current;						// 母线 电流  	输入

//==========================================================
//--------------------------- 整机信息
//==========================================================
extern uint16_t* p_System_Fault_Static;					// 故障状态		整机
extern uint16_t* p_Box_Temperature;							// 电箱 温度
extern uint32_t* p_Send_Reality_Speed;					// 下发 实际 转速


extern uint16_t* p_Support_Control_Methods;			//屏蔽控制方式
extern uint16_t* p_Motor_Pole_Number;						//电机极数
extern uint16_t* p_Breath_Light_Max;						//光圈亮度  
	
extern uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX];	//电机状态

//================= 临时变量  全局 ================================
// ----------------------------------------------------------------------------------------------
extern uint16_t* p_Surf_Mode_Info_Acceleration;  			//	冲浪模式 -- 加速度
extern uint16_t* p_Surf_Mode_Info_Prepare_Time;  			//	冲浪模式 -- 准备时间
extern uint16_t* p_Surf_Mode_Info_Low_Speed;  				//	冲浪模式 -- 低速档 -- 速度
extern uint16_t* p_Surf_Mode_Info_Low_Time;						//	冲浪模式 -- 低速档 -- 时间
extern uint16_t* p_Surf_Mode_Info_High_Speed;  				//	冲浪模式 -- 高速档 -- 速度
extern uint16_t* p_Surf_Mode_Info_High_Time;  				//	冲浪模式 -- 高速档 -- 时间
// ----------------------------------------------------------------------------------------------

extern uint8_t WIFI_Rssi;

extern uint16_t* p_Analog_key_Value;							// 虚拟按键

extern uint8_t System_PowerUp_Finish;

extern uint8_t System_Self_Testing_State;

//================= 调试使用  时间 ================================

extern uint32_t* p_System_Runing_Second_Cnt;			// 系统时间
extern uint32_t* p_No_Operation_Second_Cnt;				// 无人操作时间
extern uint32_t* p_System_Startup_Second_Cnt;			// 启动时间

//==========================================================
//--------------------------- 完成统计 (APP要)
//==========================================================
extern uint16_t* p_Finish_Statistics_Time;					//	完成统计 --> 时长
extern uint16_t* p_Finish_Statistics_Speed;					//	完成统计 --> 强度
extern uint32_t* p_Finish_Statistics_Distance;			//	完成统计 --> 游泳距离
extern uint16_t* p_Preparation_Time_BIT;						//	准备时间 Bit: 定时模式 P1-P6

extern uint16_t* p_Thread_Activity_Sign;					//	线程 活动 标志位

#ifdef __cplusplus
}
#endif

#endif /* __DATA_H__ */

