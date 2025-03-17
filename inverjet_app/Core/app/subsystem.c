/**
******************************************************************************
* @file    		subsystem.c
* @brief   		子系统查询
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "subsystem.h"
#include "usart.h"
#include "metering.h"
#include "dmx512.h"
#include "gpio.h"

uint32_t Subsystem_Task_Counter=0;

void Subsystem_Init(void)
{
	led_off();
	Dmx512_Receive_Init();
	
	Metering_Receive_Init();
}


void Subsystem_RxData(uint8_t len)
{
#ifdef SYSTEM_HARDWARE_DEBUG
		led_on();
#endif
	Metering_RxData(len);
}


void Subsystem_Handler_Task(void)
{
	//协议解析 DMX512
#ifdef SYSTEM_HARDWARE_DEBUG
	if( Subsystem_Task_Counter > (THREAD_PERIOD_METERING_MODULE_HUART/THREAD_PERIOD_DMX512_HUART))
	{
		led_off();
		Dmx512_Hardware_Debug();
	}
#else
	Dmx512_Protocol_Analysis();
#endif
	if(( Subsystem_Task_Counter % (THREAD_PERIOD_METERING_MODULE_HUART/THREAD_PERIOD_DMX512_HUART))==0)
	{
		//Subsystem_Task_Counter = 0;
		//协议解析 计量模块
		Metering_Protocol_Analysis();
		
		//Set_Dmx512_Data_Change(1);	//	自动发送
	}
	if( Subsystem_Task_Counter > (THREAD_PERIOD_DMX512_AUTO_SEND/THREAD_PERIOD_DMX512_HUART))
	{
		Subsystem_Task_Counter = 0;
		Set_Dmx512_Data_Change(1);	//	自动发送
	}
	
	Subsystem_Task_Counter ++;
}
