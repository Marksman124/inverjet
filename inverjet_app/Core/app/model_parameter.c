/**
******************************************************************************
* @file				data.c
* @brief			���ݹ���ģ��
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "model_parameter.h"


/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/

// ���뿪��
uint8_t System_Dial_Switch = 0;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

uint32_t Product_Model_Ccode[MODEL_DIAL_SWITCH_NUMBER] = 
{
	PRODUCT_MODEL_CODE_SJ230,
	PRODUCT_MODEL_CODE_SJ200,
	PRODUCT_MODEL_CODE_SJ160,
	PRODUCT_MODEL_CODE_SJ230
};


/* Private user code ---------------------------------------------------------*/

//------------------- ��ȡ������ ----------------------------
uint32_t Get_Model_Code_Num(void)
{
	System_Dial_Switch = Gpio_Get_Dial_Switch();
	
	if(System_Dial_Switch < MODEL_DIAL_SWITCH_NUMBER)
		return Product_Model_Ccode[System_Dial_Switch];
	else
		return PRODUCT_MODEL_CODE_SJ230;
}

// ÿ %1 ÿ�� ������Ӿ���� �Ŵ�100��
uint32_t Get_Every_1Percent_Distance_Per_Second(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
	else
	{
		return SJ230_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
}

//���ת�� 100%
uint32_t Get_Motor_Rpm_Speed_Max(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_RPM_SPEED_MAX;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_RPM_SPEED_MAX;
	}
	else
	{
		return SJ230_MOTOR_RPM_SPEED_MAX;
	}
}


//���ת�� 20%
uint32_t Get_Motor_Rpm_Speed_Mix(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_RPM_SPEED_MIX;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_RPM_SPEED_MIX;
	}
	else
	{
		return SJ230_MOTOR_RPM_SPEED_MIX;
	}
}

// ���� ��Ƶ
//*********************************************************************************************
//-------------- ������� ����ֵ  -------------------
uint32_t Get_Motor_Power_Alarm_Value(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_ALARM_VALUE;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_ALARM_VALUE;
	}
	else
	{
		return SJ230_MOTOR_POWER_ALARM_VALUE;
	}
}

//-------------- ������� ����  -------------------
uint32_t Get_Motor_Power_Reduce_Speed(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_REDUCE_SPEED;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_REDUCE_SPEED;
	}
	else
	{
		return SJ230_MOTOR_POWER_REDUCE_SPEED;
	}
}

//-------------- ������� �ָ�  -------------------
uint32_t Get_Motor_Power_Restore_Speed(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_RESTORE_SPEED;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_RESTORE_SPEED;
	}
	else
	{
		return SJ230_MOTOR_POWER_RESTORE_SPEED;
	}
}
//*********************************************************************************************

// ���� ��Ƶ
//*********************************************************************************************
//-------------- ������� ����ֵ  -------------------
uint32_t Get_Motor_Current_Alarm_Value(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_CURRENT_ALARM_VALUE;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_CURRENT_ALARM_VALUE;
	}
	else
	{
		return SJ230_MOTOR_CURRENT_ALARM_VALUE;
	}
}

//-------------- ������� ����  -------------------
uint32_t Get_Motor_Current_Reduce_Speed(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_CURRENT_REDUCE_SPEED;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_CURRENT_REDUCE_SPEED;
	}
	else
	{
		return SJ230_MOTOR_CURRENT_REDUCE_SPEED;
	}
}

//-------------- ������� �ָ�  -------------------
uint32_t Get_Motor_Current_Restore_Speed(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_CURRENT_RESTORE_SPEED;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_CURRENT_RESTORE_SPEED;
	}
	else
	{
		return SJ230_MOTOR_CURRENT_RESTORE_SPEED;
	}
}
//*********************************************************************************************










//---------------------------------------------------------------------------------------------------------------------------------------------------






