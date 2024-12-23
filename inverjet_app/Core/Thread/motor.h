/**
******************************************************************************
* @file    		motor.h
* @brief   		电机 相关协议  控制转速命令 200ms
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_H__
#define __MOTOR_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "usart.h"
//#include "display.h"			// 显示模块
//#include "port.h"
//#include "mbcrc.h"				// crc
#include "macro_definition.h"				// 统一宏定义
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#ifndef __MACRO_DEFINITION_H__

#define MOTOR_THREAD_LIFECYCLE								50				// 任务生命周期 50ms

// 心跳 周期 500s 
#define MOTOR_HEARTBEAT_CYCLE							(500/(MOTOR_THREAD_LIFECYCLE))				// 500 毫秒
// 命令 周期 50 ms
#define MOTOR_COMMAND_CYCLE								(200/(MOTOR_THREAD_LIFECYCLE))				// 5秒: 50 毫秒     20秒 : 200ms
// 读状态 周期 1s 
#define MOTOR_READ_STATIC_CYCLE						(1000/(MOTOR_THREAD_LIFECYCLE))				// 1 秒

//电机极数
#define	MOTOR_POLE_NUMBER				(5)

//最大转速 100%
#define	MOTOR_RPM_SPEED_MAX				(1950*MOTOR_POLE_NUMBER)		//9750
//最低转速 100%
#define	MOTOR_RPM_SPEED_MIX				(700*MOTOR_POLE_NUMBER)		//3500

// 700  1012   1324  1636   1948
//每 1% 转速 
#define	MOTOR_RPM_CONVERSION_COEFFICIENT				((MOTOR_RPM_SPEED_MAX-MOTOR_RPM_SPEED_MIX)/80)			//15.6

//每 20% 转速 
#define	MOTOR_RPM_CONVERSION_COEFFICIENT_20				((MOTOR_RPM_SPEED_MAX-MOTOR_RPM_SPEED_MIX)/4)			//312.5     


//-------------- MOS 温度报警值 90°C -------------------
#define MOS_TEMP_ALARM_VALUE								90
//-------------- MOS 温度 降速 80°C -------------------
#define MOS_TEMP_REDUCE_SPEED								80		// 降档 温度
#define MOS_TEMP_RESTORE_SPEED							75		// 恢复 温度
 
//通讯故障 报警时间
#define FAULT_MOTOR_LOSS_TIME						(30000/(MOTOR_THREAD_LIFECYCLE))				// 30 秒


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
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
// 缓冲区大小
#define MOTOR_RS485_TX_BUFF_SIZE			16
#define MOTOR_RS485_RX_BUFF_SIZE			256


//驱动板故障 标志位
#define MOTOR_FAULT_SIGN_BIT						(E102_TEMPERATURE_AMBIENT | E205_WIFI_HARDWARE | E206_BT_HARDWARE | E207_RS485_HARDWARE)
#define CLEAN_MOTOR_FAULT(n)						(n &= MOTOR_FAULT_SIGN_BIT)

#define	MOTOR_PROTOCOL_HEADER_OFFSET						3
#define	MOTOR_PROTOCOL_ADDR_MAX									73

#define	MOTOR_ADDR_MOSFET_TEMP_OFFSET						0
#define	MOTOR_ADDR_MOTOR_TEMP_OFFSET						2
#define	MOTOR_ADDR_MOTOR_CURRENT_OFFSET					4
#define	MOTOR_ADDR_MOTOR_SPEED_OFFSET						22
#define	MOTOR_ADDR_BUS_VOLTAGE_OFFSET						26

#define	MOTOR_ADDR_MOTOR_FAULT_OFFSET						52
#define	MOTOR_ADDR_NTC1_TEMP_OFFSET							58
#define	MOTOR_ADDR_NTC2_TEMP_OFFSET							60
#define	MOTOR_ADDR_NTC3_TEMP_OFFSET							62


//======= 电机故障代码 ================================================================================
#define	MOTOR_FAULT_NONE																									0						//
#define	MOTOR_FAULT_OVER_VOLTAGE																					1						//	输入电压小于 最小允许值的5%
#define	MOTOR_FAULT_UNDER_VOLTAGE																					2						//	输入电压大于 最大允许值的5%
#define	MOTOR_FAULT_DRV																										3						//	硬件短路保护
#define	MOTOR_FAULT_ABS_OVER_CURRENT																			4						//	SQRT(iq*iq+id*id)大于最大电流
#define	MOTOR_FAULT_OVER_TEMP_FET																					5						//	MOSFET温度大于最大允许温度-0.1时报过温故障
#define	MOTOR_FAULT_OVER_TEMP_MOTOR																				6						//	xxx
#define	MOTOR_FAULT_GATE_DRIVER_OVER_VLOTAGE															7						//	xxx
#define	MOTOR_FAULT_GATE_DRIVER_UNDER_VLOTAGE															8						//	xxx
#define	MOTOR_FAULT_MCU_UNDER_VLOTAGE																			9						//	 mcu电压不稳故障
#define	MOTOR_FAULT_BOOTING_FROM_WATCHDOG_RESET														10					//	机器发生看门狗复位故障
#define	MOTOR_FAULT_ENCODER_SPI																						11					//	xxx
#define	MOTOR_FAULT_ENCODER_SINCOS_BELOW_MIN_AMPLITUDE										12					//	xxx
#define	MOTOR_FAULT_ENCODER_SINCOS_ABOVE_MAN_AMPLITUDE										13					//	xxx
#define	MOTOR_FAULT_FLASH_CORRUPTION																			14					//	lash损坏（参数存储区）
#define	MOTOR_FAULT_HIGH_OFFSET_CURRENT_SENSOR_1													15					//	1.65+/-0.5V的偏差
#define	MOTOR_FAULT_HIGH_OFFSET_CURRENT_SENSOR_2													16					//	1.65+/-0.5V的偏差
#define	MOTOR_FAULT_HIGH_OFFSET_CURRENT_SENSOR_3													17					//	1.65+/-0.5V的偏差
#define	MOTOR_FAULT_UNBALANCED_CURRENTS																		18					//	三相电流不平衡故障
#define	MOTOR_FAULT_BRK																										19					//	xxx
#define	MOTOR_FAULT_RESOLVER_LOT																					20					//	xxx
#define	MOTOR_FAULT_RESOLVER_DOS																					21					//	xxx
#define	MOTOR_FAULT_RESOLVER_LOS																					22					//	xxx
#define	MOTOR_FAULT_FLASH_CURRUPTION_APP_CFG															23					//	应用参数flash校验出错-采用默认参数进行
#define	MOTOR_FAULT_FLASH_CURRUPTION_MC_CFG																24					//	电机参数flash校验出错-采用默认参数进行
#define	MOTOR_FAULT_ENCODER_NO_MAGNET																			25					//	xxx
#define	MOTOR_FAULT_ENCODER_MAGNET_TOO_STRONG															26					//	xxx


#define	MOTOR_FAULT_OUTPUT_PHASE_A_SENSOR																	27					//	A相电压传感器损坏
#define	MOTOR_FAULT_OUTPUT_PHASE_B_SENSOR																	28					//	B相电压传感器损坏
#define	MOTOR_FAULT_OUTPUT_PHASE_C_SENSOR																	29					//	C相电压传感器损坏
#define	MOTOR_FAULT_OUTPUT_PHASE_A_LOSS_POWER_ON													30					//	A相上电缺相
#define	MOTOR_FAULT_OUTPUT_PHASE_B_LOSS_POWER_ON													31					//	B相上电缺相
#define	MOTOR_FAULT_OUTPUT_PHASE_C_LOSS_POWER_ON													32					//	C相上电缺相
#define	MOTOR_FAULT_OUTPUT_PHASE_2_AND_3_LOSS_POWER_ON										33					//	上电缺2/3相
#define	MOTOR_FAULT_OUTPUT_PHASE_A_LOSS_RUNNING														34					//	运行中 缺A相
#define	MOTOR_FAULT_OUTPUT_PHASE_B_LOSS_RUNNING														35					//	运行中 缺A相
#define	MOTOR_FAULT_OUTPUT_PHASE_C_LOSS_RUNNING														36					//	运行中 缺A相
#define	MOTOR_FAULT_OUTPUT_PHASE_2_AND3_LOSS_RUNNING											37					//	运行中 缺2/3相
#define	MOTOR_FAULT_OUTPUT_LOCKROTOR																			38					//	堵转

#define	MOTOR_FAULT_MOSFET_NTC_ERR																				39					//	Mos温度传感器故障


#define	MOTOR_FAULT_CODE_START							(MOTOR_FAULT_OVER_VOLTAGE)
#define	MOTOR_FAULT_CODE_END								(MOTOR_FAULT_MOSFET_NTC_ERR)
//====================================================================================================

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
// 缓冲区大小
#define MOTOR_RS485_TX_BUFF_SIZE			16
#define MOTOR_RS485_RX_BUFF_SIZE			32


//驱动板故障 标志位
#define MOTOR_FAULT_SIGN_BIT						E102_TEMPERATURE_AMBIENT
#define CLEAN_MOTOR_FAULT(n)						(n &= ~MOTOR_FAULT_SIGN_BIT)

#define	MOTOR_PROTOCOL_HEADER_OFFSET						1
#define	MOTOR_PROTOCOL_ADDR_MAX									11

#define	MOTOR_ADDR_MOTOR_SPEED_OFFSET							0
#define	MOTOR_ADDR_MOTOR_VERSION_OFFSET						1
#define	MOTOR_ADDR_BUS_VOLTAGE_OFFSET							2
#define	MOTOR_ADDR_BUS_CURRENT_OFFSET							3
#define	MOTOR_ADDR_MOTOR_CURRENT_OFFSET						4
#define	MOTOR_ADDR_MOSFET_TEMP_NUM_OFFSET					5
#define	MOTOR_ADDR_MOSFET_TEMP_OFFSET							6

#define	MOTOR_ADDR_MOTOR_POWER_OFFSET							7
#define	MOTOR_ADDR_MOTOR_FAULT_OFFSET							9



#define MOTOR_FAULT_BUS_VOLTAGE_UNDER					0x1
#define MOTOR_FAULT_BUS_CURRENT_OVER					0x2
#define MOTOR_FAULT_SAMPLE_ERROR							0x4
#define MOTOR_FAULT_HARDWARE_OVERCURRENT			0x8
#define MOTOR_FAULT_OUT_STEP_FAULT						0x10
#define MOTOR_FAULT_STARTUP_FAILED						0x20
#define MOTOR_FAULT_TIME_OUT									0x40
#define MOTOR_FAULT_CRC_ERROR									0x80
#define MOTOR_FAULT_OUTPUT_OVERCURRENT				0x100
#define MOTOR_FAULT_LACK_PHASE								0x200

//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
#endif
/* Exported functions prototypes ---------------------------------------------*/
void Metering_Receive_Init(void);
// 重启
void Motor_Usart_Restar(void);
// 清除故障
void Clean_Motor_OffLine_Timer(void);
//------------------- 主循环函数  ----------------------------
void App_Motor_Handler(void);
	
//------------------- 电机转速更新 ----------------------------
extern uint8_t Motor_Speed_Update(void);
//------------------- 电机转速是否达到目标值 ----------------------------
extern uint8_t Motor_Speed_Is_Reach(void);
//------------------- 电机转速 目标值 设置 ----------------------------
extern void Motor_Speed_Target_Set(uint8_t speed);
//------------------- 电机 快速停止 ----------------------------
extern void Motor_Quick_Stop(void);
//------------------- 电机转速 目标值 设置 ----------------------------
extern uint8_t Motor_Speed_Target_Get(void);
//------------------- 百分比 转 转速 ----------------------------
extern uint32_t Motor_Speed_To_Rpm(uint8_t speed);
//------------------- 转速 转 百分比 ----------------------------
extern uint8_t Motor_Rpm_To_Speed(uint32_t speed_rpm);

//================================================== 内部调用接口
//-------------------- 驱动状态检验   电机转速 ----------------------------
extern void Drive_Status_Inspection_Motor_Speed(void);
//-------------------- 驱动状态检验   电机电流 ----------------------------
extern void Drive_Status_Inspection_Motor_Current(void);

//-------------------- 高温降速  mos ----------------------------
extern void Check_Down_Conversion_MOS_Temperature(short int Temperature);
//-------------------- 功率降频   ----------------------------
extern void Check_Down_Conversion_Motor_Power(uint16_t power);
//-------------------- 电流降频   ----------------------------
extern void Check_Down_Conversion_Motor_Current(uint32_t Current);
//-------------------- 降频 状态恢复   ----------------------------
extern void Down_Conversion_State_Clea(void);

//-------------------- 获取电机故障状态 ----------------------------
uint16_t Get_Motor_Fault_State(void);

//-------------------- 设置电机故障状态 ----------------------------
void Set_Motor_Fault_State(uint16_t fault_bit);
//-------------------- 清除电机故障状态 ----------------------------
void ReSet_Motor_Fault_State(uint16_t fault_bit);
//-------------------- 严重故障 ----------------------------
uint8_t Motor_Is_Serious_Fault(uint16_t fault_bit);
//-------------------- 一般故障 ----------------------------
uint8_t Motor_Is_Ordinary_Fault(uint16_t fault_bit);
//-------------------- 指定故障 ----------------------------
uint8_t Motor_Is_Specified_Fault(uint16_t fault_bit, uint16_t specified_bit);

//------------------- 发送 ----------------------------
extern void Motor_Speed_Send(uint32_t speed_rpm);
//------------------- 心跳 ----------------------------
extern void Motor_Heartbeat_Send(void);
//-------------------- 读寄存器 ----------------------------
extern void Motor_Read_Register(void);

/*-------------------- 收发处理 ----------------------------------------------*/
void Motor_State_Analysis(void);
//-------------------- 发送 ----------------------------
extern void Motor_UART_Send(uint8_t* p_buff, uint8_t len);
//-------------------- 接收 ----------------------------
extern void Motor_RxData(uint8_t len);

//-------------------- 检查电机电流 ----------------------------
extern uint8_t Check_Motor_Current(void);
//-------------------- 检查电机转速 ----------------------------
extern uint8_t Check_Motor_Speed(void);


/* Private defines -----------------------------------------------------------*/

extern uint8_t Motor_Speed_Now;			// 电机转速 

//**************** 收发缓冲区
extern uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE];//定义一个接收缓存区
extern uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE];//定义一个发送缓存区


extern DMA_HandleTypeDef hdma_usart3_rx;


#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */

