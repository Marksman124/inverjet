/**
******************************************************************************
* @file    		control_interface.h
* @brief   		control_interface ���ƽӿ�, wifi,����ֱ�ӿ���
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONTROL_INTERFACE_H__
#define __CONTROL_INTERFACE_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "state_machine.h"

#include "wifi_thread.h"							// wifi ģ��
#include "bluetooth.h"				// bluetooth ģ��
#include "display.h"					// ��ʾ ģ��

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/


//------------------- �������� ң�� & wifi ----------------------------
extern void Ctrl_GetIn_Distribution(uint16_t para);
//------------------- ����ϵͳ���� ģʽ ----------------------------
//		��λ::0��P1\2\3  ��λ:0������:1����ʱ:2��ѵ��
extern void Ctrl_Set_System_Mode(uint16_t para);
//------------------- ����ϵͳ���� ״̬ ----------------------------
//		0:��ͣ,   1:��ͣ�ָ�,   2:���¿�ʼ,  3:����
extern void Ctrl_Set_System_Status(uint16_t para);
//------------------- ����ϵͳ ��Դ  ----------------------------
//		0���ػ�  1������
extern void Ctrl_Set_System_PowerOn(uint16_t para);
//------------------- ���� ��ǰת�� (��ʱ��Ч)----------------------------
extern void Ctrl_Set_Motor_Current_Speed(uint16_t para);
//------------------- ���� ��ǰʱ�� (��ʱ��Ч)----------------------------
extern void Ctrl_Set_Motor_Current_Time(uint16_t para);
/* Private defines -----------------------------------------------------------*/




#ifdef __cplusplus
}
#endif

#endif /* __CONTROL_INTERFACE_H__ */

