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

double  Light_Brightness = 0;

double  Light_AdSample = 0;

double  Light_Brightness_cmp = 0;

uint8_t Breath_light_direction=0; // 方向  亮 灭

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


// 输入AD采样值，返回对应的PWM值
double  get_PwmDuty(double  AD_Sample)
{
	double pwm=0;
	
	
	pwm = LIGHT_BRIGHTNESS_MAX*pow(((double)AD_Sample/AD_SAMPLE_MAX),2.4);
		
	return pwm;
}

//------------------- 主循环 ----------------------------
void App_Breath_light_Handler(void)
{
	Thread_Activity_Sign_Set(THREAD_ACTIVITY_BREATH_LIGHT);
	
	Light_Brightness = get_PwmDuty(Light_AdSample);
	
	if( Light_Brightness_cmp != Light_Brightness )
	{
		if(Light_Brightness > LIGHT_BRIGHTNESS_MAX)
			Light_Brightness = LIGHT_BRIGHTNESS_MAX;
		{
			Breath_light_PwmOut((uint16_t)Light_Brightness);
			Light_Brightness_cmp = Light_Brightness;
		}
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


