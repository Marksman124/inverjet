/**
******************************************************************************
* @file    		macro_definition.h
* @brief   		系统参数宏定义
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-8-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MACRO_DEFINITION_H__
#define __MACRO_DEFINITION_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "model_parameter.h"


/* Exported types ------------------------------------------------------------*/

// 降频 类型
typedef enum 
{
	MOTOR_DOWN_CONVERSION_NO = 0,						//	无
	MOTOR_DOWN_CONVERSION_MOS_TEMPER,				//	mos 高温
	MOTOR_DOWN_CONVERSION_BOX_TEMPER,				//	机箱 高温
	MOTOR_DOWN_CONVERSION_OUT_CURRENT,			//	输出 电流
	MOTOR_DOWN_CONVERSION_OUT_POWER,				//	输出 功率
} Motor_Down_Conversion_Type_enum;



/* Exported macro ------------------------------------------------------------*/


/*========================================== <main.h> macro ====================================================*/

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

// 软件版本
#define	MACRO_SOFTWARE_VERSION_UINT32									"0.0.4"

/**
******************************************************************************
* 系统宏定义
******************************************************************************
*/

//#define SYSTEM_DEBUG_MODE								1	// 调试模式
//#define UART_PRINTF_LOG									1	// 打印日志
//#define UART_DEBUG_SEND_CTRL						1	// 通过 调试串口 发送指令
//#define SYSTEM_LONG_RUNNING_MODE				1	// 老化模式
//*******************************************************

//******************  驱动板 型号选择 **************************
#define MOTOR_DEVICE_HARDWARE_AQPED002					(1)			//	郭工 版
#define MOTOR_DEVICE_HARDWARE_TEMP001						(2)			//	蓝工 版

//*******************************************************

#define MOTOR_DEVICE_PROTOCOL_VERSION						(MOTOR_DEVICE_HARDWARE_AQPED002)				// 驱动协议版本

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
/*==============================================================================================================*/
// 串口1 --> 中控Modbus 	(485)
// 串口2 --> wifi 			(ttl)
// 串口3 --> 驱动板 			(ttl)
// 串口4 --> 调试 debug 	(ttl)
// 串口5 --> 蓝牙				(ttl)

#define MACRO_MODBUS_USART								1
#define	MACRO_WIFI_USART									2
#define	MACRO_DRIVER_USART								3
#define	MACRO_DEBUG_USART									4
#define	MACRO_BLUETOOTH_USART							5

//串口总数
#define	MACRO_SYSTEM_USER_USART_MAX										(5)


#define MACRO_POWER_ON_WAITE_TIME_TASK								(4000)			//上电等待时间 （等显示开机界面 机型码）

/*==============================================================================================================*/
/*==============================================================================================================*/
	
/*========================================== <Breath_light.h> macro ============================================*/
/*==============================================================================================================*/
#define BREATH_LIGHT_THREAD_TURN_ON					1

#if(BREATH_LIGHT_THREAD_TURN_ON)
//线程周期
#define BREATH_LIGHT_THREAD_LIFECYCLE				(20)				// ms
//光圈 pwm 通道号
#define BREATH_LIGHT_PWM_CHANNEL						(TIM_CHANNEL_1)

//******************  调试模式 **************************
#ifdef SYSTEM_DEBUG_MODE
#define LIGHT_BRIGHTNESS_MIX					(0)			// 最低亮度  
#define LIGHT_BRIGHTNESS_MAX					(50)		// 最大亮度  0~500
#else
#define LIGHT_BRIGHTNESS_MIX					(0)			// 最低亮度  
#define LIGHT_BRIGHTNESS_MAX					(500)		//(*p_Breath_Light_Max)			// 最大亮度  0~500
#endif
//*******************************************************
//档位
#define BREATH_LIGHT_GEAR_POSITION						(1000/BREATH_LIGHT_THREAD_LIFECYCLE)				// 档位 50
//步进
#define LIGHT_BRIGHTNESS_STEP									((LIGHT_BRIGHTNESS_MAX-LIGHT_BRIGHTNESS_MIX)/BREATH_LIGHT_GEAR_POSITION)

//---------- 暂停下 5秒周期
//档位
#define BREATH_LIGHT_GEAR_POSITION_LOW				(2500/BREATH_LIGHT_THREAD_LIFECYCLE)				// 档位 75
//步进
#define LIGHT_BRIGHTNESS_STEP_LOW							((LIGHT_BRIGHTNESS_MAX-LIGHT_BRIGHTNESS_MIX)/BREATH_LIGHT_GEAR_POSITION_LOW)


#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <key.h> macro =====================================================*/
/*==============================================================================================================*/
#define KEY_THREAD_TURN_ON					1

#if(KEY_THREAD_TURN_ON)
//线程周期
#define KEY_THREAD_LIFECYCLE								(20)	// 任务生命周期 200ms

#define KEY_LONG_PRESS_TIME									(2000/KEY_THREAD_LIFECYCLE)			//长按时间 2s
#define KEY_LONG_PRESS_TIME_SHORT						(1000/KEY_THREAD_LIFECYCLE)			//短一点的 长按时间  1s
//-------------- 蜂鸣器 长度 -------------------
#define KEY_BUZZER_TIME								((200/KEY_THREAD_LIFECYCLE)-2)					//周期  KEY_THREAD_LIFECYCLE 倍数
#define KEY_BUZZER_TIME_LONG					((400/KEY_THREAD_LIFECYCLE)-2)					//周期  KEY_THREAD_LIFECYCLE 倍数
#define KEY_BUZZER_TIME_LONG_32				((800/KEY_THREAD_LIFECYCLE)-2)					//周期  KEY_THREAD_LIFECYCLE 倍数
/* 蜂鸣器 音量  50最大  ------------------------------------------------------------*/
//******************  调试模式 **************************
#ifdef SYSTEM_DEBUG_MODE
#define BUZZER_FREQUENCY					1
#else
#define BUZZER_FREQUENCY					50					// wuqingguang
#endif
//*******************************************************

#define KEY_VALUE_SHAKE_TIME					(1)		//去抖动 次数

//屏幕背光 pwm 控制 
//#define LCD_BACK_LIGHT_PWM_CTRL							1

//屏幕背光 pwm 通道号
#define LCD_BACK_LIGHT_PWM_CHANNEL						(TIM_CHANNEL_2)
//屏幕背光 最大亮度
#define BACK_LIGHT_BRIGHTNESS_MAX						(500)			// 最大亮度  0~500
//休眠时间(目前无效)
#define KEY_FOR_SLEEP_TIME_SHORT						(3000/KEY_THREAD_LIFECYCLE)			//5 min  300000

// 电机速度每档增加量 5档
#define	KEY_SPEED_INCREASE_20_GEAR										(1)
// 电机速度每档增加量	100档
#define	KEY_SPEED_INCREASE_100_GEAR										(20)


#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <timing.h> macro ==================================================*/
/*==============================================================================================================*/
#define TIMING_THREAD_TURN_ON					1

#if(TIMING_THREAD_TURN_ON)
#define TIMING_THREAD_LIFECYCLE				(24)				// ms

//半秒周期数
#define TIMING_THREAD_HALF_SECOND			(480/TIMING_THREAD_LIFECYCLE)				// 0.5 s   16
//1秒周期数
#define TIMING_THREAD_ONE_SECOND			(2)				// 1 s
//******************  调试模式 **************************
#ifdef SYSTEM_DEBUG_MODE
//配网时长
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(60*TIMING_THREAD_ONE_SECOND)				// 6 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(60*TIMING_THREAD_ONE_SECOND)				// 6 s
//故障自恢复
#define SYSTEM_FAULT_TIME_CALLOUT							(20*TIMING_THREAD_ONE_SECOND)				// 20 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 次故障
#define SYSTEM_FAULT_RECOVERY_TIME						(60*TIMING_THREAD_ONE_SECOND)				// 1 分钟  60 s
//自动关机
#define AUTOMATIC_SHUTDOWN_TIME								(600*TIMING_THREAD_ONE_SECOND)				// 10 min

#else
//配网时长
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(60*TIMING_THREAD_ONE_SECOND)				// 60 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(60*TIMING_THREAD_ONE_SECOND)				// 60 s
//故障自恢复
#define SYSTEM_FAULT_TIME_CALLOUT							(120*TIMING_THREAD_ONE_SECOND)				// 120 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 次故障
#define SYSTEM_FAULT_RECOVERY_TIME						(3600*TIMING_THREAD_ONE_SECOND)				// 1 小时内  3600 s
//自动关机
#define AUTOMATIC_SHUTDOWN_TIME								(1800*TIMING_THREAD_ONE_SECOND)				// 0.5 小时内  1800 s

#endif


//-------------------------------------------------------------------------------------------------
//*********************************************************************************************
//-------------- 机箱温度报警值 90°C -------------------
#define AMBIENT_TEMP_ALARM_VALUE									(80)
//-------------- 机箱温度 降速 70°C -------------------
#define AMBIENT_TEMP_REDUCE_SPEED									(70)		// 降档 温度
#define AMBIENT_TEMP_RESTORE_SPEED								(65)		// 恢复 温度
//*********************************************************************************************
//-------------------------------------------------------------------------------------------------

//-------------- 降速检查时间 -------------------
#define TIME_SLOW_DOWN_TIME													(120)		//2 min  120*2 个周期,与线程周期相关
//-------------- 降速 档位 -------------------
#define TIME_SLOW_DOWN_SPEED_01											(10)		//第一档 降速
#define TIME_SLOW_DOWN_SPEED_02											(5)			//第二档 降速
//-------------- 降速 最低速度 -------------------
#define TIME_SLOW_DOWN_SPEED_MIX										(20)		//最低降到 20%
#define TIME_SLOW_DOWN_SPEED_MAX										(100)		//恢复速度最高恢复到 100%

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <modbus.h> macro ==================================================*/
/*==============================================================================================================*/
#define MODBUS_THREAD_TURN_ON					1

#if(MODBUS_THREAD_TURN_ON)
#define MODBUS_THREAD_LIFECYCLE							(40)				// ms 暂时不用

//1秒周期数
#define MODBUS_THREAD_ONE_SECOND						(1000/MODBUS_THREAD_LIFECYCLE)				// 1 s

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/



/*========================================== <motor.h> macro ===================================================*/
/*==============================================================================================================*/
#define MOTOR_THREAD_TURN_ON					1

#if(MOTOR_THREAD_TURN_ON)

#define MOTOR_THREAD_LIFECYCLE						(20)				// 任务生命周期 50ms

// 命令 周期 200ms 
#define MOTOR_POLLING_PERIOD							(200/MOTOR_THREAD_LIFECYCLE)

// 心跳 周期 200ms 
#define MOTOR_HEARTBEAT_CYCLE							(MOTOR_POLLING_PERIOD/3)
// 命令 周期 200ms
#define MOTOR_COMMAND_CYCLE								((MOTOR_POLLING_PERIOD/3)*2)
// 读状态 周期 200ms 
#define MOTOR_READ_STATIC_CYCLE						(0)


// 驱动状态检验   电机电流 报警时间  ------------------
//#define MOTOR_CANNOT_START_TIME						(5000 / MOTOR_POLLING_PERIOD / MOTOR_THREAD_LIFECYCLE)		// wuqingguang
// 驱动状态检验   电机转速 报警时间  ------------------
//#define MOTOR_SPEED_ERROR_TIME						(5000 / MOTOR_POLLING_PERIOD/ MOTOR_THREAD_LIFECYCLE)			// wuqingguang


//*****************************************************************************
//电机极数
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
#define	MOTOR_POLE_NUMBER									(5)
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
#define	MOTOR_POLE_NUMBER									(1)
#endif
//*****************************************************************************

// 700  1012   1324  1637   1950
//每 1% 转速
#define	MOTOR_RPM_CONVERSION_COEFFICIENT				((MOTOR_RPM_SPEED_MAX - MOTOR_RPM_SPEED_MIX) /80)			//15.6			78

//每 20% 转速
#define	MOTOR_RPM_CONVERSION_COEFFICIENT_20				((MOTOR_RPM_SPEED_MAX - MOTOR_RPM_SPEED_MIX) /4)			//312.5     1562

//电机最高速度  百分比  100%
#define	MOTOR_PERCENT_SPEED_MAX										(100)
//电机最低速度  百分比  20%
#define	MOTOR_PERCENT_SPEED_MIX										(20)

#ifdef MOTOR_CANNOT_START_TIME
//电机最低电流
#define	MOTOR_CURRENT_MIX									(1000)				//10A
#endif

#ifdef MOTOR_SPEED_ERROR_TIME
//电机转速误差范围
#define	MOTOR_SPEED_VIBRATION_RANGE					(100*MOTOR_POLE_NUMBER)				//乘以电机极数
#endif

//电机加速度
#define	MOTOR_ACCELERATION										(1)

//-------------------------------------------------------------------------------------------------
//*********************************************************************************************
//-------------- MOS 温度报警值 90°C -------------------
#define MOS_TEMP_ALARM_VALUE								(90)
//-------------- MOS 温度 降速 80°C -------------------
#define MOS_TEMP_REDUCE_SPEED								(80)		// 降档 温度
#define MOS_TEMP_RESTORE_SPEED							(75)		// 恢复 温度
//*********************************************************************************************
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//*********************************************************************************************

//-------------------------------------------------------------------------------------------------

//通讯故障 报警时间
#define FAULT_MOTOR_LOSS_TIME							(30000/(MOTOR_THREAD_LIFECYCLE))				// 30 秒
//通讯故障 尝试重启 时间
#define FAULT_MOTOR_TRY_RESTAR_TIME				(FAULT_MOTOR_LOSS_TIME/5)								// 6秒

/*------------------- IO define ----------------------------------------------*/
#define	MOTOR_MODULE_HUART				DRIVER_USART		//

#if (MOTOR_MODULE_HUART == 1)
#define MOTOR_RS485_TX_EN_PORT		RS48501_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48501_RE_Pin
#elif (MOTOR_MODULE_HUART == 4)
#define MOTOR_RS485_TX_EN_PORT		RS48504_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48504_RE_Pin
#endif

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/



/*========================================== <wifi_thread.h> macro ==================================================*/
/*==============================================================================================================*/
#define WIFI_THREAD_TURN_ON					1

#if(WIFI_THREAD_TURN_ON)
#define WIFI_THREAD_LIFECYCLE											(40)				// ms 暂时不用

//1秒周期数
#define WIFI_THREAD_ONE_SECOND									(1000/WIFI_THREAD_LIFECYCLE)				// 1 s

#define OTA_SHUTDOWN_TIME_OUT										(1800)				//  s

#define WIFI_DATE_UPLOAD_TIME_NORMAL							(1 * WIFI_THREAD_ONE_SECOND)				// 普通数据 时间 1s

#ifdef SYSTEM_LONG_RUNNING_MODE
//********* 老化工装 ***********************************************
// 老化工装 快速上报 方便查看运行状态
#define WIFI_DATE_UPLOAD_TIME											(1 * WIFI_THREAD_ONE_SECOND)				// 1s
#else
#define WIFI_DATE_UPLOAD_TIME											(10 * WIFI_THREAD_ONE_SECOND)				// 不常用数据 时间 10s
//******************************************************************
#endif

//wifi 故障 判断值
#define WIFI_RSSI_ERROR_VAULE										(50)

// 结束统计 上传 最低时间 -- 低于 3min 不计入统计  单位(秒)
#define WIFI_STATISTICE_UPLOAD_MINIMUM_TIME										(180)


#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* __MACRO_DEFINITION_H__ */

