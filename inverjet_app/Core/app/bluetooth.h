/**
******************************************************************************
* @file    		bluetooth.h
* @brief   		bluetooth ģ��
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
	BT_NO_CONNECT = 0,				//	������
	BT_DISTRIBUTION,					//	������
	BT_WORKING,								//	��������
	BT_ERROR,									//	�������	
} BT_STATE_MODE_E;


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

#define BT_DISTRIBUTION_BLINK_TIME				2				// 2 S

#define BT_ERROR_BLINK_TIME								1				// 1 S


#define BT_DISTRIBUTION_TIME_OUT					180			// 3 min


/* Exported functions prototypes ---------------------------------------------*/
//���ڷ��ͽӿ�
extern void SerialWrite(unsigned char *buff,int length);
//���ڷ��ͽӿ�
extern void BT_Read_Data_Bit(unsigned char vaule);

extern void BT_MsTimeout(void);

//�����жϵ���
extern void Usart_IRQ_CallBack(uint8_t data);
//------------------- ����wifi״̬�� ----------------------------
extern void BT_Set_Machine_State(BT_STATE_MODE_E para);
//------------------- ��ȡwifi״̬�� ----------------------------
extern BT_STATE_MODE_E BT_Get_Machine_State(void);

//------------------- ���� Modbus ���ó�ʼ�� ----------------------------
extern void BT_Modbus_Config_Init(void);
// AT ָ�� �� MTU
extern void BT_Set_MTU(uint8_t data);
// AT ָ�� ����͸��
extern void BT_Set_TRANSENTER(uint8_t data);
//
extern void BT_Module_AT_Init(void);
//���³�ʼ��
extern void BT_Module_AT_ReInit(void);

//------------------- ���մ����� ----------------------------
extern void BT_Read_Handler(void);

//------------------- �������� ----------------------------
extern void BT_Get_In_Distribution(void);
//------------------- ������� ----------------------------
extern void BT_Get_In_Error(void);
//------------------- �������� 0.5���һ��----------------------------
extern void BT_Distribution_Halder(void);

/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __BLUETOOTH_H__ */

