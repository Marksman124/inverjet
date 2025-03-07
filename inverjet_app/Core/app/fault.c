/**
******************************************************************************
* @file				fault.c
* @brief			故障模块 
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-13
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "fault.h"
#include "tm1621.h"
#include "adc.h"
#include "debug_protocol.h"
#include "key.h"
#include "ntc_3950.h"
#include "timing.h"
#include "wifi.h"
#include "down_conversion.h"

/* Private includes ----------------------------------------------------------*/


//uint8_t Fault_Recovery_Attempt_cnt=0;				//
/* Private function prototypes -----------------------------------------------*/

//------------------- 按键回调 ----------------------------
//--------------- 短按 -----------------------
// ① 上
void on_Fault_Button_1_clicked(void);
// ② 下
void on_Fault_Button_2_clicked(void);
// ③ 确认
void on_Fault_Button_3_clicked(void);
// ④ 取消
void on_Fault_Button_4_Short_Press(void);
void on_Fault_Button_1_2_Short_Press(void);
// ① + ③  组合键  短按   切换档位 80级 or 5级
void on_Fault_Button_1_3_Short_Press(void);
// ② + ③  组合键  短按
void on_Fault_Button_2_3_Short_Press(void);
// ② + ④  组合键  短按
void on_Fault_Button_2_4_Short_Press(void);
//--------------- 长按 -----------------------
// 长按
void on_Fault_Button_1_Long_Press(void);
void on_Fault_Button_2_Long_Press(void);
void on_Fault_Button_3_Long_Press(void);
void on_Fault_Button_4_Long_Press(void);
void on_Fault_Button_1_2_Long_Press(void);
void on_Fault_Button_1_3_Long_Press(void);
void on_Fault_Button_2_3_Long_Press(void);
void on_Fault_Button_2_4_Long_Press(void);

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define CHASSIS_TEMP_TIMER_MAX			9

/* Private variables ---------------------------------------------------------*/

uint16_t Fault_Label[16] = {0x001,0x002,0x003,0x004,0x005,0x006,
														0x101,0x102,
														0x201,0x202,0x203,
														0x301,0x302,0x303,0x304,
														0x401};

uint8_t Fault_Number_Sum = 0;	// 故障总数
uint8_t Fault_Number_Cnt = 0;	// 当前故障

// 发送缓冲区
uint8_t fault_send_buffer[24] = {0};


// 短按 槽函数
void (*p_Fault_Button[CALL_OUT_NUMBER_MAX])(void) = {
	on_Fault_Button_1_clicked,  on_Fault_Button_2_clicked,  on_Fault_Button_3_clicked,  on_Fault_Button_4_Short_Press,
	on_Fault_Button_1_2_Short_Press, on_Fault_Button_1_3_Short_Press,
	on_Fault_Button_2_3_Short_Press, on_Fault_Button_2_4_Short_Press,
};

// 长按 槽函数
void (*p_Fault_Long_Press[CALL_OUT_NUMBER_MAX])(void) = {
	on_Fault_Button_1_Long_Press,  on_Fault_Button_2_Long_Press,  on_Fault_Button_3_Long_Press,  on_Fault_Button_4_Long_Press,
	on_Fault_Button_1_2_Long_Press, on_Fault_Button_1_3_Long_Press,
	on_Fault_Button_2_3_Long_Press, on_Fault_Button_2_4_Long_Press,
};

static uint8_t E102_Fault_Timer_Cnt=0;

static uint8_t Automatic_Polling_Timer_Cnt=0;
/* Private user code ---------------------------------------------------------*/


// 初始化
void App_Fault_Init(void)
{
}

// 检查 故障状态 是否合法
uint8_t Fault_Check_Status_Legal(uint16_t parameter)
{
	if(parameter >= FAULT_STATE_MAX)
		return 0;
	
	return 1;
}

// 检测故障
uint8_t If_System_Is_Error(void)
{
	 float Temperature;
	//uint8_t motor_fault=0;
	uint16_t system_fault=0;
	int16_t vaule;
		
	if(System_is_Operation())//菜单
		return 0;
			
	//添加本地故障  新增功能 

	// 检查本地故障
	Temperature = Get_External_Temp();
	vaule = Temperature * 10;

	if(*p_Box_Temperature != vaule)
	{
		DEBUG_PRINT("机箱温度：%0.3f °C \n",Temperature);
		//memcpy(p_Box_Temperature, &vaule, 2);
		*p_Box_Temperature = vaule;
	}
	Check_Down_Conversion_BOX_Temperature((*p_Box_Temperature)/10);

	if(Temperature == -100)
	{
		if(E102_Fault_Timer_Cnt > 10)
			Set_Motor_Fault_State(E102_TEMPERATURE_AMBIENT);
		else
			E102_Fault_Timer_Cnt++;
	}
	else
	{
		if(E102_Fault_Timer_Cnt < 1)
			ReSet_Motor_Fault_State(E102_TEMPERATURE_AMBIENT);
		else
			E102_Fault_Timer_Cnt --;
	}
	
	//电机故障  含驱动板通讯故障
	system_fault = Get_Motor_Fault_State();
	
	if(*p_System_Fault_Static != system_fault)
	{
		if(system_fault >0)
		{
			if(Motor_Is_Software_Fault(system_fault))
			{
				if((Motor_Is_Software_Fault(*p_System_Fault_Static)==0) || (*p_System_Fault_Static == 0))
					Add_Fault_Recovery_Cnt(1);  //普通故障
			}
		}

		*p_System_Fault_Static = system_fault;
		
		//********  故障自动恢复    *********************
		// 通讯故障 可立刻恢复
		/*if(*p_System_Fault_Static == 0 )
		{
			CallOut_Fault_State();
		}*/
	}

	
	if(*p_System_Fault_Static > 0)
		return 1;
	else
		return 0;
}

// 设置故障值
void Set_Fault_Data(uint16_t type)
{
	
	*p_System_Fault_Static = type;
	
}


void Clean_Comm_Test(void)
{
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_RS485, 0);
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_KEY, 0);
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_DIAL_SWITCH, 0);
	
	System_Wifi_State_Clean();
	System_BT_State_Clean();
}


void Self_Testing_Check_Comm(void)
{
	if((*p_BLE_Rssi > BT_RSSI_ERROR_VAULE) || (*p_BLE_Rssi == 0) )
	{
		DEBUG_PRINT("蓝牙模组故障: 信号强度 %d dBm   ( 合格: %d dBm)\n",*p_BLE_Rssi, BT_RSSI_ERROR_VAULE);
		Set_Motor_Fault_State(E302_BT_HARDWARE);
	}
	else
		ReSet_Motor_Fault_State(E302_BT_HARDWARE);

	if(Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_RS485 ) == 0xAA)
	{
		ReSet_Motor_Fault_State(E303_RS485_HARDWARE);
		ReSet_Motor_Fault_State(E203_MOTOR_LOSS);
	}
	else if(Get_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_RS485 ) == 0x0A)
		Set_Motor_Fault_State(E203_MOTOR_LOSS);
	else
		Set_Motor_Fault_State(E303_RS485_HARDWARE);
	
	if(If_System_Is_Error())
	{
		Add_Fault_Recovery_Cnt(SYSTEM_FAULT_RECOVERY_MAX);//直接锁机
	}
}

//-------------------   ----------------------------

// 获取总数
uint8_t Get_Fault_Number_Sum(uint16_t para)
{
	uint8_t i;
	uint8_t result = 0;
	
	for(i=0; i<16; i++)
	{
		if(para & (1<<i))
			result ++;
	}
	
	return result;
}

// 获取故障号
uint8_t Get_Fault_Number_Now(uint16_t para, uint8_t num)
{
	uint8_t i;
	uint8_t result = 0;
	
	for(i=0; i<16; i++)
	{
		if(para & (1<<i))
		{
			result ++;
			if(num == result)
				return i+1;
		}
	}
	
	return 0;
}

void Fault_Number_Update(void)
{
	Fault_Number_Sum = Get_Fault_Number_Sum(*p_System_Fault_Static);
	
	if(Fault_Number_Cnt > Fault_Number_Sum)
		Fault_Number_Cnt = 1;
}

// 进入故障界面
void To_Fault_Menu(void)
{
	
	*p_System_State_Machine_Memory = Get_System_State_Machine();
	*p_PMode_Now_Memory = Get_System_State_Mode();
	if(Get_Down_Conversion_Speed_Old())
		*p_OP_ShowNow_Speed_Memory = Get_Down_Conversion_Speed_Old();
	else
		*p_OP_ShowNow_Speed_Memory = *p_OP_ShowNow_Speed;
	*p_OP_ShowNow_Time_Memory = *p_OP_ShowNow_Time;
	
	// 故障 菜单
	App_Fault_Init();
	
	//功能暂停
	Set_System_State_Machine(ERROR_DISPLAY_STATUS);
	//电机关闭
	*p_OP_ShowNow_Speed = 0;
	Motor_Quick_Stop();//故障立即停止  wuqingguang
	
	Fault_Number_Sum = Get_Fault_Number_Sum(*p_System_Fault_Static);
	
	Fault_Number_Cnt = Fault_Number_Sum;
	
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_System_Fault_Static,Fault_Number_Cnt));
	
	Clean_Automatic_Shutdown_Timer();  //自动关机
}
// 故障界面 更新
void Update_Fault_Menu(void)
{	
	if(++Automatic_Polling_Timer_Cnt > 4)//5
	{
		Automatic_Polling_Timer_Cnt = 0;
		if(Fault_Number_Cnt < Fault_Number_Sum)
			Fault_Number_Cnt ++;
		else
			Fault_Number_Cnt = 1;
	}
	
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_System_Fault_Static,Fault_Number_Cnt));
}

// 清除故障状态
void Clean_Fault_State(void)
{
	Clean_Motor_OffLine_Timer();
	*p_System_Fault_Static = 0;
	Fault_Number_Sum = 0;
	Fault_Number_Cnt = 0;
	E102_Fault_Timer_Cnt = 0;
	
	Set_System_State_Machine(*p_System_State_Machine_Memory);
	Set_System_State_Mode(*p_PMode_Now_Memory);
	*p_OP_ShowNow_Speed = *p_OP_ShowNow_Speed_Memory;
	*p_OP_ShowNow_Time = *p_OP_ShowNow_Time_Memory;
	//恢复后暂停
	//Arbitrarily_To_Pause();
	//恢复后直接启动
	Data_Set_Current_Speed(*p_OP_ShowNow_Speed_Memory);
}
/* Private function prototypes -----------------------------------------------*/

//------------------- 显示屏 & 接口 ----------------------------
/*
******************************************************************************
Display_Show_Number	

显示当前故障编号， 0-100
******************************************************************************
*/  
void Display_Show_Number(uint8_t no)
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

显示故障代码， E001 - E205
******************************************************************************
*/  
void Display_Show_FaultCode(uint16_t code)
{
	//字母 E
	TM1621_display_Letter(TM1621_COORDINATE_MIN_HIGH,  'E');
	TM1621_display_number(TM1621_COORDINATE_MIN_LOW,  	(code & 0x0F00)>>8);
	
	TM1621_display_number(TM1621_COORDINATE_SEC_HIGH,  	(code & 0x00F0)>>4);
	TM1621_display_number(TM1621_COORDINATE_SEC_LOW,  	(code & 0x000F));
	
	//TM1621_LCD_Redraw();
}
/*
******************************************************************************
Display_Show_Sum

显示故障总数
******************************************************************************
*/  
void Display_Show_Sum(uint8_t sum)
{
	TM1621_display_number(TM1621_COORDINATE_MODE_HIGH,  GET_NUMBER_TEN_DIGIT(sum));
	
	TM1621_display_number(TM1621_COORDINATE_MODE_LOW,  GET_NUMBER_ONE_DIGIT(sum));
	
	//TM1621_LCD_Redraw();
}

/***********************************************************************
*		显示 函数总入口
*
*
***********************************************************************/
void Lcd_Fault_Display(uint8_t sum, uint8_t now, uint16_t type)
{
	char show_mapping[9] = {0};
		
	if(System_is_Error() == 0)
	{
			return ;
	}

	Fault_Number_Update();
	//背光
	TM1621_BLACK_ON();
	
	// sum
	Display_Show_Sum(sum);
	Display_Show_Number(now);
	Display_Show_FaultCode(Fault_Label[type-1]);
	
	Lcd_Display_Symbol( LCD_Show_Bit & LCD_SYMBOL_FOT_FAULT);
	
	TM1621_LCD_Redraw();
	
	sprintf(show_mapping,"%02d%02dE%03x",sum,now,Fault_Label[type-1]);
	Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER, MB_LCD_MAPPING_SYMBOL, 0);
	Set_DataValue_Len(MB_FUNC_READ_INPUT_REGISTER,MB_LCD_MAPPING_MODE,(uint8_t *)show_mapping,8);
	
	return;
}

/* Private user code ---------------------------------------------------------*/
//------------------- 按键回调 ----------------------------

// ① 档位键
static void on_Fault_Button_1_clicked(void)
{
	if(Fault_Number_Cnt < Fault_Number_Sum)
		Fault_Number_Cnt ++;
	else
		Fault_Number_Cnt = 1;
	Automatic_Polling_Timer_Cnt = 0;
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_System_Fault_Static,Fault_Number_Cnt));
}

// ② 时间键
static void on_Fault_Button_2_clicked(void)
{
	if(Fault_Number_Cnt > 1)
		Fault_Number_Cnt --;
	else
		Fault_Number_Cnt = Fault_Number_Sum;
	Automatic_Polling_Timer_Cnt = 0;
	Lcd_Fault_Display(Fault_Number_Sum, Fault_Number_Cnt, Get_Fault_Number_Now(*p_System_Fault_Static,Fault_Number_Cnt));
}

// ③ 模式键
static void on_Fault_Button_3_clicked(void)
{

}


// ④ 开机键  短按
static void on_Fault_Button_4_Short_Press(void)
{
	
}

// ① + ②  组合键 
static void on_Fault_Button_1_2_Short_Press(void)
{
	
}


// ① + ③  组合键  短按   切换档位 100级 or 5级
static void on_Fault_Button_1_3_Short_Press(void)
{

}


// ② + ③  组合键  短按   
static void on_Fault_Button_2_3_Short_Press(void)
{
}

// ② + ④  组合键  短按
static void on_Fault_Button_2_4_Short_Press(void)
{
}

//------------------- 按键回调   长按 ----------------------------

static void on_Fault_Button_1_Long_Press(void)
{

}

static void on_Fault_Button_2_Long_Press(void)
{
}

static void on_Fault_Button_3_Long_Press(void)
{
}


static void on_Fault_Button_4_Long_Press(void)
{	
//	if(If_Fault_Recovery_Max()==1)
//	{
//		return;
//	}
		//System_Power_Off();
	CallOut_Fault_State();
	System_Power_Off();
}

static void on_Fault_Button_1_2_Long_Press(void)
{
	//   wifi配对
	Buzzer_Click_On();
	WIFI_Get_In_Distribution();
}

static void on_Fault_Button_1_3_Long_Press(void)
{
}

// 
static void on_Fault_Button_2_3_Long_Press(void)
{
	//   蓝牙配对
	Buzzer_Click_On();
	BT_Get_In_Distribution();
}

//
static void on_Fault_Button_2_4_Long_Press(void)
{

}


