/**
******************************************************************************
* @file    		pump.c
* @brief   		Ë®±Ã¿ØÖÆ
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "pump.h"
#include "dac.h"
#include "dma.h"
#include "gpio.h"
#include "tim.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "modbus.h"

uint16_t* p_Gear_Pump_1;
uint16_t* p_Gear_Pump_2;

uint16_t* p_Current_Pump_1;
uint16_t* p_Current_Pump_2;

uint16_t* p_Voltage_Pump_1;
uint16_t* p_Voltage_Pump_2;

uint16_t Gear_Pump_1;
uint16_t Gear_Pump_2;

uint16_t Current_Pump_1;
uint16_t Current_Pump_2;

uint16_t Voltage_Pump_1;
uint16_t Voltage_Pump_2;

#ifdef SYSTEM_HARDWARE_DEBUG

static uint16_t Pump_Hardware_Debug_Cnt=0;
#endif

void Call_PUMP_Handle_Init(void)
{
	p_Gear_Pump_1 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_GEAR_PUMP_1);
	p_Gear_Pump_2 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_GEAR_PUMP_2);
	
	p_Current_Pump_1 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_CURRENT_PUMP_1);
	p_Current_Pump_2 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_CURRENT_PUMP_2);
	
	p_Voltage_Pump_1 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_VOLTAGE_PUMP_1);
	p_Voltage_Pump_2 = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_DATA_ADDR_VOLTAGE_PUMP_2);
	
}


void Call_Current_Pump(void)
{
	if(Current_Pump_1 !=  *p_Current_Pump_1)
	{
		if(*p_Current_Pump_1 > AGREEMENT_CURRENT_PUMP_MAX)
		{
			*p_Current_Pump_1 = AGREEMENT_CURRENT_PUMP_MAX;
		}
		Current_Pump_1 = *p_Current_Pump_1;
		StartUp_PWM1(Current_Pump_1);
	}
	if(Current_Pump_2 !=  *p_Current_Pump_2)
	{
		if(*p_Current_Pump_2 > AGREEMENT_CURRENT_PUMP_MAX)
		{
			*p_Current_Pump_2 = AGREEMENT_CURRENT_PUMP_MAX;
		}
		Current_Pump_2 = *p_Current_Pump_2;
		StartUp_PWM2(Current_Pump_2);
	}
}


void Call_Voltage_Pump(void)
{
	if(Voltage_Pump_1 !=  *p_Voltage_Pump_1)
	{
		if(*p_Voltage_Pump_1 > AGREEMENT_VOLTAGE_PUMP_MAX)
		{
			*p_Voltage_Pump_1 = AGREEMENT_VOLTAGE_PUMP_MAX;
		}
		Voltage_Pump_1 = *p_Voltage_Pump_1;
		StartUp_DAC1(Voltage_Pump_1);
	}
	if(Voltage_Pump_2 !=  *p_Voltage_Pump_2)
	{
		if(*p_Voltage_Pump_2 > AGREEMENT_VOLTAGE_PUMP_MAX)
		{
			*p_Voltage_Pump_2 = AGREEMENT_VOLTAGE_PUMP_MAX;
		}
		Voltage_Pump_2 = *p_Voltage_Pump_2;
		StartUp_DAC2(Voltage_Pump_2);
	}
}

void Call_Gear_Pump(void)
{
	if(Gear_Pump_1 !=  *p_Gear_Pump_1)
	{
		if(*p_Gear_Pump_1 > AGREEMENT_GEAR_PUMP_MAX)
		{
			*p_Gear_Pump_1 = AGREEMENT_GEAR_PUMP_MAX;
		}
		Gear_Pump_1 = *p_Gear_Pump_1;
	}
	StartUp_Pump(0, Gear_Pump_1);
	
	if(Gear_Pump_2 !=  *p_Gear_Pump_2)
	{
		if(*p_Gear_Pump_2 > AGREEMENT_GEAR_PUMP_MAX)
		{
			*p_Gear_Pump_2 = AGREEMENT_GEAR_PUMP_MAX;
		}
		Gear_Pump_2 = *p_Gear_Pump_2;
	}
	StartUp_Pump(1, Gear_Pump_2);
}

void Call_PUMP_Handle_Task(void)
{
	Call_Current_Pump();
	
	Call_Voltage_Pump();
	
	Call_Gear_Pump();
}



void PUMP_Hardware_Debug(void)
{
#ifdef SYSTEM_HARDWARE_DEBUG
//	if( Pump_Hardware_Debug_Cnt == 500)
//	{
//		StartUp_PWM1(AGREEMENT_CURRENT_PUMP_MAX/2);
//		StartUp_PWM2(AGREEMENT_CURRENT_PUMP_MAX/2);
//		
//		StartUp_DAC1(AGREEMENT_VOLTAGE_PUMP_MAX/2);
//		StartUp_DAC2(AGREEMENT_VOLTAGE_PUMP_MAX/2);
//		
//		StartUp_Pump(0, 0xFF);
//		StartUp_Pump(1, 0xFF);
//	}
//	else if( Pump_Hardware_Debug_Cnt == 1000)
//	{
		StartUp_PWM1(AGREEMENT_CURRENT_PUMP_MAX);
		StartUp_PWM2(AGREEMENT_CURRENT_PUMP_MAX);
		
		StartUp_DAC1(AGREEMENT_VOLTAGE_PUMP_MAX);
		StartUp_DAC2(AGREEMENT_VOLTAGE_PUMP_MAX);
		
		StartUp_Pump(0,4);
		StartUp_Pump(1,4);
//	}
//	else if( Pump_Hardware_Debug_Cnt == 0)
//	{
////		StartUp_PWM1(0);
////		StartUp_PWM2(0);
//		Stop_PWM1();
//		Stop_PWM2();
//		
//		StartUp_DAC1(0);
//		StartUp_DAC2(0);
//		
//		StartUp_Pump(0,0);
//		StartUp_Pump(1,0);
//	}
	
	if(Pump_Hardware_Debug_Cnt++ > 1501)
		Pump_Hardware_Debug_Cnt = 0;
		
#endif
}


