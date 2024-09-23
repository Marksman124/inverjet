/**
******************************************************************************
* @file				bluetooth.c
* @brief			bluetooth ģ��
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "bluetooth.h"
#include "my_modbus.h"
#include "modbus.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static BT_STATE_MODE_E BT_State_Machine = BT_NO_CONNECT;
//���ڽ���
uint8_t BT_Uart_Read_Buffer;
//����һ������
ModbusSlaveObj_t Ms_BT_Modbus;

static uint16_t BT_Halder_cnt = 0;
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//���ڷ��ͽӿ�
void SerialWrite(unsigned char *buff,int length)
{
	HAL_UART_Transmit(&huart5,buff,length,0xFFFF);
}

//���ڽ��սӿ�
void BT_Read_Data_Bit(unsigned char vaule)
{
	//MsSerialRead(&Ms_BT_Modbus,&vaule,1);
	if(Ms_BT_Modbus.rxWriteLock)
	{
			return;
	}

	Ms_BT_Modbus.rxTimerEnable = 1;
	Ms_BT_Modbus.rxTimerCnt = 0;

	Ms_BT_Modbus.rxBuff[Ms_BT_Modbus.rxWriteIdx] =vaule;
	if(Ms_BT_Modbus.rxWriteIdx < (MODBUS_SLAVE_TX_RX_MAX_LEN - 1))
	{
			Ms_BT_Modbus.rxWriteIdx++;
	}
	else
	{
		Ms_BT_Modbus.rxWriteIdx = 0;
	}
	
		
}

//�����жϵ���
void Usart_IRQ_CallBack(uint8_t data)
{
	BT_Uart_Read_Buffer = data;
	MsSerialRead(&Ms_BT_Modbus,&BT_Uart_Read_Buffer,1);
}


// AT ָ�� �� MAC
void BT_Set_MAC(void)
{
	char buff[32]={"AT+BLEMAC=123456789012\r\n"};
	
	//sprintf(buff,"AT+BLENAME=inverjet\r\n");
	
	SerialWrite((uint8_t*)buff,strlen(buff));
}

// AT ָ�� �� ����
void BT_Set_Name(void)
{
	char buff[32]={"AT+BLENAME=inverjet\r\n"};
	
	//sprintf(buff,"AT+BLENAME=inverjet\r\n");
	
	SerialWrite((uint8_t*)buff,strlen(buff));
}

// AT ָ�� �� �ӻ�ģʽ
void BT_Set_Mode(uint8_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEMODE=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
}
// AT ָ�� �� MTU
void BT_Set_MTU(uint8_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEMTU=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
}
// AT ָ�� ����͸��
void BT_Set_TRANSENTER(uint8_t data)
{
	char buff[32]={0};
	
	if(data == 0)//�˳�
	{
		SerialWrite((uint8_t*)"+++",3);
	}
	else
	{
		sprintf(buff,"AT+TRANSENTER\r\n");
	
		SerialWrite((uint8_t*)buff,strlen(buff));
	}
}
//
void BT_Module_AT_Init(void)
{
	BT_Set_TRANSENTER(0);
	osDelay(1000);
	BT_Set_MTU(243);
	osDelay(2000);
	BT_Set_TRANSENTER(1);//����͸��
}
//
void BT_Module_AT_ReInit(void)
{
	// ���͸�λ
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	//osDelay(2000);
	//����λ
	SerialWrite((uint8_t*)"+++",3);
	osDelay(1000);
	
	
	
	BT_Set_Name();
	osDelay(1000);
	BT_Set_MAC();
	osDelay(1000);
	BT_Set_Mode(0);
	osDelay(5000);
	BT_Set_MTU(243);
	//osDelay(2000);
}
//------------------- ���� Modbus ���ó�ʼ�� ----------------------------
void BT_Modbus_Config_Init(void)
{
	//��ʼ������
	MsInit(&Ms_BT_Modbus,21,1,SerialWrite);
	//����01�Ĵ����Ĳ����������õĻ��᷵����Ч�Ĺ��ܴ�����
	//MsConfigureRegister(&Ms_BT_Modbus,0x01,buff01,sizeof(buff01));
	//MsConfigureRegister(&Ms_BT_Modbus,0x0F,buff01,sizeof(buff01));
	MsConfigureRegister(&Ms_BT_Modbus,0x03,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x04,Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,0),REG_INPUT_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x06,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x10,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x05,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	
}
/*******************************************************************************
*���ܣ���ʱ�������ŵ���ʱ�������У�һ��ŵ�1ms�ж��㹻����Ҫ��
*******************************************************************************/
void BT_MsTimeout(void)
{
	MsTimeout(&Ms_BT_Modbus);
}

//------------------- ����wifi״̬�� ----------------------------
void BT_Set_Machine_State(BT_STATE_MODE_E para)
{
	if(para <= BT_ERROR)
	{
		BT_State_Machine = para;
	}
}
	
//------------------- ��ȡwifi״̬�� ----------------------------
BT_STATE_MODE_E BT_Get_Machine_State(void)
{
	return BT_State_Machine;
}


//------------------- ���մ����� ----------------------------
void BT_Read_Handler(void)
{	
	MsProcess(&Ms_BT_Modbus);
}

//------------------- �������� ----------------------------
void BT_Get_In_Distribution(void)
{
	BT_Set_Machine_State( BT_DISTRIBUTION );
	
	BT_Halder_cnt = 0;
	//BT_Module_AT_ReInit();
}

//------------------- ������� ----------------------------
void BT_Get_In_Error(void)
{
	BT_Set_Machine_State( BT_ERROR );
	
}

//------------------- �������� 0.5���һ��----------------------------
void BT_Distribution_Halder(void)
{	
	if(BT_Halder_cnt == 0)
		SerialWrite((uint8_t*)"+++",3);
	else if(BT_Halder_cnt == 2)
		BT_Set_Name();
	else if(BT_Halder_cnt == 4)
		BT_Set_MAC();
	else if(BT_Halder_cnt == 6)
		BT_Set_Mode(0);
	else if(BT_Halder_cnt == 16)
		BT_Set_MTU(243);
	
	BT_Halder_cnt ++;
}


