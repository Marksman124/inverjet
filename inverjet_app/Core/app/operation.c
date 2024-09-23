/**
******************************************************************************
* @file				operation.c
* @brief			����ģ�� �������ز��� ����������������\ɾ��
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

/* Private includes ----------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/
extern void To_Free_Mode(uint8_t mode);

//------------------- �����ص� ----------------------------
//--------------- �̰� -----------------------
// �� ��
void on_Button_1_clicked(void);
// �� ��
void on_Button_2_clicked(void);
// �� ȷ��
void on_Button_3_clicked(void);
// �� ȡ��
void on_Button_4_Short_Press(void);
void on_Button_1_2_Short_Press(void);
// �� + ��  ��ϼ�  �̰�   �л���λ 80�� or 5��
void on_Button_1_3_Short_Press(void);
// �� + ��  ��ϼ�  �̰�
void on_Button_2_3_Short_Press(void);
// �� + ��  ��ϼ�  �̰�
void on_Button_2_4_Short_Press(void);
//--------------- ���� -----------------------
// ����
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

UART_HandleTypeDef* p_huart_operation = &huart3;		 //���Դ��� UART���

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static uint8_t Operation_State_Machine = 0;		//	״̬��	

static uint8_t Operation_Menu_Value = 0;		//	�˵�ֵ	


static uint16_t Operation_Addr_Value = 0;		//	��ַ	
static uint16_t Operation_Baud_Rate = 0;			//	������	
static uint16_t Operation_Shield_Value = 0;		//	���Ʒ�ʽ	
static uint16_t Operation_Motor_Poles = 0;		//	�������	
static uint16_t Operation_Speed_Mode = 0;		//	ת�ٷ�ʽ  0��ת��   1������	
//static uint16_t Operation_Breath_Light_Max = 0;		//	��Ȧ����	
// ���ͻ�����
uint8_t operation_send_buffer[24] = {0};


// �̰� �ۺ���
void (*p_Operation_Button[CALL_OUT_NUMBER_MAX])(void) = {
	on_Button_1_clicked,  on_Button_2_clicked,  on_Button_3_clicked,  on_Button_4_Short_Press,
	on_Button_1_2_Short_Press, on_Button_1_3_Short_Press,
	on_Button_2_3_Short_Press, on_Button_2_4_Short_Press,
};

// ���� �ۺ���
void (*p_Operation_Long_Press[CALL_OUT_NUMBER_MAX])(void) = {
	on_Button_1_Long_Press,  on_Button_2_Long_Press,  on_Button_3_Long_Press,  on_Button_4_Long_Press,
	on_Button_1_2_Long_Press, on_Button_1_3_Long_Press,
	on_Button_2_3_Long_Press, on_Button_2_4_Long_Press,
};


static uint16_t Baud_Rate_Value[OPERATION_BAUD_MAX] = {1200,2400,4800,9600};			//	������	
//static uint16_t Speed_Mode_Value[2] = {'r','P'};			//	ת��ģʽ	

static uint32_t button_cnt=0;
/* Private user code ---------------------------------------------------------*/


//������
extern TIM_HandleTypeDef htim1;


// ��ʼ��
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

//------------------- ��ʾ�� & �ӿ� ----------------------------
/*
******************************************************************************
Display_Show_Number	

��ʾ��ǰ���ϱ�ţ� 0-100
******************************************************************************
*/  
void Display_Oper_Number(uint8_t no)
{
	if(no > 100)
		no = 100;
	
	TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, GET_NUMBER_HUNDRED_DIGIT(no));
	TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, GET_NUMBER_TEN_DIGIT(no));
	TM1621_display_number(TM1621_COORDINATE_SPEED_LOW, GET_NUMBER_ONE_DIGIT(no));
	
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_FaultCode

��ʾ����ֵ
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

��ʾ ��ĸ
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

��ʾ��������
******************************************************************************
*/  
void Display_Mode_Hide(void)
{
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH,  0xFF);
	
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  0xFF);
	
	//TM1621_LCD_Redraw();
}

/***********************************************************************
*		��ʾ ���������
*
*
***********************************************************************/
void Lcd_Show_Operation(uint8_t type, uint16_t num)
{
	if(System_is_Operation() == 0)
	{
			return ;
	}
	//����
	TM1621_BLACK_ON();
	taskENTER_CRITICAL();
	
	// sum
	Display_Oper_Number(type);
#ifdef OPERATION_SPEED_MODE
	//��ĸ
	if(type == OPERATION_SPEED_MODE)
		Display_Oper_Letter(num);
	else
#endif
		Display_Oper_value(num);
	Display_Mode_Hide();
	
	
	//�汾����ʾС����
	if((type == OPERATION_DISPLAY_VERSION) || (type == OPERATION_DEIVES_VERSION))
		Lcd_Display_Symbol(STATUS_BIT_POINT);
	else
		Lcd_Display_Symbol(0);

	
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
	return;
}




//------------------- �л�ģʽ  ----------------------------

// ��������˵�
void To_Operation_Menu(void)
{
	Set_Software_Version();
	// ���� �˵�
	App_Operation_Init();
	Clean_Timing_Timer_Cnt();
	//������ͣ, ����ر�
	Set_System_State_Machine(OPERATION_MENU_STATUS);
#ifdef OPERATION_P5_ACCELERATION
	Operation_State_Machine = OPERATION_P5_ACCELERATION;		//	״̬��	
  Operation_Menu_Value = Temp_Data_P5_Acceleration;		//	�˵�ֵ	
#else
	Operation_State_Machine = OPERATION_ADDR_SET;		//	״̬��	
  Operation_Menu_Value = Operation_Addr_Value;		//	�˵�ֵ	
#endif
	Lcd_Show_Operation( Operation_State_Machine, Operation_Menu_Value);
	
}

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- �����ص� ----------------------------

// �� ��λ��
static void on_Button_1_clicked(void)
{
	Clean_Timing_Timer_Cnt();
	button_cnt = 0;
#ifdef OPERATION_P5_ACCELERATION
	if(Operation_State_Machine == OPERATION_P5_ACCELERATION)
	{
		if(Temp_Data_P5_Acceleration < 4)
			(Temp_Data_P5_Acceleration)++;
		else
			Temp_Data_P5_Acceleration = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_Acceleration);
	}
	else if(Operation_State_Machine == OPERATION_P5_100_TIME)
	{
		if(Temp_Data_P5_100_Time < 60)
			(Temp_Data_P5_100_Time)++;
		else
			Temp_Data_P5_100_Time = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_100_Time);
	}
	else if(Operation_State_Machine == OPERATION_P5_0_TIME)
	{
		if(Temp_Data_P5_0_Time < 60)
			(Temp_Data_P5_0_Time)++;
		else
			Temp_Data_P5_0_Time = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_0_Time);
	}
#endif
	//------- ��ַ
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value < MODBUS_LOCAL_ADDRESS_MAX)
			(Operation_Addr_Value)++;
		else
			Operation_Addr_Value = MODBUS_LOCAL_ADDRESS_MIX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	//------- ������
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(Operation_Baud_Rate < (OPERATION_BAUD_MAX-1))
			(Operation_Baud_Rate)++;
		else
			Operation_Baud_Rate = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Baud_Rate_Value[Operation_Baud_Rate]);
	}
	//------- ת��ģʽ
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
	//------- ��� ����
	else if(Operation_State_Machine == OPERATION_MOTOR_POLES)
	{
		if(Operation_Motor_Poles < OPERATION_POLES_MAX)
			(Operation_Motor_Poles)++;
		else
			Operation_Motor_Poles = OPERATION_POLES_MIX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Motor_Poles);
	}
#endif
	//------- ���� ��ʽ
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(Operation_Shield_Value < OPERATION_SHIELD_MAX)
			(Operation_Shield_Value)++;
		else
			Operation_Shield_Value = 0;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
	}
#ifdef OPERATION_BREATH_LIGHT_MAX
	//------- ��Ȧ����
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

// �� ʱ���
static void on_Button_2_clicked(void)
{
	button_cnt = 0;
	Clean_Timing_Timer_Cnt();
#ifdef OPERATION_P5_ACCELERATION
	if(Operation_State_Machine == OPERATION_P5_ACCELERATION)
	{
		if(Temp_Data_P5_Acceleration > 1)
			(Temp_Data_P5_Acceleration)--;
		else
			Temp_Data_P5_Acceleration = 4;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_Acceleration);
	}
	else if(Operation_State_Machine == OPERATION_P5_100_TIME)
	{
		if(Temp_Data_P5_100_Time > 1)
			(Temp_Data_P5_100_Time)--;
		else
			Temp_Data_P5_100_Time = 60;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_100_Time);
	}
	else if(Operation_State_Machine == OPERATION_P5_0_TIME)
	{
		if(Temp_Data_P5_0_Time > 1)
			(Temp_Data_P5_0_Time)--;
		else
			Temp_Data_P5_0_Time = 60;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_0_Time);
	}
#endif
	//------- ��ַ
	if(Operation_State_Machine == OPERATION_ADDR_SET)
	{
		if(Operation_Addr_Value > MODBUS_LOCAL_ADDRESS_MIX)
			(Operation_Addr_Value)--;
		else
			Operation_Addr_Value = MODBUS_LOCAL_ADDRESS_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
	}
	//------- ������
	else if(Operation_State_Machine == OPERATION_BAUD_RATE)
	{
		if(Operation_Baud_Rate > 0)
			(Operation_Baud_Rate)--;
		else
			Operation_Baud_Rate = (OPERATION_BAUD_MAX-1);
		
		Lcd_Show_Operation( Operation_State_Machine, Baud_Rate_Value[Operation_Baud_Rate]);
	}
	//------- ת�� ģʽ
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
	//------- ��� ����
	else if(Operation_State_Machine == OPERATION_MOTOR_POLES)
	{
		if(Operation_Motor_Poles > OPERATION_POLES_MIX)
			(Operation_Motor_Poles)--;
		else
			Operation_Motor_Poles = OPERATION_POLES_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Motor_Poles);
	}
#endif
	//------- ���� ��ʽ
	else if(Operation_State_Machine == OPERATION_SHIELD_MENU)
	{
		if(Operation_Shield_Value > 0)
			(Operation_Shield_Value)--;
		else
			Operation_Shield_Value = OPERATION_SHIELD_MAX;
		
		Lcd_Show_Operation( Operation_State_Machine, Operation_Shield_Value);
	}
#ifdef OPERATION_BREATH_LIGHT_MAX
	//------- ��Ȧ����
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

// �� ģʽ��
static void on_Button_3_clicked(void)
{
	Clean_Timing_Timer_Cnt();
	button_cnt = 0;
	
	switch(Operation_State_Machine)
	{
#ifdef OPERATION_P5_ACCELERATION
		case OPERATION_P5_ACCELERATION:
			Operation_State_Machine += 1;
			Lcd_Show_Operation( Operation_State_Machine, (Temp_Data_P5_100_Time));
		break;
		case OPERATION_P5_100_TIME:
			Operation_State_Machine += 1;
			Lcd_Show_Operation( Operation_State_Machine, (Temp_Data_P5_0_Time));
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
			
			Lcd_Show_Operation( Operation_State_Machine, ((*p_Software_Version_high)*100 + (*p_Software_Version_low)));
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
			Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_Acceleration);
#else
			Operation_State_Machine = OPERATION_ADDR_SET;
			Lcd_Show_Operation( Operation_State_Machine, Operation_Addr_Value);
#endif
		break;
	}
}


// �� ������  �̰�
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
	//���� flash
	Memset_OPMode();//��flash
	//�˳�
	To_Free_Mode(1);
}

// �� + ��  ��ϼ�  �̰�   �л���λ 100�� or 5��
static void on_Button_1_2_Short_Press(void)
{

}

// �� + ��  ��ϼ�  �̰�   �л���λ 100�� or 5��
static void on_Button_1_3_Short_Press(void)
{

}


// �� + ��  ��ϼ�  �̰�   
static void on_Button_2_3_Short_Press(void)
{
}

// �� + ��  ��ϼ�  �̰�
static void on_Button_2_4_Short_Press(void)
{
}

//------------------- �����ص�   ���� ----------------------------

static void on_Button_1_Long_Press(void)
{
#ifdef OPERATION_P5_ACCELERATION
	if(Operation_State_Machine == OPERATION_P5_100_TIME)
	{
		if(Temp_Data_P5_100_Time < 60)
			(Temp_Data_P5_100_Time)++;
		else
			Temp_Data_P5_100_Time = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_100_Time);
	}
	else if(Operation_State_Machine == OPERATION_P5_0_TIME)
	{
		if(Temp_Data_P5_0_Time < 60)
			(Temp_Data_P5_0_Time)++;
		else
			Temp_Data_P5_0_Time = 1;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_0_Time);
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
		if(Temp_Data_P5_100_Time > 1)
			(Temp_Data_P5_100_Time)--;
		else
			Temp_Data_P5_100_Time = 60;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_100_Time);
	}
	else if(Operation_State_Machine == OPERATION_P5_0_TIME)
	{
		if(Temp_Data_P5_0_Time > 1)
			(Temp_Data_P5_0_Time)--;
		else
			Temp_Data_P5_0_Time = 60;
		
		Lcd_Show_Operation( Operation_State_Machine, Temp_Data_P5_0_Time);
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

// �ָ���������
static void on_Button_2_3_Long_Press(void)
{

}

//
static void on_Button_2_4_Long_Press(void)
{

}


