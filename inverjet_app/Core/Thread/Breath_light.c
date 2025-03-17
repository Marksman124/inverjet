/**
******************************************************************************
* @file				Breath_light.c
* @brief			呼吸灯  100 阶级  50ms定时
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "Breath_light.h"
#include "main.h"
#include <math.h>

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/
TIM_HandleTypeDef* p_htim_breath_light = &htim2;

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

uint16_t  Light_Brightness = 0;

uint16_t  Light_Brightness_cmp = 0;

double  Light_AdSample = 0;

uint8_t Breath_light_direction=0; // 方向  亮 灭

uint8_t Key_Buzzer_cnt = 0; //蜂鸣器计时
uint8_t Key_Buzzer_Type = 0;	//蜂鸣器长短 类型

/* Private function prototypes -----------------------------------------------*/

void Breath_light_PwmOut(uint16_t pul);

/* Private user code ---------------------------------------------------------*/

//------------------- 硬件 & 驱动 ----------------------------

void App_Breath_light_Init(void)
{
	//默认关闭
	Breath_light_PwmOut(0);
	
	DEBUG_LED1_OFF();
	DEBUG_LED2_OFF();
}

//uint16_t pwm_out_01[110]={0};
//uint16_t pwm_out_02[260]={0};
//uint16_t offset_cnt=0;

// 输入AD采样值，返回对应的PWM值
uint16_t get_PwmDuty(double  AD_Sample)
{
	double pwm=0;
	
	pwm = LIGHT_BRIGHTNESS_MAX*pow(((double)AD_Sample/AD_SAMPLE_MAX),2.2);
	
	if(pwm > LIGHT_BRIGHTNESS_MAX)
			pwm = LIGHT_BRIGHTNESS_MAX;
	
	return (uint16_t)pwm;
}

//------------------- 主循环 ----------------------------
void App_Breath_light_Handler(void)
{
	Thread_Activity_Sign_Set(THREAD_ACTIVITY_BREATH_LIGHT);
	
	Buzzer_Click_Handler();
	
	Light_Brightness = get_PwmDuty(Light_AdSample);
	
	if( Light_Brightness_cmp != Light_Brightness )
	{
			Breath_light_PwmOut(Light_Brightness);
			Light_Brightness_cmp = Light_Brightness;	
	}
	
	if(System_is_Starting() || ((System_is_Running()||System_is_Error()) && Special_Status_Get(SPECIAL_BIT_SKIP_STARTING)))	// 软启动
	{
		if(Breath_light_direction == 0)
		{
			if(Light_AdSample < AD_SAMPLE_MAX)
			{
				Light_AdSample += BREATH_LIGHT_GEAR_POSITION;
			}
			else
			{
				Light_AdSample = AD_SAMPLE_MAX;
				Breath_light_direction = 1;
			}
		}
		else
		{
			if(Light_AdSample > 0)
			{
				Light_AdSample -= BREATH_LIGHT_GEAR_POSITION;
			}
			else
			{
				Light_AdSample = 0;
				Breath_light_direction = 0;
			}
		}
//		Light_Brightness = get_PwmDuty(Light_AdSample);
//		offset_cnt = Light_AdSample/40;
//		pwm_out_01[offset_cnt] = Light_Brightness;
	}
	else if(System_is_Pause())	// 暂停
	{
		if(Breath_light_direction == 0)
		{
			if(Light_AdSample < AD_SAMPLE_MAX)
			{
				Light_AdSample += BREATH_LIGHT_GEAR_POSITION_LOW;
			}
			else
			{
				Light_AdSample = AD_SAMPLE_MAX;
				Breath_light_direction = 1;
			}
		}
		else
		{
			if(Light_AdSample > 0)
			{
				Light_AdSample -= BREATH_LIGHT_GEAR_POSITION_LOW;
			}
			else
			{
				Light_AdSample = 0;
				Breath_light_direction = 0;
			}
		}
//		Light_Brightness = get_PwmDuty(Light_AdSample);
//		offset_cnt = Light_AdSample/16;
//		pwm_out_02[offset_cnt] = Light_Brightness;
	}
	else if(System_is_Power_Off())	// 关机
	{
		if(Light_AdSample > 0)
		{
			Light_AdSample -= BREATH_LIGHT_GEAR_POSITION_LOW;
		}
		else
		{
			Light_AdSample = 0;
			Breath_light_direction = 0;
		}
	}
	else
	{
		if(Light_AdSample < AD_SAMPLE_MAX)
		{
			Light_AdSample += BREATH_LIGHT_GEAR_POSITION_LOW;
		}
		else
		{
			Light_AdSample = AD_SAMPLE_MAX;
		}
	}
}


//------------------- pwm控制 ----------------------------
// 调节范围 : 1 - 100
void Breath_light_PwmOut(uint16_t pul)
{
	HAL_TIM_PWM_Stop_IT(p_htim_breath_light, BREATH_LIGHT_PWM_CHANNEL);
	__HAL_TIM_SetCompare(p_htim_breath_light, BREATH_LIGHT_PWM_CHANNEL, pul);//pul
	HAL_TIM_PWM_Start(p_htim_breath_light, BREATH_LIGHT_PWM_CHANNEL);
}

//------------------- 模拟调节亮度 ----------------------------

void Breath_light_Max(void)
{
	Breath_light_PwmOut(LIGHT_BRIGHTNESS_MAX);
}

void Breath_light_Off(void)
{
	Breath_light_PwmOut(0);
}

//------------------- 蜂鸣器 ----------------------------
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
