/**
******************************************************************************
* @file    		debug_protocol.c
* @brief			����Э��
*
*
* @author			WQG
* @versions   v2.0.2
* @date   		2024-4-10
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "debug_protocol.h"	///////////////////////	���ڵ���
#include "motor.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#if DEBUG_USART == 4
UART_HandleTypeDef* p_huart_debug = &huart4;		 //���Դ��� UART���
#elif DEBUG_USART == 5
UART_HandleTypeDef* p_huart_debug = &huart5;
#endif

#ifdef UART_DEBUG_SEND_CTRL
uint8_t Chassis_Temperature_Debug=0;
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

/* USER CODE END PV */

extern DMA_HandleTypeDef hdma_uart4_rx;

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/

/*
******************************************************************************
Add_Ctrl_Log	

��ӿ��� ��־

0������   1��wifi  2��bt
******************************************************************************
*/
void Add_Ctrl_Log(void)
{
	static uint8_t Log_Cnt = 0;
	uint16_t value=0;
	
	System_Ctrl_Mode_Type_enum mode = Get_Ctrl_Mode_Type();//������Դ

	uint8_t addr = MB_SYSTEM_LAST_KEY_VALUE + (Log_Cnt * 5);
	
	value = (uint8_t)mode | (Log_Cnt<<8) ;
	
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr,  	value);
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr+1,  * p_System_State_Machine);		// ״̬��
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr+2,  * p_PMode_Now);							// ��ǰģʽ
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr+3,  * p_OP_ShowNow_Speed);				// ��ǰ�ٶ�
	Set_DataAddr_Value( MB_FUNC_READ_INPUT_REGISTER,  addr+4,  * p_OP_ShowNow_Time);				// ��ǰʱ��
	
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
	HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, 5,0xFFFF); //���յ�����Ϣ���ͳ�ȥ
	//HAL_UART_Transmit_IT(p_huart_debug, Debug_Send_Buffer, 5); //���յ�����Ϣ���ͳ�ȥ
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
	// ת��������  ���ڵ���
	HAL_UART_Transmit(p_huart_debug, Debug_Send_Buffer, len,0xFFFF); //���յ�����Ϣ���ͳ�ȥ
	//HAL_UART_Transmit_IT(p_huart_debug, (uint8_t *)Debug_Send_Buffer, len+1);//ʹ��DMA��������

#else
	return;
#endif
}


//
void To_Debug_Protocol_Analysis(uint8_t len)
{
#ifdef UART_DEBUG_SEND_CTRL

	//UART_Send_Debug(Debug_Read_Buffer, len);
	
	if(Debug_Read_Buffer[0] < MOTOR_PROTOCOL_ADDR_MAX)
	{
		Motor_State_Storage[Debug_Read_Buffer[0]] = Debug_Read_Buffer[1];
		//*p_System_Fault_Static = Debug_Read_Buffer[1];
		Motor_State_Analysis();
	}
	else if(Debug_Read_Buffer[0] == 0xF1)//�����¶�
	{
		Chassis_Temperature_Debug = Debug_Read_Buffer[1];
	}
	else if(Debug_Read_Buffer[0] == 0xF2)//wifi
	{
		WIFI_Set_Machine_State(Debug_Read_Buffer[1]);
	}
	else if(Debug_Read_Buffer[0] == 0xF3)//����
	{
		BT_Set_Machine_State(Debug_Read_Buffer[1]);
	}
	
	return;
#endif
}


// ��ʼ��
void Debug_Protocol_Init(void)
{
	
#ifdef UART_DEBUG_SEND_CTRL

	__HAL_UART_ENABLE_IT(p_huart_debug, UART_IT_IDLE);//ʹ��idle�ж�
	__HAL_UART_ENABLE_IT(p_huart_debug, UART_IT_ERR);//
	
  HAL_UARTEx_ReceiveToIdle_DMA(p_huart_debug,Debug_Read_Buffer,DEBUG_PROTOCOL_RX_MAX);//�򿪴���DMA����
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
#endif
}

// ����
void Debug_Usart_Restar(void)
{
	if(HAL_UART_DeInit(p_huart_debug) != HAL_OK)
  {
    Error_Handler();
  }
  
  // ���´򿪴���
  MX_UART4_Init();
	Debug_Protocol_Init();
}


