/**
******************************************************************************
* @file    		dev.c
* @brief   		本机信息
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
#include "operation.h"		// 操作 菜单
#include "fault.h"				// 故障 菜单

//FLASH_ProcessTypeDef p_Flash; 
uint16_t* p_Local_Address;			//	本地地址

uint16_t* p_Software_Version_high;		//	软件版本
uint16_t* p_Software_Version_low;		//	软件版本 低

//与上位机modbus
uint16_t* p_Baud_Rate;							//	波特率
uint32_t Modbus_BaudRate_Table[] = 	{1200,2400,4800,9600,19200,115200};
#define MODBUS_BAUDRATE_TABLE_LEN		(sizeof(Modbus_BaudRate_Table)/sizeof(Modbus_BaudRate_Table[0]))



uint32_t Dev_BaudRate_Get(uint8_t usart_num)
{
	uint32_t all_usart_baudrate_table[SYSTEM_USER_USART_MAX] = {115200,115200,115200,115200,115200};//U1 ~ U5 默认波特率
	
	if(usart_num == DEBUG_USART)	// 调试串口
	{
		return 115200;
	}
	else if(usart_num == MODBUS_USART)	// modbus
	{
		// 先写死 115200 wuqingguang
		//if( (*p_Baud_Rate >= MODBUS_BAUDRATE_TABLE_LEN) || (*p_Baud_Rate == 0) )
		{
			*p_Baud_Rate = MODBUS_BAUDRATE_DEFAULT;	// 默认 9600
		}
		return Modbus_BaudRate_Table[*p_Baud_Rate];
	}
	else if(usart_num == WIFI_USART)	// wifi
	{
		return 9600;
	}
	else if(usart_num == DRIVER_USART)	// 驱动板
	{
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
		return 115200;//115200
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
		return 2400;
#endif
	}
	else if(usart_num == BLUETOOTH_USART)	// 蓝牙
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
	
	p_Software_Version_high = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_DISPLAY_SOFTWARE_VERSION);//	软件版本
	p_Software_Version_low = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_DISPLAY_SOFTWARE_VERSION+1);
	
	// 屏蔽 控制方式
	p_Support_Control_Methods = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_SUPPORT_CONTROL_METHODS);
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
	// 写flash
	//STMFLASH_Write(USER_FLASH_ADDR_LOCAL_ADDR, p_Local_Address, 1);
	MB_Flash_Buffer_Write();
}

void Set_Baud_Rate(uint16_t rate)
{
	*p_Baud_Rate = rate;
	// 写flash
	//STMFLASH_Write(USER_FLASH_ADDR_BAUD_RATE, p_Baud_Rate, 1);
	MB_Flash_Buffer_Write();
}

void Set_Software_Version(void)
{
	uint32_t version_u32=0;
	
	version_u32 = get_uint3_version(SOFTWARE_VERSION_UINT32);
	
	*p_Software_Version_high = version_u32>>16;
	*p_Software_Version_low = version_u32;
	
	// 写flash
	//STMFLASH_Write(USER_FLASH_ADDR_SOFTWARE_VERSION, (uint16_t*)p_Software_Version, 2);
	MB_Flash_Buffer_Write();
}
 

void Disable_Usart_Receiver(uint8_t no)
{
	if(no == MACRO_MODBUS_USART)
	{
    eMBDisable();
	}
	else if(no == 2)
	{
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
	}
	else if(no == 3)
	{
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
    __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_RXNE);
	}
	else if(no == 4)
	{
    __HAL_UART_DISABLE_IT(&huart4, UART_IT_RXNE);
    __HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_RXNE);
	}
	else if(no == 5)
	{
    __HAL_UART_DISABLE_IT(&huart5, UART_IT_RXNE);
    __HAL_UART_CLEAR_FLAG(&huart5, UART_FLAG_RXNE);
	}
}

void Enable_Usart_Receiver(uint8_t no)
{
	if(no == MACRO_MODBUS_USART)
	{
    eMBEnable();//使能modbus
	}
	else if(no == 2)
	{
    HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer2, 1);
    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
	}
	else if(no == 3)
	{
    HAL_UART_Receive_IT(&huart3, (uint8_t *)aRxBuffer3, 1);
    __HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_RXNE);
	}
	else if(no == 4)
	{
    HAL_UART_Receive_IT(&huart4, (uint8_t *)aRxBuffer4, 1);
    __HAL_UART_CLEAR_FLAG(&huart4, UART_FLAG_RXNE);
	}
	else if(no == 5)
	{
    HAL_UART_Receive_IT(&huart5, (uint8_t *)aRxBuffer5, 1);
    __HAL_UART_CLEAR_FLAG(&huart5, UART_FLAG_RXNE);
	}
}

void Dev_Check_Control_Methods( void )
{
	if( *p_Support_Control_Methods & BLOCK_BLUETOOTH_CONTROL)
		Disable_Usart_Receiver(MACRO_BLUETOOTH_USART);
	else
		Enable_Usart_Receiver(MACRO_BLUETOOTH_USART);
	
	if( *p_Support_Control_Methods & BLOCK_MODBUS_CONTROL)
		Disable_Usart_Receiver(MACRO_MODBUS_USART);
	else
		Enable_Usart_Receiver(MACRO_MODBUS_USART);
	
	if( *p_Support_Control_Methods & BLOCK_WIFI_CONTROL)
		Disable_Usart_Receiver(MACRO_WIFI_USART);
	else
		Enable_Usart_Receiver(MACRO_WIFI_USART);
	
}



