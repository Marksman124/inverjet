/**
******************************************************************************
* @file    		metering.c
* @brief   		计量模块通讯协议
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "metering.h"


/* Private variables ---------------------------------------------------------*/

#if (METERING_MODULE_HUART == 1)
UART_HandleTypeDef* p_Metering_Module_Huart = &huart1;
#elif (METERING_MODULE_HUART == 4)
UART_HandleTypeDef* p_Metering_Module_Huart = &huart4;
#endif


//**************** 收发缓冲区
uint8_t Metering_DMABuff[METERING_RS485_RX_BUFF_SIZE]={0};//定义一个接收缓存区
//uint8_t Metering_RxBuff[METERING_RS485_RX_BUFF_SIZE]={0};//定义一个正真的接收数组
uint8_t Metering_TxBuff[METERING_RS485_TX_BUFF_SIZE]={0};//定义一个发送缓存区

//**************** 轮询周期计数
uint16_t Metering_PERIODIC_CNT = 0;

#define METERING_LEISURE 						( 0 )
#define METERING_READING 						( 1 )
uint8_t Metering_PERIODIC_Static = METERING_LEISURE;

//****************  映射 参数
uint16_t* p_mb_metering_Address=NULL;			//	模块地址

//**************** 快速查询表
// 0: Modbus   1: Metering
uint16_t HoldingAddrTable[5][2] = {{0x20,0xFFFF},{0x21,0x0002},{0x22,0x0006},{0x23,0x000C},{0x24,0xFFFF},};


uint16_t InputAddrTable[16][3] = {
{0x10,0x200E,100},	{0x12,0xFFFF,1},		{0x14,0x4000,10000},{0x16,0xFFFF,1000},
{0x18,0xFFFF,1000},	{0x1A,0x2004,10000},{0x1C,0xFFFF,10000},{0x1E,0xFFFF,10000},
{0x20,0x200A,1000},	{0x22,0x2000,10000},{0x24,0x2002,10000},{0x26,0xFFFF,10},
{0x28,0xFFFF,1000},	{0x2A,0xFFFF,10},		{0x2C,0xFFFF,1000},	{0x2E,0xFFFF,1000},
};

//**************** 消息队列
osMessageQId MeteringQueueHandle;

// 外部定义
extern DMA_HandleTypeDef hdma_usart1_rx;

/* Private defines -----------------------------------------------------------*/
#define HOLDING_TABLE_LEN 			(sizeof(HoldingAddrTable[0])/sizeof(HoldingAddrTable[0][0]))
#define INPUT_TABLE_LEN 			(sizeof(InputAddrTable[0])/sizeof(InputAddrTable[0][0]))
	
#define FLOAT_DATA_ADDR 							( 0x1000 )
/* Private user code ---------------------------------------------------------*/

/*-------------------- 协议转换 ----------------------------------------------*/
//根据协议转换  地址号
uint16_t DataAddr_Metering_to_Modbus(uint16_t addr)
{
	uint16_t i,sum;
	
	sum = HOLDING_TABLE_LEN;
		
	
	for(i=0; i<sum; i++)
	{
		if(HoldingAddrTable[i][1] == addr)
			return HoldingAddrTable[i][0];
	}
	
	sum = INPUT_TABLE_LEN;
	
	for(i=0; i<sum; i++)
	{
		if(InputAddrTable[i][1] == addr)
			return InputAddrTable[i][0];
	}
	
	return 0xFFFF;
}

//根据协议转换  功能码
uint8_t FunctionCode_Metering_to_Modbus(uint16_t addr)
{
	uint16_t i,sum;
	
	
	sum = HOLDING_TABLE_LEN;
	for(i=0; i<sum; i++)
	{
		if(HoldingAddrTable[i][1] == addr)
			return MB_FUNC_READ_HOLDING_REGISTER;
	}
	
	sum = INPUT_TABLE_LEN;
	for(i=0; i<sum; i++)
	{
		if(InputAddrTable[i][1] == addr)
			return MB_FUNC_READ_INPUT_REGISTER;
	}
	
	return 0;
}

//根据协议转换  地址号
uint16_t DataAddr_Modbus_to_Metering(uint8_t fun_code, uint16_t addr)
{
	uint16_t i,sum;
	
	if( fun_code == MB_FUNC_READ_HOLDING_REGISTER )
	{
		sum = HOLDING_TABLE_LEN;
		for(i=0; i<sum; i++)
		{
			if(HoldingAddrTable[i][0] == addr)
				return HoldingAddrTable[i][1];
		}
	}
	else if( fun_code == MB_FUNC_READ_INPUT_REGISTER )
	{
		sum = INPUT_TABLE_LEN;
		for(i=0; i<sum; i++)
		{
			if(InputAddrTable[i][0] == addr)
				return InputAddrTable[i][1];
		}
	}
	
	return 0xFFFF;
}

//根据协议转换  功能码
uint8_t FunctionCode_Modbus_to_Metering(uint8_t fun_code, uint16_t addr)
{
	// 计量模块功能码  03
	return MB_FUNC_READ_HOLDING_REGISTER;
}


//
uint16_t uint8_t_to_uint16_t(uint8_t* p_buff)
{
	uint16_t metering_addr=0;
	
	metering_addr = *p_buff++ << 8;
	metering_addr |= *p_buff++;
	
	return metering_addr;
}

/*-------------------- 收发处理 ----------------------------------------------*/
//发送
void Metering_UART_Send(uint8_t* p_buff, uint8_t len)
{
#ifdef METERING_MODULE_HUART
	HAL_GPIO_WritePin(METERING_RS485_TX_EN_PORT, METERING_RS485_TX_EN_PIN, GPIO_PIN_SET);
	HAL_UART_Transmit(p_Metering_Module_Huart, p_buff, len, 50);
	HAL_GPIO_WritePin(METERING_RS485_TX_EN_PORT, METERING_RS485_TX_EN_PIN, GPIO_PIN_RESET);
#endif
}

IntUint value_data;
FloatUint float_uint;

//接收
void Metering_RxData(uint8_t len)
{
	uint16_t write_addr;
	uint8_t	write_function;
	
	uint16_t metering_addr;//上层modbus的寄存器地址
//	IntUint value_data;
//	FloatUint float_uint;
	
#ifdef METERING_MODULE_HUART
	
	if( (Metering_PERIODIC_CNT != 0xFFFF) && (Metering_PERIODIC_Static == METERING_READING) )
	{
		metering_addr = InputAddrTable[Metering_PERIODIC_CNT][1];
		write_function = FunctionCode_Metering_to_Modbus(metering_addr);
		write_addr = DataAddr_Metering_to_Modbus(metering_addr);
		
		if( (metering_addr >= FLOAT_DATA_ADDR) && (Metering_DMABuff[2] == 4) )
		{
			float_uint.c[0] = Metering_DMABuff[6];
			float_uint.c[1] = Metering_DMABuff[5];
			float_uint.c[2] = Metering_DMABuff[4];
			float_uint.c[3] = Metering_DMABuff[3];

			value_data.i = (int)(float_uint.f * InputAddrTable[Metering_PERIODIC_CNT][2]);
			
			Set_DataValue_U32( write_function, write_addr, value_data.c );
		}
		else
		{
			Set_DataAddr_Value( write_function, write_addr, (Metering_DMABuff[3]<<8 | Metering_DMABuff[4]));
		}
		Metering_PERIODIC_Static = METERING_LEISURE;
	}
	memset(Metering_DMABuff,0,METERING_RS485_RX_BUFF_SIZE);    				//清空缓存区
	__HAL_UART_CLEAR_IDLEFLAG(p_Metering_Module_Huart);               //清除标志位
	HAL_UART_Receive_DMA(p_Metering_Module_Huart,Metering_DMABuff,METERING_RS485_RX_BUFF_SIZE);  //开DMA接收，数据存入rx_buffer数组中。
#endif
}

/*-------------------- 协议组包 ----------------------------------------------*/
// 周期查询
void Metering_Periodic_Polling(uint8_t* p_buff,uint16_t data_addr)
{
	uint16_t crc_value=0;
	
	Check_Metering_Address(p_mb_metering_Address);
	
	p_buff[0] = *p_mb_metering_Address;//从机号
	p_buff[1] = FunctionCode_Modbus_to_Metering( MB_FUNC_READ_INPUT_REGISTER, data_addr );//功能码
	p_buff[2] = ( data_addr >> 8 );
	p_buff[3] = ( data_addr & 0xFF );
	p_buff[4] = 0;
	if(data_addr >= FLOAT_DATA_ADDR)
		p_buff[5] = 2;
	else
		p_buff[5] = 1;
	crc_value = usMBCRC16(p_buff, 6 );
	p_buff[6] = ( crc_value & 0xFF );
	p_buff[7] = ( crc_value >> 8 );
}


//组包 功能04 
uint8_t Metering_Check_Addr_Legal(uint8_t fun_code, uint16_t data_addr)
{
	if(fun_code == 3)
	{
		if( (data_addr < 0x0020) || (data_addr > 0x0024) )
			return 0;
	}
	else if(fun_code == 4)
	{
		if( (data_addr < METERING_ADDR_START) || (data_addr > METERING_ADDR_END) )
			return 0;
	}
	
	return 1;
}



//组包 功能04 
void Metering_Packet_Type(uint8_t* p_buff,uint8_t fun_code, uint16_t data_addr, uint16_t value)
{
	uint16_t crc_value=0;
	
	p_buff[0] = *p_mb_metering_Address;//从机号
	p_buff[1] = fun_code;//功能码
	p_buff[2] = ( data_addr >> 8 );
	p_buff[3] = ( data_addr & 0xFF );
	p_buff[4] = value>>8;
	p_buff[5] = value & 0xFF;
	crc_value = usMBCRC16(p_buff, 6 );
	p_buff[6] = ( crc_value & 0xFF );
	p_buff[7] = ( crc_value >> 8 );
}


//获取轮询
uint16_t Metering_Get_polling_cnt(uint16_t cnt)
{
	uint16_t i,sum,result;
	
	sum = INPUT_TABLE_LEN;
	
	if((cnt + 1) >= sum)
		result = 0;
	else
		result = cnt + 1;
	
	for(i=result; i<sum; i++)
	{
		if(InputAddrTable[i][1] != 0xFFFF)
		{
			result = i;
			return result;
		}
	}
	
	for(i=0; i<sum; i++)
	{
		if(InputAddrTable[i][1] != 0xFFFF)
		{
			result = i;
			return result;
		}
	}	
	return result;
}



/*-------------------- 功能接口 ----------------------------------------------*/

// 修改波特率
void USART_Config(uint32_t baud)
{
	/* Disable the peripheral */
  __HAL_UART_DISABLE(p_Metering_Module_Huart);
#if (METERING_MODULE_HUART == 1)
	huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
#elif (METERING_MODULE_HUART == 4)
	huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
#endif
	
	/* Enable the peripheral */
  __HAL_UART_ENABLE(p_Metering_Module_Huart);
}



/*-------------------- 基本接口 ----------------------------------------------*/
// 初始化
void Metering_Receive_Init(void)
{
#ifdef METERING_MODULE_HUART
	//__HAL_UART_ENABLE_IT(p_Metering_Module_Huart, UART_IT_RXNE); //使能IDLE中断
	__HAL_UART_ENABLE_IT(p_Metering_Module_Huart, UART_IT_IDLE);//使能idle中断
	__HAL_UART_ENABLE_IT(p_Metering_Module_Huart, UART_IT_ERR);//
	
  HAL_UART_Receive_DMA(p_Metering_Module_Huart,Metering_DMABuff,METERING_RS485_RX_BUFF_SIZE);//打开串口DMA接收
	
	//定义消息队列的名称，大小，类型
	osMessageQDef(MeteringQueue, 32, uint32_t);
	//创建消息队列
	MeteringQueueHandle = osMessageCreate(osMessageQ(MeteringQueue), NULL);
#endif
}

// 主处理函数   500ms 周期
void Metering_Protocol_Analysis(void)
{
#ifdef METERING_MODULE_HUART
	
  osEvent QueueEvent;
	uint16_t modbus_addr,meter_addr;
	uint16_t value;
	uint8_t table_mode;
	
	// 主动命令
  QueueEvent = osMessageGet (MeteringQueueHandle, 0);
	if(QueueEvent.status == osEventMessage)
	{
		modbus_addr = QueueEvent.value.v>>16;
		value = QueueEvent.value.v & 0xFFFF;
		meter_addr = DataAddr_Modbus_to_Metering(MB_FUNC_READ_HOLDING_REGISTER, modbus_addr);
		if(meter_addr != 0xFFFF)
		{
			Metering_Packet_Type(Metering_TxBuff, MB_FUNC_WRITE_REGISTER, meter_addr, value);
			Metering_UART_Send(Metering_TxBuff, METERING_RS485_TX_BUFF_SIZE);
			HAL_Delay(50); // 等待回复？
			if(meter_addr == METERING_ADDR_BAUDRATE) // 设置波特率
			{
				__HAL_UART_DISABLE(p_Metering_Module_Huart);
#if METERING_MODULE_HUART == 1
				MX_USART1_UART_Init();
#elif METERING_MODULE_HUART == 3
				MX_USART3_UART_Init();
#endif
				__HAL_UART_ENABLE(p_Metering_Module_Huart);
			}
		}
	}
	// 定期轮询
	if(Metering_PERIODIC_Static == METERING_LEISURE)
		Metering_PERIODIC_CNT = Metering_Get_polling_cnt(Metering_PERIODIC_CNT);
	
	if(Metering_PERIODIC_CNT != 0xFFFF)
	{
		//if(DataAddrTable[Metering_PERIODIC_CNT][1] <= )
		{
			Metering_Periodic_Polling(Metering_TxBuff, InputAddrTable[Metering_PERIODIC_CNT][1]);
			Metering_UART_Send(Metering_TxBuff, METERING_RS485_TX_BUFF_SIZE);
			Metering_PERIODIC_Static = METERING_READING;
		}
	}
#endif
}

uint8_t Check_Metering_Address(uint16_t* p_addr)
{
	if(*p_addr > METERING_ADDR_MAX)
	{
		*p_addr = MB_METERING_ADDR_DEFAULT;
	}

	return 1;
}


