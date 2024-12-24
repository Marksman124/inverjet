/**
******************************************************************************
* @file				tm1621.c
* @brief			tm1621 function implementation
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "tm1621.h"
#include <string.h>
#include "cmsis_os.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

#define NOP __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();

/********************** TM1261 模块命令 ********************************/

#define SYSDIS 		0x00
#define SYSEN			0x02
#define LCDOFF		0x04
#define LCDON			0x06
#define TONEON		0x12
#define TONEOFF		0x10

#define TONE_4K		0x80
#define TONE_2K		0xC0

#define XTAL 			0x28
#define RC				0x30
#define BIAS			0x52
#define WDTDIS 		0x0A
#define WDTEN			0x0E


//******************  调试模式 **************************
#ifdef SYSTEM_DEBUG_MODE
#define BUZZER_VOLUME_MAX					1						//20
#define BUZZER_TONE_CONFIG				TONE_2K
#else
#define BUZZER_VOLUME_MAX					200					// 200
#define BUZZER_TONE_CONFIG				TONE_4K			// TONE_4K
#endif
//*******************************************************


/* Private variables ---------------------------------------------------------*/

uint8_t Lcd_ram[SCREEN_NUMBER_MAX]={0};


static uint8_t Back_Light_Mode = 0;

// 数字显示0-9，任何显示数字的区域，都是调用这里的数据

//uint8_t Lcd_Number_table_1[10]={
//0x3f,0x06,
//0x5b,0x4f,
//0x66,0x3d,//0x6d
//0x7d,0x07,
//0x7f,0x6f};

// 1-4
uint8_t Lcd_Number_table_1[10]={0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F};

// 数字 5 -8
uint8_t Lcd_Number_table_2[10]={0xF5,0x60,0xD3,0xF2,0x66,0xB6,0xB7,0xE0,0xF7,0xF6};

// 字母显示 1-4
uint8_t Lcd_Letter_table_1[TM1621_LETTER_MAX][2] = {
	{0x77,'A'}, // A
	{0x59,'C'}, // C
	{0x79,'E'}, // E
	{0x71,'F'}, // F
	{0x76,'H'}, // H
	{0x0E,'J'}, // J
	{0x58,'L'}, // L
	{0x5F,'O'}, // O
	{0x73,'P'}, // P
	{0x3D,'S'}, // S
	{0x5E,'U'}, // U
	{0x7C,'b'}, // b
	{0x68,'c'}, // c
	{0x6E,'d'}, // d
	{0x74,'h'}, // h
	{0x40,'i'}, // i
	{0x64,'n'}, // n
	{0x6C,'o'}, // o
	{0x60,'r'}, // r
	{0x4C,'u'}, // u
};

// 字母显示 5-8
uint8_t Lcd_Letter_table_2[TM1621_LETTER_MAX][2] = {
	{0xE7,'A'},
	{0x95,'C'}, // C
	{0x97,'E'}, // E
	{0x87,'F'}, // F
	{0x67,'H'}, // H
	{0x70,'J'}, // J
	{0x15,'L'}, // L
	{0xF5,'O'}, // O
	{0xC7,'P'}, // P
	{0xB6,'S'}, // S
	{0x75,'U'}, // U
	{0x37,'b'}, // b
	{0x13,'c'}, // c
	{0x73,'d'}, // d
	{0x27,'h'}, // h
	{0x01,'i'}, // i
	{0x23,'n'}, // n
	{0x33,'o'}, // o
	{0x03,'r'}, // r
	{0x31,'u'} // u
};



//extern TIM_HandleTypeDef htim3;
/* Private user code ---------------------------------------------------------*/

static void Tm1621_Delay(uint32_t mdelay)
{
  uint32_t Delay = mdelay * (30);
  do
  {
    __NOP();
  }
  while (Delay --);
				
	//__HAL_TIM_SET_COUNTER(&htim3,0);
	//while(__HAL_TIM_GET_COUNTER(&htim3) < mdelay);
}

/**************************************************************************************
* FunctionName   : TM1621_SendBitMsb()
* Description    : 发送发送多位[高位在前]
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendBitMsb(uint8_t dat, uint8_t cnt)
{
	for (uint8_t i=0; i<cnt; i++)
	{
		(dat & 0x80) ?  TM1621_DATA_HIG() :
                        TM1621_DATA_LOW();
		dat <<= 1;
		TM1621_WR_LOW();
		Tm1621_Delay(3);
		TM1621_WR_HIG();
	}
}

/**************************************************************************************
* FunctionName   : TM1621_SendBitLsb()
* Description    : 发送多位[低位在前]
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendBitLsb(uint8_t dat, uint8_t cnt)
{
	for (uint8_t i=0; i<cnt; i++)
	{
		(dat & 0x01) ?  TM1621_DATA_HIG() :TM1621_DATA_LOW();
		dat >>= 1;
		TM1621_WR_LOW();
		Tm1621_Delay(3);
		TM1621_WR_HIG();
	}
}

///**************************************************************************************
//* FunctionName   : TM1621_SendCmd()
//* Description    : 发送命令
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void TM1621_SendCmd(uint8_t cmd)
{
    TM1621_CS_LOW();
    TM1621_SendBitMsb(0x80, 3);                                                     // 前面3位命令代码
    TM1621_SendBitMsb(cmd, 9);                                                      // 后面10位: a5~a0[RAM地址]+d3~d0[RAM数据]
    TM1621_CS_HIG();
}

/**************************************************************************************
* FunctionName   : HTBSendNDat()
* Description    : 发送N数据
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendNDat(uint8_t addr, uint8_t *pDat, uint8_t cnt, uint8_t bitNum)
{
    TM1621_CS_LOW();
    TM1621_SendBitMsb(0xA0, 3);                                                    // 前面3位命令代码
    TM1621_SendBitMsb(addr<<2, 6);                                                 // a5~a0[RAM地址]

    for (uint8_t i=0; i<cnt; i++)
    {        
			TM1621_SendBitLsb(*pDat++, bitNum);   // RAM数TM1621_SendBitLsb据
    }
    TM1621_CS_HIG();
}


/**
  * @brief  TM1621 Write CMD.
  * @param  cmd 指向写入的命令.
  * @return void
  */
void TM1621_Write_CMD(uint8_t cmd)
{
	TM1621_CS_LOW();
  TM1621_SendBitMsb(0x80, 4);                                                    // 前面3位命令代码
  TM1621_SendBitMsb(cmd, 8);                                                 // a5~a0[RAM地址]
  TM1621_CS_HIG();
}

/*
******************************************************************************
TM1621_display_number	

显示缓存
******************************************************************************
*/  
void TM1621_display_number(uint8_t coordinate, uint8_t value)
{
	uint8_t symbol;
	uint8_t* p_number_table;
	
	if(coordinate < 4)
	{
		p_number_table = Lcd_Number_table_2;
		symbol = Lcd_ram[coordinate] & 0x08;
	}
	else
	{
		p_number_table = Lcd_Number_table_1;
		symbol = Lcd_ram[coordinate] & 0x80;
	}
	
	if(value == 0xFF)// 不显示
		Lcd_ram[coordinate] = 0;
	else
		Lcd_ram[coordinate] = p_number_table[value];
	
	Lcd_ram[coordinate] |= symbol;
	
}

/*
******************************************************************************
TM1621_display_Letter	

显示 字母 value:字母的ascii码
******************************************************************************
*/  
void TM1621_display_Letter(uint8_t coordinate, uint8_t value)
{
	uint8_t symbol;
	uint8_t i;
	
	uint8_t(* p_letter_table)[2];
	
	if(coordinate < 4)
	{
		p_letter_table = Lcd_Letter_table_2;
		symbol = Lcd_ram[coordinate] & 0x08;
	}
	else
	{
		p_letter_table = Lcd_Letter_table_1;
		symbol = Lcd_ram[coordinate] & 0x80;
	}
	
	if(value == 0xFF)// 不显示
		Lcd_ram[coordinate] = 0;
	else
	{
		for(i=0; i<TM1621_LETTER_MAX; i++)
		{
			if(p_letter_table[i][1] == value)
				Lcd_ram[coordinate] = p_letter_table[i][0];
		}
	}
	Lcd_ram[coordinate] |= symbol;

}
/*
******************************************************************************
TM1621_Show_Colon	

显示符号
******************************************************************************
*/  
void TM1621_Show_Symbol(uint8_t coordinate, uint8_t value)
{
	
	if(coordinate < 4)
	{
		if(value)
			Lcd_ram[coordinate]  |= 0x08;
		else
			Lcd_ram[coordinate]  &= 0xF7;
	}
	else
	{
		if(value)
			Lcd_ram[coordinate]  |= 0x80;
		else
			Lcd_ram[coordinate]  &= 0x7F;
	}
}

/*
******************************************************************************
TM1621_LCD_Redraw	

屏幕刷新
******************************************************************************
*/ 
void TM1621_LCD_Redraw(void)
{
 	TM1621_SendNDat(0,Lcd_ram,SCREEN_NUMBER_MAX,SCREEN_NUMBER_MAX);
}


/*
******************************************************************************
TM1621_Show_LCD_Test	

测试显示, 全部显示一个符号
******************************************************************************
*/ 
void TM1621_Show_LCD_Test(uint8_t value)
{
	for( uint8_t i=0; i<SCREEN_NUMBER_MAX; i++)
	{
		if(	value < 10 )
			TM1621_display_number(i, value);
		else
			TM1621_display_Letter(i, value);
	}
	TM1621_LCD_Redraw();
}

/*
******************************************************************************
TM1621_Show_Repeat_All	

屏幕循环显示
******************************************************************************
*/ 
void TM1621_Show_Repeat_All(void)
{
	uint8_t i;
	
	for( i=0; i<10; i++)
	{
		TM1621_Show_LCD_Test(i);
		//Tm1621_Delay(3000*100);
		osDelay(1100);
	}
	
//	for( i=0; i<TM1621_LETTER_MAX; i++)
//	{
//		TM1621_Show_LCD_Test( Lcd_Letter_table_1[i][1]);
//		//Tm1621_Delay(3000*100);
//		osDelay(1000);
//	}

}

/*
******************************************************************************
TM1621_Show_All	

全显示
******************************************************************************
*/ 
void TM1621_Show_All(void)
{
	uint8_t show_all[SCREEN_NUMBER_MAX] = { 0xF7,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0xFF};
	//背光
	TM1621_BLACK_ON();
	
	memcpy(Lcd_ram, show_all, SCREEN_NUMBER_MAX);
	
 	TM1621_LCD_Redraw();
}
/*
******************************************************************************
TM1621_Show_Off	

息屏
******************************************************************************
*/ 
void TM1621_Show_Off(void)
{	
	memset(Lcd_ram, 0, SCREEN_NUMBER_MAX);
	
 	TM1621_LCD_Redraw();
}

//------------------- pwm控制 ----------------------------
// 调节范围 : 1 - 100
void Buzzer_IO_PwmOut(uint16_t pul)
{
	HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_3);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, pul);//pul
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
}

/*
******************************************************************************
TM1621_Show_Off	

息屏
******************************************************************************
*/ 
void TM1621_Buzzer_Off(void)
{	
	Buzzer_IO_PwmOut(0);
	//TM1621_Write_CMD(TONEOFF);
}
/*
******************************************************************************
TM1621_Show_Off	

息屏
******************************************************************************
*/ 
void TM1621_Buzzer_On(void)
{	
	Buzzer_IO_PwmOut(BUZZER_FREQUENCY);
	//TM1621_Write_CMD(TONEON);
}

void TM1621_Buzzer_Delay(uint16_t ms)
{
/* 启动定时器 */
	__HAL_TIM_CLEAR_IT(&htim6,TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim6,TIM_IT_UPDATE);
	__HAL_TIM_SET_COUNTER(&htim6,0);
	__HAL_TIM_SET_AUTORELOAD(&htim6,ms*10);
	__HAL_TIM_ENABLE(&htim6);
	HAL_TIM_Base_Start_IT(&htim6);
}

// 鸣笛  不可打断,尽量不要用这个
void TM1621_Buzzer_Whistle(uint16_t ms) 
{
	TM1621_Buzzer_On();
	TM1621_Buzzer_Delay(ms);
	TM1621_Buzzer_Off();
}

// 嘀一下
void TM1621_Buzzer_Click(void) 
{
	TM1621_Buzzer_Off();
	//-------------- 蜂鸣器 长度 -------------------

	TM1621_Buzzer_On();
	//用定时器不稳定, 在线程循环里计时
	//TM1621_Buzzer_Delay(BUZZER_VOLUME_MAX);
	//TM1621_Buzzer_Off();

//*******************************************************
}

void TM1621_Buzzer_Init(void) 
{
	Tm1621_Delay(10);
	//蜂鸣器
	TM1621_Write_CMD(BUZZER_TONE_CONFIG);
	TM1621_Write_CMD(TONEOFF);
}
/*
******************************************************************************
LCD_Init	
初始化命令到HT1621 
******************************************************************************
*/  
 
void TM1621_LCD_Init(void) 
{	
	TM1621_CS_HIG();
	TM1621_DATA_HIG();
	TM1621_WR_HIG();
	
	Tm1621_Delay(2000); //延时使LCD工作电压稳定
	
	TM1621_Write_CMD(BIAS);
	TM1621_Write_CMD(RC);
	TM1621_Write_CMD(SYSDIS);
	TM1621_Write_CMD(WDTDIS);
	TM1621_Write_CMD(SYSEN);
	TM1621_Write_CMD(LCDON);
	
}



//------------------- pwm控制 ----------------------------

void TM1621_light_Max(void)
{
	HAL_TIM_PWM_Stop_IT(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL);
	__HAL_TIM_SetCompare(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL, BACK_LIGHT_BRIGHTNESS_MAX);
	HAL_TIM_PWM_Start(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL);
}

void TM1621_light_Half(void)
{
	HAL_TIM_PWM_Stop_IT(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL);
	__HAL_TIM_SetCompare(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL, BACK_LIGHT_BRIGHTNESS_MAX/2);
	HAL_TIM_PWM_Start(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL);
}

void TM1621_light_Off(void)
{
	HAL_TIM_PWM_Stop_IT(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL);
	__HAL_TIM_SetCompare(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL, 0);
	HAL_TIM_PWM_Start(&htim2, LCD_BACK_LIGHT_PWM_CHANNEL);
}


void TM1621_light_On(void)
{
	if(Back_Light_Mode == 0)
		TM1621_light_Max();
	else
		TM1621_light_Half();
}

void TM1621_Set_light_Mode(uint8_t mode)
{
	Back_Light_Mode = mode;

}
