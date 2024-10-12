/**
******************************************************************************
* @file    		modbus.h
* @brief   		modbus协议
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MODBUS_H__
#define __MODBUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mb.h"
#include "mbport.h"
#include "usart.h"
#include "iap.h"
#include "string.h"
#include "gpio.h"

#include "metering.h" // 消息队列

#include "control_interface.h"	 // 控制接口
/* Private defines -----------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#ifndef __MACRO_DEFINITION_H__

#define MODBUS_THREAD_LIFECYCLE							10000				// ms 暂时不用

#endif

extern UART_HandleTypeDef* p_huart_mb;		 //UART句柄

/* ----------------------- Defines ------------------------------------------*/
// 03
#define REG_HOLDING_START               ( MB_SLAVE_NODE_ADDRESS )
#define REG_HOLDING_NREGS               ( MB_HOLDING_BUFFER_SIZE_MAX )   //MB_USER_TRAIN_MODE_TIME_P4_50

// 04
#define REG_INPUT_START 								( MB_DISPLAY_SOFTWARE_VERSION )
#define REG_INPUT_NREGS 								( MB_INPUT_BUFFER_SIZE_MAX )

// 21
#define REG_FILE_NUMBER_MAX 								( 0x270F )
#define REG_FILE_NUMBER_STAR 								( 0 )
#define REG_FILE_NUMBER_END 								( 0xFFFF )
#define REG_FILE_LENTH_MAX 									( 0x77 )

#define MODBUS_RESTART_TIMEOUT 							( 10 )					// 100MS * N


// 03 Holding

#define MB_SLAVE_NODE_ADDRESS								( 0x00 )	//	从机节点地址
#define MB_SLAVE_BAUD_RATE             			( 0x01 )	//	波特率
#define MB_SUPPORT_CONTROL_METHODS          ( 0x02 )	//	0:可控;1：屏蔽  // 1bit：蓝牙; 2bit：Modbus-RS485; 3bit：wifi;
#define MB_DISTRIBUTION_NETWORK_CONTROL  		( 0x03 )	//	配网控制 遥控 & wifi
#define MB_MOTOR_POLE_NUMBER  							( 0x04 )	//	电机磁极数
#define MB_MOTOR_SPEED_MODE  								( 0x05 )	//	转速 方式
#define MB_MOTOR_BREATH_LIGHT_MAX  					( 0x06 )	//	光圈亮度
// ----------------------------------------------------------------------------------------------
#define MB_PREPARATION_TIME_BIT     				( 0x20 )	//	预备时间（标志位）  准备时间 Bit: 定时模式 P1-P6
// ----------------------------------------------------------------------------------------------
#define MB_SYSTEM_WORKING_MODE     					( 0x21 )	//	系统工作模式  高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
#define MB_SYSTEM_WORKING_STATUS      			( 0x22 )	//	系统工作状态  0:暂停,   1:暂停恢复,   2:重新开始,  3:结束
// ----------------------------------------------------------------------------------------------
#define MB_MOTOR_CURRENT_SPEED		        	( 0x23 )	//	当前转速 (临时有效)
#define MB_MOTOR_CURRENT_TIME        				( 0x24 )	//	当前时间
// ----------------------------------------------------------------------------------------------
#define MB_MOTOR_LEATER_SPEED		        		( 0x30 )	//	当前转速 (临时有效)
#define MB_MOTOR_LEATER_TIME        				( 0x31 )	//	当前时间
// ----------------------------------------------------------------------------------------------
#define MB_ANALOG_KEY_VALUE        					( 0x40 )	//	虚拟按键 (一次有效)
// ----------------------------------------------------------------------------------------------
#define MB_USER_FREE_MODE_SPEED		        	( 0x80 )	//	用户 自由模式 	转速
#define MB_USER_FREE_MODE_TIME        			( 0x81 )	//								时间
#define MB_USER_TIME_MODE_SPEED		        	( 0x82 )	//	用户 定时模式 	转速
#define MB_USER_TIME_MODE_TIME        			( 0x83 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P1_1		        ( 0x100 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P1_1        		( 0x101 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P1_2		        ( 0x102 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P1_2        		( 0x103 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P1_50		      ( 0x162 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P1_50        		( 0x163 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P2_1		        ( 0x180 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_1        		( 0x181 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P2_2		        ( 0x182 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_2        		( 0x183 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P2_50		      ( 0x1E2 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_50        		( 0x1E3 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P3_1		        ( 0x200 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_1        		( 0x201 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P3_2		        ( 0x202 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_2        		( 0x203 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P3_50		      ( 0x262 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_50        		( 0x263 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P4_1		        ( 0x280 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_1        		( 0x281 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P4_2		        ( 0x282 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_2        		( 0x283 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P4_50		      ( 0x2E2 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_50        		( 0x2E3 )	//								时间

#define MB_HOLDING_BUFFER_SIZE_MAX        			( 0x2FF )	//	
// ----------------------------------------------------------------------------------------------


// 04 Input
#define MB_DISPLAY_SOFTWARE_VERSION								( 0x00 )	//	显示板 软件版本
#define MB_DISPLAY_HARDWARE_VERSION             	( 0x02 )	//	显示板 硬件版本
#define MB_DRIVER_SOFTWARE_VERSION								( 0x04 )	//	驱动板 软件版本
#define MB_DRIVER_HARDWARE_VERSION             		( 0x06 )	//	驱动板 硬件版本
#define MB_SYSTEM_FAULT_STATUS					          ( 0x08 )	//	系统故障状态
//#define MB_CHASSIS_TEMPERATURE					          ( 0x09 )	//	机箱  温度

#define MB_MOTOR_FAULT_STATUS					          	( 0x09 )	// 	电机 	故障状态
#define MB_MOS_TEMPERATURE					          		( 0x0A )	//	mos 	温度
#define MB_BOX_TEMPERATURE					          		( 0x0B )	//	电箱	温度
#define MB_MOTOR_BUS_VOLTAGE					          	( 0x0C )	//	母线 	电压
#define MB_MOTOR_BUS_CURRENT					          	( 0x0D )	//	母线 	电流
#define MB_MOTOR_CURRENT					          			( 0x0E )	// 	（2字节） 电机 	电流
#define MB_MOTOR_REALITY_SPEED					          ( 0x10 )	//	（2字节） 电机 	实际 转速
#define MB_SEND_REALITY_SPEED					          	( 0x12 )	//	（2字节） 电机 	实际 转速  下发
#define MB_MOTOR_REALITY_POWER					          ( 0x14 )	//	（2字节） 电机 	实际 功率
#define MB_SIMULATED_SWIM_DISTANCE		      			( 0x16 )	//	模拟游泳距离

#define MB_FINISH_STATISTICS_TIME		      				( 0x17 )	//	完成统计 --> 时长
#define MB_FINISH_STATISTICS_SPEED		      			( 0x18 )	//	完成统计 --> 游泳强度
#define MB_FINISH_STATISTICS_DISTANCE		      		( 0x19 )	//	完成统计 --> 游泳距离
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
#define MB_SYSTEM_LAST_KEY_VALUE        		( 0x22 )	//	最后一次按键
#define MB_SYSTEM_LAST_KEY_MODE        			( 0x23 )	//	最后一次 模式
#define MB_SYSTEM_LAST_KEY_STATUS        		( 0x24 )	//	最后一次 状态
#define MB_SYSTEM_LAST_KEY_SPEED        		( 0x25 )	//	最后一次 速度
#define MB_SYSTEM_LAST_KEY_TIME        			( 0x26 )	//	最后一次 时间

// ----------------------------------------------------------------------------------------------
#define MB_LCD_MAPPING_MODE        					( 0x30 )	//	模式
#define MB_LCD_MAPPING_SPEED        				( 0x31 )	//	速度
#define MB_LCD_MAPPING_TIME_HIGH        		( 0x32 )	//	时间 高
#define MB_LCD_MAPPING_TIME_LOW        			( 0x33 )	//	时间 低
#define MB_LCD_MAPPING_SYMBOL        				( 0x34 )	//	符号
// ----------------------------------------------------------------------------------------------
#define MB_SYSTEM_RUNNING_TIME        			( 0x40 )	//	运行时间
#define MB_NO_OPERATION_TIME        				( 0x42 )	//	无人操作时间
#define MB_SYSTEM_SLEEP_TIME        				( 0x44 )	//	休眠时间
// ----------------------------------------------------------------------------------------------
#define MB_INPUT_BUFFER_SIZE_MAX        					( 0xFF )	//	
/*
#define MB_DEFAULT_FREE_MODE_SPEED		        		( 0x11 )	//	默认 自由模式 	转速
#define MB_DEFAULT_FREE_MODE_TIME        					( 0x12 )	//								时间
#define MB_DEFAULT_TIME_MODE_SPEED		        		( 0x13 )	//	默认 定时式 	转速
#define MB_DEFAULT_TIME_MODE_TIME        					( 0x14 )	//								时间
#define MB_DEFAULT_TRAIN_MODE_SPEED_P1_1		      ( 0x15 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P1_1        		( 0x16 )	//								时间
#define MB_DEFAULT_TRAIN_MODE_SPEED_P1_2		      ( 0x17 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P1_2        		( 0x18 )	//								时间
//		.......  12
#define MB_DEFAULT_TRAIN_MODE_SPEED_P1_12		      ( 0x2B )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P1_12        	( 0x2C )	//								时间

#define MB_DEFAULT_TRAIN_MODE_SPEED_P2_1		      ( 0x2D )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P2_1        		( 0x2E )	//								时间
#define MB_DEFAULT_TRAIN_MODE_SPEED_P2_2		      ( 0x2F )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P2_2        		( 0x30 )	//								时间
//		.......  12
#define MB_DEFAULT_TRAIN_MODE_SPEED_P2_12		      ( 0x43 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P2_12        	( 0x44 )	//								时间

#define MB_DEFAULT_TRAIN_MODE_SPEED_P3_1		      ( 0x45 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P3_1        		( 0x46 )	//								时间
#define MB_DEFAULT_TRAIN_MODE_SPEED_P3_2		      ( 0x47 )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P3_2        		( 0x48 )	//								时间
//		.......  12
#define MB_DEFAULT_TRAIN_MODE_SPEED_P3_12		      ( 0x5B )	//	默认 训练模式 	转速
#define MB_DEFAULT_TRAIN_MODE_TIME_P3_12        	( 0x5C )	//								时间

*/

/* Exported functions prototypes ---------------------------------------------*/

extern void Modbus_Init(void);
extern void Modbus_Handle_Task(void);

// *********  修改内部数据接口  *************************************
extern void Modbus_Buffer_Init(void);
extern void MB_Flash_Buffer_Write(void);
extern void MB_Flash_Buffer_Read(void);
extern uint16_t* Get_DataAddr_Pointer(UCHAR ucFunctionCode, USHORT addr);
extern uint16_t Get_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr);
extern void Set_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr, uint16_t value);
extern void Set_DataValue_U32(UCHAR ucFunctionCode, USHORT addr, uint32_t value);
extern void Set_DataValue_Len(UCHAR ucFunctionCode, USHORT addr, uint8_t* p_data, uint8_t len);

extern void Get_Mapping_Register(void);
#ifdef __cplusplus
}
#endif

#endif /* __MODBUS_H__ */

