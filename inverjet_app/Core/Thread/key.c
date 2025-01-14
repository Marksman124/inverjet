/**
******************************************************************************
* @file				key.c
* @brief			Key function implementation
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "tm1621.h"
#include "data.h"
#include "timing.h"
#include "display.h"
#include "Breath_light.h"
#include "debug_protocol.h"
#include "wifi.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

#define KEY_IO_NUMBER_MAX			4

//------------------- 按键 & 引脚 ----------------------------
#define KEY_SPEED_IO_PORT			GPIOC
#define KEY_SPEED_IO_PIN			GPIO_PIN_7

#define KEY_TIME_IO_PORT			GPIOC
#define KEY_TIME_IO_PIN				GPIO_PIN_8

#define KEY_TRAIN_IO_PORT			GPIOC
#define KEY_TRAIN_IO_PIN			GPIO_PIN_9

#define KEY_POWER_IO_PORT			GPIOA
#define KEY_POWER_IO_PIN			GPIO_PIN_8


//------------------- led & 引脚 ----------------------------
#define LED_SPEED_IO_PORT			GPIOC
#define LED_SPEED_IO_PIN			GPIO_PIN_3

#define LED_TIME_IO_PORT			GPIOC
#define LED_TIME_IO_PIN				GPIO_PIN_2

#define LED_TRAIN_IO_PORT			GPIOC
#define LED_TRAIN_IO_PIN			GPIO_PIN_1

#define LED_POWER_IO_PORT			GPIOC//GPIOA
#define LED_POWER_IO_PIN			GPIO_PIN_0//GPIO_PIN_1

/* Private variables ---------------------------------------------------------*/

// 按键对应 引脚
IO_Hardware_Pin Key_Gpio_Pin_Array[KEY_IO_NUMBER_MAX] = {
	{ KEY_SPEED_IO_PORT, KEY_SPEED_IO_PIN }, 
	{ KEY_TIME_IO_PORT, 	KEY_TIME_IO_PIN }, 
	{ KEY_TRAIN_IO_PORT, KEY_TRAIN_IO_PIN }, 
	{ KEY_POWER_IO_PORT, KEY_POWER_IO_PIN }
};

// 按键灯对应 引脚
uint8_t Key_IO_Ordering_Value[KEY_CALL_OUT_NUMBER_MAX]={
	KEY_VALUE_BIT_BUTTON_1,
	KEY_VALUE_BIT_BUTTON_2,
	KEY_VALUE_BIT_BUTTON_3,
	KEY_VALUE_BIT_BUTTON_4,
	KEY_VALUE_BIT_BUTTON_1|KEY_VALUE_BIT_BUTTON_2,
	KEY_VALUE_BIT_BUTTON_1|KEY_VALUE_BIT_BUTTON_3,
	KEY_VALUE_BIT_BUTTON_2|KEY_VALUE_BIT_BUTTON_3,
	KEY_VALUE_BIT_BUTTON_2|KEY_VALUE_BIT_BUTTON_4,
};

// 短按 槽函数
void (*p_Funtion_Button[KEY_CALL_OUT_NUMBER_MAX])(void) = {
	on_pushButton_clicked,  on_pushButton_2_clicked,  on_pushButton_3_clicked,  on_pushButton_4_Short_Press,
	on_pushButton_1_2_Short_Press, on_pushButton_1_3_Short_Press, 
	on_pushButton_2_3_Short_Press, on_pushButton_2_4_Short_Press,
};

// 长按 槽函数
void (*p_Funtion_Long_Press[KEY_CALL_OUT_NUMBER_MAX])(void) = {
	on_pushButton_1_Long_Press,  on_pushButton_2_Long_Press,  on_pushButton_3_Long_Press,  on_pushButton_4_Long_Press,
	on_pushButton_1_2_Long_Press, on_pushButton_1_3_Long_Press,
	on_pushButton_2_3_Long_Press, on_pushButton_2_4_Long_Press,
};

// 各槽 长按判断 时长
uint32_t Key_Long_Press_Confirm_Value[KEY_CALL_OUT_NUMBER_MAX]={
	KEY_LONG_PRESS_TIME_1S, KEY_LONG_PRESS_TIME_2S, KEY_LONG_PRESS_TIME_2S, KEY_LONG_PRESS_TIME_2S,
	KEY_LONG_PRESS_TIME_2S, KEY_LONG_PRESS_TIME_2S,KEY_LONG_PRESS_TIME_2S,KEY_LONG_PRESS_TIME_2S,
};



//------------------- 变量 声明 ----------------------------
uint8_t Key_IO_Hardware = 0;		// 读取key 值
uint8_t Key_IO_Old = 0;					// 上一次 key 值


uint8_t Key_Multiple_Clicks_cnt = 0;				//8 次连击计数
uint8_t Key_Multiple_Clicks_Old = 0;		// 上一次 key 值
uint32_t Key_Multiple_Clicks_time = 0;		// 第一次 key 时间
uint32_t Key_Handler_Timer=0;

uint32_t Key_For_Sleep_time = 0;		// 睡眠时间

uint32_t Key_Long_Press_cnt[KEY_CALL_OUT_NUMBER_MAX]={0};	// 长按 计数器

uint8_t System_Self_Testing_State;

uint8_t Key_Buzzer_cnt = 0; //蜂鸣器计时
uint8_t Key_Buzzer_Type = 0;	//蜂鸣器长短 类型
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
/**********************************************************************************************
*
*						按键回调    短按
*
**********************************************************************************************/
//================================== ① 档位键
void on_pushButton_clicked(void)
{
	if((System_is_Power_Off()) || System_is_Pause() || System_is_Stop() ||  System_Mode_Surf())
			return;
		
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
	{
		if(*p_OP_ShowNow_Speed >= MOTOR_PERCENT_SPEED_MAX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MIX;
		else
			*p_OP_ShowNow_Speed += KEY_SPEED_INCREASE_100_GEAR;
		
		if(Motor_is_Start())
		{
			Special_Status_Add(SPECIAL_BIT_SPEED_CHANGE);
			Clean_Change_Speed_Timer();
		}
		else
			Arbitrarily_To_Initial();
		
		Update_OP_Speed();
		//Lcd_Show();
	}
	else
	{
		if((*p_OP_ShowNow_Speed % MOTOR_PERCENT_SPEED_MIX) != 0)
			*p_OP_ShowNow_Speed += (MOTOR_PERCENT_SPEED_MIX-(*p_OP_ShowNow_Speed % MOTOR_PERCENT_SPEED_MIX));
		else
			*p_OP_ShowNow_Speed += KEY_SPEED_INCREASE_20_GEAR;
		
		if(*p_OP_ShowNow_Speed > MOTOR_PERCENT_SPEED_MAX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MIX;
		if(Motor_is_Start())
		{
			Special_Status_Add(SPECIAL_BIT_SPEED_CHANGE);
			Clean_Change_Speed_Timer();
		}
		else
			Arbitrarily_To_Initial();
		
		Update_OP_Speed();
		//Lcd_Show();
	}
}

//================================== ② 时间键
void on_pushButton_2_clicked(void)
{
	if(System_is_Power_Off())
		return;
	//Clean_Swimming_Distance();//清除计算距离
	Clean_Timing_Timer_Cnt();
	
	if(Get_System_State_Machine() == TIMING_MODE_INITIAL)
	{
			if(*p_OP_ShowNow_Time >= MOTOR_TIME_GEAR_MAX)
					*p_OP_ShowNow_Time = MOTOR_TIME_GEAR_MIX;
			else
			{
					if((*p_OP_ShowNow_Time % MOTOR_TIME_GEAR_OFFSET) != 0)
							*p_OP_ShowNow_Time += (MOTOR_TIME_GEAR_OFFSET-(*p_OP_ShowNow_Time % MOTOR_TIME_GEAR_OFFSET));
					else
							*p_OP_ShowNow_Time += MOTOR_TIME_GEAR_OFFSET;
			}
			
			Update_OP_Time();
			//Lcd_Show();
	}
	else
	{
			To_Timing_Mode();
	}
	Arbitrarily_To_Initial();
	
//	if(Get_System_State_Machine() <= TIMING_MODE_STOP)	// 定时
//	{
//		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
//	}
}

//================================== ③ 模式键
void on_pushButton_3_clicked(void)
{
	if(System_is_Power_Off())
		return;
	//Clean_Swimming_Distance();//清除计算距离
		
	if(System_Mode_Free())
	{
			To_Train_Mode(1);
	}
	else if(System_Mode_Train())
	{
			if(Get_System_State_Mode() >= TRAINING_MODE_NUMBER_MAX)
			{
					To_Free_Mode(FREE_MODE_AUTO_START);
			}
			else
			{
					To_Train_Mode((Get_System_State_Mode()+1));
			}
	}
	else
	{
			To_Free_Mode(FREE_MODE_AUTO_START);
	}
}

//================================== ④ 开机键  短按
void on_pushButton_4_Short_Press(void)
{
	if(System_is_Power_Off())//关机中 执行开机
	{
			return;
	}

	if(System_is_Initial() && (Special_Status_Get(SPECIAL_BIT_SKIP_INITIAL)))	// 初始 --> 立即启动
	{
			Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
			Arbitrarily_To_Running();
			Data_Set_Current_Speed(p_OP_ShowLater->speed);//注意,需要在切完运行状态后再设置速度,如"启动"
	}
	else if(System_is_Pause())	// 暂停 --> 启动
	{
		if(p_OP_ShowLater->speed == 0)
			DEBUG_PRINT("\n\n\np_OP_ShowLater->speed 错误 *p_OP_ShowNow_Speed = %d\n",*p_OP_ShowNow_Speed);
		
		Arbitrarily_To_Running();
		Data_Set_Current_Speed(p_OP_ShowLater->speed);//注意,需要在切完运行状态后再设置速度,如"启动"
	}
	else	// 其它 --> 暂停
	{
		if((*p_OP_ShowNow_Speed == 0)&&(p_OP_ShowLater->speed > 0))
		{
			DEBUG_PRINT("\n\n\np_OP_ShowNow_Speed 错误 p_OP_ShowLater->speed = %d\n",p_OP_ShowLater->speed);
		}
		else
		{
			p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
			*p_OP_ShowNow_Speed = 0;
			Data_Set_Current_Speed(0);//注意,需要在切完运行状态后再设置速度,如"启动"
		}
		Arbitrarily_To_Pause();
	}
	
	//*p_OP_ShowNow_Time = 20;
	//Lcd_Show();
}

//================================== ① + ②  组合键  测试 设置故障
void on_pushButton_1_2_Short_Press(void)
{
	//Set_Fault_Data(0xAC);
}

//================================== ① + ③  组合键  短按   切换档位 100级 or 5级
void on_pushButton_1_3_Short_Press(void)
{
//	if(System_is_Power_Off())
//		return;
//	
//	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
//		Special_Status_Delete(SPECIAL_BIT_SPEED_100_GEAR);
//	else
//		Special_Status_Add(SPECIAL_BIT_SPEED_100_GEAR);
}

//================================== ② + ③  组合键  恢复出厂设置
void on_pushButton_2_3_Short_Press(void)
{
	//Restore_Factory_Settings();
}

//================================== ② + ④  组合键  进入操作菜单
void on_pushButton_2_4_Short_Press(void)
{
	//To_Operation_Menu();
}



/**********************************************************************************************
*
*						按键回调    长按
*
**********************************************************************************************/
//================================== ① 档位键
void on_pushButton_1_Long_Press(void)
{
	static uint8_t pushButton_1_delay_cnt = 0;
	//刷太快了  20ms
	if(pushButton_1_delay_cnt++ < KEY_LONG_PRESS_STEP)
		return;
	pushButton_1_delay_cnt = 0;
	
	if((System_is_Power_Off()) || System_is_Pause() || System_is_Stop() ||  System_Mode_Surf())
			return;
//	if(Key_Long_Press_cnt[0] == KEY_LONG_PRESS_TIME_2S)
//	{
//		//长按可设 高精度转速
//		if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR) == 0)
//			Special_Status_Add(SPECIAL_BIT_SPEED_100_GEAR);
//	}
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
	{
		if(*p_OP_ShowNow_Speed >= MOTOR_PERCENT_SPEED_MAX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MIX;
		else
			*p_OP_ShowNow_Speed += KEY_SPEED_INCREASE_100_GEAR;

		Arbitrarily_To_Initial();
		//Lcd_Show();
	}

}
//================================== ② 时间键
void on_pushButton_2_Long_Press(void)
{
	//Set_Fault_Data(0);
}
//================================== ③ 模式键
void on_pushButton_3_Long_Press(void)
{
}
//================================== ④ 开机键  短按
void on_pushButton_4_Long_Press(void)
{
	Buzzer_Click_Long_On(1);
	
	if(System_is_Power_Off())//关机中 执行开机
	{
			System_Power_On();
	}
	else
	{
    System_Power_Off();
	}
	
	//Key_Long_Press_cnt[3] = 0;
	
}

//================================== ① + ②  组合键
//   wifi配对
void on_pushButton_1_2_Long_Press(void)
{
	//if(Motor_is_Start() ==0)
	{
		Buzzer_Click_On();
		WIFI_Get_In_Distribution();
	}
}

//================================== ① + ③  组合键
void on_pushButton_1_3_Long_Press(void)
{
	if(System_is_Power_Off())
		return;
	
	Buzzer_Click_On();
	
	if(Special_Status_Get(SPECIAL_BIT_SPEED_100_GEAR))
		Special_Status_Delete(SPECIAL_BIT_SPEED_100_GEAR);
	else
		Special_Status_Add(SPECIAL_BIT_SPEED_100_GEAR);
	
}
//================================== ② + ③  组合键
//   蓝牙配对
void on_pushButton_2_3_Long_Press(void)
{
//	if(System_is_Power_Off())
//	{
//		To_Operation_Menu();
//	}
	//if(Motor_is_Start() ==0)
	{
		Buzzer_Click_On();
		BT_Get_In_Distribution();
	}
}
//================================== ② + ④  组合键
//  关机下   恢复出厂
void on_pushButton_2_4_Long_Press(void)
{
//	if(System_is_Power_Off())
//	{
//		Restore_Factory_Settings();
//	}
}

//***************************************************************************************************


//------------------- 硬件 & 驱动 ----------------------------
// 初始化
void App_Key_Init(void)
{
	Led_Button_On(0x0F);	// 按键
	Buzzer_Click_Long_On(1);// 开机长响
	
//	System_Boot_Screens();
//	System_Power_Off();
	
}

//------------------- 按键灯 ----------------------------
// 按键灯
void Led_Button_On(uint8_t para)
{
	
	if(para & 1<<0)
		HAL_GPIO_WritePin(LED_SPEED_IO_PORT, LED_SPEED_IO_PIN, GPIO_PIN_SET);		// 1
	else
		HAL_GPIO_WritePin(LED_SPEED_IO_PORT, LED_SPEED_IO_PIN, GPIO_PIN_RESET);	// 0	
	
	if(para & 1<<1)
		HAL_GPIO_WritePin(LED_TIME_IO_PORT, LED_TIME_IO_PIN, GPIO_PIN_SET);		// 1
	else
		HAL_GPIO_WritePin(LED_TIME_IO_PORT, LED_TIME_IO_PIN, GPIO_PIN_RESET);	// 0	
	
	if(para & 1<<2)
		HAL_GPIO_WritePin(LED_TRAIN_IO_PORT, LED_TRAIN_IO_PIN, GPIO_PIN_SET);		// 1
	else
		HAL_GPIO_WritePin(LED_TRAIN_IO_PORT, LED_TRAIN_IO_PIN, GPIO_PIN_RESET);	// 0	
	
	if(para & 1<<3)
		HAL_GPIO_WritePin(LED_POWER_IO_PORT, LED_POWER_IO_PIN, GPIO_PIN_SET);		// 1
	else
		HAL_GPIO_WritePin(LED_POWER_IO_PORT, LED_POWER_IO_PIN, GPIO_PIN_RESET);	// 0
	
	
}
/**********************************************************************************************
*
*						特殊按键规则
*
**********************************************************************************************/
void Special_Button_Rules(uint8_t key_value)
{
	if(Get_Upgradation_Static() == UPDATE_START_CMD)
		return;
	
	//关机下 计数 8次
	if( Key_Multiple_Clicks_Old != key_value)
	{
		Key_Multiple_Clicks_cnt = 1;
		Key_Multiple_Clicks_Old = key_value;
		Key_Multiple_Clicks_time =  Key_Handler_Timer;
	}

	if( ( Key_Handler_Timer - Key_Multiple_Clicks_time ) <= (KEY_MULTIPLE_CLICKS_TIME/KEY_THREAD_LIFECYCLE))
	{
		if(Key_Multiple_Clicks_cnt >= KEY_MULTIPLE_CLICKS_MAX)
		{
			// 自测
			if(key_value == KEY_VALUE_BIT_BUTTON_1)
			{
				IN_SELF_TEST_MODE();
			}
			// 菜单
			else if(key_value == KEY_VALUE_BIT_BUTTON_2)
			{
				Buzzer_Click_Long_On(1);
				To_Operation_Menu();
			}
			// 恢复出厂
			else if(key_value == KEY_VALUE_BIT_BUTTON_3)
			{
				//Buzzer_Click_Long_On(1);
				Restore_Factory_Settings();
			}
		}
	}
	else
	{
		Key_Multiple_Clicks_cnt = 1;
		Key_Multiple_Clicks_time =  Key_Handler_Timer;
	}
	
	Key_Multiple_Clicks_cnt ++;
}

void Buzzer_Click_On(void)
{
	if(System_is_Power_Off())
		return;
	
	Key_Buzzer_cnt = 1;
	Key_Buzzer_Type = 0;
}


void Buzzer_Click_Long_On(uint8_t type)
{
	Key_Buzzer_Type = type;
	Key_Buzzer_cnt = 1;
}

void Buzzer_Click_Handler(void)
{
	if(Key_Buzzer_cnt == 1)
		{
			TM1621_Buzzer_Off();
		}
		else if(Key_Buzzer_cnt == 2)
		{
			TM1621_Buzzer_Click();
		}
		else if(Key_Buzzer_cnt > KEY_BUZZER_TIME)
		{
			if(Key_Buzzer_Type == 0)
			{
				TM1621_Buzzer_Off();
				Key_Buzzer_cnt = 0;
			}
			else if(Key_Buzzer_Type == 1)
			{
				if(Key_Buzzer_cnt > KEY_BUZZER_TIME_LONG)
				{
					TM1621_Buzzer_Off();
					Key_Buzzer_cnt = 0;
					Key_Buzzer_Type = 0;
				}
			}
			else if(Key_Buzzer_Type == 2)
			{
				if(Key_Buzzer_cnt > KEY_BUZZER_TIME_LONG_32)
				{
					TM1621_Buzzer_Off();
					Key_Buzzer_cnt = 0;
					Key_Buzzer_Type = 0;
				}
			}
		}
		
		if(Key_Buzzer_cnt > 0)
			Key_Buzzer_cnt++;
		else
			TM1621_Buzzer_Off();
}


// 按键主循环任务
//  20 ms
void App_Key_Task(void)
{
	//static uint8_t self_test_cnt=0;
		
	uint8_t i;	
	Key_Handler_Timer ++;
	
	if(System_PowerUp_Finish == 0)
		return;
	
		//进入睡眠
		if(Key_Handler_Timer > (Key_For_Sleep_time + KEY_FOR_SLEEP_TIME_SHORT))
		{
			TM1621_Set_light_Mode(1);
		}
		
		if(Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER,MB_SYSTEM_SELF_TEST_STATE) == 0xAA )
		{
			if(IS_SELF_TEST_MODE() == 0)
			{
				IN_CHECK_ERROR_MODE();
			}
		}
		
		for(i=0; i<KEY_CALL_OUT_NUMBER_MAX; i++)
		{
			if(Key_IO_Hardware == Key_IO_Ordering_Value[i])
			{
				*p_No_Operation_Second_Cnt = 0;
				Key_For_Sleep_time = Key_Handler_Timer;// 睡眠计时
				TM1621_Set_light_Mode(0);
				
				Key_Long_Press_cnt[i]++;
				
				if(Key_Long_Press_cnt[i] == KEY_LONG_PRESS_TIME_2S)//长按
				{
					Buzzer_Click_On();
					
					if((Key_IO_Hardware == KEY_VALUE_BIT_BUTTON_1)||(Key_IO_Hardware == KEY_VALUE_BIT_BUTTON_2))
						Key_Long_Press_cnt[i] --;
				
					//测试发送串口
					DEBUG_PRINT("[按键长按]: %d\n",Key_IO_Ordering_Value[i]);
					
					if(System_is_Operation())
						p_Operation_Long_Press[i]();
					else if(System_is_Error())
						p_Fault_Long_Press[i]();
					else
						p_Funtion_Long_Press[i]();
				}
			}
			else if(Key_IO_Hardware == 0)
			{
				if(Key_IO_Old == Key_IO_Ordering_Value[i])//已经按下
				{
					if(Key_Long_Press_cnt[i] >= KEY_LONG_PRESS_TIME_2S)//
					{
						//测试发送串口
						DEBUG_PRINT("[按键短按]: %d\n",Key_IO_Ordering_Value[i]);
					}
					else
					{
						Buzzer_Click_On();
						//测试发送串口
						DEBUG_PRINT("[按键点击]: %d\n",i);
						
						if(System_is_Operation())
							p_Operation_Button[i]();
						else if(System_is_Error())
							p_Fault_Button[i]();
						else
						{
							p_Funtion_Button[i]();
							Set_Ctrl_Mode_Type(CTRL_FROM_KEY);//标记控制来源
						}
						
						//关机下 计数 8次
						if(System_is_Power_Off())
						{
							Special_Button_Rules(Key_IO_Ordering_Value[i]);
						}
						
						Key_Long_Press_cnt[i] = 0;
					}
					
				}
				else
					Key_Long_Press_cnt[i] = 0;
			}
			else
			{
				Key_Long_Press_cnt[i] = 0;
			}
		}
		
		Key_IO_Old = Key_IO_Hardware;
}

// 按键主循环任务
//  20 ms
void App_Key_Handler(void)
{
	static uint8_t io_shake=0;
	static uint8_t io_shake_cnt=0;
	uint8_t i;
	
	Thread_Activity_Sign_Set(THREAD_ACTIVITY_KEY_BUTTON);
	
	if(*p_Analog_key_Value > 0)
	{
		if((*p_Analog_key_Value>>8) >= 2)
		{
			//长按
			Key_IO_Hardware = (*p_Analog_key_Value & 0xFF);
			for(i=0; i<KEY_CALL_OUT_NUMBER_MAX; i++)
			{
				if(Key_IO_Hardware == Key_IO_Ordering_Value[i])
				{
					Key_Long_Press_cnt[i] = KEY_LONG_PRESS_TIME_2S-1;
				}
			}
			
		}
		
		else
		{
			Key_IO_Hardware = *p_Analog_key_Value;
		}
		
		App_Key_Task();
		*p_Analog_key_Value = 0;
	}
	else
	{
		io_shake = Key_Get_IO_Input();
		if(Key_IO_Hardware == io_shake)
		{
			if(++io_shake_cnt >= KEY_VALUE_SHAKE_TIME)
			{
				Buzzer_Click_Handler();
				
				//自测模式
				if(IS_SELF_TEST_MODE())
				{
					if(Key_IO_Hardware > 0)
					{
						Buzzer_Click_Long_On(1);
						
						Key_IO_Old |= Key_IO_Hardware;
						Led_Button_On(Key_IO_Old);	// 按键
						Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_KEY, Key_IO_Old);
					}
				}
				else//正常使用
				{
					App_Key_Task();
				}
			}
		}
		else
		{
			Key_IO_Hardware = io_shake;
			io_shake_cnt = 0;
		}
	}
	
}


// 获取按键
uint8_t Key_Get_IO_Input(void)
{
	uint8_t result=0;
	GPIO_PinState read_io;
	uint8_t i;
	
	for(i=0; i<KEY_IO_NUMBER_MAX; i++) // KEY_IO_NUMBER_MAX
	{
		read_io = HAL_GPIO_ReadPin(Key_Gpio_Pin_Array[i].port, Key_Gpio_Pin_Array[i].pin);
		if(read_io == 0)//低有效
			result |= (1<<i);
	}
	
//	if(System_is_Power_Off())
//	{
//		result &= KEY_VALUE_BIT_BUTTON_4;
//	}
	
	return result;
}


//------------------- 功能接口 ----------------------------

//	开机 进入自由模式
void System_Power_On(void)
{
	Out_Of_Upgradation();
	Freertos_TaskResume_All();
	// 检查各模式 属性
	if(Check_Data_Init())
	{
		Write_MbBuffer_Now();
	}
	//	状态
	To_Free_Mode(FREE_MODE_AUTO_START);			// ui
	
	Led_Button_On(0x0F);	// 按键
	
	//System_Check_Timer_Update();
	// 后台定时器
	//BlackGround_Task_On();
}

//	关机
void System_Power_Off(void)
{
	char show_mapping[9] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	
	//Clean_Swimming_Distance();//清除计算距离
	//清除故障状态
	Timing_Clean_Fault_State();
	//清除降频状态
	Down_Conversion_State_Clean();
	//清除计数器
	//Clean_Fault_Recovery_Cnt();
	//to 关机模式
	To_Power_Off();
	//设置电机转速
	Data_Set_Current_Speed(0);//注意,需要在切完运行状态后再设置速度,如"暂停"

	Led_Button_On(0x0F);	// 按键
	
	// 后台定时器
	//BlackGround_Task_Off();
	Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER, MB_LCD_MAPPING_SYMBOL, 0);
	Set_DataValue_Len(MB_FUNC_READ_INPUT_REGISTER,MB_LCD_MAPPING_MODE,(uint8_t *)show_mapping,8);
	
	//退出100档位模式
	Special_Status_Delete(SPECIAL_BIT_SPEED_100_GEAR);
	
	// 存储flash
	Write_MbBuffer_Now();
}

//	开机画面
void System_Boot_Screens(void)
{
	
////******************  调试模式 **************************
//#ifdef SYSTEM_DEBUG_MODE
//	return;
//#endif
////*******************************************************
	
	osDelay(200);
	//全亮 2s
	Breath_light_Max();
	TM1621_Show_All();
	osDelay(2000);
	//机型码 & 拨码状态 2s
	Lcd_System_Information();
	osDelay(2000);
	//Breath_light_Off();
}

//	恢复出厂设置
void Restore_Factory_Settings(void)
{
	TM1621_Buzzer_Click();
	// data 恢复
	App_Data_ReInit();
	
	// wifi 恢复
	
	//TM1621_Show_All();
	osDelay(500);
	TM1621_Buzzer_Off();
	//osDelay(1500);
	//System_Power_Off();
	SysSoftReset();// 软件复位
	// 返回 自由模式 初始状态
	
}

//	OTA
uint8_t System_To_OTA(void)
{
	if(Motor_is_Start())
		return ERROR;
	
	return SUCCESS;
}


	
