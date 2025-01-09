/**
******************************************************************************
* @file    		modbus.c
* @brief   		Modbus �ӿ�
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "modbus.h"
#include "dev.h"
#include "key.h"
/* Private variables ---------------------------------------------------------*/
ULONG OTA_Pack_Len=0;					// �ܳ���

uint32_t ModbusTimerCnt=0;



/* ----------------------- Static variables ---------------------------------*/
//USHORT   usRegHoldingStart = REG_HOLDING_START;
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS+1];

//USHORT   usRegInputStart = REG_INPUT_START;
USHORT   usRegInputBuf[REG_INPUT_NREGS+1] = {0};

USHORT		MB_Data_Addr_Need_CallOut[] = {
	MB_SLAVE_NODE_ADDRESS,MB_SLAVE_BAUD_RATE,
	MB_SYSTEM_WORKING_MODE,MB_SYSTEM_WORKING_STATUS,MB_MOTOR_CURRENT_SPEED,
	//MB_MOTOR_CURRENT_SPEED,MB_MOTOR_CURRENT_TIME,
};

BOOL Dmx512_Data_Change_Sign =0;
/* ----------------------- Start implementation -----------------------------*/
extern TIM_HandleTypeDef htim1;

void Clean_ModbusTimerCnt()
{
	ModbusTimerCnt = 0;
}

BOOL 
Check_Need_CallOut(USHORT addr)
{
	USHORT sum,i;
	
	sum = sizeof(MB_Data_Addr_Need_CallOut)/sizeof(MB_Data_Addr_Need_CallOut[0]);
	
	for(i=0; i<sum; i++)
	{
		if(addr == MB_Data_Addr_Need_CallOut[i])
			return TRUE;
	}
	
	return FALSE;
}

// Call Out
void
HoldingCallOut( USHORT usAddress )
{
	//uint16_t iRegIndex;
	//uint32_t message_info=0;
	
	//iRegIndex = ( usAddress );
	
	if(usAddress == MB_SLAVE_NODE_ADDRESS) // �ӻ���ַ
	{
		MB_Node_Address_Set(*p_Local_Address);
	}
	else if(usAddress == MB_SLAVE_BAUD_RATE) // ���ò�����
	{
		__HAL_UART_DISABLE(p_huart_mb);
#if MODBUS_USART == 1
		MX_USART1_UART_Init();
#elif MODBUS_USART == 3
		MX_USART3_UART_Init();
#elif MODBUS_USART == 4
		MX_UART4_Init();
#elif MODBUS_USART == 5
		MX_UART5_Init();
#endif
		__HAL_UART_ENABLE(p_huart_mb);
	}
	else if(usAddress == MB_SYSTEM_WORKING_MODE) //	ϵͳ����ģʽ  ��λ::0��P1\2\3  ��λ:0������:1����ʱ:2��ѵ��
	{
		System_Para_Set_PMode(usRegHoldingBuf[MB_SYSTEM_WORKING_MODE], CTRL_FROM_RS485);
	}
	else if(usAddress == MB_SYSTEM_WORKING_STATUS) //	״̬��
	{
		System_Para_Set_Status(usRegHoldingBuf[MB_SYSTEM_WORKING_STATUS], CTRL_FROM_RS485);
	}
	else if(usAddress == MB_MOTOR_CURRENT_SPEED)
	{
		System_Para_Set_Speed(usRegHoldingBuf[MB_MOTOR_CURRENT_SPEED], CTRL_FROM_RS485);
	}
	else if(usAddress == MB_MOTOR_CURRENT_TIME)
	{
		System_Para_Set_Time(usRegHoldingBuf[MB_MOTOR_CURRENT_TIME], CTRL_FROM_RS485);
	}
}


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

	Clean_ModbusTimerCnt();
	
    if( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS )
    {
        //iRegIndex = ( int )( usAddress - usRegInputStart );
				iRegIndex = ( int )( usAddress );
        while( usNRegs > 0 )
        {
					*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
					*pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
					iRegIndex++;
					usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
	
		Clean_ModbusTimerCnt();
	
    if( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) 
    {
				//iRegIndex = ( int )( usAddress - usRegHoldingStart );
        iRegIndex = ( int )( usAddress );
        switch ( eMode )
        {
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
							*pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
							*pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
              iRegIndex++;
              usNRegs--;
            }
            break;

        case MB_REG_WRITE:
						if(If_Accept_External_Control(BLOCK_MODBUS_CONTROL))
						{
							while( usNRegs > 0 )
							{
								//״̬��
								if((iRegIndex == MB_SYSTEM_WORKING_STATUS) && ( *(pucRegBuffer+1) > TRAINING_MODE_STOP ))
								{
									pucRegBuffer++;
									pucRegBuffer++;
								}
								else
								{
									usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
									usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
								}
								iRegIndex++;
								usNRegs--;
							}
							if(Check_Need_CallOut(usAddress))
							{
								HoldingCallOut(usAddress);
							}
							//����
							Write_MbBuffer_Later();
						}
						break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegFileCB( UCHAR * pucRegBuffer, USHORT fileNumber, USHORT fileLength,
                 eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
	ULONG write_addr=0;						// flash д���ַ
	ULONG sign=0;
	
	Clean_ModbusTimerCnt();
	
	if( (( fileNumber <= REG_FILE_NUMBER_MAX ) || ( fileNumber == REG_FILE_NUMBER_END )) && ( fileLength <= REG_FILE_LENTH_MAX ) )
	{
		switch ( eMode )
		{
		case MB_REG_READ:
			break;
		case MB_REG_WRITE:
			if(fileNumber == REG_FILE_NUMBER_STAR) // ��ʼ��
			{OTA_Pack_Len = 0;}
			
			taskENTER_CRITICAL();
			write_addr = (FLASH_APP_PATCH_ADDR + OTA_Pack_Len);
			iap_write_appbin(write_addr,pucRegBuffer,fileLength);
			OTA_Pack_Len += (fileLength*2);
			taskEXIT_CRITICAL();
			
			if(fileNumber == REG_FILE_NUMBER_END)//���һ֡
			{
				STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PACK_LEN,(uint16_t*)&OTA_Pack_Len,2); // д������ (��crc)
				sign = PRODUCT_BOOT_PASSWORD;
				STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PASSWORD,(uint16_t*)&sign,2); // ����OTA 

				SysSoftReset();// �����λ
			}
			break;
		}
	}
	else
	{
			eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
	Clean_ModbusTimerCnt();
	//����״̬
	eMBErrorCode eStatus = MB_ENOERR;
//	//�Ĵ�������
//	int16_t iNCoils = ( int16_t )usNCoils;
//	//�Ĵ���ƫ����
//	int16_t usBitOffset;

//	//���Ĵ����Ƿ���ָ����Χ��
//	if( ( (int16_t)usAddress >= REG_COILS_START ) &&
//	( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
//	{
//	//����Ĵ���ƫ����
//	usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
//	switch ( eMode )
//	{
//	//������
//	case MB_REG_READ:
//	while( iNCoils > 0 )
//	{
//	*pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
//	( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
//	iNCoils -= 8;
//	usBitOffset += 8;
//	}
//	break;

//	//д����
//	case MB_REG_WRITE:
//	while( iNCoils > 0 )
//	{
//	xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
//	( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
//	*pucRegBuffer++ );
//	iNCoils -= 8;
//	}
//	break;
//	}

//	}
//	else
//	{
//	eStatus = MB_ENOREG;
//	}
	return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	Clean_ModbusTimerCnt();
	//����״̬
	eMBErrorCode eStatus = MB_ENOERR;
//	//�����Ĵ�������
//	int16_t iNDiscrete = ( int16_t )usNDiscrete;
//	//ƫ����
//	uint16_t usBitOffset;

//	//�жϼĴ���ʱ�����ƶ���Χ��
//	if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
//	( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
//	{
//	//���ƫ����
//	usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );

//	while( iNDiscrete > 0 )
//	{
//	*pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
//	( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
//	iNDiscrete -= 8;
//	usBitOffset += 8;
//	}

//	}
//	else
//	{
//	eStatus = MB_ENOREG;
//	}
	return eStatus;
}




void Modbus_Init(void)
{
	// ��д�� 0xAA  wuqingguang
	if( (*p_Local_Address >= 0xFF) || (*p_Local_Address == 0) )
	{
		*p_Local_Address = MODBUS_LOCAL_ADDRESS;	// Ĭ�� 0xAA
	}
	eMBInit( MB_RTU, *p_Local_Address, 0, Dev_BaudRate_Get(MODBUS_USART), MB_PAR_ODD);//��ʼ��modbus����modbusRTU����վ��ַΪ0xAA������Ϊ2��
	eMBEnable(  );//ʹ��modbus
	
}

void Modbus_Handle_Task(void)
{
	ModbusTimerCnt ++;
	Thread_Activity_Sign_Set(THREAD_ACTIVITY_RS485_MODBUS);
	
	( void )eMBPoll(  );//����modbus����
	
	if(ModbusTimerCnt > MODBUS_RESTART_TIMEOUT)
	{
		ModbusTimerCnt = 0;
		vMBPortSerialEnable( TRUE, FALSE );
	}
}



// *********  Buf ������ݽӿ�  *************************************

void Modbus_Buffer_Init(void)
{
	STMFLASH_Read(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );// REG_HOLDING_NREGS
	
}

void MB_Flash_Buffer_Write(void)
{
	taskENTER_CRITICAL();
	//������2048�� ���� usRegHoldingBuf һ��д
	STMFLASH_Write(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
	taskEXIT_CRITICAL();
	//Eeprom_I2C_Write(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
}

void MB_Flash_Buffer_Read(void)
{
	//������2048�� ���� usRegHoldingBuf һ��д
	STMFLASH_Read(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
	
	//Eeprom_I2C_Write(FLASH_APP_PARAM_ADDR, usRegHoldingBuf, REG_HOLDING_NREGS );
}

uint16_t* Get_DataAddr_Pointer(UCHAR ucFunctionCode, USHORT addr)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			return &usRegHoldingBuf[addr];
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			return &usRegInputBuf[addr];
		}
	}
	
	return NULL;
}


uint16_t Get_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			return usRegHoldingBuf[addr];
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			return usRegInputBuf[addr];
		}
	}
	
	return NULL;
}

void Set_DataAddr_Value(UCHAR ucFunctionCode, USHORT addr, uint16_t value)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		if(addr <= REG_HOLDING_NREGS)
		{
			usRegHoldingBuf[addr] = value;
		}
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		if(addr <= REG_INPUT_NREGS)
		{
			usRegInputBuf[addr] = value;
		}
	}
}

void Set_DataValue_U32(UCHAR ucFunctionCode, USHORT addr, uint32_t value)
{
	Set_DataAddr_Value( ucFunctionCode, addr, value>>16 );
	Set_DataAddr_Value( ucFunctionCode, addr+1, value&0xFFFF );
}

void Set_DataValue_Len(UCHAR ucFunctionCode, USHORT addr, uint8_t* p_data, uint8_t len)
{
	if(ucFunctionCode == MB_FUNC_READ_HOLDING_REGISTER)
	{
		memcpy(&usRegHoldingBuf[addr], p_data, len);
	}
	else if(ucFunctionCode == MB_FUNC_READ_INPUT_REGISTER)
	{
		memcpy(&usRegInputBuf[addr], p_data, len);
	}
}


//================= ����ģʽ ȫ�� ���� ================================
void Surf_Mode_Info_Get_Mapping(void)
{
	// ----------------------------------------------------------------------------------------------
	p_Surf_Mode_Info_Acceleration = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,		MB_SURF_MODE_INFO_ACCELERATION);		//	����ģʽ -- ���ٶ�
	p_Surf_Mode_Info_Prepare_Time = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,		MB_SURF_MODE_INFO_PREPARE_TIME);		//	����ģʽ -- ׼��ʱ��
	p_Surf_Mode_Info_Low_Speed		= Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,		MB_SURF_MODE_INFO_LOW_SPEED);				//	����ģʽ -- ���ٵ� -- �ٶ�
	p_Surf_Mode_Info_Low_Time 		= Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,		MB_SURF_MODE_INFO_LOW_TIME);				//	����ģʽ -- ���ٵ� -- ʱ��
	p_Surf_Mode_Info_High_Speed 	= Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,		MB_SURF_MODE_INFO_HIGH_SPEED);			//	����ģʽ -- ���ٵ� -- �ٶ�
	p_Surf_Mode_Info_High_Time 		= Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,		MB_SURF_MODE_INFO_HIGH_TIME);				//	����ģʽ -- ���ٵ� -- ʱ��
	// ----------------------------------------------------------------------------------------------
}



void MB_Get_Mapping_Register(void)
{
	p_OP_ShowLater = 		(Operating_Parameters*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_MOTOR_LEATER_SPEED);
	p_OP_Free_Mode = 		(Operating_Parameters*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_FREE_MODE_SPEED);
	p_OP_Timing_Mode = 	(Operating_Parameters*)Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TIME_MODE_SPEED);
	p_OP_PMode =				(Operating_Parameters(*)[TRAINING_MODE_PERIOD_MAX])Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_USER_TRAIN_MODE_SPEED_P1_1);
	//��Ȧ����
	p_Breath_Light_Max = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER , MB_MOTOR_BREATH_LIGHT_MAX);
	if(*p_Breath_Light_Max > 500)
		*p_Breath_Light_Max = 500;

	p_Motor_Pole_Number = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_MOTOR_POLE_NUMBER);
	
	//ϵͳ ����״̬
	p_System_Fault_Static = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER , MB_SYSTEM_FAULT_STATUS);
	//��� ����״̬
	p_Motor_Fault_Static = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOTOR_FAULT_STATUS);
	//mos �¶�
	p_Mos_Temperature = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOS_TEMPERATURE);
	//���� �¶�
	p_Box_Temperature = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_BOX_TEMPERATURE);
	//��� ����
	p_Motor_Current = (uint32_t*)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOTOR_CURRENT);
	//��� ʵ�� ת��
	p_Motor_Reality_Speed = (uint32_t*)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOTOR_REALITY_SPEED);
	//��� �·� ʵ�� ת��
	p_Send_Reality_Speed = (uint32_t*)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_SEND_REALITY_SPEED);
	
	//��� ʵ�� ����
	p_Motor_Reality_Power = (uint32_t*)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOTOR_REALITY_POWER);
	
	//ĸ�� ��ѹ
	p_Motor_Bus_Voltage = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOTOR_BUS_VOLTAGE);
	//ĸ�� ����
	p_Motor_Bus_Current = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOTOR_BUS_CURRENT);

	//--------------------------- ϵͳ����
	// ״̬��
	p_System_State_Machine = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_SYSTEM_WORKING_STATUS);
	// ��ǰģʽ
	p_PMode_Now = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_SYSTEM_WORKING_MODE);
	// ��ǰ�ٶ�
	p_OP_ShowNow_Speed = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_MOTOR_CURRENT_SPEED);
	// ��ǰʱ��
	p_OP_ShowNow_Time = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_MOTOR_CURRENT_TIME);

	//****************************************************************************************************************************************
	//--------------------------- ��ʱ ���ڹ��ϵȽ����¼����ֵ
	// ״̬��
	p_System_State_Machine_Memory = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_SYSTEM_WORKING_STATUS_MEMORY);
	// ��ǰģʽ
	p_PMode_Now_Memory = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_SYSTEM_WORKING_MODE_MEMORY);
	// ��ǰ�ٶ�
	p_OP_ShowNow_Speed_Memory = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOTOR_CURRENT_SPEED_MEMORY);
	// ��ǰʱ��
	p_OP_ShowNow_Time_Memory = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_MOTOR_CURRENT_TIME_MEMORY);
	//****************************************************************************************************************************************

	//--------------------------- ���� ʹ��
	// ϵͳʱ��
	p_System_Runing_Second_Cnt = (uint32_t *)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER, MB_SYSTEM_RUNNING_TIME);		// ϵͳʱ��
	// ���˲���ʱ��
	p_No_Operation_Second_Cnt = (uint32_t *)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER, MB_NO_OPERATION_TIME);		// ���˲���
	// ����ʱ��
	p_System_Startup_Second_Cnt = (uint32_t *)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER, MB_SYSTEM_SLEEP_TIME);		// ����ʱ��
	
	//--------------------------- 
	p_Analog_key_Value = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_ANALOG_KEY_VALUE);
	
	//--------------------------- ���ͳ�� (APPҪ)
	p_Finish_Statistics_Time 	= Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_FINISH_STATISTICS_TIME);			//	���ͳ�� --> ʱ��
	p_Finish_Statistics_Speed = Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_FINISH_STATISTICS_SPEED);			//	���ͳ�� --> ǿ��
	p_Finish_Statistics_Distance = (uint32_t*)Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_FINISH_STATISTICS_DISTANCE);	//	���ͳ�� --> ��Ӿ����
	p_Preparation_Time_BIT 		= Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,MB_PREPARATION_TIME_BIT);			//	׼��ʱ�� Bit: ��ʱģʽ P1-P6
	
	p_Thread_Activity_Sign 	= Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,MB_THREAD_ACTIVITY_SGIN);			//	�߳� � ��־λ

	//================= ����ģʽ ȫ�� ���� ================================
	Surf_Mode_Info_Get_Mapping();
	
	//================= �ź�ֵ  ================================
	p_BLE_Rssi = 	Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_BLUETOOTH);
	p_WIFI_Rssi = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER, MB_COMM_TEST_WIFI);
	
	//================= ota ��С  ================================
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER,MB_DEBUG_OTA_PAGE_SIZE,*(uint16_t *)BOOT_FLASH_ADDR_OTA_PACK_LEN);
	//Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER,MB_DEBUG_OTA_PAGE_SIZE,*(uint16_t *)BOOT_FLASH_ADDR_DOWNLOAD_PACK_SIZE);
	
}


void MB_InputBuffer_Init(void)
{
	memset(usRegInputBuf,sizeof(usRegInputBuf),0);
}


