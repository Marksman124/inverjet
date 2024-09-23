/**
******************************************************************************
* @file				motor.c
* @brief			电机 相关协议  控制转速命令 200ms  《老郭版》
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "motor.h"
#include "debug_protocol.h"
#include "math.h"
#include "fault.h"
#include "timing.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/

#if (MOTOR_MODULE_HUART == 3)//驱动板串口 UART句柄
UART_HandleTypeDef* p_huart_motor = &huart3;
#elif (MOTOR_MODULE_HUART == 1)
UART_HandleTypeDef* p_huart_motor = &huart1;
#elif (MOTOR_MODULE_HUART == 4)
UART_HandleTypeDef* p_huart_motor = &huart4;
#endif

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
#define MOTOR_SPEED_STEPPING			(1)

#define MOTOR_FAULT_TIMER_MAX			(3-1)
#define MOTOR_TEMP_TIMER_MAX			(10-1)
/* Private variables ---------------------------------------------------------*/

//**************** 收发缓冲区
uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE]={0};//定义一个接收缓存区
uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE]={0};//定义一个发送缓存区

uint8_t Motor_Speed_Now = 0;			// 电机转速 

uint8_t Motor_Speed_Target = 0;

static uint16_t Motor_Timer_Cnt=0;

uint8_t Motor_Heartbeat_Cnt=0;


//uint16_t* p_Speed_Mode; 

uint16_t Motor_Fault_State=0;//电机故障状态

static uint32_t Motor_Rx_Timer_cnt= 0;

static uint32_t Motor_Fault_Timer_cnt= 0;	//故障计数器
static uint32_t Motor_TEMP_Timer_cnt= 0;	//高温 计数器

/* Private user code ---------------------------------------------------------*/

// 初始化
void Metering_Receive_Init(void)
{
	
	//p_Speed_Mode = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_SPEED_MODE );
	//__HAL_UART_ENABLE_IT(p_Metering_Module_Huart, UART_IT_RXNE); //使能IDLE中断
	__HAL_UART_ENABLE_IT(p_huart_motor, UART_IT_IDLE);//使能idle中断
	__HAL_UART_ENABLE_IT(p_huart_motor, UART_IT_ERR);//
	
  HAL_UARTEx_ReceiveToIdle_DMA(p_huart_motor,Motor_DMABuff,MOTOR_RS485_RX_BUFF_SIZE);//打开串口DMA接收
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		   // 手动关闭DMA_IT_HT中断
}

// 重启
void Motor_Usart_Restar(void)
{
	if(HAL_UART_DeInit(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  
  // 重新打开串口
  MX_USART3_UART_Init();
	Metering_Receive_Init();
}


// 清除故障
void Clean_Motor_OffLine_Timer(void)
{
	Motor_Rx_Timer_cnt = 0;
	//Motor_Fault_State &= ~FAULT_MOTOR_LOSS;
	
	Motor_Fault_State = 0;
}

//------------------- 主循环函数  ----------------------------
void App_Motor_Handler(void)
{
	//通信故障计数器
//******************  调试模式 **************************
#ifndef SYSTEM_DEBUG_MODE
	Motor_Rx_Timer_cnt++;  //不报故障 记得删  wuqingguang 2024-09-09
#endif
	
	if(Motor_Timer_Cnt < 10000)
		Motor_Timer_Cnt ++;
	else
		Motor_Timer_Cnt = 0;
	// ===================  通讯故障
	if(Motor_Rx_Timer_cnt > FAULT_MOTOR_LOSS_TIME)
	{
		//驱动板 通讯故障
		Motor_Fault_State |= FAULT_MOTOR_LOSS;
	}
	// ===================  尝试重启串口
	if( ((Motor_Rx_Timer_cnt % FAULT_MOTOR_TRY_RESTAR_TIME)==0) && (Motor_Rx_Timer_cnt >= FAULT_MOTOR_TRY_RESTAR_TIME))
	{
		DEBUG_PRINT("[ERROR]\t驱动板通讯故障 cnt:\t%d\t重启串口\n",Motor_Rx_Timer_cnt);
		Motor_Usart_Restar();
	}
	// ===================  设置转速
	if((Motor_Timer_Cnt % MOTOR_POLLING_PERIOD)==MOTOR_COMMAND_CYCLE)
	{
		//设置转速
		Motor_Speed_Update();
	}
	// ===================  读状态
	else if((Motor_Timer_Cnt % MOTOR_POLLING_PERIOD) == MOTOR_READ_STATIC_CYCLE)
	{
		//读状态  1 s
		Motor_Read_Register();
	}
	// ===================  心跳
	else if((Motor_Timer_Cnt % MOTOR_POLLING_PERIOD) == MOTOR_HEARTBEAT_CYCLE)
	{
		//发心跳
		if((Motor_Speed_Now != 0))//驱动板要求  停机后不发心跳
		{
			//心跳 200ms
			Motor_Heartbeat_Send();
		}
	}
}


//------------------- 同步电机转速 ----------------------------
// 持续发
uint8_t Motor_Speed_Update(void)
{
	uint8_t result=1;
	uint8_t Motor_Acceleration=0;
	
	if((*p_System_State_Machine == TRAINING_MODE_RUNNING)&&(PMode_Now == 5)&&(OP_ShowNow.time > 10))					// 训练 P5
		Motor_Acceleration = Temp_Data_P5_Acceleration;
	else
		Motor_Acceleration = MOTOR_ACCELERATION;
	
	
	if( Motor_Speed_Now > Motor_Speed_Target )
	{
		if((Motor_Speed_Target + Motor_Acceleration) < Motor_Speed_Now)
			Motor_Speed_Now -= Motor_Acceleration;
		else
			Motor_Speed_Now = Motor_Speed_Target;
		result = 1;
	}
	else if( Motor_Speed_Now < Motor_Speed_Target )
	{
		if((Motor_Speed_Now + Motor_Acceleration) < Motor_Speed_Target)
			Motor_Speed_Now += Motor_Acceleration;
		else
			Motor_Speed_Now = Motor_Speed_Target;
		result = 1;
	}
	else
	{
		result = 0;
	}
	
	// 驱动板要求, 停机后不要再发控制指令
	if((Motor_Speed_Now == 0)&&(result == 0))
		return result;
	
	*p_Send_Reality_Speed = Motor_Speed_To_Rpm(Motor_Speed_Now);
	//发送 驱动板
	Motor_Speed_Send(*p_Send_Reality_Speed);
	
	if(result == 1)
	{
		//测试发送串口
		DEBUG_PRINT("电极转速:\t%d\t\t转速:\t%d\t档位:\t%d%%\n",*p_Send_Reality_Speed,*p_Send_Reality_Speed/5,Motor_Speed_Now);
	}
	
	return result;
}

//------------------- 电机转速是否达到目标值 ----------------------------
uint8_t Motor_Speed_Is_Reach(void)
{
	if(Motor_Speed_Now == Motor_Speed_Target)
		return 1;
	else
		return 0;
}

//------------------- 电机转速 目标值 设置 ----------------------------
void Motor_Speed_Target_Set(uint8_t speed)
{
	if(speed > 100)
		speed = 100;
	
	Motor_Speed_Target = speed;
	Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//光圈自动判断
}

//------------------- 电机转速 目标值 设置 ----------------------------
uint8_t Motor_Speed_Target_Get(void)
{
	return Motor_Speed_Target;

}
//------------------- 百分比功率 转 电机转速 ----------------------------
// 100% -->  1960 rpm
// P2/P1=(N2/N1)^3=(Q2/Q1)^3=(V1/V2)^3

uint32_t Motor_Speed_To_Rpm(uint8_t speed)
{
	double speed_rpm;
	double p = 1.0;
	double a = 1.0;
	
	if(speed > 100)
		speed = 100;
	//功率 百分比
//	p = (double)speed/100;
//  a = pow(p, 1.0 / 3);
//	
//	a = a*MOTOR_RPM_SPEED_MAX;
//	
//	speed_rpm = (uint32_t)(a);

	//转速百分比
	//speed_rpm = speed*MOTOR_RPM_CONVERSION_COEFFICIENT;
	if(speed < 20)
	{
		speed_rpm = speed*(MOTOR_RPM_SPEED_MIX/20);
	}
	else if(speed == 20)
		speed_rpm = MOTOR_RPM_SPEED_MIX;
	else if(speed == 100)
		speed_rpm = MOTOR_RPM_SPEED_MAX;
	else
	{
		p = MOTOR_RPM_CONVERSION_COEFFICIENT;
		a = MOTOR_RPM_CONVERSION_COEFFICIENT_20;
		
		speed_rpm = (((speed-20)/20)*a) + ((speed%20)*p)+ MOTOR_RPM_SPEED_MIX;			
	}
	
	return (uint32_t)speed_rpm;
}

//------------------- 故障类型转换 ----------------------------
uint8_t Change_Faule_To_Upper(uint8_t type)
{
	uint8_t change_fault=0;
	
	if(type > 0)
	{
		//-----------母线电压 过压 | 欠压
		if((type == 0x01)||(type == 0x02))
			change_fault = FAULT_BUS_VOLTAGE_ABNORMAL;
		//----------- 输出电压异常
		else if(type == 0x03)
			change_fault = FAULT_ABNORMAL_OUTPUT_VOLTAGE;
		//-----------过流
		else if(type == 0x04)
			change_fault = FAULT_BUS_CURRENT_ABNORMAL;
		//----------- MOS 过热
		else if(type == 0x05)
			change_fault = FAULT_TEMPERATURE_AMBIENT;
		//-----------电流传感器1\2\3 偏置故障-硬件误差过大
		else if(type == 0x12)
			change_fault = FAULT_BUS_CURRENT_BIAS;
//		//----------- mos管 过热
//		else if(type == 0x06)
//			change_fault = FAULT_TEMPERATURE_MOS;
		//----------- 其它 故障
		else
			change_fault = FAULT_MOTOR_DRIVER;
	}
	
	return change_fault;
}

/*-------------------- 收发处理 ----------------------------------------------*/
uint16_t CRC16_XMODEM_T(uint8_t *ptr, uint16_t len)
{
    const uint16_t crc_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    };

    uint16_t crc = 0x0000;
    
    while(len--) 
    {
        crc = (crc << 8) ^ crc_table[(crc >> 8 ^ *ptr++) & 0xff];
    }
    
    return(crc);
}

//-------------------- 电机状态解析 ----------------------------
void Motor_State_Analysis(void)
{
	//static uint32_t Rx_cnt= 0;
	uint16_t result_fault=0;
	short int Temperature=0;
	Motor_Rx_Timer_cnt = 0;
	uint16_t ntc_tmp[3]={0};
	
	//驱动板 通讯故障 恢复
	Motor_Fault_State &= ~FAULT_MOTOR_LOSS;

	//
	// 滤波后的mosfet温度
	Temperature = Motor_State_Storage[MOTOR_ADDR_MOSFET_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_MOSFET_TEMP_OFFSET+1];
	memcpy(p_Mos_Temperature, &Temperature, 2);
	
	// 滤波后的电机温度 改用 软件版本
	*p_Driver_Software_Version = Motor_State_Storage[MOTOR_ADDR_MOTOR_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_MOTOR_TEMP_OFFSET+1];
	// 电机平均电流
	double mosfet_current = Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET]<<24 |Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET+1]<<16 |Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET+2]<<8 | Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET+3];
	*p_Motor_Current = (uint16_t)(mosfet_current/1.4);
	// 当前电气转速erpm
	*p_Motor_Reality_Speed = Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET]<<24 |Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET+1]<<16 |Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET+2]<<8 | Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET+3];
	// 母线电压
	*p_Motor_Bus_Voltage = Motor_State_Storage[MOTOR_ADDR_BUS_VOLTAGE_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_BUS_VOLTAGE_OFFSET+1];
	// 获取电机故障
	*p_Motor_Fault_Static = Motor_State_Storage[MOTOR_ADDR_MOTOR_FAULT_OFFSET];
	
	// 10KNTC温度1 2 3
	ntc_tmp[0] = Motor_State_Storage[MOTOR_ADDR_NTC1_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_NTC1_TEMP_OFFSET+1];
	ntc_tmp[1] = Motor_State_Storage[MOTOR_ADDR_NTC2_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_NTC2_TEMP_OFFSET+1];
	ntc_tmp[2] = Motor_State_Storage[MOTOR_ADDR_NTC3_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_NTC3_TEMP_OFFSET+1];
	
	DEBUG_PRINT("\n\n\nmosfet温度:\t%d.%d\n电机温度:\t%d.%d\n电机电流:\t%d.%d\n转速:\t\t\t\t%d\n母线电压:\t%d.%d\n电机故障:\t\t%d\n10KNTC温度1 2 3:\t%d.%d\t%d.%d\t%d.%d\n\n",
			Temperature/10,Temperature%10,*p_Driver_Software_Version/10,*p_Driver_Software_Version%10,*p_Motor_Current/100,*p_Motor_Current%100,
			*p_Motor_Reality_Speed,*p_Motor_Bus_Voltage/10,*p_Motor_Bus_Voltage%10,*p_Motor_Fault_Static,
			ntc_tmp[0]/10,ntc_tmp[0]%10,ntc_tmp[1]/10,ntc_tmp[1]%10,ntc_tmp[2]/10,ntc_tmp[2]%10);
		
	CLEAN_MOTOR_FAULT(Motor_Fault_State);
	if(*p_Motor_Fault_Static > 0)
	{
		if(Motor_Fault_Timer_cnt < MOTOR_FAULT_TIMER_MAX)
			Motor_Fault_Timer_cnt ++;
		else
		{
			result_fault = Change_Faule_To_Upper(*p_Motor_Fault_Static);
			Motor_Fault_State |= result_fault;
		}
	}
	else
	{
		Motor_Fault_Timer_cnt = 0;
	}
	
	// 高温降速	mos

	if(Temperature >= (MOS_TEMP_ALARM_VALUE*10))				//-------------  停机
	{
		if(Motor_TEMP_Timer_cnt < (MOTOR_TEMP_TIMER_MAX*2))
			Motor_TEMP_Timer_cnt ++;
		else
		{
			Motor_Fault_State |= FAULT_TEMPERATURE_AMBIENT;
		}
	}
	else if(Temperature >= (MOS_TEMP_REDUCE_SPEED*10))				//-------------  降速
	{
		if(Motor_TEMP_Timer_cnt < MOTOR_TEMP_TIMER_MAX)
			Motor_TEMP_Timer_cnt ++;
		else
		{
			//预警 降速
			if(Get_Temp_Slow_Down_State() == 0)
				Set_Temp_Slow_Down_State(1);
		}
	}
	else if(Temperature <= (MOS_TEMP_RESTORE_SPEED*10))				//-------------  恢复
	{
		if(Motor_TEMP_Timer_cnt > 0)
			Motor_TEMP_Timer_cnt = 0;
		else
		{
			if(Get_Temp_Slow_Down_State() == 1)
				Set_Temp_Slow_Down_State(0);
		}
	}
	
}

//================================================== 内部调用接口

//-------------------- 获取电机故障状态 ----------------------------
uint16_t Get_Motor_Fault_State(void)
{
	
	return Motor_Fault_State;
}


//-------------------- 发送 ----------------------------
void Motor_UART_Send(uint8_t* p_buff, uint8_t len)
{
#ifdef MOTOR_MODULE_HUART
	HAL_UART_Transmit(p_huart_motor, p_buff, len, 0xFFFF);
	
#endif
}
//-------------------- 接收 ----------------------------
void Motor_RxData(uint8_t len)
{
	uint16_t crc_value=0;
	
	// 检查 长度
	if((Motor_DMABuff[1]+5) != len)
	{
		DEBUG_PRINT("[ERROR]\t接收长度错误:\t收到:\t%d   实际:\t%d\n",Motor_DMABuff[1]+5, len);
		return;
	}
	
	//检查crc
	crc_value = CRC16_XMODEM_T(&Motor_DMABuff[2], Motor_DMABuff[1] );
	//crc_read = ((Motor_DMABuff[len-2]<<8) & Motor_DMABuff[len-1]);
	if(crc_value != ((Motor_DMABuff[len-3]<<8) | Motor_DMABuff[len-2]))
	{
		DEBUG_PRINT("[ERROR]\tcrc校验错误:\t计算得到:\t%d   收到的:\t%d\n",crc_value, ((Motor_DMABuff[len-3]<<8) & Motor_DMABuff[len-2]));
		return;
	}
	
	memcpy(Motor_State_Storage, &Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET], MOTOR_PROTOCOL_ADDR_MAX);
	Motor_State_Analysis();

}



/*-------------------- 快速发送命令 ----------------------------------------------*/
//------------------- 设置电机转速 ----------------------------
void Motor_Speed_Send(uint32_t speed_rpm)
{
	uint8_t buffer[10]={0x02,0x05,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x03};
	uint16_t crc_value=0;
	
	buffer[3] = speed_rpm>>24;
	buffer[4] = speed_rpm>>16;
	buffer[5] = speed_rpm>>8;
	buffer[6] = speed_rpm & 0xFF;
	
	//crc
	
	crc_value = CRC16_XMODEM_T(&buffer[2], 5 );
	
	buffer[7] = crc_value>>8;
	buffer[8] = crc_value & 0xFF;
	
	Motor_UART_Send(buffer, 10);
}


//------------------- 心跳 ----------------------------
void Motor_Heartbeat_Send(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}

//------------------- 读故障状态 ----------------------------
void Motor_ReadFault_Send(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}

//-------------------- 读寄存器 ( 全读 )----------------------------
void Motor_Read_Register(void)
{
	uint8_t buffer[10]={0x02,0x01,0x04,0x40,0x84,0x03};

	Motor_UART_Send(buffer, 6);
}

//-------------------- 读 软硬件版本 ----------------------------
void Motor_Read_Version(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}

//-------------------- 读 故障 ----------------------------
void Motor_Read_Fault(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}

//-------------------- 检查电机电流 ----------------------------
uint8_t Check_Motor_Current(void)
{
#ifdef MOTOR_CURRENT_MIX
	if(Motor_Speed_Now >= MOTOR_POWER_SPEED)
	{
		if(*p_Motor_Current < MOTOR_CURRENT_MIX)
			return 0;
	}
#endif
	return 1;
}

//-------------------- 检查电机转速 ----------------------------
uint8_t Check_Motor_Speed(void)
{
#ifdef MOTOR_SPEED_VIBRATION_RANGE
	uint32_t rpm = Motor_Speed_To_Rpm(Motor_Speed_Now);
	
	if(rpm  > *p_Motor_Reality_Speed)
	{
		if((rpm - *p_Motor_Current) > MOTOR_SPEED_VIBRATION_RANGE)
			return 0;
	}
	else
	{
		if((*p_Motor_Current - rpm) > MOTOR_SPEED_VIBRATION_RANGE)
			return 0;
	}
#endif
	return 1;
}
