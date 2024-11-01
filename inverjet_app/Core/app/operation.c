/**
******************************************************************************
* @file				operation.c
* @brief			操作模块 用于隐藏操作 单独出来便于屏蔽\删除
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-13
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "operation.h"
#include "tm1621.h"
#include "key.h"
#include "stdio.h"
#include "wifi.h"					// wifi 模组

/* Private includes ----------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/

//------------------- 按键回调 ----------------------------
//--------------- 短按 -----------------------
// ① 上
void on_Button_1_clicked(void);
// ② 下
void on_Button_2_clicked(void);
// ③ 确认
void on_Button_3_clicked(void);
// ④ 取消
void on_Button_4_Short_Press(void);
void on_Button_1_2_Short_Press(void);
// ① + ③  组合键  短按   切换档位 80级 or 5级
void on_Button_1_3_Short_Press(void);
// ② + ③  组合键  短按
void on_Button_2_3_Short_Press(void);
// ② + ④  组合键  短按
void on_Button_2_4_Short_Press(void);
//--------------- 长按 -----------------------
// 长按
void on_Button_1_Long_Press(void);
void on_Button_2_Long_Press(void);
void on_Button_3_Long_Press(void);
void on_Button_4_Long_Press(void);
void on_Button_1_2_Long_Press(void);
void on_Button_1_3_Long_Press(void);
void on_Button_2_3_Long_Press(void);
void on_Button_2_4_Long_Press(void);

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

UART_HandleTypeDef* p_huart_operation = &huart3;		 //调试串口 UART句柄

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static uint8_t Operation_State_Machine = 0;		//	状态机	

static uint8_t Operation_Menu_Value = 0;		//	菜单值	


static uint16_t Operation_Addr_Value = 0;		//	地址	
static uint16_t Operation_Baud_Rate = 0;			//	波特率	
static uint16_t Operation_Shield_Value = 0;		//	控制方式	
static uint16_t Operation_Motor_Poles = 0;		//	电机极数	
static uint16_t Operation_Speed_Mode = 0;		//	转速方式  0：转速   1：功率	
//static uint16_t Operation_Breath_Light_Max = 0;		//	光圈亮度	
// 发送缓冲区
uint8_t operation_send_buffer[24] = {0};


// 短按 槽函数
void (*p_Operation_Button[CALL_OUT_NUMBER_MAX])(void) = {
	on_Button_1_clicked,  on_Button_2_clicked,  on_Button_3_clicked,  on_Button_4_Short_Press,
	on_Button_1_2_Short_Press, on_Button_1_3_Short_Press,
	on_Button_2_3_Short_Press, on_Button_2_4_Short_Press,
};

// 长按 槽函数
void (*p_Operation_Long_Press[CALL_OUT_NUMBER_MAX])(void) = {
	on_Button_1_Long_Press,  on_Button_2_Long_Press,  on_Button_3_Long_Press,  on_Button_4_Long_Press,
	on_Button_1_2_Long_Press, on_Button_1_3_Long_Press,
	on_Button_2_3_Long_Press, on_Button_2_4_Long_Press,
};


static uint16_t Baud_Rate_Value[OPERATION_BAUD_MAX] = {1200,2400,4800,9600};			//	波特率	
//static uint16_t Speed_Mode_Value[2] = {'r','P'};			//	转速模式	

static uint32_t button_cnt=0;
/* Private user code ---------------------------------------------------------*/


//测试用
extern TIM_HandleTypeDef htim1;


// 初始化
void App_Operation_Init(void)
{
	Operation_Addr_Value = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_NODE_ADDRESS );
	if((Operation_Addr_Value > MODBUS_LOCAL_ADDRESS_MAX) || (Operation_Addr_Value < MODBUS_LOCAL_ADDRESS_MIX))
	{
		Operation_Addr_Value = MODBUS_LOCAL_ADDRESS;
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_NODE_ADDRESS, Operation_Addr_Value );
	}
	
	Operation_Baud_Rate = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_BAUD_RATE );
	if(Operation_Baud_Rate > (OPERATION_BAUD_MAX-1))
	{
		Operation_Baud_Rate = (OPERATION_BAUD_MAX-1);
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_BAUD_RATE, Operation_Baud_Rate );
	}
	
	Operation_Shield_Value = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS );
	if(Operation_Shield_Value > (OPERATION_SHIELD_MAX))
	{
		Operation_Shield_Value = (OPERATION_SHIELD_MAX);
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS, Operation_Shield_Value );
	}
	
	Operation_Motor_Poles = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_POLE_NUMBER );
	if(Operation_Motor_Poles > (OPERATION_POLES_MAX))
	{
		Operation_Motor_Poles = (OPERATION_POLES_MAX);
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_POLE_NUMBER, Operation_Motor_Poles );
	}
	
	Operation_Speed_Mode = Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_SPEED_MODE );
	if(Operation_Speed_Mode > (1))
	{
		Operation_Speed_Mode = (1);
		Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_SPEED_MODE, Operation_Speed_Mode );
	}
}

//static void _Delay(uint32_t mdelay)
//{
//	__HAL_TIM_SET_COUNTER(&htim1,0);
//	while(__HAL_TIM_GET_COUNTER(&htim1) < mdelay);
//}

//------------------- 显示屏 & 接口 ----------------------------
/*
******************************************************************************
Display_Show_Number	

显示当前故障编号， 0-100
******************************************************************************
*/  
void Display_Oper_Number(uint8_t no)
{
	if(no > 100)
		no = 100;
//-------------------- 电机状态解析 ----------------------------
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
	TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, GET_NUMBER_HUNDRED_DIGIT(no));
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
	TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, GET_NUMBER_HUNDRED_DIGIT(no)+2);
#endif
	TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, GET_NUMBER_TEN_DIGIT(no));
	TM1621_display_number(TM1621_COORDINATE_SPEED_LOW, GET_NUMBER_ONE_DIGIT(no));
	
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_FaultCode

显示参数值
******************************************************************************
*/  
void Display_Oper_value(uint16_t value)
{
	TM1621_display_number(TM1621_COORDINATE_MIN_HIGH,  (value / 1000)%10);
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  	(value / 100)%10);
	
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH,  	(value / 10)%10);
	TM1621_display_number(TM1621_COORDINATE_SEC_LOW,  	(value % 10));
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_FaultCode

显示 字母
******************************************************************************
*/  
void Display_Oper_Letter(uint8_t value)
{
	
	TM1621_display_number(TM1621_COORDINATE_MIN_HIGH,  	0xFF);
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  	0xFF);
	
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH,  	0xFF);
	TM1621_display_Letter(TM1621_COORDINATE_SEC_LOW,  	value);
	//TM1621_LCD_Redraw();
}

/*
******************************************************************************
Display_Show_Sum

隐藏模式
******************************************************************************
*/  
void Display_Mode_Hide(void)
{
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH,  0xFF);
	
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  0xFF);
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Mode_Show

显示模式
******************************************************************************
*/  
void Display_Mode_Show(uint8_t sum)
{
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH, (sum/10)%10);
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  	sum%10);
}

/***********************************************************************
*		显示 函数总入口
*
*
***********************************************************************/
void Lcd_Show_Operation(uint8_t type, uint16_t num)
{
	char show_mapping[9] = {0};
	
	if(System_is_Operation() == 0)
	{
			return ;
	}
	//背光
	TM1621_BLACK_ON();
	taskENTER_CRITICAL();
	
	// sum
	Display_Oper_Number(type);
#ifdef OPERATION_SPEED_MODE
	//字母
	if(type == OPERATION_SPEED_MODE)
		Display_Oper_Letter(num);
	else
#endif
		Display_Oper_value(num);
	
	
	//版本号显示小数点
	if((type == OPERATION_DISPLAY_VERSION) || (type == OPERATION_DEIVES_VERSION))
	{
		Display_Mode_Show(*p_Software_Version_low);
		Lcd_Display_Symbol(STATUS_BIT_POINT);
		Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER, MB_LCD_MAPPING_SYMBOL, 8);
	}
	else
	{
		Display_Mode_Hide();
		Lcd_Display_Symbol(0);
		Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER, MB_LCD_MAPPING_SYMBOL, 0);
	}
	
	sprintf(show_mapping,"%02d%02d%04d",0,type,num);
	show_mapping[0] = 0xFF;
	show_mapping[1] = 0xFF;
	Set_DataValue_Len(MB_FUNC_READ_INPUT_REGISTER,MB_LCD_MAPPING_MODE,(uint8_t *)show_mapping,8);
	
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
	return;
}




//------------------- 切换模式  ----------------------------

// 进入操作菜单
void To_Operation_Menu(void)
{
	Set_Software_Version();
	// 操作 菜单
	App_Operation_Init();
	//功能暂停, 电机关闭
	Set_System_State_Machine(OPERATION_MENU_STATUS);
#ifdef OPERATION_P5_ACCELERATION
	Operation_State_Machine = OPERATION_P5_ACCELERATION;		//	状态机	
  Operation_Menu_Value = *p_Surf_Mode_Info_Acceleration;		//	菜单值	
#else
	Operation_State_Machine = OPERATION_ADDR_SET;		//	状态机	
  Operation_Menu_Value = Operation_Addr_Value;		//	菜单值	
#endif
	Lcd_Show_Operation( Operation_State_Machine, Operation_Menu_Value);
	
}

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- 按键回调 ----------------------------

// ① 档位键
static void on_Button_1_clicked(void)
{
	button_cnt = 0;
#ifdef OPERATION_P5_ACCELERATION
	if(Operation_State_Machine == OPERATION_P5_ACCELERATION)
	{
		if(*p_Surf_Mode_Info_Acceleration < 4)
			(*p_Surf_Mode_Info_Acceleration)++;
		else
			*p_Surf_Mode_Info_Acceleration = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_Acceleration);
	}
	else if(Operation_State_Machine == OPERATION_P5_100_TIME)
	{
		if(*p_Surf_Mode_Info_High_Time < 60)
			(*p_Surf_Mode_Info_High_Time)++;
		else
			*p_Surf_Mode_Info_High_Time = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_High_Time);
	}
	else if(Operation_State_Machine == OPERATION_P5_0_TIME)
	{
		if(*p_Surf_Mode_Info_Low_Time < 60)
			(*p_Surf_Mode_Info_Low_Time)++;
		else
			*p_Surf_Mode_Info_Low_Time = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_Low_Time);
	}
#endif
	//------- 地址
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value < MODBUS_LOCAL_ADDRESS_MAX)
			(Operation_Addr_Value)++;
		else
			Operation_Addr_Value = MODBUS_LOCAL_ADDRESS_MIX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	//------- 波特率
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(Operation_Baud_Rate < (OPERATION_BAUD_MAX-1))
			(Operation_Baud_Rate)++;
		else
			Operation_Baud_Rate = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Baud_Rate_Value[Operation_Baud_Rate]);
	}
	//------- 转速模式
#ifdef OPERATION_SPEED_MODE
	else if(Operation_State_Machine == OPERATION_SPEED_MODE)
	{
		if(Operation_Speed_Mode < 1)
			(Operation_Speed_Mode)++;
		else
			Operation_Speed_Mode = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Speed_Mode_Value[Operation_Speed_Mode]);
	}
#endif
#ifdef OPERATION_MOTOR_POLES
	//------- 电机 极数
	else if(Operation_State_Machine == OPERATION_MOTOR_POLES)
	{
		if(Operation_Motor_Poles < OPERATION_POLES_MAX)
			(Operation_Motor_Poles)++;
		else
			Operation_Motor_Poles = OPERATION_POLES_MIX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Motor_Poles);
	}
#endif
	//------- 屏蔽 方式
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(Operation_Shield_Value < OPERATION_SHIELD_MAX)
			(Operation_Shield_Value)++;
		else
			Operation_Shield_Value = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
	}
#ifdef OPERATION_BREATH_LIGHT_MAX
	//------- 光圈亮度
	if(Operation_State_Machine == OPERATION_BREATH_LIGHT_MAX)
	{
		if(Operation_Breath_Light_Max < 500)
			(Operation_Breath_Light_Max)+= 10;
		else
			Operation_Breath_Light_Max = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Breath_Light_Max);
	}
#endif
}

// ② 时间键
static void on_Button_2_clicked(void)
{
	button_cnt = 0;
#ifdef OPERATION_P5_ACCELERATION
	if(Operation_State_Machine == OPERATION_P5_ACCELERATION)
	{
		if(*p_Surf_Mode_Info_Acceleration > 1)
			(*p_Surf_Mode_Info_Acceleration)--;
		else
			*p_Surf_Mode_Info_Acceleration = 4;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_Acceleration);
	}
	else if(Operation_State_Machine == OPERATION_P5_100_TIME)
	{
		if(*p_Surf_Mode_Info_High_Time > 1)
			(*p_Surf_Mode_Info_High_Time)--;
		else
			*p_Surf_Mode_Info_High_Time = 60;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_High_Time);
	}
	else if(Operation_State_Machine == OPERATION_P5_0_TIME)
	{
		if(*p_Surf_Mode_Info_Low_Time > 1)
			(*p_Surf_Mode_Info_Low_Time)--;
		else
			*p_Surf_Mode_Info_Low_Time = 60;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_Low_Time);
	}
#endif
	//------- 地址
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value > MODBUS_LOCAL_ADDRESS_MIX)
			(Operation_Addr_Value)--;
		else
			Operation_Addr_Value = MODBUS_LOCAL_ADDRESS_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	//------- 波特率
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(Operation_Baud_Rate > 0)
			(Operation_Baud_Rate)--;
		else
			Operation_Baud_Rate = (OPERATION_BAUD_MAX-1);
		
		Lcd_Show_Operation( Operation_State_Machine, Baud_Rate_Value[Operation_Baud_Rate]);
	}
	//------- 转速 模式
#ifdef OPERATION_SPEED_MODE
	else if(Operation_State_Machine == OPERATION_SPEED_MODE)
	{
		if(Operation_Speed_Mode < 1)
			(Operation_Speed_Mode)++;
		else
			Operation_Speed_Mode = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Speed_Mode_Value[Operation_Speed_Mode]);
	}
#endif
#ifdef OPERATION_MOTOR_POLES
	//------- 电机 极数
	else if(Operation_State_Machine == OPERATION_MOTOR_POLES)
	{
		if(Operation_Motor_Poles > OPERATION_POLES_MIX)
			(Operation_Motor_Poles)--;
		else
			Operation_Motor_Poles = OPERATION_POLES_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Motor_Poles);
	}
#endif
	//------- 屏蔽 方式
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(Operation_Shield_Value > 0)
			(Operation_Shield_Value)--;
		else
			Operation_Shield_Value = OPERATION_SHIELD_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
	}
#ifdef OPERATION_BREATH_LIGHT_MAX
	//------- 光圈亮度
	if(Operation_State_Machine == OPERATION_BREATH_LIGHT_MAX)
	{
		if(Operation_Breath_Light_Max > 10)
			(Operation_Breath_Light_Max)-=10;
		else
			Operation_Breath_Light_Max = 500;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Breath_Light_Max);
	}
#endif
}

// ③ 模式键
static void on_Button_3_clicked(void)
{
	button_cnt = 0;
	
	switch(Operation_State_Machine)
	{
#ifdef OPERATION_P5_ACCELERATION
		case OPERATION_P5_ACCELERATION:
			Operation_State_Machine += 1;
			Lcd_Show_Operation( Operation_State_Machine, (*p_Surf_Mode_Info_High_Time));
		break;
		case OPERATION_P5_100_TIME:
			Operation_State_Machine += 1;
			Lcd_Show_Operation( Operation_State_Machine, (*p_Surf_Mode_Info_Low_Time));
		break;
		case OPERATION_P5_0_TIME:
			Operation_State_Machine = OPERATION_ADDR_SET;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
		break;
#endif
		case OPERATION_ADDR_SET:
			Operation_State_Machine = OPERATION_BAUD_RATE;
			Lcd_Show_Operation( Operation_State_Machine, Baud_Rate_Value[Operation_Baud_Rate]);
		break;
		case OPERATION_BAUD_RATE:
#ifdef OPERATION_SPEED_MODE
			Operation_State_Machine = OPERATION_SPEED_MODE;
			Lcd_Show_Operation( Operation_State_Machine, Speed_Mode_Value[Operation_Speed_Mode]);
		break;
		case OPERATION_SPEED_MODE:
#endif
#ifdef OPERATION_MOTOR_POLES
			Operation_State_Machine += OPERATION_MOTOR_POLES;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Motor_Poles);
		break;
		case OPERATION_MOTOR_POLES:
#endif
			Operation_State_Machine = OPERATION_SHIELD_MENU;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
		break;

		case OPERATION_SHIELD_MENU:
			Operation_State_Machine = OPERATION_DISPLAY_VERSION;
			
			Lcd_Show_Operation( Operation_State_Machine, ((*p_Software_Version_high)*100 + (*p_Software_Version_middle)));
		break;
		
		case OPERATION_DISPLAY_VERSION:
#ifdef OPERATION_BREATH_LIGHT_MAX
			Operation_State_Machine = OPERATION_BREATH_LIGHT_MAX;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Breath_Light_Max);
		break;
		
		case OPERATION_BREATH_LIGHT_MAX:
#endif
			Operation_State_Machine = OPERATION_DEIVES_VERSION;
			//Lcd_Show_Operation( Operation_State_Machine, (DEVICES_VERSION_HIGH*100 + DEVICES_VERSION_LOW));
		Lcd_Show_Operation( Operation_State_Machine, (*p_Driver_Software_Version));
		break;
		default:
#ifdef OPERATION_P5_ACCELERATION
			Operation_State_Machine = OPERATION_P5_ACCELERATION;
			Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_Acceleration);
#else
			Operation_State_Machine = OPERATION_ADDR_SET;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
#endif
		break;
	}
}


// ④ 开机键  短按
static void on_Button_4_Short_Press(void)
{
#ifdef OPERATION_P5_ACCELERATION
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS, Operation_Shield_Value );
#endif
#ifdef OPERATION_SPEED_MODE
			Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_SPEED_MODE, Operation_Speed_Mode );
#endif
#ifdef OPERATION_MOTOR_POLES
			Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_POLE_NUMBER, Operation_Motor_Poles );
#endif
#ifdef OPERATION_BREATH_LIGHT_MAX
			Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_BREATH_LIGHT_MAX, Operation_Breath_Light_Max );
#endif
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_NODE_ADDRESS, Operation_Addr_Value );
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SLAVE_BAUD_RATE, Operation_Baud_Rate );
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_SUPPORT_CONTROL_METHODS, Operation_Shield_Value );
	//保存 flash
	Write_MbBuffer_Now();
	//Dev_Check_Control_Methods();

	//mcu_reset_wifi();// 复位模组
	SysSoftReset();// 软件复位
}

// ① + ③  组合键  短按   切换档位 100级 or 5级
static void on_Button_1_2_Short_Press(void)
{

}

// ① + ③  组合键  短按   切换档位 100级 or 5级
static void on_Button_1_3_Short_Press(void)
{

}


// ② + ③  组合键  短按   
static void on_Button_2_3_Short_Press(void)
{
}

// ② + ④  组合键  短按
static void on_Button_2_4_Short_Press(void)
{
}

//------------------- 按键回调   长按 ----------------------------

static void on_Button_1_Long_Press(void)
{
#ifdef OPERATION_P5_ACCELERATION
	if(Operation_State_Machine == OPERATION_P5_100_TIME)
	{
		if(*p_Surf_Mode_Info_High_Time < 60)
			(*p_Surf_Mode_Info_High_Time)++;
		else
			*p_Surf_Mode_Info_High_Time = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_High_Time);
	}
	else if(Operation_State_Machine == OPERATION_P5_0_TIME)
	{
		if(*p_Surf_Mode_Info_Low_Time < 60)
			(*p_Surf_Mode_Info_Low_Time)++;
		else
			*p_Surf_Mode_Info_Low_Time = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_Low_Time);
	}
#endif
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value < MODBUS_LOCAL_ADDRESS_MAX)
		{
//			button_cnt++;
//			if(button_cnt > 5)
//			{
//				(Operation_Addr_Value) += 5;
//			}
//			else if(button_cnt > 10)
//				(Operation_Addr_Value) += 10;
//			else
				(Operation_Addr_Value)++;
			
			if(Operation_Addr_Value > MODBUS_LOCAL_ADDRESS_MAX)
				Operation_Addr_Value = MODBUS_LOCAL_ADDRESS_MAX;
		}
		else
			Operation_Addr_Value = MODBUS_LOCAL_ADDRESS_MIX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(button_cnt++ > 5)
		{
			button_cnt = 0;
			if(Operation_Baud_Rate < (OPERATION_BAUD_MAX-1))
				(Operation_Baud_Rate)++;
			else
				Operation_Baud_Rate = 0;
			
			Lcd_Show_Operation( Operation_State_Machine, Baud_Rate_Value[Operation_Baud_Rate]);
		}
	}
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(button_cnt++ > 3)
		{
			button_cnt = 0;
			if(Operation_Shield_Value < OPERATION_SHIELD_MAX)
				(Operation_Shield_Value)++;
			else
				Operation_Shield_Value = 0;
			
			Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
		}
	}
#ifdef OPERATION_BREATH_LIGHT_MAX
	else if(Operation_State_Machine == OPERATION_BREATH_LIGHT_MAX)
	{
		if(Operation_Breath_Light_Max < 500)
		{
			button_cnt++;
			if(button_cnt > 5)
			{
				(Operation_Breath_Light_Max) += 5;
			}
			else if(button_cnt > 10)
				(Operation_Breath_Light_Max) += 10;
			else
				(Operation_Breath_Light_Max)++;
			
			if(Operation_Breath_Light_Max > 500)
				Operation_Breath_Light_Max = 500;
		}
		else
			Operation_Breath_Light_Max = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Breath_Light_Max);
	}
#endif
}

static void on_Button_2_Long_Press(void)
{
#ifdef OPERATION_P5_ACCELERATION
	if(Operation_State_Machine == OPERATION_P5_100_TIME)
	{
		if(*p_Surf_Mode_Info_High_Time > 1)
			(*p_Surf_Mode_Info_High_Time)--;
		else
			*p_Surf_Mode_Info_High_Time = 60;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_High_Time);
	}
	else if(Operation_State_Machine == OPERATION_P5_0_TIME)
	{
		if(*p_Surf_Mode_Info_Low_Time > 1)
			(*p_Surf_Mode_Info_Low_Time)--;
		else
			*p_Surf_Mode_Info_Low_Time = 60;
		
		Lcd_Show_Operation( Operation_State_Machine, *p_Surf_Mode_Info_Low_Time);
	}
#endif
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value > MODBUS_LOCAL_ADDRESS_MIX)
		{
//			button_cnt++;
//			if(button_cnt > 5)
//			{
//				(Operation_Addr_Value) -= 5;
//			}
//			else if(button_cnt > 10)
//			{
//				(Operation_Addr_Value) -= 10;
//			}
//			else
				(Operation_Addr_Value)--;
			
		}
		else
			Operation_Addr_Value = MODBUS_LOCAL_ADDRESS_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(button_cnt++ > 3)
		{
			button_cnt = 0;
			if(Operation_Baud_Rate > 0)
				(Operation_Baud_Rate)--;
			else
				Operation_Baud_Rate = (OPERATION_BAUD_MAX-1);
			
			Lcd_Show_Operation( Operation_State_Machine, Baud_Rate_Value[Operation_Baud_Rate]);
		}
	}
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(button_cnt++ > 5)
		{
			button_cnt = 0;
			if(Operation_Shield_Value > 0)
				(Operation_Shield_Value)--;
			else
				Operation_Shield_Value = OPERATION_SHIELD_MAX;
			
			Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
		}
	}
#ifdef OPERATION_BREATH_LIGHT_MAX
	else if(Operation_State_Machine == OPERATION_BREATH_LIGHT_MAX)
	{
		if(Operation_Breath_Light_Max > 0)
		{
			button_cnt++;
			if(button_cnt > 5)
			{
				(Operation_Breath_Light_Max) -= 5;
			}
			else if(button_cnt > 10)
			{
				(Operation_Breath_Light_Max) -= 10;
			}
			else
				(Operation_Breath_Light_Max)--;
			
		}
		else
			Operation_Breath_Light_Max = 500;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Breath_Light_Max);
	}
#endif
}

static void on_Button_3_Long_Press(void)
{
}


static void on_Button_4_Long_Press(void)
{
	System_Power_Off();
}

static void on_Button_1_2_Long_Press(void)
{
}

static void on_Button_1_3_Long_Press(void)
{
}

// 恢复出厂设置
static void on_Button_2_3_Long_Press(void)
{

}

//
static void on_Button_2_4_Long_Press(void)
{

}


