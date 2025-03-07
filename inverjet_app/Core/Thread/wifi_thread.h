/**
******************************************************************************
* @file    		wifi.h
* @brief   		wifi 模组
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WIFI_H__
#define __WIFI_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

	
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum 
{
	WIFI_NO_CONNECT = 0,				//	无连接
	WIFI_DISTRIBUTION,					//	配网中
	WIFI_WORKING,								//	正常运行
	WIFI_ERROR,									//	网络故障	
} WIFI_STATE_MODE_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define WIFI_DISTRIBUTION_BLINK_TIME				3				// 1500ms  (半秒)

#define WIFI_ERROR_BLINK_TIME								10				// 250ms		(25ms)


#define WIFI_DISTRIBUTION_TIME_OUT				180				// 3 min


/* Exported functions prototypes ---------------------------------------------*/

//------------------- 设置wifi状态机 ----------------------------
extern void WIFI_Set_Machine_State(WIFI_STATE_MODE_E para);
//------------------- 获取wifi状态机 ----------------------------
extern WIFI_STATE_MODE_E WIFI_Get_Machine_State(void);


//------------------- 接收处理函数 ----------------------------
extern void WIFI_Read_Handler(void);

//------------------- 进入配网 ----------------------------
extern void WIFI_Get_In_Distribution(void);
//------------------- 进入故障 ----------------------------
extern void WIFI_Get_In_Error(void);

//------------------- 上传状态更新 ----------------------------
extern void WIFI_Update_State_Upload(void);

//-------------- 上传 完成统计  -------------------
extern void WIFI_Finish_Statistics_Upload( void );
	


/* Private defines -----------------------------------------------------------*/
// 初始化
extern void wifi_Module_Init(void);
// 按键主循环任务
//  20 ms
extern void Wifi_Module_Handler(void);


// 上传dp点 (统一接口)
extern  void Wifi_DP_Data_Update(uint16_t id);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H__ */

