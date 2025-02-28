/**
******************************************************************************
* @file				down_conversion.c
* @brief			��Ƶ����
*
* @author			WQG
* @versions		v1.0
* @date				2025-2-21
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "down_conversion.h"
#include "motor.h"
#include "fault.h"
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

#define DOWN_CONVERSION_BUFFER_SIZE							(5)			

/* Private variables ---------------------------------------------------------*/

//-------------- ͣ�� ���� -------------------
static uint8_t Current_Fault_To_Stop_Timer = 0;
static uint16_t Temperature_Fault_To_Stop_Timer = 0;

//-------------- ��Ƶ ���� -------------------

static Down_Conversion_Type_enum 		Down_Conversion_Type = Down_Conversion_Off;						//	��Ƶ����
static Down_Conversion_Rate_enum 		Down_Conversion_Rate = Down_Conversion_Rate_Off;			//	��Ƶ����

static uint8_t Down_Conversion_Time=0;				//	��Ƶ ����

static uint8_t Temp_Slow_Down_Speed_Old=0;		//	��Ƶ �ٶ�  �ٷֱ�
static uint8_t Temp_Slow_Down_Speed_Now=0;		//	��Ƶ �ٶ�  �ٷֱ�

static uint32_t Down_Conversion_Handler_Timing_Cnt=0;		// ��ʱ��

//-------------- mos �¶� -------------------

//static uint8_t Temperature_Mos_Old=0;
static uint8_t Temperature_Mos_Now=0;

//static uint32_t Temperature_Mos_Timer_Old=0;
//static uint32_t Temperature_Mos_Timer_Now=0;

static uint8_t Mos_Temperature_Array[5]={0};
static uint8_t Mos_Temperature_Array_cnt = 0;

//-------------- ���� �¶� -------------------

//static uint8_t Temperature_Box_Old=0;
static uint8_t Temperature_Box_Now=0;

//static uint32_t Temperature_Box_Timer_Old=0;
//static uint32_t Temperature_Box_Timer_Now=0;

static uint8_t Box_Temperature_Array[5]={0};
static uint8_t Box_Temperature_Array_cnt = 0;

//-------------- ��� ���� -------------------

//static uint8_t Current_Motor_Old=0;
static uint8_t Current_Motor_Now=0;

//static uint32_t Current_Motor_Timer_Old=0;
//static uint32_t Current_Motor_Timer_Now=0;

static uint8_t Out_Current_Array[5]={0};
static uint8_t Out_Current_Array_cnt = 0;


/* Private user code ---------------------------------------------------------*/

// �������в���״̬
void UpDate_Global_Status_Old(void)
{
//	Temperature_Mos_Old = Temperature_Mos_Now;
//	Temperature_Box_Old = Temperature_Box_Now;
//	Current_Motor_Old = Current_Motor_Now;
//	
//	Temperature_Mos_Timer_Old = Down_Conversion_Handler_Timing_Cnt;
//	Temperature_Box_Timer_Old = Down_Conversion_Handler_Timing_Cnt;
//	Current_Motor_Timer_Old = Down_Conversion_Handler_Timing_Cnt;
}


// ��� ���� ��Ƶ ״̬
void Clean_All_Down_Conversion_Status(void)
{
	if(Down_Conversion_Time == 0)
		return;
	
	
	Current_Fault_To_Stop_Timer = 0;
	Temperature_Fault_To_Stop_Timer = 0;
	
	Down_Conversion_Type = Down_Conversion_Off;						//	��Ƶ����
	Down_Conversion_Rate = Down_Conversion_Rate_Off;			//	��Ƶ����

	if(Temp_Slow_Down_Speed_Old > 0)
		Data_Set_Current_Speed(Temp_Slow_Down_Speed_Old);
	
	Down_Conversion_Time=0;				//	��Ƶ ����
	Temp_Slow_Down_Speed_Old=0;		//	��Ƶ �ٶ�  �ٷֱ�
	Temp_Slow_Down_Speed_Now=0;		//	��Ƶ �ٶ�  �ٷֱ�
	Down_Conversion_Handler_Timing_Cnt=0;		// ��ʱ��

//-------------- mos �¶� -------------------
	//Temperature_Mos_Old=0;
	Temperature_Mos_Now=0;

	//Temperature_Mos_Timer_Old=0;

	memset(Mos_Temperature_Array,0,5);
	Mos_Temperature_Array_cnt = 0;

//-------------- ���� �¶� -------------------
	//Temperature_Box_Old=0;
	Temperature_Box_Now=0;

	//Temperature_Box_Timer_Old=0;

	memset(Box_Temperature_Array,0,5);
	Box_Temperature_Array_cnt = 0;

//-------------- ��� ���� -------------------
	//Current_Motor_Old=0;
	Current_Motor_Now=0;

	//Current_Motor_Timer_Old=0;

	memset(Out_Current_Array,0,5);
	Out_Current_Array_cnt = 0;
}

// ����ת��
void Down_Conversion_Set_Speed(uint8_t speed)
{
	Temp_Slow_Down_Speed_Now = speed;
	Data_Set_Current_Speed(Temp_Slow_Down_Speed_Now);
	//����ʱ��
	UpDate_Global_Status_Old();
}

// ��Ƶ����
void Do_Down_Conversion_interface(void)
{
	// ��һ�ν�
	if(Down_Conversion_Time == 0)
		Temp_Slow_Down_Speed_Old = Motor_Speed_Target_Get();
	
//	if(Down_Conversion_Time == 0)
//	{
//		Temp_Slow_Down_Speed_Old = Motor_Speed_Target_Get();
//		if((Temp_Slow_Down_Speed_Old - TIME_SLOW_DOWN_SPEED_01) >= TIME_SLOW_DOWN_SPEED_MIX)
//			Down_Conversion_Set_Speed(Temp_Slow_Down_Speed_Old - TIME_SLOW_DOWN_SPEED_01);
//		else
//			Down_Conversion_Set_Speed(TIME_SLOW_DOWN_SPEED_MIX);
//		Down_Conversion_Time ++;
//	}
//	else
	{
		Temp_Slow_Down_Speed_Now = Motor_Speed_Target_Get();
		if(Temp_Slow_Down_Speed_Now > TIME_SLOW_DOWN_SPEED_MIX)
		{
			Down_Conversion_Set_Speed(--Temp_Slow_Down_Speed_Now);
			Down_Conversion_Time ++;
		}
	}
}

// �жϵ�ǰ��Ƶģʽ
void Check_Down_Conversion_State(void)
{
	if((Down_Conversion_Type == Down_Conversion_Off)&&(Motor_Speed_Target_Get() > TIME_SLOW_DOWN_SPEED_MIX))
	{
		if(Current_Motor_Now >= MOTOR_CURRENT_REDUCE_SPEED)
			Down_Conversion_Type = Down_Conversion_Out_Current;
		else if(Temperature_Mos_Now >= MOS_TEMP_REDUCE_SPEED)
			Down_Conversion_Type = Down_Conversion_Mos_Temper;
		else if(Temperature_Box_Now >= AMBIENT_TEMP_REDUCE_SPEED)
			Down_Conversion_Type = Down_Conversion_Box_Temper;
	}
}

// �жϵ�ǰ��Ƶ����
void Check_Down_Conversion_Rate(void)
{
	//uint32_t time_difference_temp = 0;
	Down_Conversion_Rate_enum		rate_mos_temper  = Down_Conversion_Rate_Off;
	Down_Conversion_Rate_enum		rate_box_temper  = Down_Conversion_Rate_Off;
	Down_Conversion_Rate_enum		rate_out_current = Down_Conversion_Rate_Off;
	
	if(Temperature_Mos_Now >= MOS_TEMP_REDUCE_SPEED+3)
		rate_mos_temper = Down_Conversion_Rate_Realtime;
	else if(Temperature_Mos_Now >= MOS_TEMP_REDUCE_SPEED+2)
		rate_mos_temper = Down_Conversion_Rate_High;
	else if(Temperature_Mos_Now >= MOS_TEMP_REDUCE_SPEED+1)
		rate_mos_temper = Down_Conversion_Rate_Normal;		
	else if(Temperature_Mos_Now >= MOS_TEMP_REDUCE_SPEED)
		rate_mos_temper = Down_Conversion_Rate_Low;
	else
		rate_mos_temper = Down_Conversion_Rate_Off;

	if(Temperature_Box_Now >= AMBIENT_TEMP_REDUCE_SPEED+3)
		rate_box_temper = Down_Conversion_Rate_Realtime;
	else if(Temperature_Box_Now >= AMBIENT_TEMP_REDUCE_SPEED+2)
		rate_box_temper = Down_Conversion_Rate_High;
	else if(Temperature_Box_Now >= AMBIENT_TEMP_REDUCE_SPEED+1)
		rate_box_temper = Down_Conversion_Rate_Normal;		
	else if(Temperature_Box_Now >= AMBIENT_TEMP_REDUCE_SPEED)
		rate_box_temper = Down_Conversion_Rate_Low;
	else
		rate_box_temper = Down_Conversion_Rate_Off;
	
	if(Current_Motor_Now >= MOTOR_CURRENT_REDUCE_SPEED+3)
		rate_out_current = Down_Conversion_Rate_Realtime;
	else if(Current_Motor_Now >= MOTOR_CURRENT_REDUCE_SPEED+2)
		rate_out_current = Down_Conversion_Rate_High;
	else if(Current_Motor_Now >= MOTOR_CURRENT_REDUCE_SPEED+1)
		rate_out_current = Down_Conversion_Rate_Normal;		
	else if(Current_Motor_Now >= MOTOR_CURRENT_REDUCE_SPEED)
		rate_out_current = Down_Conversion_Rate_Low;
	else
		rate_out_current = Down_Conversion_Rate_Off;
	
	//ȡ���ֵ
	Down_Conversion_Rate = GET_MAX_FROM_3(rate_mos_temper, rate_box_temper, rate_out_current);
}
//void Check_Down_Conversion_Rate(void)
//{
//	uint32_t time_difference_temp = 0;
//	Down_Conversion_Rate_enum		rate_mos_temper  = Down_Conversion_Rate_Off;
//	Down_Conversion_Rate_enum		rate_box_temper  = Down_Conversion_Rate_Off;
//	Down_Conversion_Rate_enum		rate_out_current = Down_Conversion_Rate_Off;
//	
//	if(Temperature_Mos_Now > MOS_TEMP_REDUCE_SPEED)
//	{
//		rate_mos_temper = Down_Conversion_Rate_Low;
//		if(Temperature_Mos_Now > Temperature_Mos_Old)
//		{
//			time_difference_temp = Down_Conversion_Handler_Timing_Cnt - Temperature_Mos_Timer_Old;
//			
//			if(time_difference_temp < MOS_TEMPER_RATE_REALTIME)				//	1min		�쳣����
//				rate_mos_temper = Down_Conversion_Rate_Realtime;
//			else if(time_difference_temp < MOS_TEMPER_RATE_HIGH)			//	2min		����
//				rate_mos_temper = Down_Conversion_Rate_High;
//			else if(time_difference_temp < MOS_TEMPER_RATE_NORMAL)		//	5min		�����ٶ�
//				rate_mos_temper = Down_Conversion_Rate_Normal;
//		}
//	}
//	
//	if(Temperature_Box_Now > AMBIENT_TEMP_REDUCE_SPEED)
//	{
//		rate_box_temper = Down_Conversion_Rate_Low;
//		if(Temperature_Box_Now > Temperature_Box_Old)
//		{
//			time_difference_temp = Down_Conversion_Handler_Timing_Cnt - Temperature_Box_Timer_Old;
//			
//			if(time_difference_temp < BOX_TEMPER_RATE_REALTIME)				//	1min		�쳣����
//				rate_box_temper = Down_Conversion_Rate_Realtime;
//			else if(time_difference_temp < BOX_TEMPER_RATE_HIGH)			//	2min		����
//				rate_box_temper = Down_Conversion_Rate_High;
//			else if(time_difference_temp < BOX_TEMPER_RATE_NORMAL)		//	5min		�����ٶ�
//				rate_box_temper = Down_Conversion_Rate_Normal;
//		}
//	}
//	
//	if(Current_Motor_Now > MOTOR_CURRENT_REDUCE_SPEED)
//	{
//		rate_out_current = Down_Conversion_Rate_Low;
//		if(Current_Motor_Now > (Current_Motor_Old+1))
//		{
//			time_difference_temp = Down_Conversion_Handler_Timing_Cnt - Current_Motor_Timer_Old;
//			
//			if(time_difference_temp < OUT_CURRENT_RATE_REALTIME)				//	10S		�쳣����
//				rate_out_current = Down_Conversion_Rate_Realtime;
//			else if(time_difference_temp < OUT_CURRENT_RATE_HIGH)				//	30S		����
//				rate_out_current = Down_Conversion_Rate_High;
//			else if(time_difference_temp < OUT_CURRENT_RATE_NORMAL)			//	1min		�����ٶ�
//				rate_out_current = Down_Conversion_Rate_Normal;
//		}
//	}
//	
//	//ȡ���ֵ
//	Down_Conversion_Rate = GET_MAX_FROM_3(rate_mos_temper, rate_box_temper, rate_out_current);
//}


// �������ʽ�Ƶ
void Update_Down_Conversion_Speed(void)
{
	static uint16_t local_timer_cnt=0;
	uint8_t result = 0;
	
	local_timer_cnt++;
	
	switch( Down_Conversion_Rate )
	{
		case Down_Conversion_Rate_Off:					//	��
			return;
			//break;
		case Down_Conversion_Rate_Low:					//	����						1%/2min
			if( local_timer_cnt > DOWN_CONVERSION_RATE_LOW)
				result = 1;
			break;
		case Down_Conversion_Rate_Normal:				//	�����ٶ�					1%/min
			if( local_timer_cnt > DOWN_CONVERSION_RATE_NORMAL)
				result = 1;
			break;
		case Down_Conversion_Rate_High:					//	����						1%/30s
			if( local_timer_cnt > DOWN_CONVERSION_RATE_HIGH)
				result = 1;
			break;
		case Down_Conversion_Rate_Realtime:			//	�쳣����					1%/10s
			if( local_timer_cnt > DOWN_CONVERSION_RATE_REALTIME)
				result = 1;
			break;
	}
	if( (result == 1) || ( Down_Conversion_Time == 0) )
	{
		Do_Down_Conversion_interface();
		local_timer_cnt = 0;
	}
	
}

// ͣ�� �ж�
void Check_Fault_to_Stop(void)
{
//	uint8_t DCR_Timer[Down_Conversion_Rate_Realtime+1] = {
//		DOWN_CONVERSION_RATE_LOW,DOWN_CONVERSION_RATE_LOW,
//		DOWN_CONVERSION_RATE_NORMAL,
//		DOWN_CONVERSION_RATE_HIGH,
//		DOWN_CONVERSION_RATE_REALTIME};
	
	if(Current_Motor_Now > MOTOR_CURRENT_ALARM_VALUE)
	{
		if(Current_Fault_To_Stop_Timer++ > 5)		// 5��
		{
			Set_Motor_Fault_State(E002_BUS_CURRENT_ABNORMAL);
		}
	}
	else
		Current_Fault_To_Stop_Timer = 0;
	
	//ͣ��ǰ3���׶�
	// �� �ٶȴ��� 80% ʱ ֱ�ӿ���
	// �� �ٶ�С�� 80% ���� 20%  ֱ�ӽ��� 20%
	// �� ���ղ�ͣ��
	if((Temperature_Mos_Now > MOS_TEMP_ALARM_VALUE) || (Temperature_Box_Now > AMBIENT_TEMP_ALARM_VALUE))
	{
		Temperature_Fault_To_Stop_Timer ++;
		if(Temperature_Fault_To_Stop_Timer >= DOWN_CONVERSION_RATE_LOW)	//2����
		{
			Temp_Slow_Down_Speed_Now = Motor_Speed_Target_Get();
			if(Temp_Slow_Down_Speed_Now > 80)
			{
				Down_Conversion_Set_Speed(Temp_Slow_Down_Speed_Now/2);
				Temperature_Fault_To_Stop_Timer = 0;
			}
			else if(Temp_Slow_Down_Speed_Now > 20)
			{
				Down_Conversion_Set_Speed(TIME_SLOW_DOWN_SPEED_MIX);
				Temperature_Fault_To_Stop_Timer = 0;
			}
			else
			{
				if(Temperature_Mos_Now > MOS_TEMP_ALARM_VALUE)
					Set_Motor_Fault_State(E101_TEMPERATURE_MOS);
				else if(Temperature_Box_Now > AMBIENT_TEMP_ALARM_VALUE)
					Set_Motor_Fault_State(E102_TEMPERATURE_AMBIENT);
			}
		}
	}
}

//  ���� ����
void Do_Down_Conversion_Rebound(uint8_t time)
{
	static uint16_t Rebound_timer_cnt=0;
	
	if((Rebound_timer_cnt % time) == 0)	//2���� or ��һ��
	{
		Temp_Slow_Down_Speed_Now = Motor_Speed_Target_Get();
		
		if(Temp_Slow_Down_Speed_Now < Temp_Slow_Down_Speed_Old)
		{
			Down_Conversion_Set_Speed(++Temp_Slow_Down_Speed_Now);
		}
		else
		{
			Clean_All_Down_Conversion_Status();		//�˳���Ƶ
		}
	}
	Rebound_timer_cnt ++;
}

// ����ʱ ����
void Check_Down_Conversion_Rebound(void)
{
	switch(Down_Conversion_Type)
	{
		case Down_Conversion_Off:							//	��
			break;
		case Down_Conversion_Mos_Temper:			//	mos ����
			if(Temperature_Mos_Now < (MOS_TEMP_RESTORE_SPEED-10))
				Do_Down_Conversion_Rebound(REBOUND_FREQUENCHY_TIME_HIGH);
			else if(Temperature_Mos_Now < MOS_TEMP_RESTORE_SPEED)
				Do_Down_Conversion_Rebound(REBOUND_FREQUENCHY_TIME_LOW);
			break;
		case Down_Conversion_Box_Temper:			//	���� ����
			if(Temperature_Box_Now < (AMBIENT_TEMP_RESTORE_SPEED-10))
				Do_Down_Conversion_Rebound(REBOUND_FREQUENCHY_TIME_HIGH);
			else if(Temperature_Box_Now < AMBIENT_TEMP_RESTORE_SPEED)
				Do_Down_Conversion_Rebound(REBOUND_FREQUENCHY_TIME_LOW);
			break;
		case Down_Conversion_Out_Current:			//	��� ����
			if(Temperature_Box_Now < (MOTOR_CURRENT_RESTORE_SPEED-10))
				Do_Down_Conversion_Rebound(REBOUND_FREQUENCHY_TIME_HIGH);
			else if(Temperature_Box_Now < MOTOR_CURRENT_RESTORE_SPEED)
				Do_Down_Conversion_Rebound(REBOUND_FREQUENCHY_TIME_LOW);
			break;
		case Down_Conversion_Out_Power:				//	��� ����
			break;
	}
}

void update_buffer(uint8_t num, uint8_t *buffer, uint8_t *count) {
    if (*count < DOWN_CONVERSION_BUFFER_SIZE) {
        buffer[*count] = num;
        (*count)++;
    } else {
        // �Ƴ����Ԫ�ز�ǰ��
        for (int i = 0; i < DOWN_CONVERSION_BUFFER_SIZE-1; i++) {
            buffer[i] = buffer[i+1];
        }
        buffer[DOWN_CONVERSION_BUFFER_SIZE-1] = num;
    }
}

int compare(const void *a, const void *b) {
    return (*(uint8_t*)a - *(uint8_t*)b);
}

int get_median(uint8_t *buffer) {
    int temp[DOWN_CONVERSION_BUFFER_SIZE];
    for (int i = 0; i < DOWN_CONVERSION_BUFFER_SIZE; i++) {
        temp[i] = buffer[i];
    }
    qsort(temp, DOWN_CONVERSION_BUFFER_SIZE, sizeof(int), compare);
    return temp[DOWN_CONVERSION_BUFFER_SIZE/2]; // ֱ�ӷ����м�Ԫ��
}
//-------------------- ���½�Ƶ  mos ----------------------------
// Temperature ��Ϊ����
void Check_Down_Conversion_MOS_Temperature(short int Temperature)
{
	// 200ms ��һ��
	uint8_t temp=0;
	
	if(Temperature < 0)
		return;
	
	temp = Temperature;
	
	update_buffer(temp,Mos_Temperature_Array,&Mos_Temperature_Array_cnt);

	Temperature_Mos_Now = get_median(Mos_Temperature_Array);
	
}

//-------------------- ���½�Ƶ  ���� ----------------------------
// Temperature ��Ϊ����
void Check_Down_Conversion_BOX_Temperature(short int Temperature)
{
	// 200ms ��һ��
	uint8_t temp=0;
	
	if(Temperature < 0)
		return;
	
	temp = Temperature;
		
	update_buffer(temp,Box_Temperature_Array,&Box_Temperature_Array_cnt);

	Temperature_Box_Now = get_median(Box_Temperature_Array);
}

//-------------------- ���ʽ�Ƶ   ----------------------------
void Check_Down_Conversion_Motor_Power(uint16_t power)
{

}

//-------------------- ������Ƶ   ----------------------------
void Check_Down_Conversion_Motor_Current(uint32_t Current)
{
	// 200ms ��һ��
	uint8_t temp=0;
	
//	if(Current < 0)
//		return;
	
	temp = Current;
	
	update_buffer(temp,Out_Current_Array,&Out_Current_Array_cnt);

	Current_Motor_Now = get_median(Out_Current_Array);
}

//-------------------- ��ȡ��Ƶ���� ----------------------------
uint8_t Get_Down_Conversion_Time(void)
{
	return Down_Conversion_Time;
}

//-------------------- ��ȡ��Ƶǰ�ٶ� ----------------------------
uint8_t Get_Down_Conversion_Speed_Old(void)
{
	return Temp_Slow_Down_Speed_Old;
}
//-------------------- ��ȡ��Ƶǰ�ٶ� ----------------------------
void Set_Down_Conversion_Speed_Old(uint8_t vaule)
{
	Temp_Slow_Down_Speed_Old = vaule;
}
//-------------------- ��ȡ��Ƶ�����ٶ� ----------------------------
uint8_t Get_Down_Conversion_Speed_Now(void)
{
	return Temp_Slow_Down_Speed_Now;
}

//-------------------- ���ý�Ƶģʽ ----------------------------
//void Set_Temp_Slow_Down_State(uint8_t vaule)
//{
//	Down_Conversion_Type = vaule;
//}

//-------------------- ��ȡ���½���ģʽ ----------------------------
uint8_t Get_Temp_Slow_Down_State(void)
{
	return Down_Conversion_Type;
}

//-------------------- ���ý�Ƶ MOS �¶� ----------------------------
//void Down_Conversion_Set_Temperature_Mos(uint8_t vaule)
//{
//	Temperature_Mos_Now = vaule;
//}

//-------------------- ��ȡ��Ƶ MOS �¶� ----------------------------
uint8_t Down_Conversion_Get_Temperature_Mos(void)
{
	return Temperature_Mos_Now;
}

//-------------------- ���ý�Ƶ Box �¶� ----------------------------
//void Down_Conversion_Set_Temperature_Box(uint8_t vaule)
//{
//	Temperature_Box_Now = vaule;
//}

//-------------------- ��ȡ��Ƶ Box �¶� ----------------------------
uint8_t Down_Conversion_Get_Temperature_Box(void)
{
	return Temperature_Box_Now;
}

//-------------------- ���ý�Ƶ Current ���� ----------------------------
//void Down_Conversion_Set_Current_Motor(uint8_t vaule)
//{
//	Current_Motor_Now = vaule;
//}

//-------------------- ��ȡ��Ƶ Current ���� ----------------------------
uint8_t Down_Conversion_Get_Current_Motor(void)
{
	return Current_Motor_Now;
}

//-------------------- ģ�����ݲ��� ----------------------------
void Down_Conversion_Analog_Data_Test(void)
{
	static uint8_t direction=0;
	static uint8_t timer=0;
//	*p_Mos_Temperature
//	*p_Box_Temperature
//	*p_Motor_Current
	if(++timer < 1)
		return;
	
	timer = 0;
	// 90 - 60
	if(direction == 0)//up
	{
		if(Temperature_Mos_Now < 60)
			Temperature_Mos_Now = 60;
		else if(Temperature_Mos_Now < 90)
			Temperature_Mos_Now ++;
		else
			direction = 1;
	}
	else
	{
		if(Temperature_Mos_Now > 60)
			Temperature_Mos_Now --;
		else
			direction = 0;
	}
	
	
//	Temperature_Mos_Now	=	
//	Temperature_Box_Now
//	Current_Motor_Now
//	
}


// ��Ƶ�������߳� 	ÿ���һ��
void Down_Conversion_Handler(void)
{
	//Down_Conversion_Analog_Data_Test();
	
	Down_Conversion_Handler_Timing_Cnt ++;
	// �жϵ�ǰ��Ƶģʽ
	Check_Down_Conversion_State();
	// �жϵ�ǰ��Ƶ����
	Check_Down_Conversion_Rate();
	// �������ʽ�Ƶ
	Update_Down_Conversion_Speed();
	// ͣ�� �ж�
	Check_Fault_to_Stop();
	// ����ʱ����
	Check_Down_Conversion_Rebound();
}
