/**
******************************************************************************
* @file    		wifi.h
* @brief   		wifi ģ��
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
	WIFI_NO_CONNECT = 0,				//	������
	WIFI_DISTRIBUTION,					//	������
	WIFI_WORKING,								//	��������
	WIFI_ERROR,									//	�������	
} WIFI_STATE_MODE_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define WIFI_DISTRIBUTION_BLINK_TIME				2				// 2.5 S

#define WIFI_ERROR_BLINK_TIME								8				// 1 S


#define WIFI_DISTRIBUTION_TIME_OUT				180				// 3 min


/* Exported functions prototypes ---------------------------------------------*/

//------------------- ����wifi״̬�� ----------------------------
extern void WIFI_Set_Machine_State(WIFI_STATE_MODE_E para);
//------------------- ��ȡwifi״̬�� ----------------------------
extern WIFI_STATE_MODE_E WIFI_Get_Machine_State(void);


//------------------- ���մ����� ----------------------------
extern void WIFI_Read_Handler(void);

//------------------- �������� ----------------------------
extern void WIFI_Get_In_Distribution(void);
//------------------- ������� ----------------------------
extern void WIFI_Get_In_Error(void);

//------------------- �ϴ�״̬���� ----------------------------
extern void WIFI_Update_State_Upload(void);
/* Private defines -----------------------------------------------------------*/
// ��ʼ��
extern void wifi_Module_Init(void);
// ������ѭ������
//  20 ms
extern void Wifi_Module_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H__ */

