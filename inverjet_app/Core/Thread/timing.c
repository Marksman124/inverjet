/**
******************************************************************************
* @file				timing.c
* @brief			定时线程  间隔 1 s, 执行倒数和计时工作
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
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
// 通用计数器
static uint8_t Timing_Timer_Cnt = 0;

// 任务计时器
static uint32_t Timing_Thread_Task_Cnt = 0;
static uint32_t Timing_Half_Second_Cnt = 0;
static uint32_t WIFI_Distribution_Timing_Cnt=0;		// wifi状态图标 计时器
static uint32_t BT_Distribution_Timing_Cnt=0;			// 蓝牙状态图标 计时器
static uint32_t System_Fault_Timing_Cnt=0;				// 故障恢复 计时器
static uint8_t System_Fault_Recovery_Cnt=0;				// 故障恢复次数 计数器
static uint8_t Fault_Recovery_Timing_Cnt=0;				// 故障恢复次数 计时器

static uint8_t Change_Speed_Timing_Cnt = 0;				// 更改速度 3秒后开始动作 计时器

static uint32_t Automatic_Shutdown_Timing_Cnt=0;				// 自动关机 计时器

//  高温 降速
static uint32_t Temp_Slow_Down_Timing_Cnt= 0;		//计时器

static uint8_t Temp_Slow_Down_State=0;//			高温降速 标志  bit 1 :mos  bit 2 机箱温度
static uint8_t Temp_Slow_Down_Speed=0;//			高温降速 速度  百分比

//  刷新屏幕
static uint8_t LCD_Refresh_State= 0;

#ifdef SYSTEM_LONG_RUNNING_MODE
//********* 老化工装 ***********************************************
// 老化工装 计时器
static uint32_t Old_Chemical_Equipment_Cnt =0;
//******************************************************************
#endif
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

// 硬件 & 驱动
void App_Timing_Init(void)
{
	
	LCD_Show_Bit = STATUS_BIT_PERCENTAGE;
	
	System_Boot_Screens();
	Dev_Check_Control_Methods();
	System_Power_Off();
	//开机完成
	System_PowerUp_Finish = 0xAA;
}

void Clean_Timing_Timer_Cnt(void)
{
	Timing_Timer_Cnt = 0;
}

// 设置刷新标志
void LCD_Refresh_Set(uint8_t value)
{
	LCD_Refresh_State = value;
}

// 获取刷新标志
uint8_t LCD_Refresh_Get(void)
{
	return LCD_Refresh_State;
}

// 恢复刷新
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
		MB_Flash_Buffer_Write();
		//STMFLASH_Write(FLASH_APP_PARAM_ADDR+(MB_USER_TIME_MODE_SPEED*2), (uint16_t *)p_OP_Timing_Mode, 2 );// REG_HOLDING_NREGS
	}
	else if(system_state == FREE_MODE_INITIAL)
	{
		p_OP_Free_Mode->speed = op_node.speed;
		p_OP_Free_Mode->time = op_node.time;
		MB_Flash_Buffer_Write();
		//STMFLASH_Write(FLASH_APP_PARAM_ADDR+(MB_USER_FREE_MODE_SPEED*2), (uint16_t *)p_OP_Free_Mode, 2 );// REG_HOLDING_NREGS
	}
}

// wifi 状态基    TIMING_THREAD_LIFECYCLE
void WIFI_State_Handler(void)
{
	static uint8_t wifi_state = 0;
	
	if(WIFI_Get_Machine_State() == WIFI_ERROR)
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
	else
	{
		if(Timing_Thread_Task_Cnt >= TIMING_THREAD_HALF_SECOND) //半秒
		{
			if( mcu_get_wifi_work_state() != wifi_state )
			{
				wifi_state = mcu_get_wifi_work_state();
				DEBUG_PRINT("wifi状态: %d\n",wifi_state);
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
				
				//超时配网恢复
		//		if( (Timing_Half_Second_Cnt - WIFI_Distribution_Timing_Cnt) > WIFI_DISTRIBUTION_TIME_CALLOUT)
		//		{
		//			WIFI_Distribution_Timing_Cnt = 0;
		//			WIFI_Set_Machine_State(WIFI_NO_CONNECT);
		//			LCD_Show_Bit &= ~STATUS_BIT_WIFI;
		//		}
			}
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

// 蓝牙 状态基  TIMING_THREAD_LIFECYCLE
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
		if(Timing_Thread_Task_Cnt >= TIMING_THREAD_HALF_SECOND) //半秒
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
				
				//超时配网恢复
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



// 清除 故障 状态基
void CallOut_Fault_State(void)
{
	Clean_Fault_State();
	System_Fault_Timing_Cnt = 0;
	Check_Motor_Current_Cnt = 0;
	
	System_Wifi_State_Clean();
	Lcd_Show();
}

// 故障 状态基  1秒进一次
void Fault_State_Handler(void)
{
	if(ERROR_DISPLAY_STATUS != Get_System_State_Machine())
	{
		To_Fault_Menu();
	}
	else if(System_is_Error())//故障 2min恢复 非通讯故障
	{
		if((If_Fault_Recovery_Max()==0)&&(Get_Motor_Fault_State() != FAULT_MOTOR_LOSS))
		{
			// 3次以内 退出故障, 超过3次只能重启
			if((Timing_Half_Second_Cnt - Fault_Recovery_Timing_Cnt) > SYSTEM_FAULT_RECOVERY_TIME)
			{
				DEBUG_PRINT("故障恢复计时超过1小时,清除计数器:\t%d\n",System_Fault_Recovery_Cnt);
				//超过 1 小时 清除计数器
				Clean_Fault_Recovery_Cnt();
				Fault_Recovery_Timing_Cnt = Timing_Half_Second_Cnt; // 重新计时
			}

			if(( System_Fault_Timing_Cnt == 0)||(System_Fault_Timing_Cnt > Timing_Half_Second_Cnt))
			{
				System_Fault_Timing_Cnt = Timing_Half_Second_Cnt;
				if( Fault_Recovery_Timing_Cnt == 0)
				{
					Fault_Recovery_Timing_Cnt = Timing_Half_Second_Cnt;
				}
			}
			
			//超时 故障 恢复
			if( (Timing_Half_Second_Cnt - System_Fault_Timing_Cnt) > SYSTEM_FAULT_TIME_CALLOUT)
			{
				System_Fault_Recovery_Cnt++;
				CallOut_Fault_State();
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

// 软启动 状态基  1秒进一次
void Starting_State_Handler(void)
{
	all_data_update();		// wifi 上传
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

// 运行 状态基  1秒进一次
void Running_State_Handler(void)
{
	uint8_t slow_down_speed;//高温降速使用
	
	if(*p_System_State_Machine == FREE_MODE_RUNNING)									// 自由
	{
		*p_OP_ShowNow_Time = (*p_OP_ShowNow_Time + 1);
		if(*p_OP_ShowNow_Time >= 6000)
		{
			*p_OP_ShowNow_Time = 0;
		}
	}
	else if(*p_System_State_Machine == TIMING_MODE_RUNNING)					// 定时
	{
		*p_OP_ShowNow_Time = (*p_OP_ShowNow_Time - 1);
		Lcd_Show();
		if(*p_OP_ShowNow_Time == 0)
		{
			Arbitrarily_To_Stop();
			*p_OP_ShowNow_Time = p_OP_Timing_Mode->time;
		}
	}
	else if(*p_System_State_Machine == TRAINING_MODE_RUNNING)					// 训练
	{
		*p_OP_ShowNow_Time = (*p_OP_ShowNow_Time + 1);
		if(*p_PMode_Now == SURFING_MODE_NUMBER_ID)//冲浪
		{
			if(*p_OP_ShowNow_Time >= 6000)
			{
				*p_OP_ShowNow_Time = 0;
			}
			
			if((*p_OP_ShowNow_Time < 10))
				Data_Set_Current_Speed(30);//注意,需要在切完运行状态后再设置速度,如"暂停"
			else if(((*p_OP_ShowNow_Time-10) % (Temp_Data_P5_100_Time+Temp_Data_P5_0_Time)) == 0)
				Data_Set_Current_Speed(100);//注意,需要在切完运行状态后再设置速度,如"暂停"
			else if(((*p_OP_ShowNow_Time-10) % (Temp_Data_P5_100_Time+Temp_Data_P5_0_Time)) == Temp_Data_P5_100_Time)
				Data_Set_Current_Speed(30);//注意,需要在切完运行状态后再设置速度,如"暂停"
		}
		else if(Is_Mode_Legal(*p_PMode_Now))
		{
			if(*p_OP_ShowNow_Time >= p_OP_PMode[*p_PMode_Now-1][Period_Now].time)
			{
				if((Period_Now == (TRAINING_MODE_PERIOD_MAX-1))||(p_OP_PMode[*p_PMode_Now-1][Period_Now+1].time < p_OP_PMode[*p_PMode_Now-1][Period_Now].time))
				{
					Arbitrarily_To_Stop();
					*p_OP_ShowNow_Speed = p_OP_PMode[*p_PMode_Now-1][0].speed;
				}
				else
				{
					Period_Now ++;
					Data_Set_Current_Speed(p_OP_PMode[*p_PMode_Now-1][Period_Now].speed);//注意,需要在切完运行状态后再设置速度,如"暂停"
				}
			}
		}
	}
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_CHANGE))
	{
		Change_Speed_Timing_Cnt ++;
		if(Change_Speed_Timing_Cnt >= 3)
		{
			Change_Speed_Timing_Cnt = 0;
			Special_Status_Delete(SPECIAL_BIT_SPEED_CHANGE);
			Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
		}
	}
	// 计算 模拟 距离
	Calculate_Swimming_Distance();
	// 转速达到目标值
	if(Motor_Speed_Is_Reach())
	{
		// 光圈 常亮
		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		{
			Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
		}
	}
	//降频
	if(Get_Temp_Slow_Down_State())
	{
		Temp_Slow_Down_Timing_Cnt ++;
		if(( Temp_Slow_Down_Timing_Cnt == 1 ) ||((Temp_Slow_Down_Timing_Cnt % TIME_SLOW_DOWN_TIME)==0))
		{
			if( Temp_Slow_Down_Timing_Cnt == 1 )
				slow_down_speed = TIME_SLOW_DOWN_SPEED_01;
			else
				slow_down_speed = TIME_SLOW_DOWN_SPEED_02;
			
			if( Motor_Speed_Target_Get()  >= (slow_down_speed + TIME_SLOW_DOWN_SPEED_MIX))
			{
				Temp_Slow_Down_Speed += slow_down_speed;
				Data_Set_Current_Speed(Motor_Speed_Target_Get() - slow_down_speed);
			}
		}
		
	}
	/*else if(Temp_Slow_Down_Timing_Cnt > 0) // 缓慢恢复
	{

		//高温 恢复
		if(Motor_Speed_Target_Get() >= TIME_SLOW_DOWN_SPEED_MAX)
		{
			slow_down_speed = 0;
			Temp_Slow_Down_Speed = 0;
			Temp_Slow_Down_Timing_Cnt = 0;//可提前退出
		}
				
		if(( Temp_Slow_Down_Timing_Cnt == 1 ) ||((Temp_Slow_Down_Timing_Cnt % TIME_SLOW_DOWN_TIME)==0))
		{
			if( Temp_Slow_Down_Timing_Cnt == 1 )
				slow_down_speed = TIME_SLOW_DOWN_SPEED_01;
			else
				slow_down_speed = TIME_SLOW_DOWN_SPEED_02;
			
			if(Temp_Slow_Down_Speed >= slow_down_speed)
			{
				Temp_Slow_Down_Speed -= slow_down_speed;
				
				Data_Set_Current_Speed(Motor_Speed_Target_Get() + slow_down_speed);
			}
			else if(Temp_Slow_Down_Speed == 0)
				Temp_Slow_Down_Timing_Cnt = 0;//可提前退出
			else
			{
				Data_Set_Current_Speed(Motor_Speed_Target_Get() + Temp_Slow_Down_Speed);
				Temp_Slow_Down_Speed = 0;
			}
				
		}
		
		Temp_Slow_Down_Timing_Cnt --;
	}*/
	else //直接恢复
	{
		if(Temp_Slow_Down_Timing_Cnt > 0)
		{
			Data_Set_Current_Speed(Motor_Speed_Target_Get() + Temp_Slow_Down_Speed);
			Temp_Slow_Down_Speed = 0;
			Temp_Slow_Down_Timing_Cnt = 0;//可提前退出
		}
	}
		
	//减速界面 2秒1刷
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

// 暂停 状态基  1秒进一次
void Pause_State_Handler(void)
{
	if(*p_OP_ShowNow_Speed != 0)
		Data_Set_Current_Speed(0);//注意,需要在切完运行状态后再设置速度,如"暂停"
	
	if(((Timing_Half_Second_Cnt - Automatic_Shutdown_Timing_Cnt) > AUTOMATIC_SHUTDOWN_TIME)&&(Timing_Half_Second_Cnt > Automatic_Shutdown_Timing_Cnt))
	{
		System_Power_Off();
	}
	
	if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
}
// 菜单 状态基  0.5秒进一次
void Operation_State_Handler(void)
{
	Timing_Timer_Cnt++;
	// 3秒 闪烁
	if(Timing_Timer_Cnt > 60)
	{
		//保存 flash
		Memset_OPMode();//存flash
		System_Power_Off();
		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
			Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
	}
}
// 停止 状态基  1秒进一次
void Stop_State_Handler(void)
{
	Timing_Timer_Cnt++;
	// 3秒 闪烁
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
		// 返回 自由模式 初始状态
		To_Free_Mode(1);
		Lcd_Show();
		Timing_Timer_Cnt = 0;
	}
	if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))
		Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
}

// 初始 状态基  1秒进一次
void Initial_State_Handler(void)
{
	if(Special_Status_Get( SPECIAL_BIT_SKIP_INITIAL))// 跳过 自动启动
	{
		if(((Timing_Half_Second_Cnt - Automatic_Shutdown_Timing_Cnt) > AUTOMATIC_SHUTDOWN_TIME) && (Timing_Half_Second_Cnt > Automatic_Shutdown_Timing_Cnt))
		{
			System_Power_Off();
		}
		return;
	}
	
	// 3秒 闪烁
	if(Timing_Timer_Cnt < 2)
	{
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
	}
	else
	{
		LCD_Refresh_Set(0);//恢复刷新
		
		Add_Ctrl_Log();
		
		if(*p_System_State_Machine == TIMING_MODE_INITIAL)
		{
			p_OP_ShowLater->time = *p_OP_ShowNow_Time;
		}
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//光圈自动判断
		
		
//		if(Special_Status_Get(SPECIAL_BIT_SKIP_STARTING))	//跳过 软启动
		{
			//Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING); //底层转速同步后再删除
			p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
			
			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
			
			//保存
			
			Lcd_Show();
			Arbitrarily_To_Running();
		}
//		else
//		{
//			// 进入 软启动
//			Arbitrarily_To_Starting();
//			//*p_OP_ShowLater->time = *p_OP_ShowNow_Time;
//			*p_OP_ShowNow_Time = 20;
//			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
//			Lcd_Show();
//		}
		Timing_Timer_Cnt = 0;
	}
	
	Timing_Timer_Cnt++;
}

// 定时任务主线程
void App_Timing_Task(void)
{
	if(System_is_Power_Off())
	{
		Timing_Thread_Task_Cnt++;		
		if(Timing_Thread_Task_Cnt >= (TIMING_THREAD_HALF_SECOND*2)) //半秒
		{
			Timing_Thread_Task_Cnt = 0;
			*p_System_Runing_Second_Cnt += 1;			// 运行时间
			*p_No_Operation_Second_Cnt += 1;			// 无操作时间
			*p_System_Startup_Second_Cnt = 0;			// 启动时间
#ifdef SYSTEM_LONG_RUNNING_MODE
		//********* 老化工装 ***********************************************
		// 老化工装 2h关  7200
			Old_Chemical_Equipment_Cnt++;
			if(Old_Chemical_Equipment_Cnt > 7200)
			{
				Buzzer_Click_Long_On(1);
				Old_Chemical_Equipment_Cnt = 0;
				System_Power_On();
			}
		//******************************************************************
#endif
		}
		return;
	}
	
	Timing_Thread_Task_Cnt++;
	
	WIFI_State_Handler();
	BT_State_Handler();
	
	if(Timing_Thread_Task_Cnt >= TIMING_THREAD_HALF_SECOND) //半秒
	{
		Timing_Thread_Task_Cnt = 0;
		Timing_Half_Second_Cnt ++;

		static uint8_t half_second_state=0;
		if(half_second_state == 1)
		{
			half_second_state = 0;
			*p_System_Runing_Second_Cnt += 1;			// 运行时间
			*p_No_Operation_Second_Cnt += 1;			// 无操作时间
			*p_System_Startup_Second_Cnt += 1;		// 休眠时间
			
#ifdef SYSTEM_LONG_RUNNING_MODE
			//********* 老化工装 ***********************************************
			// 老化工装 4h开  14400
			Old_Chemical_Equipment_Cnt ++;
			if(Old_Chemical_Equipment_Cnt > 14400)
			{
				Buzzer_Click_Long_On(1);
				Old_Chemical_Equipment_Cnt = 0;
				System_Power_Off();
			}
			//******************************************************************
#endif
			
			// 时间 : 闪烁  半秒
			if(System_is_Normal_Operation())
			{
				LCD_Show_Bit |= STATUS_BIT_COLON;
				TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		1);
				TM1621_LCD_Redraw();
			}
		}
		else
		{
			half_second_state = 1;
			if(Motor_is_Start()==0)
			{
				// 时间 : 闪烁  半秒
				LCD_Show_Bit &= ~STATUS_BIT_COLON;
				TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		0);
				TM1621_LCD_Redraw();
			}
			//故障检测
			if(If_System_Is_Error())
			{
				Fault_State_Handler();
			}
			else
			{
				System_Fault_Timing_Cnt = 0;
				
				if(ERROR_DISPLAY_STATUS == Get_System_State_Machine())// && (If_Fault_Recovery_Max() == 0))
				{
					Clean_Fault_State();
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
				else if(System_is_Pause())//暂停
				{
					Pause_State_Handler();
				}
				else if(System_is_Stop())//结束
				{
					Stop_State_Handler();
				}
				else if(System_is_Operation())//菜单
				{
					Operation_State_Handler();
				}
			}
			
		}
	}
	
	if(LCD_Refresh_Get()== 1)
		Lcd_Speed_Off();
	else
		Lcd_Show();
}

// 定时任务主线程
void App_Timing_Handler(void)
{
//	static uint16_t restart_cnt=0;
//	
//	if(System_Restart_State == 0xAA)
//	{
//		if(restart_cnt++ > 10)
//		{
//			SysSoftReset();// 软件复位
//		}
//	}
	
	if(System_Self_Testing_State == 0xAA)
	{
		System_Self_Testing_Porgram();
		//System_Power_Off();
		System_Power_On();
		System_Self_Testing_State = 0;
	}
	else
	{
		App_Timing_Task();
	}
}


// 开启线程
void App_Timing_On(void)
{
	
}


// 关闭线程
void App_Timing_Off(void)
{
	
}



//-------------------- 设置高温降速模式 0:关   1开 ----------------------------
void Set_Temp_Slow_Down_State(uint8_t vaule)
{
	Temp_Slow_Down_State = vaule;
}

//-------------------- 获取高温降速模式 ----------------------------
uint8_t Get_Temp_Slow_Down_State(void)
{
	return Temp_Slow_Down_State;
}

//-------------------- 设置高温降速 速度 下降的速度 ----------------------------
void Set_Temp_Slow_Down_Speed(uint8_t vaule)
{
	Temp_Slow_Down_Speed = vaule;
}

//-------------------- 获取高温降速 速度 ----------------------------
uint8_t Get_Temp_Slow_Down_Speed(void)
{
	return Temp_Slow_Down_Speed;
}

//-------------------- 清除故障恢复计数器 ----------------------------
void Clean_Fault_Recovery_Cnt(void)
{
	 System_Fault_Recovery_Cnt = 0;
}

//-------------------- 累计故障恢复计数器 ----------------------------
void Add_Fault_Recovery_Cnt(void)
{
	 System_Fault_Recovery_Cnt ++;
}

//-------------------- 超过最大次数 ----------------------------
uint8_t If_Fault_Recovery_Max(void)
{
	if(System_Fault_Recovery_Cnt >= (SYSTEM_FAULT_RECOVERY_MAX-1))
		return 1;
	else
		return 0;
}

//-------------------- 清除 自动关机计时器 ----------------------------
void Clean_Automatic_Shutdown_Timer(void)
{
	 Automatic_Shutdown_Timing_Cnt = Timing_Half_Second_Cnt;
}

//-------------------- 清除 更改速度计时器 ----------------------------
void Clean_Change_Speed_Timer(void)
{
	 Change_Speed_Timing_Cnt = 0;
}

