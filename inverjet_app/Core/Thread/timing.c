/**
******************************************************************************
* @file				timing.c
* @brief			��ʱ�߳�  ��� 1 s, ִ�е����ͼ�ʱ����
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "timing.h"
#include "wifi_thread.h"
#include "tm1621.h"
#include "motor.h"
#include "display.h"
#include "key.h"
#include "debug_protocol.h"
#include "wifi.h"
#include "down_conversion.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
// ͨ�ü�����
static uint32_t Timing_Timer_Cnt = 0;

// �����ʱ��
static uint32_t Timing_Thread_Task_Cnt = 0;
static uint32_t Timing_Half_Second_Cnt = 0;
static uint32_t WIFI_Distribution_Timing_Cnt=0;		// wifi״̬ͼ�� ��ʱ��
static uint32_t BT_Distribution_Timing_Cnt=0;			// ����״̬ͼ�� ��ʱ��
static uint32_t System_Fault_Timing_Cnt=0;				// ���ϻָ� ��ʱ��
static uint32_t Fault_Recovery_Timing_Cnt=0;			// ���ϻָ����� ��ʱ��		1Сʱ

static uint32_t Automatic_Shutdown_Timing_Cnt=0;				// �Զ��ػ� ��ʱ��

static uint8_t System_Fault_Recovery_Cnt=0;				// ���ϻָ����� ������
static uint8_t Change_Speed_Timing_Cnt = 0;				// �����ٶ� 3���ʼ���� ��ʱ��

//  ���� ����
static uint32_t Temp_Slow_Down_Timing_Cnt= 0;		//��ʱ��

//  ˢ����Ļ
static uint8_t LCD_Refresh_State= 0;

#ifdef SYSTEM_LONG_RUNNING_MODE
//********* �ϻ���װ ***********************************************
// �ϻ���װ ��ʱ��
static uint32_t Old_Chemical_Equipment_Cnt =0;
//******************************************************************
#endif
/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

// Ӳ�� & ����
void App_Timing_Init(void)
{
	LCD_Show_Bit = STATUS_BIT_PERCENTAGE;
	
	System_Boot_Screens();			// 5s
	Dev_Check_Control_Methods();
	//�������
	System_PowerUp_Finish = 0xAA;
	//all_data_update();		// wifi �ϴ�
	
	System_Power_Off();
	//System_Power_On();//wuqingguang �ϵ��Ϊ���� ����������ʹ��
}

static uint32_t wifi_timing_old = 0;

void System_Check_Timer_Clean(void)
{	
	wifi_timing_old = 0;
	*p_Wifi_Timing_Value_Old = 0xFFFFFF;
	*p_Check_Timing_Add_More = 0;
	*p_Check_Timing_Minus_More = 0;
}

void Use_Wifi_Timing_Check(void)
{
	if((*p_Wifi_Timing_Value > 86400) || (*p_Check_Timing_Error_Cnt > 10))
		return;
		
	if(*p_Wifi_Timing_Value == wifi_timing_old)
	{
		*p_Check_Timing_Error_Cnt += 1;
	}
	else
	{
		if(wifi_timing_old == 0)
			wifi_timing_old = *p_Wifi_Timing_Value;
		*p_Check_Timing_Error_Cnt = 0;
	}
	
	if((*p_Wifi_Timing_Value_Old > 86400) || ((*p_Wifi_Timing_Value - *p_Wifi_Timing_Value_Old) > 2))
	{
		*p_Wifi_Timing_Value_Old = *p_Wifi_Timing_Value;
		return;
	}
	
	if(*p_Wifi_Timing_Value_Old >= *p_Wifi_Timing_Value)  //�߿���
	{
		Timing_Thread_Task_Cnt --;
		*p_Check_Timing_Minus_More += 1;
		*p_Check_Timing_Add_More = 0;
	}
	else if((*p_Wifi_Timing_Value - *p_Wifi_Timing_Value_Old) > 1) //������
	{
		Timing_Thread_Task_Cnt ++;
		*p_Check_Timing_Add_More += 1;
		if(*p_Check_Timing_Add_More > 40)
			Timing_Thread_Task_Cnt ++;
		*p_Check_Timing_Minus_More = 0;
	}
	else
	{
		*p_Check_Timing_Add_More = 0;
		*p_Check_Timing_Minus_More = 0;
	}
	
	*p_Wifi_Timing_Value_Old += 1;
}

void Clean_Timing_Timer_Cnt(void)
{
	Timing_Timer_Cnt = 0;
}

// ����ˢ�±�־
void LCD_Refresh_Set(uint8_t value)
{
	LCD_Refresh_State = value;
}

// ��ȡˢ�±�־
uint8_t LCD_Refresh_Get(void)
{
	return LCD_Refresh_State;
}

// �ָ�ˢ��
void LCD_Refresh_Restore(void)
{
	if(LCD_Refresh_Get()== 1)
		LCD_Refresh_Set(0);
}

void Speed_Save_Flash(Operating_Parameters op_node,uint8_t system_state)
{
	if(system_state == TIMING_MODE_INITIAL)
	{
		p_OP_Timing_Mode->speed = op_node.speed;
		p_OP_Timing_Mode->time = op_node.time;
		Write_MbBuffer_Later();
		//MB_Flash_Buffer_Write();
		//STMFLASH_Write(FLASH_APP_PARAM_ADDR+(MB_USER_TIME_MODE_SPEED*2), (uint16_t *)p_OP_Timing_Mode, 2 );// REG_HOLDING_NREGS
	}
	else if(system_state == FREE_MODE_INITIAL)
	{
		p_OP_Free_Mode->speed = op_node.speed;
		p_OP_Free_Mode->time = op_node.time;
		Write_MbBuffer_Later();
		//MB_Flash_Buffer_Write();
		//STMFLASH_Write(FLASH_APP_PARAM_ADDR+(MB_USER_FREE_MODE_SPEED*2), (uint16_t *)p_OP_Free_Mode, 2 );// REG_HOLDING_NREGS
	}
}

// wifi ״̬��    TIMING_THREAD_LIFECYCLE
void WIFI_State_Handler(void)
{
	static uint8_t wifi_state = 0;
	//static uint8_t wifi_time_cnt = 0;
	
	// -------- ����
	if(Dev_Is_Control_Methods(MACRO_WIFI_USART))
	{
		LCD_Show_Bit &= ~STATUS_BIT_WIFI;
//		// ���� 3s
//		if(wifi_time_cnt < (TIMING_THREAD_HALF_SECOND*2))
//		{
//			LCD_Show_Bit |= STATUS_BIT_WIFI;
//		}
//		else if(wifi_time_cnt < (TIMING_THREAD_HALF_SECOND*4))
//		{
//			if((wifi_time_cnt & 5)==0)
//			{
//				if((LCD_Show_Bit & STATUS_BIT_WIFI) == 0)
//					LCD_Show_Bit |= STATUS_BIT_WIFI;
//				else
//					LCD_Show_Bit &= ~STATUS_BIT_WIFI;
//			}
//		}
//		else if(wifi_time_cnt < (TIMING_THREAD_HALF_SECOND*6))
//		{
//			LCD_Show_Bit &= ~STATUS_BIT_WIFI;
//		}
//		else
//			wifi_time_cnt = 0;
//		
//		wifi_time_cnt ++;
	}
	// -------- ����
	else if(WIFI_Get_Machine_State() == WIFI_ERROR)
	{
		if((Timing_Thread_Task_Cnt % WIFI_ERROR_BLINK_TIME) == 0)
		{
			WIFI_Distribution_Timing_Cnt = 0;
			if((LCD_Show_Bit & STATUS_BIT_WIFI) == 0)
				LCD_Show_Bit |= STATUS_BIT_WIFI;
			else
				LCD_Show_Bit &= ~STATUS_BIT_WIFI;
		}
	}
	// -------- ����
	else
	{
		if(Timing_Thread_Task_Cnt >= TIMING_THREAD_HALF_SECOND) //����
		{
			if( mcu_get_wifi_work_state() != wifi_state )
			{
				wifi_state = mcu_get_wifi_work_state();
				DEBUG_PRINT("wifi״̬: %d\n",wifi_state);
			}
			
			if(WIFI_Get_Machine_State() == WIFI_DISTRIBUTION)
			{
				if(( WIFI_Distribution_Timing_Cnt == 0)||(WIFI_Distribution_Timing_Cnt > Timing_Half_Second_Cnt))
				{
					WIFI_Distribution_Timing_Cnt = Timing_Half_Second_Cnt;
				}
				
				if((Timing_Half_Second_Cnt % WIFI_DISTRIBUTION_BLINK_TIME) == 0)
				{
					if((LCD_Show_Bit & STATUS_BIT_WIFI) == 0)
						LCD_Show_Bit |= STATUS_BIT_WIFI;
					else
						LCD_Show_Bit &= ~STATUS_BIT_WIFI;
				}
			}
			else if(WIFI_Get_Machine_State() == WIFI_WORKING)
			{
				WIFI_Distribution_Timing_Cnt = 0;
				LCD_Show_Bit |= STATUS_BIT_WIFI;
			}
			else
			{
				WIFI_Distribution_Timing_Cnt = 0;
				LCD_Show_Bit &= ~STATUS_BIT_WIFI;
			}
		}
	}
}

// ���� ״̬��  TIMING_THREAD_LIFECYCLE
void BT_State_Handler(void)
{
	if(BT_Get_Machine_State() == BT_ERROR)
	{
		BT_Distribution_Timing_Cnt = 0;
		if((Timing_Thread_Task_Cnt % BT_ERROR_BLINK_TIME) == 0)
		{
			if((LCD_Show_Bit & STATUS_BIT_BLUETOOTH) == 0)
				LCD_Show_Bit |= STATUS_BIT_BLUETOOTH;
			else
				LCD_Show_Bit &= ~STATUS_BIT_BLUETOOTH;
		}
	}
	else
	{
		if(Timing_Thread_Task_Cnt >= TIMING_THREAD_HALF_SECOND) //����
		{
			if(BT_Get_Machine_State() == BT_DISTRIBUTION)
			{
				BT_Distribution_Halder();
				
				if(( BT_Distribution_Timing_Cnt == 0)||(BT_Distribution_Timing_Cnt > Timing_Half_Second_Cnt))
				{
					BT_Distribution_Timing_Cnt = Timing_Half_Second_Cnt;
				}
				
				if((Timing_Half_Second_Cnt % BT_DISTRIBUTION_BLINK_TIME) == 0)
				{
					if((LCD_Show_Bit & STATUS_BIT_BLUETOOTH) == 0)
						LCD_Show_Bit |= STATUS_BIT_BLUETOOTH;
					else
						LCD_Show_Bit &= ~STATUS_BIT_BLUETOOTH;
				}
				
				//��ʱ�����ָ�
				if( (Timing_Half_Second_Cnt - BT_Distribution_Timing_Cnt) > BT_DISTRIBUTION_TIME_CALLOUT)
				{
					BT_Distribution_Timing_Cnt = 0;
					BT_Set_Machine_State(BT_NO_CONNECT);
					LCD_Show_Bit &= ~STATUS_BIT_BLUETOOTH;
				}
			}
			else if(BT_Get_Machine_State() == BT_WORKING)
			{
				BT_Distribution_Timing_Cnt = 0;
				LCD_Show_Bit |= STATUS_BIT_BLUETOOTH;
			}
			else
			{
				BT_Distribution_Timing_Cnt = 0;
				LCD_Show_Bit &= ~STATUS_BIT_BLUETOOTH;
			}
		}
	}
}


// ��� ���� ״̬��
void Timing_Clean_Fault_State(void)
{
	if(If_Fault_Recovery_Max())
		return;
	
	Clean_Fault_State();
	System_Fault_Timing_Cnt = 0;
	//System_Wifi_State_Clean();
}


// �˳� ���� ״̬
void CallOut_Fault_State(void)
{
	Timing_Clean_Fault_State();
	if(System_is_Power_Off())//�ػ�
	{
		System_Power_Off();
	}
	Lcd_Show();
}

// ���� ״̬��  1���һ��
void Fault_State_Handler(void)
{
	if(ERROR_DISPLAY_STATUS != Get_System_State_Machine())
	{
		To_Fault_Menu();
	}
	else if(System_is_Error())//���� 2min�ָ� ��ͨѶ����
	{
		if(If_Fault_Recovery_Max()==0)//&&(*p_System_Fault_Static != E203_MOTOR_LOSS))
		{
			if(( System_Fault_Timing_Cnt == 0)||(System_Fault_Timing_Cnt > Timing_Half_Second_Cnt))
			{
				System_Fault_Timing_Cnt = Timing_Half_Second_Cnt;
			}
			
			//��ʱ ���� �ָ�
			if( (Timing_Half_Second_Cnt - System_Fault_Timing_Cnt) > SYSTEM_FAULT_TIME_CALLOUT)
			{
				if((IS_SELF_TEST_MODE() == 0)&&( *p_Motor_Fault_Static == 0))
				{
					CallOut_Fault_State();
				}
				//Fault_Recovery_Attempt_cnt = RECOVERY_ATTEMPT_TIME;
			}
		}
	}
	
	if(Motor_Speed_Is_Reach())
	{
		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		{
			Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
		}
	}
	
	
	Update_Fault_Menu();
}

// ������ ״̬��  1���һ��
void Starting_State_Handler(void)
{
	System_Check_Timer_Clean();
	Arbitrarily_To_Running();
	return;
	
//	*p_OP_ShowNow_Time --;
//	Lcd_Show();
//	if(*p_OP_ShowNow_Time == 0)
//	{
//		Arbitrarily_To_Running();
//		*p_OP_ShowNow_Time = p_OP_ShowLater->time;
//		Lcd_Show();
//	}
}

// ���� ״̬��  1���һ��
void Running_State_Handler(void)
{

	if(Get_System_State_Machine() == FREE_MODE_RUNNING)									// ����
	{
		*p_OP_ShowNow_Time = (*p_OP_ShowNow_Time + 1);
		if(*p_OP_ShowNow_Time >= MOTOR_TIME_SHOW_MAX)
		{
			*p_OP_ShowNow_Time = 0;
		}
	}
	else if(Get_System_State_Machine() == TIMING_MODE_RUNNING)					// ��ʱ
	{
		Finish_Statistics_Count(1);	//���� ���ͳ��
		*p_OP_ShowNow_Time = (*p_OP_ShowNow_Time - 1);
		Lcd_Show();
		if(*p_OP_ShowNow_Time == 0)
		{
			Arbitrarily_To_Stop();
			*p_OP_ShowNow_Time = p_OP_Timing_Mode->time;
		}
	}
	else if(Get_System_State_Machine() == TRAINING_MODE_RUNNING)					// ѵ��
	{
		Finish_Statistics_Count(1);	//���� ���ͳ��
		*p_OP_ShowNow_Time = (*p_OP_ShowNow_Time + 1);
		
		if(Get_System_State_Mode() == SURFING_MODE_NUMBER_ID)//����
		{
			if(*p_OP_ShowNow_Time >= MOTOR_TIME_SHOW_MAX)
			{
				*p_OP_ShowNow_Time = 0;
			}
			if((*p_OP_ShowNow_Time < *p_Surf_Mode_Info_Prepare_Time))
				Data_Set_Current_Speed(*p_Surf_Mode_Info_Low_Speed);//ע��,��Ҫ����������״̬���������ٶ�,��"��ͣ"
			else if(((*p_OP_ShowNow_Time - (*p_Surf_Mode_Info_Prepare_Time)) % (*p_Surf_Mode_Info_High_Time + *p_Surf_Mode_Info_Low_Time)) == 0)
				Data_Set_Current_Speed(*p_Surf_Mode_Info_High_Speed);//ע��,��Ҫ����������״̬���������ٶ�,��"��ͣ"
			else if(((*p_OP_ShowNow_Time - (*p_Surf_Mode_Info_Prepare_Time)) % (*p_Surf_Mode_Info_High_Time + *p_Surf_Mode_Info_Low_Time)) == *p_Surf_Mode_Info_High_Time)
				Data_Set_Current_Speed(*p_Surf_Mode_Info_Low_Speed);//ע��,��Ҫ����������״̬���������ٶ�,��"��ͣ"
		}
		else if(Is_Mode_Legal(Get_System_State_Mode()))
		{
			if(*p_OP_ShowNow_Time >= p_OP_PMode[Get_System_State_Mode()-1][Period_Now].time)
			{
				if((Period_Now == (TRAINING_MODE_PERIOD_MAX-1))||(p_OP_PMode[Get_System_State_Mode()-1][Period_Now+1].time < p_OP_PMode[Get_System_State_Mode()-1][Period_Now].time))
				{
					Arbitrarily_To_Stop();
					*p_OP_ShowNow_Speed = p_OP_PMode[Get_System_State_Mode()-1][0].speed;
				}
				else
				{
					Period_Now ++;
					Data_Set_Current_Speed(p_OP_PMode[Get_System_State_Mode()-1][Period_Now].speed);//ע��,��Ҫ����������״̬���������ٶ�,��"��ͣ"
				}
			}
		}
	}
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_CHANGE))
	{
		Change_Speed_Timing_Cnt ++;
		if(Get_Temp_Slow_Down_State())
			Clean_Temp_Slow_Down_Timer();
		if(Change_Speed_Timing_Cnt >= 3)
		{
			if(Get_Temp_Slow_Down_State())
			{
				Set_Down_Conversion_Speed_Old(*p_OP_ShowNow_Speed);
				Clean_All_Down_Conversion_Status();
			}
			Change_Speed_Timing_Cnt = 0;
			Special_Status_Delete(SPECIAL_BIT_SPEED_CHANGE);
			if(*p_OP_ShowNow_Speed != Motor_Speed_Target_Get())
				Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
		
		}
	}
	// ת�ٴﵽĿ��ֵ
	if(Motor_Speed_Is_Reach())
	{
		// ��Ȧ ����
		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		{
			Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
		}
	}
	//��Ƶ
	Down_Conversion_Handler();

	if(Get_Temp_Slow_Down_State())
		Temp_Slow_Down_Timing_Cnt++;
	else
		Temp_Slow_Down_Timing_Cnt = 0;
	
	
	//���ٽ��� 2��1ˢ
	if(((Temp_Slow_Down_Timing_Cnt % 4)==2)||((Temp_Slow_Down_Timing_Cnt % 4)==3))
	{
		LCD_Refresh_Set(1);
		Lcd_Show_Slow_Down(Get_Temp_Slow_Down_State());
	}
	else
	{
		Lcd_Show();
		LCD_Refresh_Set(0);
	}
	
}

// ��ͣ ״̬��  1���һ��
void Pause_State_Handler(void)
{
	if(*p_OP_ShowNow_Speed != 0)
		Data_Set_Current_Speed(0);//ע��,��Ҫ����������״̬���������ٶ�,��"��ͣ"
	
	if(Timing_Half_Second_Cnt >= Automatic_Shutdown_Timing_Cnt)
	{
		if((Timing_Half_Second_Cnt - Automatic_Shutdown_Timing_Cnt) > AUTOMATIC_SHUTDOWN_TIME)
		{
			System_Power_Off();
		}
	}
	else
	{
		Clean_Automatic_Shutdown_Timer();
	}
		
	if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
}
// �˵� ״̬��  1���һ��
void Operation_State_Handler(void)
{
	Sleep_Time_Count(1);
	// 3�� ��˸
	if(Check_Sleep_Time_Out())
	{
		//���� flash
		Write_MbBuffer_Later();//��flash
		System_Power_Off();
		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
			Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
	}
}
// ֹͣ ״̬��  1���һ��
void Stop_State_Handler(void)
{
	if(Timing_Timer_Cnt == 0)
		Finish_Statistics_Upload();			// �ϴ�<ͳ������>
	Timing_Timer_Cnt++;
	// 3�� ��˸
	if(Timing_Timer_Cnt < 4)
	{
		if( (Timing_Timer_Cnt % 2) == 1)
		{
			LCD_Refresh_Set(1);
			//Lcd_Off();
			Lcd_Speed_Off();
		}
		else
			Lcd_Show();
	}
	else
	{
		// ���� ����ģʽ ��ʼ״̬
		To_Free_Mode(FREE_MODE_NOT_AUTO_START);
		// ���� ����ģʽ ��ͣ״̬
		p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
		*p_OP_ShowNow_Speed = 0;
		Data_Set_Current_Speed(0);//ע��,��Ҫ����������״̬���������ٶ�,��"����"
		Arbitrarily_To_Pause();
		
		Lcd_Show();
		Timing_Timer_Cnt = 0;
	}
	if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
}

// ��ʼ ״̬��  1���һ��
void Initial_State_Handler(void)
{
	if(Special_Status_Get( SPECIAL_BIT_SKIP_INITIAL))// ���� �Զ�����
	{
		/* wuqingguang ������˸
		if((Timing_Timer_Cnt % 2) == 1)
		{
			LCD_Refresh_Set(1);
			Lcd_Speed_Off();
		}
		else
		{
			//LCD_Refresh_Set(0);
			Lcd_Show();
		}
		*/
		if(((Timing_Half_Second_Cnt - Automatic_Shutdown_Timing_Cnt) > AUTOMATIC_SHUTDOWN_TIME) && (Timing_Half_Second_Cnt > Automatic_Shutdown_Timing_Cnt))
		{
			System_Power_Off();
		}
	}
	else
	{
		// 3�� ��˸
		if(Timing_Timer_Cnt < 2)
		{
			if(Timing_Timer_Cnt == 1)
			{
				LCD_Refresh_Set(1);
				Lcd_Speed_Off();
			}
			else
			{
				//LCD_Refresh_Set(0);
				Lcd_Show();
			}
		}
		else
		{
			LCD_Refresh_Set(0);//�ָ�ˢ��
			
			//Add_Ctrl_Log();
			
			if(Get_System_State_Machine() == TIMING_MODE_INITIAL)
			{
				p_OP_ShowLater->time = *p_OP_ShowNow_Time;
			}
			Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//��Ȧ�Զ��ж�
			
			
	//		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))	//���� ������
			{
				//Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING); //�ײ�ת��ͬ������ɾ��
				p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
				
				Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
				
				//����
				Lcd_Show();
				Arbitrarily_To_Running();
			}
	//		else
	//		{
	//			// ���� ������
	//			Arbitrarily_To_Starting();
	//			//*p_OP_ShowLater->time = *p_OP_ShowNow_Time;
	//			*p_OP_ShowNow_Time = 20;
	//			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
	//			Lcd_Show();
	//		}
			Timing_Timer_Cnt = 0;
		}
	}
	Timing_Timer_Cnt++;
}

// ��ʱ�������߳�
void App_Timing_Task(void)
{
	static uint8_t half_second_state=0;
	
	Timing_Thread_Task_Cnt++;
	
	WIFI_State_Handler();
	BT_State_Handler();
	
	if(Timing_Thread_Task_Cnt >= TIMING_THREAD_HALF_SECOND)//����
	{
		Timing_Thread_Task_Cnt = 0;
		Timing_Half_Second_Cnt ++;
		
		if(half_second_state == 1)
		{
			half_second_state = 0;
			*p_System_Runing_Second_Cnt += 1;			// ����ʱ��
			*p_No_Operation_Second_Cnt += 1;			// �޲���ʱ��
			*p_System_Startup_Second_Cnt += 1;		// ����ʱ��
							
			if((Fault_Recovery_Timing_Cnt > 0) && ((Timing_Half_Second_Cnt - Fault_Recovery_Timing_Cnt) > SYSTEM_FAULT_RECOVERY_TIME))
			{
				DEBUG_PRINT("���ϻָ���ʱ����1Сʱ,���������:\t%d\n",System_Fault_Recovery_Cnt);
				Clean_Fault_Recovery_Cnt();//���� 1 Сʱ ���������
				Fault_Recovery_Timing_Cnt = 0; // ���¼�ʱ
			}
#ifdef SYSTEM_LONG_RUNNING_MODE
			//********* �ϻ���װ ***********************************************
			// �ϻ���װ 4h��  14400 ͣ2h  7200
			// �չ� ��������  ��3min  ͣ1min : 180 : 60
			Old_Chemical_Equipment_Cnt ++;
			if(System_is_Power_Off())//�ػ�
			{
				if(Old_Chemical_Equipment_Cnt > 7200)
				{
					Buzzer_Click_Long_On(1);
					Old_Chemical_Equipment_Cnt = 0;
					System_Power_On();
				}
			}
			else
			{
				if(Old_Chemical_Equipment_Cnt > 79200)
				{
					Buzzer_Click_Long_On(1);
					Old_Chemical_Equipment_Cnt = 0;
					System_Power_Off();
				}
			}
			//******************************************************************
#endif
			
			// ʱ�� : ��˸  ����
			if(System_is_Normal_Operation())
			{
				LCD_Show_Bit |= STATUS_BIT_COLON;
				TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		1);
				TM1621_LCD_Redraw();
			}
			// ����߳�����
			Motor_Function_In_One_Second();
		}
		else
		{
			half_second_state = 1;
			if(System_is_Power_Off())
				return;
			
			//���Уʱ
			Use_Wifi_Timing_Check();
			
			if(Motor_is_Start()==0)
			{
				// ʱ�� : ��˸  ����
				LCD_Show_Bit &= ~STATUS_BIT_COLON;
				TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		0);
				TM1621_LCD_Redraw();
			}
			//�����Ƶ״̬
			if(System_is_Running() == 0)
				Clean_All_Down_Conversion_Status();
			//���ϼ��
//-----------------  չʾ���� -------------------------
#ifndef SYSTEM_SHOW_MODEL_MACHINE
			if(If_System_Is_Error())
			{
				Fault_State_Handler();
			}
			else
#endif
			{
				System_Fault_Timing_Cnt = 0;
				
				if(ERROR_DISPLAY_STATUS == Get_System_State_Machine())// && (If_Fault_Recovery_Max() == 0))
				{
					Timing_Clean_Fault_State();
					Lcd_Show();
				}
					
				if(System_is_Initial())
				{
					Initial_State_Handler();
				}
				else if(System_is_Starting())
				{
					Starting_State_Handler();
				}
				else if(System_is_Running())
				{
					Running_State_Handler();
				}
				else if(System_is_Pause())//��ͣ
				{
					Pause_State_Handler();
				}
				else if(System_is_Stop())//����
				{
					Stop_State_Handler();
				}
				else if(System_is_Operation())//�˵�
				{
					Operation_State_Handler();
				}
			}
			
		}
	}
	
	if(LCD_Refresh_Get() == 0)
		Lcd_Show();
}

// ��ʱ�������߳�
void App_Timing_Handler(void)
{
//	static uint16_t restart_cnt=0;
//	
//	if(System_Restart_State == 0xAA)
//	{
//		if(restart_cnt++ > 10)
//		{
//			SysSoftReset();// �����λ
//		}
//	}
	Thread_Activity_Sign_Clean();
	MB_Write_Timer_CallOut();
	
	if(IS_SELF_TEST_MODE())
	{
		if(IS_CHECK_ERROR_MODE())
			System_Self_Checking_Porgram();
		else
			System_Self_Testing_Porgram();
		while((Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_KEY) != 0x7F) || (Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_DIAL_SWITCH) != 0x09))
		{
			Lcd_System_Information();//������ & ����״̬ 6s
			If_System_Is_Error();
			osDelay(THREAD_PERIOD_MAIN_TASK);
		};
			
		//System_Power_Off();
		System_Power_On();
		
		if(IS_CHECK_ERROR_MODE())
			Self_Testing_Check_Comm();
		
		//To_Free_Mode(FREE_MODE_NOT_AUTO_START);
		Arbitrarily_To_Pause();
		
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SYSTEM_SELF_TEST_STATE, 0);
		Write_MbBuffer_Now();
		OUT_SELF_TEST_MODE();
	}
	else
	{
		App_Timing_Task();
	}
}


// �����߳�
void App_Timing_On(void)
{
	
}


// �ر��߳�
void App_Timing_Off(void)
{
	
}


//-------------------- ������ϻָ������� ----------------------------
void Clean_Fault_Recovery_Cnt(void)
{
	 System_Fault_Recovery_Cnt = 0;
}

//-------------------- �ۼƹ��ϻָ������� ----------------------------
void Add_Fault_Recovery_Cnt(uint8_t no)
{
	if(System_Fault_Recovery_Cnt == 0)
		Fault_Recovery_Timing_Cnt = Timing_Half_Second_Cnt; // ���¼�ʱ
	
	System_Fault_Recovery_Cnt += no;
	
	if(System_Fault_Recovery_Cnt >= SYSTEM_FAULT_RECOVERY_MAX)
		Fault_Recovery_Timing_Cnt = 0;
}

//-------------------- ���������� ----------------------------
uint8_t If_Fault_Recovery_Max(void)
{
	if(System_Fault_Recovery_Cnt >= SYSTEM_FAULT_RECOVERY_MAX)
		return 1;
	else
		return 0;
}

//-------------------- ��� �Զ��ػ���ʱ�� ----------------------------
void Clean_Automatic_Shutdown_Timer(void)
{
	 Automatic_Shutdown_Timing_Cnt = Timing_Half_Second_Cnt;
}

//-------------------- ��� �����ٶȼ�ʱ�� ----------------------------
void Clean_Change_Speed_Timer(void)
{
	 Change_Speed_Timing_Cnt = 0;
}

//-------------------- ��� ��Ƶˢ�¼�ʱ�� ----------------------------
void Clean_Temp_Slow_Down_Timer(void)
{
	Temp_Slow_Down_Timing_Cnt = 0;
}

