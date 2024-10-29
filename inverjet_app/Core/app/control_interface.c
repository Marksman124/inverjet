/**
******************************************************************************
* @file				control_interface.c
* @brief			control_interface 控制接口, wifi,蓝牙直接控制
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

//------------------- 配网控制 遥控 & wifi ----------------------------
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
	
//------------------- 设置系统工作 模式 ----------------------------
//		高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
void Ctrl_Set_System_Mode(uint16_t para)
{
	uint8_t mode;
	uint8_t plan;
	
	plan = para>>8;
	mode = (para & 0xFF);
	
	if(mode == 0)
	{
		// 3s后自动运行
		To_Free_Mode(FREE_MODE_AUTO_START);	//	自由模式
	}
	else if(mode == 1)
	{
		To_Timing_Mode();	//	定时模式
	}
	else if(mode == 2)
	{
		if(plan < TRAINING_MODE_NUMBER_MAX)
		{
			To_Train_Mode(plan);	//	训练模式
		}
	}
}


//------------------- 设置系统工作 状态 ----------------------------
//		0:暂停,   1:暂停恢复,   2:重新开始,  3:结束
void Ctrl_Set_System_Status(uint16_t para)
{
	if(para == 0)
	{
		if(System_is_Pause() == 0)	// 暂停 --> 启动
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
		if(System_is_Pause())	// 暂停 --> 启动
		{
			Arbitrarily_To_Running();
			*p_OP_ShowNow_Speed = p_OP_ShowLater->speed;
			*p_OP_ShowNow_Time = 20;
		}
	}
	else if(para == 2)
	{
		// 自由模式
		if(System_Mode_Free())
		{
			To_Free_Mode(FREE_MODE_AUTO_START);	//	自由 模式
		}
		else if(System_Mode_Time())
		{
			To_Timing_Mode();	//	定时 模式
		}
		else if(System_Mode_Train())
		{
			To_Train_Mode(Get_System_State_Mode());	//	训练 模式
		}
	}
	else if(para == 3)
	{
		// 返回 自由模式 初始状态
		To_Free_Mode(FREE_MODE_NOT_AUTO_START);
		Lcd_Show();
	}
}


//------------------- 设置系统 电源  ----------------------------
//		0：关机  1：开机
void Ctrl_Set_System_PowerOn(uint16_t para)
{
	if(para == 0)
	{
		System_Power_Off();
	}
	else
	{
		if(System_is_Power_Off())//关机中 执行开机
		{
				System_Power_On();
		}
	}
}


//------------------- 设置 当前转速 (临时有效)----------------------------
void Ctrl_Set_Motor_Current_Speed(uint16_t para)
{
	Data_Set_Current_Speed((uint8_t)para&0xFF);
}


//------------------- 设置 当前时间 (临时有效)----------------------------
void Ctrl_Set_Motor_Current_Time(uint16_t para)
{
	Data_Set_Current_Time(para);
}


