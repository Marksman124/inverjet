/**
******************************************************************************
* @file    		dev.c
* @brief   		������Ϣ
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "dev.h"
#include "iap.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "modbus.h"
#include "operation.h"		// ���� �˵�
#include "fault.h"				// ���� �˵�

//FLASH_ProcessTypeDef p_Flash; 
uint16_t* p_Local_Address;			//	���ص�ַ

uint16_t* p_Software_Version_high;		//	����汾
uint16_t* p_Software_Version_low;		//	����汾 ��

//����λ��modbus
uint16_t* p_Baud_Rate;							//	������
uint32_t Modbus_BaudRate_Table[] = 	{2400,4800,9600,19200,115200};
#define MODBUS_BAUDRATE_TABLE_LEN		(sizeof(Modbus_BaudRate_Table)/sizeof(Modbus_BaudRate_Table[0]))



uint32_t Dev_BaudRate_Get(uint8_t usart_num)
{
	uint32_t all_usart_baudrate_table[SYSTEM_USER_USART_MAX] = {115200,115200,115200,115200,115200};//U1 ~ U5 Ĭ�ϲ�����
	
	if(usart_num == DEBUG_USART)	// ���Դ���
	{
		return 115200;
	}
	else if(usart_num == MODBUS_USART)	// modbus
	{
		// ��д�� 115200 wuqingguang
		//if( (*p_Baud_Rate >= MODBUS_BAUDRATE_TABLE_LEN) || (*p_Baud_Rate == 0) )
		{
			*p_Baud_Rate = MODBUS_BAUDRATE_DEFAULT;	// Ĭ�� 9600
		}
		return Modbus_BaudRate_Table[*p_Baud_Rate];
	}
	else if(usart_num == WIFI_USART)	// wifi
	{
		return 9600;
	}
	else if(usart_num == DRIVER_USART)	// ������
	{
		return 115200;//115200
	}
	else if(usart_num == BLUETOOTH_USART)	// ����
	{
		return 115200;
	}
	else if( (usart_num <= SYSTEM_USER_USART_MAX) && (usart_num > 0) )
	{
		return all_usart_baudrate_table[usart_num -1];
	}
	else
		return 0;
}


void Dev_Information_Init(void)
{
	p_Local_Address = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_SLAVE_NODE_ADDRESS);
	
	p_Baud_Rate = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_SLAVE_BAUD_RATE);
	Dev_BaudRate_Get(MODBUS_USART);
	
	p_Software_Version_high = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_DISPLAY_SOFTWARE_VERSION);//	����汾
	p_Software_Version_low = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_DISPLAY_SOFTWARE_VERSION+1);

}


uint16_t Read_Local_Address(void)
{
	return *p_Local_Address;
}

uint16_t Read_Baud_Rate(void)
{
	return *p_Baud_Rate;
}

uint32_t Read_Software_Version(void)
{
	uint32_t version=0;
	
	version = (*p_Software_Version_high)<<16 | (*p_Software_Version_low);

	return version;
}

void Set_Local_Address(uint16_t addr)
{
	*p_Local_Address = addr;
	// дflash
	//STMFLASH_Write(USER_FLASH_ADDR_LOCAL_ADDR, p_Local_Address, 1);
	MB_Flash_Buffer_Write();
}

void Set_Baud_Rate(uint16_t rate)
{
	*p_Baud_Rate = rate;
	// дflash
	//STMFLASH_Write(USER_FLASH_ADDR_BAUD_RATE, p_Baud_Rate, 1);
	MB_Flash_Buffer_Write();
}

void Set_Software_Version(void)
{
	uint32_t version_u32=0;
	
	version_u32 = get_uint3_version(SOFTWARE_VERSION_UINT32);
	
	*p_Software_Version_high = version_u32>>16;
	*p_Software_Version_low = version_u32;
	
	// дflash
	//STMFLASH_Write(USER_FLASH_ADDR_SOFTWARE_VERSION, (uint16_t*)p_Software_Version, 2);
	MB_Flash_Buffer_Write();
}
 

