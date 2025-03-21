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
#define REG_HOLDING_START               ( 0 )
#define REG_HOLDING_NREGS               ( MB_HOLDING_BUFFER_SIZE_MAX )   //MB_USER_TRAIN_MODE_TIME_P4_50

// 04
#define REG_INPUT_START 								( 0 )
#define REG_INPUT_NREGS 								( MB_INPUT_BUFFER_SIZE_MAX )

// 21
#define REG_FILE_NUMBER_MAX 								( 0x270F )
#define REG_FILE_NUMBER_STAR 								( 0 )
#define REG_FILE_NUMBER_END 								( 0xFFFF )
#define REG_FILE_LENTH_MAX 									( 0x77 )

#define MODBUS_RESTART_TIMEOUT 							( 10 * MODBUS_THREAD_ONE_SECOND)					// 100MS * N


// 03 Holding

#define MB_SLAVE_NODE_ADDRESS								( 0x00 )	//	从机节点地址
#define MB_SLAVE_BAUD_RATE             			( 0x01 )	//	波特率
#define MB_SUPPORT_CONTROL_METHODS          ( 0x02 )	//	0:可控;1：屏蔽  // 1bit：蓝牙; 2bit：Modbus-RS485; 3bit：wifi;
#define MB_WIFI_DP_UPLOAD_LEVEL          		( 0x03 )	//	WIFI  dp点上传等级
#define MB_MOTOR_POLE_NUMBER  							( 0x04 )	//	电机磁极数
#define MB_MOTOR_SPEED_MODE  								( 0x05 )	//	转速 方式
#define MB_MOTOR_BREATH_LIGHT_MAX  					( 0x06 )	//	光圈亮度
#define MB_MOTOR_DRIVE_MODE        					( 0x07 )	//	驱动板模式
#define MB_MOTOR_MODEL_CODE        					( 0x08 )	//	电机型号
// ----------------------------------------------------------------------------------------------
//====================== 冲浪模式 全局 参数 ======================================================
#define MB_SURF_MODE_INFO_ACCELERATION  			( 0x10 )	//	冲浪模式 -- 加速度
#define MB_SURF_MODE_INFO_PREPARE_TIME  			( 0x11 )	//	冲浪模式 -- 准备时间
#define MB_SURF_MODE_INFO_LOW_SPEED  					( 0x12 )	//	冲浪模式 -- 低速档 -- 速度
#define MB_SURF_MODE_INFO_LOW_TIME						( 0x13 )	//	冲浪模式 -- 低速档 -- 时间
#define MB_SURF_MODE_INFO_HIGH_SPEED  				( 0x14 )	//	冲浪模式 -- 高速档 -- 速度
#define MB_SURF_MODE_INFO_HIGH_TIME  					( 0x15 )	//	冲浪模式 -- 高速档 -- 时间
// ----------------------------------------------------------------------------------------------
#define MB_PREPARATION_TIME_BIT     				( 0x20 )	//	预备时间（标志位）  准备时间 Bit: 定时模式 P1-P6
// ----------------------------------------------------------------------------------------------
#define MB_SYSTEM_WORKING_MODE     					( 0x21 )	//	系统工作模式  高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
#define MB_SYSTEM_WORKING_STATUS      			( 0x22 )	//	系统工作状态  0:暂停,   1:暂停恢复,   2:重新开始,  3:结束
// ----------------------------------------------------------------------------------------------
#define MB_MOTOR_CURRENT_SPEED		        	( 0x23 )	//	当前转速
#define MB_MOTOR_CURRENT_TIME        				( 0x24 )	//	当前时间
// ----------------------------------------------------------------------------------------------
#define MB_MOTOR_LEATER_SPEED		        		( 0x30 )	//	当前转速 (临时有效)
#define MB_MOTOR_LEATER_TIME        				( 0x31 )	//	当前时间
// ----------------------------------------------------------------------------------------------
#define MB_ANALOG_KEY_VALUE        					( 0x40 )	//	虚拟按键 (一次有效)
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
#define MB_BT_CONNECT_DONE        					( 0x5F )	//	蓝牙配网完成
#define MB_SYSTEM_SELF_TEST_STATE        		( 0x60 )	//	
#define MB_COMM_TEST_BLUETOOTH        			( 0x61 )	//	BT 		写入测试 (测试使用)
#define MB_COMM_TEST_RS485        					( 0x62 )	//	RS485 写入测试 (测试使用)
#define MB_COMM_TEST_WIFI        						( 0x63 )	//	WIFI 		写入测试 (测试使用)
#define MB_COMM_TEST_KEY        						( 0x64 )	//	按键 		写入测试 (测试使用)
#define MB_COMM_TEST_DIAL_SWITCH        		( 0x65 )	//	拨码 		写入测试 (测试使用)
// ----------------------------------------------------------------------------------------------
#define MB_DEBUG_OTA_PAGE_SIZE		      		( 0x70 )	//	下载ota包大小
#define MB_DEBUG_OTA_FAIL_PROGRESS		      ( 0x72 )	//	记录ota失败原因   MB_DEBUG_OTA_FAIL_PROGRESS
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
#define MB_USER_TRAIN_MODE_SPEED_P2_1		        ( 0x164 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_1        		( 0x165 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P2_2		        ( 0x166 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_2        		( 0x167 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P2_50		      ( 0x1C6 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P2_50        		( 0x1C7 )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P3_1		        ( 0x1C8 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_1        		( 0x1C9 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P3_2		        ( 0x1CA )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_2        		( 0x1CB )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P3_50		      ( 0x22A )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P3_50        		( 0x22B )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P4_1		        ( 0x22C )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_1        		( 0x22D )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P4_2		        ( 0x22E )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_2        		( 0x22F )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P4_50		      ( 0x28E )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P4_50        		( 0x28F )	//								时间
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P5_1		        ( 0x290 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P5_1        		( 0x291 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P5_2		        ( 0x292 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P5_2        		( 0x293 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P5_50		      ( 0x2F2 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P5_50        		( 0x2F3 )	//			
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P6_1		        ( 0x2F4 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P6_1        		( 0x2F5 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P6_2		        ( 0x2F6 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P6_2        		( 0x2F7 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P6_50		      ( 0x356 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P6_50        		( 0x357 )	//			
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P7_1		        ( 0x358 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P7_1        		( 0x359 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P7_2		        ( 0x35A )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P7_2        		( 0x35B )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P7_50		      ( 0x3BA )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P7_50        		( 0x3BB )	//

// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P8_1		        ( 0x3BC )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P8_1        		( 0x3BD )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P8_2		        ( 0x3BE )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P8_2        		( 0x3BF )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P8_50		      ( 0x41E )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P8_50        		( 0x41F )	//		
// ----------------------------------------------------------------------------------------------
#define MB_USER_TRAIN_MODE_SPEED_P9_1		        ( 0x420 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P9_1        		( 0x421 )	//								时间
#define MB_USER_TRAIN_MODE_SPEED_P9_2		        ( 0x422 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P9_2        		( 0x423 )	//								时间
//		.......  50
#define MB_USER_TRAIN_MODE_SPEED_P9_50		      ( 0x482 )	//	用户 训练模式 	转速
#define MB_USER_TRAIN_MODE_TIME_P9_50        		( 0x483 )	//
// ----------------------------------------------------------------------------------------------
#define MB_HOLDING_BUFFER_SIZE_MAX        			( MB_USER_TRAIN_MODE_TIME_P7_50 )	//	2K 
// ----------------------------------------------------------------------------------------------


// 04 Input
#define MB_MACHINE_MODEL_CODE											( 0x00 )					//	机型码
#define MB_MODBUS_RS485_VERSION										( 0x01 )					//	485 协议版本

#define MB_DISPLAY_SOFTWARE_VERSION_HIGH					( 0x02 )					//	显示板 软件版本 高   如：V2.1.3 中的“2”字段
#define MB_DISPLAY_SOFTWARE_VERSION_LOW						( 0x03 )					//	显示板 软件版本 低   如：V2.1.3 中，低8位：“3”字段， 高8位：“1”字段
#define MB_DISPLAY_HARDWARE_VERSION_HIGH					( 0x04 )					//	显示板 硬件版本 高   如：V2.1.3 中的“2”字段
#define MB_DISPLAY_HARDWARE_VERSION_LOW						( 0x05 )					//	显示板 硬件版本 低   如：V2.1.3 中，低8位：“3”字段， 高8位：“1”字段

#define MB_DRIVER_SOFTWARE_VERSION_HIGH						( 0x06 )					//	驱动板 软件版本 高   如：V2.1.3 中的“2”字段
#define MB_DRIVER_SOFTWARE_VERSION_LOW						( 0x07 )					//	驱动板 软件版本 低   如：V2.1.3 中，低8位：“3”字段， 高8位：“1”字段
#define MB_DRIVER_HARDWARE_VERSION_HIGH						( 0x08 )					//	驱动板 硬件版本 高   如：V2.1.3 中的“2”字段
#define MB_DRIVER_HARDWARE_VERSION_LOW						( 0x09 )					//	驱动板 硬件版本 低   如：V2.1.3 中，低8位：“3”字段， 高8位：“1”字段

#define MB_SYSTEM_FAULT_STATUS					          ( 0x0A )					//	系统故障状态
#define MB_MOTOR_FAULT_STATUS					          	( 0x0B )					// 	电机 	故障状态
#define MB_MOS_TEMPERATURE					          		( 0x0C )					//	mos 	温度
#define MB_BOX_TEMPERATURE					          		( 0x0D )					//	电箱	温度
#define MB_MOTOR_BUS_VOLTAGE					          	( 0x0E )					//	母线 	电压
#define MB_MOTOR_BUS_CURRENT					          	( 0x0F )					//	母线 	电流

#define MB_MOTOR_CURRENT					          			( 0x10 )					// 	（2字节） 电机 	电流
#define MB_MOTOR_REALITY_SPEED					          ( 0x12 )					//	（2字节） 电机 	实际 转速
#define MB_SEND_REALITY_SPEED					          	( 0x14 )					//	（2字节） 电机 	实际 转速  下发
#define MB_MOTOR_REALITY_POWER					          ( 0x16 )					//	（2字节） 电机 	实际 功率

#define MB_FINISH_STATISTICS_TIME		      				( 0x31 )					//	完成统计 --> 时长
#define MB_FINISH_STATISTICS_SPEED		      			( 0x32 )					//	完成统计 --> 游泳强度
#define MB_FINISH_STATISTICS_DISTANCE		      		( 0x33 )					//	（2字节）完成统计 --> 游泳距离
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
#define MB_SYSTEM_LAST_KEY_VALUE        					( 0x35 )					//	最后一次按键
#define MB_MOSFET_TEMPERATURE_01					        ( 0x36 )					//	mos 	温度  01
#define MB_MOSFET_TEMPERATURE_02					        ( 0x37 )					//	mos 	温度  02
#define MB_MOSFET_TEMPERATURE_03					        ( 0x38 )					//	mos 	温度  03
// ----------------------------------------------------------------------------------------------
#define MB_LCD_MAPPING_MODE        					( 0x40 )							//	模式
#define MB_LCD_MAPPING_SPEED        				( 0x41 )							//	速度
#define MB_LCD_MAPPING_TIME_HIGH        		( 0x42 )							//	时间 高
#define MB_LCD_MAPPING_TIME_LOW        			( 0x43 )							//	时间 低
#define MB_LCD_MAPPING_SYMBOL        				( 0x44 )							//	符号
// ----------------------------------------------------------------------------------------------
#define MB_SYSTEM_RUNNING_TIME        			( 0x50 )							//	（2字节）运行时间
#define MB_NO_OPERATION_TIME        				( 0x52 )							//	（2字节）无人操作时间
#define MB_SYSTEM_SLEEP_TIME        				( 0x54 )							//	（2字节）休眠时间
#define MB_THREAD_ACTIVITY_SGIN		      		( 0x56 )							//	线程 活动 标志位
#define MB_WIFI_TIMING_VALUE		      			( 0x57 )							//	（2字节）校时
#define MB_WIFI_TIMING_VALUE_OLD		      	( 0x59 )							//	（2字节）比较时间
#define MB_CHECK_TIMING_ADD_MORE		      	( 0x5B )							//	加
#define MB_CHECK_TIMING_MINUS_MORE		      ( 0x5C )							//	减
#define MB_CHECK_TIMING_ERROR_CNT		      	( 0x5D )							//	错误计数器
// ----------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------
#define MB_SYSTEM_WORKING_MODE_MEMORY     				( 0x60 )				//	系统工作模式  高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
#define MB_SYSTEM_WORKING_STATUS_MEMORY      			( 0x61 )				//	系统工作状态  0:暂停,   1:暂停恢复,   2:重新开始,  3:结束
// ----------------------------------------------------------------------------------------------
#define MB_MOTOR_CURRENT_SPEED_MEMORY		        	( 0x62 )				//	当前转速 (临时有效)
#define MB_MOTOR_CURRENT_TIME_MEMORY        			( 0x63 )				//	当前时间
// ----------------------------------------------------------------------------------------------

#define MB_INPUT_BUFFER_END        								( 0x80 )							//	结束
// ----------------------------------------------------------------------------------------------
#define MB_INPUT_BUFFER_SIZE_MAX        					( MB_INPUT_BUFFER_END )	//	
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

//================= 冲浪模式 全局 参数 ================================
extern void Surf_Mode_Info_Get_Mapping(void);

extern void MB_Get_Mapping_Register(void);

extern void MB_HoldBuffer_Temp_Clean(void);

extern void MB_InputBuffer_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* __MODBUS_H__ */

