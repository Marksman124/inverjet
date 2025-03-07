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

#define WIFI_DISTRIBUTION_BLINK_TIME				3				// 1500ms  (����)

#define WIFI_ERROR_BLINK_TIME								10				// 250ms		(25ms)


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

//-------------- �ϴ� ���ͳ��  -------------------
extern void WIFI_Finish_Statistics_Upload( void );
	


/* Private defines -----------------------------------------------------------*/
// ��ʼ��
extern void wifi_Module_Init(void);
// ������ѭ������
//  20 ms
extern void Wifi_Module_Handler(void);


// �ϴ�dp�� (ͳһ�ӿ�)
extern  void Wifi_DP_Data_Update(uint16_t id);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H__ */

