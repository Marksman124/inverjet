/**
******************************************************************************
* @file				display.c
* @brief			��ʾģ��  ��ʾģ�鱾��ռ���߳�,�������߳��������
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "display.h"
#include "tm1621.h"
#include "key.h"
#include "wifi.h"
#include "debug_protocol.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
UART_HandleTypeDef* p_huart_display = &huart3;		 //���Դ��� UART���

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

// ��ʾͼ��
uint8_t LCD_Show_Bit;

// ���ͻ�����
uint8_t send_buffer[24] = {0};

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- Ӳ�� & ���� ----------------------------
// ��ʼ��
void App_Display_Init(void)
{
	
	App_State_Machine_Init();
	
	LCD_Show_Bit = 0;
	
	Led_Button_On(0x0F);	// ����
}
//------------------- ��ʾ�� & �ӿ� ----------------------------
/*
******************************************************************************
Display_Show_Speed	

��ʾ�ٶȣ� 0-100
******************************************************************************
*/  
void Display_Show_Speed(uint8_t speed)
{
	if(speed > 100)
		speed = 100;
	
	TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, GET_NUMBER_HUNDRED_DIGIT(speed));
	if(speed <10)
		TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, 0xFF);
	else	
		TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, GET_NUMBER_TEN_DIGIT(speed));
	TM1621_display_number(TM1621_COORDINATE_SPEED_LOW, GET_NUMBER_ONE_DIGIT(speed));
	
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_Min	

��ʾ���ӣ� 0-99
******************************************************************************
*/  
void Display_Show_Min(uint8_t min)
{
	TM1621_display_number(TM1621_COORDINATE_MIN_HIGH, GET_NUMBER_TEN_DIGIT(min));
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  GET_NUMBER_ONE_DIGIT(min));
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_Sec	

��ʾ �룬 0-60
******************************************************************************
*/  
void Display_Show_Sec(uint8_t sec)
{
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH, GET_NUMBER_TEN_DIGIT(sec));
	TM1621_display_number(TM1621_COORDINATE_SEC_LOW, GET_NUMBER_ONE_DIGIT(sec));
	
	//TM1621_LCD_Redraw();
}

/*
******************************************************************************
Display_Show_Mode	

��ʾģʽ�� P1~P3
******************************************************************************
*/  
void Display_Show_Mode(uint8_t mode)
{
	//��ĸ p
	TM1621_display_Letter(TM1621_COORDINATE_MODE_HIGH,  'P');
	
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  GET_NUMBER_ONE_DIGIT(mode));
	
	//TM1621_LCD_Redraw();
}
//------------------- ��ʾ�� Ϩ�� & �ӿ� ----------------------------
/*
******************************************************************************
Display_Show_Speed	

���� �ٶȣ� 0-100
******************************************************************************
*/  
void Display_Hide_Speed(uint8_t para)
{
	if(para & (1<<2))
	{
		TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, 0);
		
	}
	if(para & (1<<1))
		TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, 0xFF);
	if(para & (1<<0))
		TM1621_display_number(TM1621_COORDINATE_SPEED_LOW, 0xFF);
}
/*
******************************************************************************
Display_Hide_Min	

���� ����  0-99
******************************************************************************
*/  
void Display_Hide_Min(uint8_t para)
{
	if(para & (1<<1))
		TM1621_display_number(TM1621_COORDINATE_MIN_HIGH, 0xFF);
	if(para & (1<<0))
		TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  0xFF);
}
/*
******************************************************************************
Display_Hide_Sec	

���� �� 0-60
******************************************************************************
*/  
void Display_Hide_Sec(uint8_t para)
{
	if(para & (1<<1))
		TM1621_display_number(TM1621_COORDINATE_SEC_HIGH, 0xFF);
	if(para & (1<<0))
		TM1621_display_number(TM1621_COORDINATE_SEC_LOW,  0xFF);
}

/*
******************************************************************************
Display_Hide_Mode	

���� ģʽ�� P1~P3
******************************************************************************
*/  
void Display_Hide_Mode(uint8_t para)
{
	if(para & (1<<1))
		TM1621_display_number(TM1621_COORDINATE_MODE_HIGH, 0xFF);
	if(para & (1<<0))
		TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  0xFF);
}

//------------------- ��ʾ & ���� ----------------------------

void Lcd_Display_Symbol(uint8_t status_para)
{
	TM1621_Show_Symbol(TM1621_COORDINATE_BLUETOOTH, 		(status_para & STATUS_BIT_BLUETOOTH));
	TM1621_Show_Symbol(TM1621_COORDINATE_PERCENTAGE, 		(status_para & STATUS_BIT_PERCENTAGE));
	TM1621_Show_Symbol(TM1621_COORDINATE_WIFI, 					(status_para & STATUS_BIT_WIFI));
	TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		(status_para & STATUS_BIT_COLON));
	TM1621_Show_Symbol(TM1621_COORDINATE_DECIMAL_POINT, (status_para & STATUS_BIT_POINT));
	
	return;
}

void Lcd_Test(uint8_t num)
{
	Display_Show_Speed(num*11);
	Display_Show_Min((num*11));
	Display_Show_Sec((num*11));
	
	return;
}

void Lcd_Display(uint16_t speed, uint16_t time, uint8_t status_para, uint8_t mode)
{
	char show_mapping[9] = {0};
	
	//speed
	Display_Show_Speed(speed);
	// time
	Display_Show_Min(GET_TIME_MINUTE_DIGIT(time));
	Display_Show_Sec(GET_TIME_SECOND_DIGIT(time));
	// mode
	if(System_Mode_Free())
		Display_Show_Mode(0);
	else if(System_Mode_Train())
		Display_Show_Mode(mode);
	else
		Display_Hide_Mode(0xFF);
	
	
	Lcd_Display_Symbol(status_para);
	
	sprintf(show_mapping,"P%01d%02d%02d%02d",mode,speed%100,GET_TIME_MINUTE_DIGIT(time),GET_TIME_SECOND_DIGIT(time));
	if(System_Mode_Time())
	{
		show_mapping[0] = 0xFF;
		show_mapping[1] = 0xFF;
	}
	if(speed < 10)
		show_mapping[2] = 0xFF;
	
	if(speed >= 100)
		Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER, MB_LCD_MAPPING_SYMBOL, 7);
	else
		Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER, MB_LCD_MAPPING_SYMBOL, 6);
	
	Set_DataValue_Len(MB_FUNC_READ_INPUT_REGISTER,MB_LCD_MAPPING_MODE,(uint8_t *)show_mapping,8);

	return;
}



// Ϣ��
void Lcd_Off(void)
{
	//����
	TM1621_BLACK_OFF();
	
	TM1621_Show_Off();
}


// �ٶ� Ϩ��
void Lcd_No_Speed(uint16_t time, uint8_t status_para, uint8_t mode)
{
	//speed
	Display_Hide_Speed(0xFF);
	// time
	Display_Show_Min(GET_TIME_MINUTE_DIGIT(time));
	Display_Show_Sec(GET_TIME_SECOND_DIGIT(time));
	
	// mode
	if(System_Mode_Free())
		Display_Show_Mode(0);
	else if(System_Mode_Train())
		Display_Show_Mode(mode);
	else
		Display_Hide_Mode(0xFF);
	
	Lcd_Display_Symbol(status_para);
}

//------------------- �ⲿ�ӿ�  ----------------------------
/***********************************************************************
*		��ʾ ���������
*
*
***********************************************************************/
void Lcd_Show(void)
{
	if(System_is_Error())
	{
		Lcd_Display_Symbol( LCD_Show_Bit & LCD_SYMBOL_FOT_FAULT);
		TM1621_LCD_Redraw();
		return ;
	}
	
	if(System_is_Operation() || System_is_Power_Off()|| (System_Self_Testing_State == 0xAA))
	{
			return ;
	}
	
	//����
	TM1621_BLACK_ON();
	
	LCD_Refresh_Restore();//�ָ�ˢ��
	taskENTER_CRITICAL();
	//
	Lcd_Display(*p_OP_ShowNow_Speed, *p_OP_ShowNow_Time, LCD_Show_Bit,Get_System_State_Mode());
	
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
}

/***********************************************************************
*		��ʾ ����
*
*
***********************************************************************/
void Lcd_Show_Upgradation(uint8_t sum, uint8_t num)
{
	//uint8_t schedule=0;
	
	//����
	TM1621_BLACK_ON();
	

	taskENTER_CRITICAL();
	//��ǰ��
	//schedule = (num*100)/sum;
	
	Display_Show_Speed(num);
	TM1621_Show_Symbol(TM1621_COORDINATE_PERCENTAGE, 		1);
	//TM1621_Show_Symbol(TM1621_COORDINATE_BLUETOOTH, 		1);
	
	TM1621_Show_Symbol(TM1621_COORDINATE_TIME_COLON, 		0);
	TM1621_Show_Symbol(TM1621_COORDINATE_DECIMAL_POINT, 0);
	//TM1621_Show_Symbol(TM1621_COORDINATE_WIFI, 					0);
	
	
	//�ܰ���
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH, (sum/10)%10);
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  	sum%10);
	
	//����
	TM1621_display_Letter(TM1621_COORDINATE_MIN_HIGH, 	'U');
	TM1621_display_Letter(TM1621_COORDINATE_MIN_LOW, 		'P');
	TM1621_display_Letter(TM1621_COORDINATE_SEC_HIGH,  	'd');
	TM1621_display_Letter(TM1621_COORDINATE_SEC_LOW,   	'A');

	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
}

// ������ & ����
void Lcd_System_Information(void)
{
	//����
	TM1621_BLACK_ON();
	
	//	���뿪��
	System_Dial_Switch = Gpio_Get_Dial_Switch();
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH,  GET_NUMBER_TEN_DIGIT(System_Dial_Switch));
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  GET_NUMBER_ONE_DIGIT(System_Dial_Switch));
	
	//speed
	//Display_Hide_Speed(0xFF);
	//TM1621_display_number(TM1621_COORDINATE_SPEED_HIGH, 5);
	TM1621_display_Letter(TM1621_COORDINATE_SPEED_HIGH, 'S');
	TM1621_display_Letter(TM1621_COORDINATE_SPEED_LOW, 'J');
	// time
	if(SYSTEM_PRODUCT_MODEL_CODE < 1000)
		TM1621_display_number(TM1621_COORDINATE_MIN_HIGH, 0xFF);
	else
		TM1621_display_number(TM1621_COORDINATE_MIN_HIGH, (SYSTEM_PRODUCT_MODEL_CODE/1000)%10);
	
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  (SYSTEM_PRODUCT_MODEL_CODE/100)%10);
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH,  (SYSTEM_PRODUCT_MODEL_CODE/10)%10);
	TM1621_display_number(TM1621_COORDINATE_SEC_LOW,  (SYSTEM_PRODUCT_MODEL_CODE)%10);
	
	Lcd_Display_Symbol(0);
	TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, 		0);
	
	TM1621_LCD_Redraw();
}

// �ٶ� Ϩ��
void Lcd_Speed_Off(void)
{
	//���� ��
	//TM1621_BLACK_OFF()
	//
	taskENTER_CRITICAL();
	Lcd_No_Speed(*p_OP_ShowNow_Time, LCD_Show_Bit,Get_System_State_Mode());
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
}

// ���� ���� 2��1ˢ
void Lcd_Show_Slow_Down(uint8_t value)
{
	taskENTER_CRITICAL();
	
	TM1621_Show_Symbol(TM1621_COORDINATE_SPEED_HUNDRED, 0);
	TM1621_display_Letter(TM1621_COORDINATE_SPEED_HIGH, 'A');
	TM1621_display_number(TM1621_COORDINATE_SPEED_LOW, value);
	// time
	Display_Show_Min(GET_TIME_MINUTE_DIGIT(*p_OP_ShowNow_Time));
	Display_Show_Sec(GET_TIME_SECOND_DIGIT(*p_OP_ShowNow_Time));
	
	TM1621_LCD_Redraw();
	taskEXIT_CRITICAL();
}


/*
******************************************************************************
*	ģʽ�л�
******************************************************************************
*/

//-------------  To-->�ػ� ------------------------------------
void To_Power_Off(void)
{
	System_Self_Testing_State = 0;
	
	Set_System_State_Machine(POWER_OFF_STATUS);		// ״̬��
	*p_OP_ShowNow_Speed = 0;											// ��ǰ�ٶ�
	*p_OP_ShowNow_Time = 0;												// ��ǰʱ��
	
	Special_Status_Bit = 0;

	Lcd_Off();
}

//-------------  To-->����ģʽ ------------------------------------
void To_Free_Mode(uint8_t mode)
{
	if(mode == 0)
		Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	else
		Special_Status_Add(SPECIAL_BIT_SKIP_INITIAL);
	
	Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
	
	Set_System_State_Machine(FREE_MODE_INITIAL);
	*p_OP_ShowLater = *p_OP_Free_Mode;

	*p_OP_ShowNow_Speed = p_OP_ShowLater->speed;
	*p_OP_ShowNow_Time  = p_OP_ShowLater->time;
	
	LCD_Show_Bit |= STATUS_BIT_PERCENTAGE;
	Lcd_Show();
}

//-------------  To-->��ʱģʽ ------------------------------------
void To_Timing_Mode(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
	Set_System_State_Machine(TIMING_MODE_INITIAL);

	*p_OP_ShowLater = *p_OP_Timing_Mode;

	*p_OP_ShowNow_Speed = p_OP_ShowLater->speed;
	*p_OP_ShowNow_Time  = p_OP_ShowLater->time;
	
	LCD_Show_Bit |= STATUS_BIT_PERCENTAGE;
	Lcd_Show();
}

//-------------  To-->ѵ��ģʽ  num:1-4 ------------------------------------
void To_Train_Mode(uint8_t num)
{
	uint8_t plan=1;
	
	if(Is_Mode_Legal(num) == 0)
		plan = 1;
	else
		plan = num;
	
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	Set_System_State_Machine(TRAINING_MODE_INITIAL);
	Set_System_State_Mode(plan);
	Special_Status_Delete(SPECIAL_BIT_SKIP_STARTING);
	
	p_OP_ShowLater->speed = p_OP_PMode[plan-1][0].speed;
	p_OP_ShowLater->time = 0;
	
	*p_OP_ShowNow_Speed = p_OP_PMode[plan-1][0].speed;
	*p_OP_ShowNow_Time = 0;
	
	LCD_Show_Bit |= STATUS_BIT_PERCENTAGE;
	
	
	Lcd_Show();
}


//-------------  To-->�Բ� ------------------------------------
void System_Self_Testing_Porgram(void)
{
	System_Self_Testing_State = 0xAA;
	//wifi ����
	
	mcu_start_wifitest();
	
	Led_Button_On(0);	// ����
	// ��Ļ
	TM1621_BLACK_ON();
	TM1621_Show_All();//ȫ�� 2s
	osDelay(2000);
	TM1621_Show_Repeat_All();//ȫ��ѭ��
	//
}

extern TaskHandle_t Breath_Light_TaHandle;
extern TaskHandle_t Rs485_Modbus_TaHandle;
extern TaskHandle_t Main_TaskHandle;
extern TaskHandle_t Key_Button_TaskHandle;
extern TaskHandle_t Motor_TaskHandle;
extern TaskHandle_t wifi_moduleHandle;

void Freertos_TaskSuspend_Wifi(void)
{
	// ��ͣ����
	osThreadSuspend(Key_Button_TaskHandle);
	osThreadSuspend(Breath_Light_TaHandle);
	osThreadSuspend(Rs485_Modbus_TaHandle);
	osThreadSuspend(Main_TaskHandle);
	//osThreadSuspend(Motor_TaskHandle);
	//osThreadSuspend(wifi_moduleHandle);
}



void Freertos_TaskSuspend_RS485(void)
{
	// ��ͣ����
	osThreadSuspend(Key_Button_TaskHandle);
	osThreadSuspend(Breath_Light_TaHandle);
	//osThreadSuspend(Rs485_Modbus_TaHandle);
	osThreadSuspend(Main_TaskHandle);
	//osThreadSuspend(Motor_TaskHandle);
	osThreadSuspend(wifi_moduleHandle);
	
}




void Freertos_TaskResume_All(void)
{
	// �ָ�����
	osThreadResume(Key_Button_TaskHandle);
	osThreadResume(Breath_Light_TaHandle);
	osThreadResume(Rs485_Modbus_TaHandle);
	osThreadResume(Main_TaskHandle);
	osThreadResume(Motor_TaskHandle);
	osThreadResume(wifi_moduleHandle);
}


