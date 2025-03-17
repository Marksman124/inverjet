/**
******************************************************************************
* @file    		debug_protocol.c
* @brief			调试协议
*
*
* @author			WQG
* @versions   v2.0.2
* @date   		2024-4-10
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "debug_protocol.h"	///////////////////////	串口调试
#include "motor.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#if DEBUG_USART == 4
UART_HandleTypeDef* p_huart_debug = &huart4;		 //调试串口 UART句柄
#elif DEBUG_USART == 5
UART_HandleTypeDef* p_huart_debug = &huart5;
#endif


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t Debug_Protocol_Mode	=	0;
uint8_t Debug_Send_Buffer[DEBUG_PROTOCOL_TX_MAX];
uint8_t Debug_Read_Buffer[DEBUG_PROTOCOL_RX_MAX];

uint8_t debug_buffer[DEBUG_PROTOCOL_TX_MAX]={0};


uint32_t Print_Flag_Position = 0;

/* USER CODE END PV */

extern DMA_HandleTypeDef hdma_uart4_rx;

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/

/*
******************************************************************************
Add_Ctrl_Log	

添加控制 日志

0：按键   1：wifi  2：bt
******************************************************************************
*/
void Add_Ctrl_Log(void)
{
	static uint8_t Log_Cnt = 0;
	uint16_t value=0;
	
	System_Ctrl_Mode_Type_enum mode = Get_Ctrl_Mode_Type();//控制来源

	uint8_t addr = MB_SYSTEM_LAST_KEY_VALUE + (Log_Cnt * 5);
	
	value = (uint8_t)mode | (Log_Cnt<<8) ;
	
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr,  	value);
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr+1,  * p_System_State_Machine);		// 状态机
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr+2,  * p_PMode_Now);							// 当前模式
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr+3,  * p_OP_ShowNow_Speed);				// 当前速度
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr+4,  * p_OP_ShowNow_Time);				// 当前时间
	
	if(Log_Cnt < 10)
		Log_Cnt ++;
	else
		Log_Cnt = 0;
}

void Set_Debug_Protocol_Mode(uint8_t mode)
{
	if( mode == 2)
		Debug_Protocol_Mode = 2;
	else if( mode == 1)
		Debug_Protocol_Mode = 1;
	else
		Debug_Protocol_Mode = 0;
	
	Debug_Send_Buffer[0] = 0xAA;
	Debug_Send_Buffer[1] = 0xAA;
	Debug_Send_Buffer[2] = 0x60;
	Debug_Send_Buffer[3] = 0xDE;
	Debug_Send_Buffer[4] = Debug_Protocol_Mode;
	
#ifdef DEBUG_USART
	HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, 5,50); //将收到的信息发送出去
	//HAL_UART_Transmit_IT(p_huart_debug, Debug_Send_Buffer, 5); //将收到的信息发送出去
#endif
}


void UART_Send_Debug(uint8_t * p_buff, uint8_t len)
{
#ifdef UART_PRINTF_LOG
//	if(Debug_Protocol_Mode != 1)
//		return;
	
	memset(Debug_Send_Buffer, 0, DEBUG_PROTOCOL_TX_MAX);
	//Debug_Send_Buffer[0] = cmd;
	memcpy(Debug_Send_Buffer, p_buff, len);
	// 转发至串口  用于调试
	HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, len,50); //将收到的信息发送出去
	//HAL_UART_Transmit_IT(p_huart_debug, (uint8_t *)Debug_Send_Buffer, len+1);//使用DMA发送数据

#else
	return;
#endif
}

void Main_Modbus_Send(uint8_t * p_buff, uint8_t len)
{
	HAL_GPIO_WritePin(Main_RS485_EN_GPIO_Port, Main_RS485_EN_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(p_huart_debug, p_buff, len,50); //将收到的信息发送出去
	HAL_GPIO_WritePin(Main_RS485_EN_GPIO_Port, Main_RS485_EN_Pin, GPIO_PIN_RESET);
	
	//HAL_UART_Transmit_IT(p_huart_debug, (uint8_t *)Debug_Send_Buffer, len+1);//使用DMA发送数据
}

void Main_Modbus_Send_Auto_Run(void)
{
	uint8_t Buffer[8] = {0x15, 0x06, 0x00, 0x60, 0xA0, 0x70, 0xF3, 0x24};
	
	HAL_GPIO_WritePin(Main_RS485_EN_GPIO_Port, Main_RS485_EN_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(p_huart_debug, Buffer, 8,200); //将收到的信息发送出去
	HAL_GPIO_WritePin(Main_RS485_EN_GPIO_Port, Main_RS485_EN_Pin, GPIO_PIN_RESET);
	
	//HAL_UART_Transmit_IT(p_huart_debug, (uint8_t *)Debug_Send_Buffer, len+1);//使用DMA发送数据
}

// 初始化
void Debug_Protocol_Init(void)
{
	__HAL_UART_ENABLE_IT(p_huart_debug, UART_IT_IDLE);//使能idle中断
	__HAL_UART_ENABLE_IT(p_huart_debug, UART_IT_ERR);//
	
  HAL_UARTEx_ReceiveToIdle_DMA(p_huart_debug,Debug_Read_Buffer,DEBUG_PROTOCOL_RX_MAX);//打开串口DMA接收
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);		   // 手动关闭DMA_IT_HT中断

}

// 重启
void Debug_Usart_Restar(void)
{
	if(HAL_UART_DeInit(p_huart_debug) != HAL_OK)
  {
    Error_Handler();
  }
  
  // 重新打开串口
  MX_UART4_Init();
	Debug_Protocol_Init();
}


