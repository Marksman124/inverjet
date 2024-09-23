/**
******************************************************************************
* @file    		tm1621.h
* @brief   		tm1621 function implementation
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TM1621_H__
#define __TM1621_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "macro_definition.h"				// ͳһ�궨��
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/



/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/


#if(LCD_BACK_LIGHT_PWM_CTRL)
//PWM ����
#define TM1621_BLACK_ON()			TM1621_light_On();
#define TM1621_BLACK_OFF()		TM1621_light_Off();
#else
//IO����
#define TM1621_BLACK_ON()			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
#define TM1621_BLACK_OFF()		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
#endif



#define TM1621_CS_HIG()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define TM1621_CS_LOW()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)

#define TM1621_DATA_HIG()		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)
#define TM1621_DATA_LOW()		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)

#define TM1621_WR_HIG()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define TM1621_WR_LOW()			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)


// ���ټ���
#define GET_NUMBER_HUNDRED_DIGIT(n)						((n>=100)?1:0)
#define GET_NUMBER_TEN_DIGIT(n)								((n/10)%10)
#define GET_NUMBER_ONE_DIGIT(n)								(n%10)

#define GET_TIME_MINUTE_DIGIT(n)							((n/60)%100)
#define GET_TIME_SECOND_DIGIT(n)							(n%60)

#define GET_LETTER_HIGH_DIGIT(n)							((n&0xF0)>>4)
#define GET_LETTER_LOW_DIGIT(n)								(n&0x0F)

/* ��Ļ ����  ------------------------------------------------------------*/

#define SCREEN_NUMBER_MAX			8		// ��Ļ���ֵ�Ԫ����

#define TM1621_LETTER_MAX			20	//��ĸ����

#define TM1621_DIGITAL_NUMBER_1			7
#define TM1621_DIGITAL_NUMBER_2			6
#define TM1621_DIGITAL_NUMBER_3			5
#define TM1621_DIGITAL_NUMBER_4			4
#define TM1621_DIGITAL_NUMBER_5			0
#define TM1621_DIGITAL_NUMBER_6			1
#define TM1621_DIGITAL_NUMBER_7			2
#define TM1621_DIGITAL_NUMBER_8			3

#define TM1621_SYMBOL_S1			7
#define TM1621_SYMBOL_S2			4
#define TM1621_SYMBOL_S3			5
#define TM1621_SYMBOL_S4			1
#define TM1621_SYMBOL_S5			2
#define TM1621_SYMBOL_S6			3



#define TM1621_COORDINATE_SPEED_HIGH				TM1621_DIGITAL_NUMBER_1
#define TM1621_COORDINATE_SPEED_LOW					TM1621_DIGITAL_NUMBER_2
#define TM1621_COORDINATE_MIN_HIGH					TM1621_DIGITAL_NUMBER_5
#define TM1621_COORDINATE_MIN_LOW						TM1621_DIGITAL_NUMBER_6
#define TM1621_COORDINATE_SEC_HIGH					TM1621_DIGITAL_NUMBER_7
#define TM1621_COORDINATE_SEC_LOW						TM1621_DIGITAL_NUMBER_8
#define TM1621_COORDINATE_MODE_HIGH					TM1621_DIGITAL_NUMBER_3
#define TM1621_COORDINATE_MODE_LOW					TM1621_DIGITAL_NUMBER_4

#define TM1621_COORDINATE_SPEED_HUNDRED			TM1621_SYMBOL_S1
#define TM1621_COORDINATE_TIME_COLON				TM1621_SYMBOL_S4
#define TM1621_COORDINATE_DECIMAL_POINT			TM1621_SYMBOL_S5
#define TM1621_COORDINATE_BLUETOOTH					TM1621_SYMBOL_S2
#define TM1621_COORDINATE_PERCENTAGE				TM1621_SYMBOL_S3
#define TM1621_COORDINATE_WIFI							TM1621_SYMBOL_S6



/* Exported functions prototypes ---------------------------------------------*/

//------------------- ���ܽӿ� ----------------------------
void Delay_us(uint16_t us);
/**************************************************************************************
* FunctionName   : TM1621_SendBitMsb()
* Description    : ���ͷ��Ͷ�λ[��λ��ǰ]
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendBitMsb(uint8_t dat, uint8_t cnt);
/**************************************************************************************
* FunctionName   : TM1621_SendBitLsb()
* Description    : ���Ͷ�λ[��λ��ǰ]
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendBitLsb(uint8_t dat, uint8_t cnt);
///**************************************************************************************
//* FunctionName   : TM1621_SendCmd()
//* Description    : ��������
//* EntryParameter : None
//* ReturnValue    : None
//**************************************************************************************/
void TM1621_SendCmd(uint8_t cmd);
/**************************************************************************************
* FunctionName   : HTBSendNDat()
* Description    : ����N����
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void TM1621_SendNDat(uint8_t addr, uint8_t *pDat, uint8_t cnt, uint8_t bitNum);
/**
  * @brief  TM1621 Write CMD.
  * @param  cmd ָ��д�������.
  * @return void
  */
void TM1621_Write_CMD(uint8_t cmd);
/*
******************************************************************************
TM1621_display_number	

��ʾ����
******************************************************************************
*/  
void TM1621_display_number(uint8_t coordinate, uint8_t value);
/*
******************************************************************************
TM1621_display_Letter	

��ʾ ��ĸ
******************************************************************************
*/  
void TM1621_display_Letter(uint8_t coordinate, uint8_t value);
/*
******************************************************************************
TM1621_Show_Colon	

��ʾ����
******************************************************************************
*/  
void TM1621_Show_Symbol(uint8_t coordinate, uint8_t value);
/*
******************************************************************************
TM1621_LCD_Redraw	

��Ļˢ��
******************************************************************************
*/ 
void TM1621_LCD_Redraw(void);
/*
******************************************************************************
TM1621_Show_Repeat_All	

��Ļѭ����ʾ
******************************************************************************
*/ 
void TM1621_Show_Repeat_All(void);
/*
******************************************************************************
TM1621_Show_All	

ȫ��ʾ
******************************************************************************
*/ 
void TM1621_Show_All(void);
/*
******************************************************************************
TM1621_Show_Off	

Ϣ��
******************************************************************************
*/ 
void TM1621_Show_Off(void);

/*******************************************************************************
������
*******************************************************************************/ 
void TM1621_Buzzer_Off(void);
void TM1621_Buzzer_On(void);
// ����  ���ɴ��,������Ҫ�����
void TM1621_Buzzer_Whistle(uint16_t us);
// ��һ��
void TM1621_Buzzer_Click(void);
/*
******************************************************************************
��ʼ�����HT1621 
******************************************************************************
*/  
void TM1621_Buzzer_Init(void);
void TM1621_LCD_Init(void);

//------------------- pwm���� ----------------------------
extern void TM1621_light_Max(void);
extern void TM1621_light_Half(void);
extern void TM1621_light_Off(void);
extern void TM1621_light_On(void);
extern void TM1621_Set_light_Mode(uint8_t mode);
/* Private defines -----------------------------------------------------------*/



#ifdef __cplusplus
}
#endif

#endif /* __TM1621_H__ */
