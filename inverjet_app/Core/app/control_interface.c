/**
******************************************************************************
* @file				control_interface.c
* @brief			control_interface ���ƽӿ�, wifi,����ֱ�ӿ���
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "control_interface.h"
#include "key.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

//------------------- �������� ң�� & wifi ----------------------------
void Ctrl_GetIn_Distribution(uint16_t para)
{
	if(para == 1)
	{
		WIFI_Get_In_Distribution();
	}
	else if(para == 2)
	{
		BT_Get_In_Distribution();
	}
}
	
//------------------- ����ϵͳ���� ģʽ ----------------------------
//		��λ::0��P1\2\3  ��λ:0������:1����ʱ:2��ѵ��
void Ctrl_Set_System_Mode(uint16_t para)
{
	uint8_t mode;
	uint8_t plan;
	
	plan = para>>8;
	mode = (para & 0xFF);
	
	if(mode == 0)
	{
		// 3s���Զ�����
		To_Free_Mode(0);	//	����ģʽ
		Fun_Change_Mode();
	}
	else if(mode == 1)
	{
		To_Timing_Mode();	//	��ʱģʽ
		Fun_Change_Mode();
	}
	else if(mode == 2)
	{
		if(plan < TRAINING_MODE_NUMBER_MAX)
		{
			To_Train_Mode(plan);	//	ѵ��ģʽ
			Fun_Change_Mode();
		}
	}
}


//------------------- ����ϵͳ���� ״̬ ----------------------------
//		0:��ͣ,   1:��ͣ�ָ�,   2:���¿�ʼ,  3:����
void Ctrl_Set_System_Status(uint16_t para)
{
	if(para == 0)
	{
		if(System_is_Pause() == 0)	// ��ͣ --> ����
		{
			Arbitrarily_To_Pause();
			
			p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
			p_OP_ShowLater->time = *p_OP_ShowNow_Time;
			
			*p_OP_ShowNow_Speed = 0;
			*p_OP_ShowNow_Time = 20;
		}
	}
	else if(para == 1)
	{
		if(System_is_Pause())	// ��ͣ --> ����
		{
			Arbitrarily_To_Starting();
			*p_OP_ShowNow_Speed = p_OP_ShowLater->speed;
			*p_OP_ShowNow_Time = 20;
		}
	}
	else if(para == 2)
	{
		// ����ģʽ
		if(System_Mode_Free())
		{
			To_Free_Mode(0);	//	���� ģʽ
		}
		else if(System_Mode_Time())
		{
			To_Timing_Mode();	//	��ʱ ģʽ
		}
		else if(System_Mode_Train())
		{
			To_Train_Mode(*p_PMode_Now);	//	ѵ�� ģʽ
		}
	}
	else if(para == 3)
	{
		// ���� ����ģʽ ��ʼ״̬
		To_Free_Mode(1);
		Lcd_Show();
	}
}


//------------------- ����ϵͳ ��Դ  ----------------------------
//		0���ػ�  1������
void Ctrl_Set_System_PowerOn(uint16_t para)
{
	if(para == 0)
	{
		System_Power_Off();
	}
	else
	{
		if(System_is_Power_Off())//�ػ��� ִ�п���
		{
				System_Power_On();
		}
	}
}


//------------------- ���� ��ǰת�� (��ʱ��Ч)----------------------------
void Ctrl_Set_Motor_Current_Speed(uint16_t para)
{
	Data_Set_Current_Speed((uint8_t)para&0xFF);
}


//------------------- ���� ��ǰʱ�� (��ʱ��Ч)----------------------------
void Ctrl_Set_Motor_Current_Time(uint16_t para)
{
	Data_Set_Current_Time(para);
}


