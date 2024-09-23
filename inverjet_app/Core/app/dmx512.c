/**
******************************************************************************
* @file    		dmx512.c
* @brief   		灯板通讯协议
*						使用 USART4 ,  RS485 方式通讯 
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "dmx512.h"
#include <string.h>
#include "usart.h"
#include "modbus.h"

/* Private defines -----------------------------------------------------------*/

//#define	DMX512_SEPARATE_BUFFER					1

/* Private variables ---------------------------------------------------------*/

//通过接收缓存区后再存入 modbus_buffer ???
#ifdef DMX512_SEPARATE_BUFFER
//uint8_t Dmx512_RxBuff[DMX512_RS485_RX_BUFF_SIZE]={0};//定义一个接收数组
uint8_t Dmx512_TxBuff[DMX512_RS485_TX_BUFF_SIZE]={0};//定义一个发送缓存区
#endif

uint8_t* p_Dmx512_RxBuff;
uint8_t* p_Dmx512_TxBuff;

uint16_t* p_Dmx512_DataLen;

//uint8_t Dmx512_Rx_Flag=1;//接收标志
//uint8_t Dmx512_PERIODIC_CNT = 0;					//轮询周期计数

uint8_t Dmx512_Send_Buff[DMX512_RS485_TX_BUFF_SIZE+1] = {0};

#if (DMX512_HUART == 1)
UART_HandleTypeDef* p_Dmx512_Huart = &huart1;
#elif (DMX512_HUART == 3)
UART_HandleTypeDef* p_Dmx512_Huart = &huart3;
#elif (DMX512_HUART == 4)
UART_HandleTypeDef* p_Dmx512_Huart = &huart4;
#elif (DMX512_HUART == 5)
UART_HandleTypeDef* p_Dmx512_Huart = &huart5;
#endif

extern TIM_HandleTypeDef htim1;
/* Private user code ---------------------------------------------------------*/

static void Dmx512_Delay(uint32_t mdelay)
{
//  uint32_t Delay = mdelay * (16000000 / 8U / 1000U);
//  do
//  {
//    __NOP();
//  }
//  while (Delay --);
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < mdelay);
}


void Dmx512_Rs485_RecMode(void)
{
#if ((DMX512_HUART == 1)||(DMX512_HUART == 4))
	HAL_GPIO_WritePin(DMX512_RS485_EN_PORT, DMX512_RS485_EN_PIN, GPIO_PIN_RESET);
#endif
}

void Dmx512_Rs485_SendMode(void)
{
#if ((DMX512_HUART == 1)||(DMX512_HUART == 4))
	HAL_GPIO_WritePin(DMX512_RS485_EN_PORT, DMX512_RS485_EN_PIN, GPIO_PIN_SET);
#endif
}

void Dmx512_Receive_Init(void)
{
#ifdef DMX512_HUART
#ifdef DMX512_SEPARATE_BUFFER
	p_Dmx512_RxBuff = Dmx512_RxBuff;
	p_Dmx512_TxBuff = Dmx512_TxBuff;
#else
	p_Dmx512_RxBuff = (uint8_t*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,REG_DATA_ADDR_DMX512_START);
	p_Dmx512_TxBuff = (uint8_t*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,REG_DATA_ADDR_DMX512_START);
	p_Dmx512_DataLen = (uint16_t*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,REG_DATA_ADDR_DMX512_LENTH);
#endif
	
	__HAL_UART_ENABLE_IT(p_Dmx512_Huart, UART_IT_IDLE);//使能idle中断
	HAL_UART_Receive_IT(p_Dmx512_Huart, p_Dmx512_RxBuff, DMX512_RS485_TX_BUFF_SIZE);
	
	//Dmx512_Rs485_RecMode();

	Dmx512_Rs485_SendMode();			//设置为发送模式
	
	Set_Dmx512_Data_Change(0);//标志置0
#endif
}

void UartPortModeChang(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(mode){
		GPIO_InitStruct.Pin = DMX512_RS485_TX_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(DMX512_RS485_TX_PORT, &GPIO_InitStruct);
	}else{
		GPIO_InitStruct.Pin = DMX512_RS485_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DMX512_RS485_TX_PORT, &GPIO_InitStruct);
	}
}

void Dmx512_Send(uint8_t buff[],uint16_t num)		//num为数据长度，因为有0X00所以不能用strlen计算长度
{
	uint16_t i;
	
	//Dmx512_Rs485_SendMode();			//设置为发送模式
	
	if( num > DMX512_RS485_TX_BUFF_SIZE )
		num = DMX512_RS485_TX_BUFF_SIZE;
	
	Dmx512_Send_Buff[0] = 0x00;//起始码
	for(i = 0; i < num; i++)
	{
		Dmx512_Send_Buff[i+1] = buff[i];
	}
	//Dmx512_Send_Buff[num + 1] = 0x00;//结束码  不需要
	p_Dmx512_Huart->TxXferCount = (num+1);

	UartPortModeChang(1);
	HAL_GPIO_WritePin(DMX512_RS485_TX_PORT,DMX512_RS485_TX_PIN,GPIO_PIN_RESET);
	Dmx512_Delay(88);
	HAL_GPIO_WritePin(DMX512_RS485_TX_PORT,DMX512_RS485_TX_PIN,GPIO_PIN_SET);
	Dmx512_Delay(80);
	UartPortModeChang(0);
	
	//Dmx512_Rs485_SendMode();
	HAL_UART_Transmit(p_Dmx512_Huart,Dmx512_Send_Buff,num+1,0xffff);//此处为可以发0X00进行了一点修改
	//Dmx512_Rs485_RecMode();
	//Dmx512_Delay(100);
	
//	UartPortModeChang(1);
//	HAL_GPIO_WritePin(DMX512_RS485_TX_PORT,DMX512_RS485_TX_PIN,GPIO_PIN_SET);

}

//void Dmx512_Send(uint8_t buff[],uint16_t num)
//{
//	uint16_t i;
//	
//	Dmx512_Rs485_SendMode();			//设置为发送模式
//	Dmx512_Delay(50);
//	UartPortModeChang(1);					//设置发送的引脚为普通IO

//	HAL_GPIO_WritePin(DMX512_RS485_TX_PORT,DMX512_RS485_TX_PIN,GPIO_PIN_RESET); 									//输出低电平,BREAK
//	Dmx512_Delay(176); 							//延时176us
//	HAL_GPIO_WritePin(DMX512_RS485_TX_PORT,DMX512_RS485_TX_PIN,GPIO_PIN_SET); 										//输出高电平,MAB
//	Dmx512_Delay(12); 							//延时12us
//	
//	UartPortModeChang(0);					//设置发送的引脚为复用IO
//	Dmx512_Delay(10);
//  //HAL_UART_Transmit(p_Dmx512_Huart,buff,num,0xffff);//此处为可以发0X00进行了一点修改
//	for(i = 0; i <= 512; i++)  //0:startcode   1-512:调光数据
//	{
//		if(i == 0)
//		{
//				UART4->DR = 0x00 | 0x100;//第一帧startcode
//				while((UART4->SR & 0X40) == 0); //确保数据发送完成
//		}

//		else
//		{
//				UART4->DR = 0x100 | 0x03;
//				while((UART4->SR & 0X40) == 0);     //确保数据发送完成
//		}
//	}
//	
//	Dmx512_Rs485_RecMode();			//设置为接收模式
//}

void Dmx512_RxData(uint8_t len)
{
#ifdef DMX512_SEPARATE_BUFFER
	Set_Dmx512_Data(p_Dmx512_RxBuff,DMX512_RS485_RX_BUFF_SIZE);
	memset(p_Dmx512_RxBuff,0,DMX512_RS485_RX_BUFF_SIZE);    //清空缓存区
#endif
	__HAL_UART_CLEAR_IDLEFLAG(p_Dmx512_Huart);               //清除标志位
	HAL_UART_Receive_IT(p_Dmx512_Huart, p_Dmx512_RxBuff, DMX512_RS485_TX_BUFF_SIZE);
}


void Dmx512_Periodic_Polling(uint8_t* p_buff,uint8_t num)
{
	uint8_t i;
	
	for(i=0; i<num; i++)
	{
		p_buff[i] = i;
	}
}


uint8_t Dmx512_Get_polling_cnt(uint8_t cnt)
{
	uint8_t vaule=cnt;
	
	if(cnt < 10)
		vaule++;
	else
		vaule = 0;
	
	return vaule;
}


void Dmx512_Protocol_Analysis(void)
{
#ifdef DMX512_HUART

	if(Get_Dmx512_Data_Change() == 1)
	{
		Dmx512_Send(p_Dmx512_TxBuff, *p_Dmx512_DataLen );
		Set_Dmx512_Data_Change(0);//标志置0
	}
	
#endif
}


void Dmx512_Hardware_Debug(void)
{
	uint16_t i,time;
	
	for(time=0; time<4; time++)
	{
		for(i=0; i<DMX512_RS485_TX_BUFF_SIZE; i++)
		{
			if( (i%4) == time)
				*(p_Dmx512_TxBuff + i) = 0xFF;
			else
				*(p_Dmx512_TxBuff + i) = 0;
		}
		Dmx512_Send(p_Dmx512_TxBuff, DMX512_RS485_TX_BUFF_SIZE );

		osDelay(2000);//2秒
		
	}
}

