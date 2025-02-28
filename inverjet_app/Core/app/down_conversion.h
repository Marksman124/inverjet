/**
******************************************************************************
* @file    		down_conversion.h
* @brief   		��Ƶ����
*
*
* @author			WQG
* @versions   v1.0
* @date   		2025-2-21
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DOWN_CONVESION_H__
#define __DOWN_CONVESION_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

//#include "display.h"			// ��ʾģ��
#include "macro_definition.h"				// ͳһ�궨��

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

// ��Ƶ ����
typedef enum
{
	Down_Conversion_Off = 0,					//	��
	Down_Conversion_Mos_Temper,				//	mos ����
	Down_Conversion_Box_Temper,				//	���� ����
	Down_Conversion_Out_Current,			//	��� ����
	Down_Conversion_Out_Power,				//	��� ����
} Down_Conversion_Type_enum;


// ��Ƶ Ƶ��
typedef enum
{
	Down_Conversion_Rate_Off = 0,						//	��
	Down_Conversion_Rate_Low,								//	����						1%/2min
	Down_Conversion_Rate_Normal,						//	�����ٶ�					1%/min
	Down_Conversion_Rate_High,							//	����						1%/30s
	Down_Conversion_Rate_Realtime,					//	�쳣����					1%/10s
} Down_Conversion_Rate_enum;

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/

//-------------  ��ƵƵ�� ���� -------------
// MOS �¶� ��1��C  steady
//			#define MOS_TEMPER_RATE_STABLE					(15 * 60)				//	15min 	�ȶ�
#define MOS_TEMPER_RATE_LOW							(10 * 60)				//	10min 	����
#define MOS_TEMPER_RATE_NORMAL					( 5 * 60)				//	5min		�����ٶ�
#define MOS_TEMPER_RATE_HIGH						( 2 * 60)				//	2min		����
#define MOS_TEMPER_RATE_REALTIME				( 1 * 60)				//	1min		�쳣����

// BOX �¶� ��1��C
//			#define BOX_TEMPER_RATE_STABLE					(15 * 60)				//	15min 	�ȶ�
#define BOX_TEMPER_RATE_LOW							(10 * 60)				//	10min 	����
#define BOX_TEMPER_RATE_NORMAL					( 5 * 60)				//	5min		�����ٶ�
#define BOX_TEMPER_RATE_HIGH						( 2 * 60)				//	2min		����
#define BOX_TEMPER_RATE_REALTIME				( 1 * 60)				//	1min		�쳣����

// ��� ���� ��2A
//			#define OUT_CURRENT_RATE_STABLE					(15 * 60)				//	15min 	�ȶ�
#define OUT_CURRENT_RATE_LOW						(2 * 60)				//	2min 	����
#define OUT_CURRENT_RATE_NORMAL					( 60 )					//	1min		�����ٶ�
#define OUT_CURRENT_RATE_HIGH						( 30 )					//	30S		����
#define OUT_CURRENT_RATE_REALTIME				( 10 )					//	10s		�쳣����


// ��Ƶ Ƶ��  N ��ÿ 1%
#define DOWN_CONVERSION_RATE_LOW						(120)
#define DOWN_CONVERSION_RATE_NORMAL					( 60)
#define DOWN_CONVERSION_RATE_HIGH						( 30)
#define DOWN_CONVERSION_RATE_REALTIME				( 10)


// ���� �ٶ� ��/1%
#define REBOUND_FREQUENCHY_TIME_LOW						(120)		//	2min 	����
#define REBOUND_FREQUENCHY_TIME_HIGH					(30)		//	30s 	����


//��ʽ
#define GET_MAX_FROM_3(x, y, z) ((x) > (y) ? ((x) > (z) ? (x) : (z)) : ((y) > (z) ? (y) : (z)))

/* Exported functions prototypes ---------------------------------------------*/
// ��� ���� ��Ƶ ״̬
extern void Clean_All_Down_Conversion_Status(void);


//-------------------- ���½���  mos ----------------------------
extern void Check_Down_Conversion_MOS_Temperature(short int Temperature);
//-------------------- ���½�Ƶ  ���� ----------------------------
extern void Check_Down_Conversion_BOX_Temperature(short int Temperature);
//-------------------- ���ʽ�Ƶ   ----------------------------
extern void Check_Down_Conversion_Motor_Power(uint16_t power);
//-------------------- ������Ƶ   ----------------------------
extern void Check_Down_Conversion_Motor_Current(uint32_t Current);

//-------------------- ��ȡ��Ƶ���� ----------------------------
uint8_t Get_Down_Conversion_Time(void);
//-------------------- ��ȡ��Ƶǰ�ٶ� ----------------------------
uint8_t Get_Down_Conversion_Speed_Old(void);
//-------------------- ���ý�Ƶǰ�ٶ� ----------------------------
void Set_Down_Conversion_Speed_Old(uint8_t vaule);
//-------------------- ��ȡ��Ƶ�����ٶ� ----------------------------
uint8_t Get_Down_Conversion_Speed_Now(void);

//-------------------- ��ȡ���½���ģʽ ----------------------------
extern uint8_t Get_Temp_Slow_Down_State(void);


// ��Ƶ�������߳� 	ÿ���һ��
extern void Down_Conversion_Handler(void);

/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __DOWN_CONVESION_H__ */

