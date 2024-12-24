/**
******************************************************************************
* @file    		bluetooth.h
* @brief   		bluetooth 模组
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

	
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	BT_NO_CONNECT = 0,				//	无连接
	BT_DISTRIBUTION,					//	配网中
	BT_WORKING,								//	正常运行
	BT_ERROR,									//	网络故障	
} BT_STATE_MODE_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define BT_DISTRIBUTION_BLINK_TIME				2				// 2 S

#define BT_ERROR_BLINK_TIME								1				// 1 S


#define BT_DISTRIBUTION_TIME_OUT					180			// 3 min


/* Exported functions prototypes ---------------------------------------------*/
//串口发送接口
extern void SerialWrite(unsigned char *buff,int length);

extern void BT_UART_Send(uint8_t* p_buff, uint8_t len);
//串口发送接口
extern void BT_Read_Data_Bit(unsigned char vaule);

extern void BT_MsTimeout(void);

//接收中断调用
extern void Usart_IRQ_CallBack(uint8_t data);
//------------------- 设置wifi状态机 ----------------------------
extern void BT_Set_Machine_State(BT_STATE_MODE_E para);
//------------------- 获取wifi状态机 ----------------------------
extern BT_STATE_MODE_E BT_Get_Machine_State(void);

//------------------- 蓝牙 Modbus 配置初始化 ----------------------------
extern void BT_Modbus_Config_Init(void);
// AT 指令 设 MTU
extern void BT_Set_MTU(uint8_t data);
// AT 指令 设 功率
extern void BT_Set_Power(uint8_t data);
// AT 指令 进入透传
extern void BT_Set_TRANSENTER(uint8_t data);
//
extern void BT_Module_AT_Init(void);
//重新初始化
extern void BT_Module_AT_ReInit(void);

// AT 指令 连接 主机
extern void BT_Connect_OnlineServer(void);

//------------------- 蓝牙 进入工装 ----------------------------
extern void BT_Module_AT_InTest(void);
//------------------- 蓝牙 持续测试 ----------------------------
extern void BT_Module_AT_DoTest(void);

//------------------- 接收处理函数 ----------------------------
extern void BT_Read_Handler(void);

//------------------- 进入配网 ----------------------------
extern void BT_Get_In_Distribution(void);
//------------------- 进入故障 ----------------------------
extern void BT_Get_In_Error(void);
//------------------- 配网处理 0.5秒进一次----------------------------
extern void BT_Distribution_Halder(void);
//------------------- 联网处理 0.5秒进一次----------------------------
extern void BT_Online_Connect_Halder(void);

/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_H__ */

