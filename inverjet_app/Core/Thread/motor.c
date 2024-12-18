/**
******************************************************************************
* @file				motor.c
* @brief			��� ���Э��  ����ת������ 200ms  ���Ϲ��桷
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

#if (MOTOR_MODULE_HUART == 3)//�����崮�� UART���
UART_HandleTypeDef* p_huart_motor = &huart3;
#elif (MOTOR_MODULE_HUART == 1)
UART_HandleTypeDef* p_huart_motor = &huart1;
#elif (MOTOR_MODULE_HUART == 4)
UART_HandleTypeDef* p_huart_motor = &huart4;
#endif

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
#define MOTOR_SPEED_STEPPING			(1)

/* Private variables ---------------------------------------------------------*/

//**************** �շ�������
uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE]={0};//����һ�����ջ�����
uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE]={0};//����һ�����ͻ�����

uint8_t Motor_Speed_Now = 0;			// ���ת�� 

uint8_t Motor_Speed_Target = 0;

static uint16_t Motor_Timer_Cnt=0;

uint8_t Motor_Heartbeat_Cnt=0;


//uint16_t* p_Speed_Mode; 

uint16_t Motor_Fault_State=0;//�������״̬

static uint32_t Motor_Rx_Timer_cnt= 0;

static uint32_t Motor_Fault_Timer_cnt= 0;			// ���ϼ�����
static uint32_t Motor_TEMP_Timer_cnt= 0;			// ���� ������
static uint32_t Motor_Current_Timer_cnt= 0;		// ���� ������
static uint32_t Motor_Power_Timer_cnt= 0;			// ���� ������


#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
static uint8_t Rx_State= 0;		//�ӻ�����״̬
#endif

/* Private user code ---------------------------------------------------------*/

// ��ʼ��
void Metering_Receive_Init(void)
{
	
	//p_Speed_Mode = Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER, MB_MOTOR_SPEED_MODE );
	//__HAL_UART_ENABLE_IT(p_Metering_Module_Huart, UART_IT_RXNE); //ʹ��IDLE�ж�
	__HAL_UART_ENABLE_IT(p_huart_motor, UART_IT_IDLE);//ʹ��idle�ж�
	__HAL_UART_ENABLE_IT(p_huart_motor, UART_IT_ERR);//
	
  HAL_UARTEx_ReceiveToIdle_DMA(p_huart_motor,Motor_DMABuff,MOTOR_RS485_RX_BUFF_SIZE);//�򿪴���DMA����
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
}

// ����
void Motor_Usart_Restar(void)
{
	if(HAL_UART_DeInit(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  
  // ���´򿪴���
  MX_USART3_UART_Init();
	Metering_Receive_Init();
}


// �������
void Clean_Motor_OffLine_Timer(void)
{
	Motor_Rx_Timer_cnt = 0;
	
	Motor_Fault_State = 0;
}

//------------------- ��ѭ������  ----------------------------
void App_Motor_Handler(void)
{
	Thread_Activity_Sign_Set(THREAD_ACTIVITY_MOTOR);
	
	//ͨ�Ź��ϼ�����
	Motor_Rx_Timer_cnt++;  //�������� �ǵ�ɾ  wuqingguang 2024-09-09
	
	if(Motor_Timer_Cnt < 10000)
		Motor_Timer_Cnt ++;
	else
		Motor_Timer_Cnt = 0;
	// ===================  ͨѶ����
	//******************  ����ģʽ **************************
#ifndef SYSTEM_DEBUG_MODE
	if(IS_SELF_TEST_MODE())
	{
		if(Motor_Rx_Timer_cnt > (FAULT_MOTOR_LOSS_TIME/3))
			Set_Motor_Fault_State( E203_MOTOR_LOSS );							//������ ͨѶ����
	}
	else
	{
		if(Motor_Rx_Timer_cnt > FAULT_MOTOR_LOSS_TIME)
			Set_Motor_Fault_State( E203_MOTOR_LOSS );							//������ ͨѶ����
	}
#endif
	// ===================  ������������
	if( ((Motor_Rx_Timer_cnt % FAULT_MOTOR_TRY_RESTAR_TIME)==0) && (Motor_Rx_Timer_cnt >= FAULT_MOTOR_TRY_RESTAR_TIME))
	{
		DEBUG_PRINT("[ERROR]\t������ͨѶ���� cnt:\t%d\t��������\n",Motor_Rx_Timer_cnt);
		Motor_Usart_Restar();
	}
	
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
//����������������������������������������������������������������������������������������������������
	//*********************************************************************************************
	//* ------ ���������� ------ *******************************************************************
	//*********************************************************************************************
	
	// ===================  ����ת��
	if((Motor_Timer_Cnt % MOTOR_POLLING_PERIOD)==MOTOR_COMMAND_CYCLE)
	{
		//����ת��
		Motor_Speed_Update();
	}
	// ===================  ��״̬
	else if((Motor_Timer_Cnt % MOTOR_POLLING_PERIOD) == MOTOR_READ_STATIC_CYCLE)
	{
		//��״̬  1 s
		Motor_Read_Register();
	}
	// ===================  ����
	else if((Motor_Timer_Cnt % MOTOR_POLLING_PERIOD) == MOTOR_HEARTBEAT_CYCLE)
	{
		//������
		if((Motor_Speed_Now != 0))//������Ҫ��  ͣ���󲻷�����
		{
			//���� 200ms
			Motor_Heartbeat_Send();
		}
	}
//����������������������������������������������������������������������������������������������������
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
//����������������������������������������������������������������������������������������������������
	//*********************************************************************************************
	//* ------ ���������� ------ *******************************************************************
	//*********************************************************************************************
	if(Rx_State == 1)	// ����
	{
		//Rx_State = 2;
		Motor_State_Analysis();
//	}
//	else if(Rx_State == 2)	// �ظ�
//	{
		Rx_State = 0;
		Motor_Speed_Update();
	}
//����������������������������������������������������������������������������������������������������
#endif
}


//------------------- ͬ�����ת�� ----------------------------
// ������
uint8_t Motor_Speed_Update(void)
{
	uint8_t result=1;
	uint8_t Motor_Acceleration=0;
	
	if((Get_System_State_Machine() == TRAINING_MODE_RUNNING)&&(PMode_Now == 5)&&(OP_ShowNow.time > 10))					// ѵ�� P5
		Motor_Acceleration = *p_Surf_Mode_Info_Acceleration;
	else
		Motor_Acceleration = MOTOR_ACCELERATION;
	
	
	if( Motor_Speed_Now > Motor_Speed_Target )
	{
		if((Motor_Speed_Target == 0) && (Motor_Speed_Now <= MOTOR_ACTUAL_SPEED_MIN))// ���ֱ�ӹ�
			Motor_Speed_Now = 0;
		else
		{
			if((Motor_Speed_Target + Motor_Acceleration) < Motor_Speed_Now)
				Motor_Speed_Now -= Motor_Acceleration;
			else
				Motor_Speed_Now = Motor_Speed_Target;
		}
		result = 1;
	}
	else if( Motor_Speed_Now < Motor_Speed_Target )
	{
		if((Motor_Speed_Now == 0) && (Motor_Speed_Target >= MOTOR_ACTUAL_SPEED_MIN))// ���ֱ�ӿ�
			Motor_Speed_Now = MOTOR_ACTUAL_SPEED_MIN;
		else
		{
			if((Motor_Speed_Now + Motor_Acceleration) < Motor_Speed_Target)
				Motor_Speed_Now += Motor_Acceleration;
			else
				Motor_Speed_Now = Motor_Speed_Target;
		}
		result = 1;
	}
	else
	{
		result = 0;
	}
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
	// ������Ҫ��, ͣ����Ҫ�ٷ�����ָ��
	if((Motor_Speed_Now == 0)&&(result == 0))
		return result;
#endif
	
	*p_Send_Reality_Speed = Motor_Speed_To_Rpm(Motor_Speed_Now);
	//���� ������
	Motor_Speed_Send(*p_Send_Reality_Speed);
	
	if(result == 1)
	{
		//���Է��ʹ���
		DEBUG_PRINT("����ת��:\t%d\t\tת��:\t%d\t��λ:\t%d%%\n",*p_Send_Reality_Speed,*p_Send_Reality_Speed/5,Motor_Speed_Now);
	}
	
	return result;
}

//------------------- ���ת���Ƿ�ﵽĿ��ֵ ----------------------------
uint8_t Motor_Speed_Is_Reach(void)
{
	if(Motor_Speed_Now == Motor_Speed_Target)
		return 1;
	else
		return 0;
}

//------------------- ���ת�� Ŀ��ֵ ���� ----------------------------
void Motor_Speed_Target_Set(uint8_t speed)
{
	if(speed > MOTOR_PERCENT_SPEED_MAX)
		speed = MOTOR_PERCENT_SPEED_MAX;
	
	if((speed < MOTOR_PERCENT_SPEED_MIX) && (speed > 0))
		speed = MOTOR_PERCENT_SPEED_MIX;

	if(Motor_Speed_Target != speed)
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//��Ȧ�Զ��ж�
	
	Motor_Speed_Target = speed;
}

//------------------- ��� ����ֹͣ ----------------------------
void Motor_Quick_Stop(void)
{
	if(Motor_Speed_Target != 0)
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//��Ȧ�Զ��ж�
	
	Motor_Speed_Target = 0;
	Motor_Speed_Now = 1;
}

//------------------- ���ת�� Ŀ��ֵ ���� ----------------------------
uint8_t Motor_Speed_Target_Get(void)
{
	return Motor_Speed_Target;

}
//------------------- �ٷֱȹ��� ת ���ת�� ----------------------------
// 100% -->  1960 rpm
// P2/P1=(N2/N1)^3=(Q2/Q1)^3=(V1/V2)^3

uint32_t Motor_Speed_To_Rpm(uint8_t speed)
{
	double speed_rpm;
	double p = 1.0;
	double a = 1.0;
	
	if(speed > 100)
		speed = 100;
	//���� �ٷֱ�
//	p = (double)speed/100;
//  a = pow(p, 1.0 / 3);
//	
//	a = a*MOTOR_RPM_SPEED_MAX;
//	
//	speed_rpm = (uint32_t)(a);

	//ת�ٰٷֱ�
	//speed_rpm = speed*MOTOR_RPM_CONVERSION_COEFFICIENT;
	if(speed < MOTOR_PERCENT_SPEED_MIX)
	{
		speed_rpm = speed*(MOTOR_RPM_SPEED_MIX/MOTOR_PERCENT_SPEED_MIX);
	}
	else if(speed == MOTOR_PERCENT_SPEED_MIX)
		speed_rpm = MOTOR_RPM_SPEED_MIX;
	else if(speed == MOTOR_PERCENT_SPEED_MAX)
		speed_rpm = MOTOR_RPM_SPEED_MAX;
	else
	{
		p = MOTOR_RPM_CONVERSION_COEFFICIENT;
		a = MOTOR_RPM_CONVERSION_COEFFICIENT_20;
		
		speed_rpm = (((speed-MOTOR_PERCENT_SPEED_MIX)/MOTOR_PERCENT_SPEED_MIX)*a) + ((speed%MOTOR_PERCENT_SPEED_MIX)*p)+ MOTOR_RPM_SPEED_MIX;			
	}
	
	return (uint32_t)speed_rpm;
}

uint8_t Motor_Rpm_To_Speed(uint32_t speed_rpm)
{
	uint32_t speed;
	
	if(speed_rpm > MOTOR_RPM_SPEED_MAX)
		return MOTOR_PERCENT_SPEED_MAX;

	if(speed_rpm < MOTOR_RPM_SPEED_MIX)
	{
		speed = speed_rpm/(MOTOR_RPM_SPEED_MIX/MOTOR_PERCENT_SPEED_MIX);
	}
	else if(speed_rpm == MOTOR_RPM_SPEED_MIX)
		speed = MOTOR_PERCENT_SPEED_MIX;
	else if(speed_rpm == MOTOR_RPM_SPEED_MAX)
		speed = MOTOR_PERCENT_SPEED_MAX;
	else
	{
		speed = MOTOR_PERCENT_SPEED_MIX + ((speed_rpm-MOTOR_RPM_SPEED_MIX)*80/(MOTOR_RPM_SPEED_MAX - MOTOR_RPM_SPEED_MIX));
	}
	
	return (speed&0xFF);
}

//------------------- ��������ת�� ----------------------------
uint16_t Change_Faule_To_Upper(uint8_t type)
{
	uint16_t change_fault=0;
	
	if(type == 0)
		return 0;
	
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
//����������������������������������������������������������������������������������������������������
	if((type >= MOTOR_FAULT_CODE_START) && (type <= MOTOR_FAULT_CODE_END))
	{
		if((type == MOTOR_FAULT_OVER_VOLTAGE ) || (type == MOTOR_FAULT_UNDER_VOLTAGE ))	//-----------ĸ�ߵ�ѹ ��ѹ | Ƿѹ 01 02
			change_fault = E001_BUS_VOLTAGE_ABNORMAL;
		
		else if(type == MOTOR_FAULT_ABS_OVER_CURRENT)			//----------- ����  04
			change_fault = E002_BUS_CURRENT_ABNORMAL;
		
		else if((type >= MOTOR_FAULT_HIGH_OFFSET_CURRENT_SENSOR_1) && (type <= MOTOR_FAULT_UNBALANCED_CURRENTS ) )//-----------������������ƽ��  15 - 18
			change_fault = E003_BUS_CURRENT_BIAS;
		
		else if(type == MOTOR_FAULT_DRV)		//-----------��·  03
			change_fault = E004_ABNORMAL_SHORT_CIRCUIT;
		
		else if((type >= MOTOR_FAULT_OUTPUT_PHASE_A_LOSS_POWER_ON) && (type <= MOTOR_FAULT_OUTPUT_PHASE_2_AND3_LOSS_RUNNING ) )//----------- ȱ�� 34 - 37
			change_fault = E005_LACK_PHASE;
		
		else if(type == MOTOR_FAULT_OUTPUT_LOCKROTOR)		//-----------��ת  38
			change_fault = E006_LOCK_ROTOR;
		
		else if(type == MOTOR_FAULT_OVER_TEMP_FET)			//----------- MOS ���� 05
			change_fault = E101_TEMPERATURE_MOS;

		//else if((type >= MOTOR_FAULT_OUTPUT_PHASE_A_SENSOR) && (type <= MOTOR_FAULT_OUTPUT_PHASE_C_SENSOR ) )//----------- ȱ�� ������
			//change_fault = E205_VOLTAGE_AMBIENT;
		
		else if(type == MOTOR_FAULT_MOSFET_NTC_ERR)			//----------- MOS ���������� 39
			change_fault = E201_TEMPERATURE_HARDWARE;
		
		//----------- ���� ����
		else
			change_fault = E202_MOTOR_DRIVER;
	}
//����������������������������������������������������������������������������������������������������
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
//����������������������������������������������������������������������������������������������������
	//ĸ�ߵ�ѹ�쳣
	if((type & MOTOR_FAULT_BUS_VOLTAGE_UNDER)||(type & MOTOR_FAULT_BUS_CURRENT_OVER))
		change_fault |= E001_BUS_VOLTAGE_ABNORMAL;
	
	//Ӳ������
	if((type & MOTOR_FAULT_OUT_STEP_FAULT)||(type & MOTOR_FAULT_STARTUP_FAILED))
		change_fault |= E202_MOTOR_DRIVER;
	
	//ͨ�Ź���
	if((type & MOTOR_FAULT_TIME_OUT)||(type & MOTOR_FAULT_CRC_ERROR))
		change_fault |= E203_MOTOR_LOSS;
		
	//ƫ���쳣
	if(type & MOTOR_FAULT_SAMPLE_ERROR)
		change_fault |= E003_BUS_CURRENT_BIAS;
	
	//Ӳ������  --��·
	if(type & MOTOR_FAULT_HARDWARE_OVERCURRENT)
		change_fault |= E004_ABNORMAL_SHORT_CIRCUIT;
	
	//�������
	if(type & MOTOR_FAULT_OUTPUT_OVERCURRENT)
		change_fault |= E002_BUS_CURRENT_ABNORMAL;
	
	//ȱ��
	if(type & MOTOR_FAULT_LACK_PHASE)
		change_fault |= E005_LACK_PHASE;
//����������������������������������������������������������������������������������������������������
#endif
	
	return change_fault;
}

/*-------------------- �շ����� ----------------------------------------------*/
// crc16-XMODEM
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

// У��� ȡ��
uint8_t CRC8_ADD(uint8_t *ptr, uint16_t len)
{
	uint8_t crc = 0x00;
    
    while(len--) 
    {
        crc = (crc + *ptr++);
    }
#ifdef SYSTEM_DEBUG_MODE
    return(crc);
#else
		return(~crc);
#endif
}

//-------------------- ���״̬���� ----------------------------
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
//����������������������������������������������������������������������������������������������������
void Motor_State_Analysis(void)
{
	static uint8_t Motor_State_Old[MOTOR_PROTOCOL_ADDR_MAX]={0};//���״̬
	uint16_t result_fault=0;
	short int Temperature=0;
	Motor_Rx_Timer_cnt = 0;
	uint16_t ntc_tmp[3]={0};
	
	//������ ͨѶ���� �ָ�
	ReSet_Motor_Fault_State(E203_MOTOR_LOSS);

	//
	// �˲����mosfet�¶�
	Temperature = Motor_State_Storage[MOTOR_ADDR_MOSFET_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_MOSFET_TEMP_OFFSET+1];
	memcpy(p_Mos_Temperature, &Temperature, 2);
	
	// �˲���ĵ���¶� ���� ����汾
	
	Driver_Software_Version_Read = Motor_State_Storage[MOTOR_ADDR_MOTOR_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_MOTOR_TEMP_OFFSET+1];

	Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_DRIVER_SOFTWARE_VERSION_HIGH, Driver_Software_Version_Read/100);
	Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_DRIVER_SOFTWARE_VERSION_LOW, Driver_Software_Version_Read%100);
	// ���ƽ������
	double mosfet_current = Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET]<<24 |Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET+1]<<16 |Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET+2]<<8 | Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET+3];
	*p_Motor_Current = (uint16_t)(mosfet_current/1.4);
	// ��ǰ����ת��erpm
	*p_Motor_Reality_Speed = Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET]<<24 |Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET+1]<<16 |Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET+2]<<8 | Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET+3];
	// ĸ�ߵ�ѹ
	*p_Motor_Bus_Voltage = Motor_State_Storage[MOTOR_ADDR_BUS_VOLTAGE_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_BUS_VOLTAGE_OFFSET+1];
	// ��ȡ�������
	*p_Motor_Fault_Static = Motor_State_Storage[MOTOR_ADDR_MOTOR_FAULT_OFFSET];
	
	// 10KNTC�¶�1 2 3
	ntc_tmp[0] = Motor_State_Storage[MOTOR_ADDR_NTC1_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_NTC1_TEMP_OFFSET+1];
	ntc_tmp[1] = Motor_State_Storage[MOTOR_ADDR_NTC2_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_NTC2_TEMP_OFFSET+1];
	ntc_tmp[2] = Motor_State_Storage[MOTOR_ADDR_NTC3_TEMP_OFFSET]<<8 | Motor_State_Storage[MOTOR_ADDR_NTC3_TEMP_OFFSET+1];
	
	Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_01, ntc_tmp[0]);
	Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_02, ntc_tmp[1]);
	Set_DataAddr_Value(MB_FUNC_READ_INPUT_REGISTER , MB_MOSFET_TEMPERATURE_03, ntc_tmp[2]);
	
	//----- ���ڴ�ӡ   ------------------------------------------------------------------------------
	if(memcmp(Motor_State_Old, Motor_State_Storage, MOTOR_PROTOCOL_ADDR_MAX) != 0)
	{
		DEBUG_PRINT("\n\n\
		mosfet�¶�:\t%d.%d (��C)\n\
		�����汾:\t%d.%d \n\
		�������:\t%d.%d (A)\n\
		��ǰת��:\t\t\t\t%d (rpm)\n\
		ĸ�ߵ�ѹ:\t%d.%d (V)\n\
		�������:\t\t%X \n\
		10KNTC�¶�1 2 3:\t%d.%d (��C)\t%d.%d (��C)\t%d.%d (��C)\n\n",
				Temperature/10,Temperature%10,Driver_Software_Version_Read/10,Driver_Software_Version_Read%10,*p_Motor_Current/100,*p_Motor_Current%100,
				*p_Motor_Reality_Speed,*p_Motor_Bus_Voltage/10,*p_Motor_Bus_Voltage%10,*p_Motor_Fault_Static,
				ntc_tmp[0]/10,ntc_tmp[0]%10,ntc_tmp[1]/10,ntc_tmp[1]%10,ntc_tmp[2]/10,ntc_tmp[2]%10);
		
		if((ntc_tmp[0]>Temperature)||(ntc_tmp[1]>Temperature)||(ntc_tmp[2]>Temperature))
			DEBUG_PRINT("\n mosfet�¶ȴ��� \t%d.%d (��C)\n",Temperature/10,Temperature%10);
		
		memcpy(Motor_State_Old, Motor_State_Storage, MOTOR_PROTOCOL_ADDR_MAX);
	}
	//===============================================================================================
	
	CLEAN_MOTOR_FAULT(Motor_Fault_State);
	if(*p_Motor_Fault_Static > 0)
	{
		if(Motor_Fault_Timer_cnt < MOTOR_CHECK_FAULT_TIMER)
			Motor_Fault_Timer_cnt ++;
		else
		{
			result_fault = Change_Faule_To_Upper(*p_Motor_Fault_Static);
			Set_Motor_Fault_State(result_fault);
		}
	}
	else
	{
		Motor_Fault_Timer_cnt = 0;
	}
	
	// ���� 		��Ƶ	mos
	Check_Down_Conversion_MOS_Temperature(Temperature/10);
	// ������� 	��Ƶ
	Check_Down_Conversion_Motor_Current(*p_Motor_Current/100);
	// ������� 		��Ƶ
	//Check_Down_Conversion_Motor_Power(*p_Motor_Reality_Power);
	
	// ����״̬����   ���ת��
	Drive_Status_Inspection_Motor_Speed();
	// ����״̬����   �������
	Drive_Status_Inspection_Motor_Current();
	
}
//����������������������������������������������������������������������������������������������������
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
//����������������������������������������������������������������������������������������������������
void Motor_State_Analysis(void)
{
	//static uint32_t Rx_cnt= 0;
	uint16_t result_fault=0;
	uint16_t Temperature=0;
	Motor_Rx_Timer_cnt = 0;
	static uint16_t ntc_tmp[3]={0};

	
	//������ ͨѶ���� �ָ�
	ReSet_Motor_Fault_State(E203_MOTOR_LOSS);
	
	// ��ǰ ת��
	*p_Motor_Reality_Speed = Motor_State_Storage[MOTOR_ADDR_MOTOR_SPEED_OFFSET]*10;
	// ����汾
	Driver_Software_Version_Read = Motor_State_Storage[MOTOR_ADDR_MOTOR_VERSION_OFFSET];
	// ĸ�ߵ�ѹ
	*p_Motor_Bus_Voltage = Motor_State_Storage[MOTOR_ADDR_BUS_VOLTAGE_OFFSET] *10;
	// ĸ�ߵ���
	*p_Motor_Bus_Current = Motor_State_Storage[MOTOR_ADDR_BUS_CURRENT_OFFSET] *100;
	// �������
	*p_Motor_Current = Motor_State_Storage[MOTOR_ADDR_MOTOR_CURRENT_OFFSET] *100;
	// mosfet�¶�
	if((Motor_State_Storage[MOTOR_ADDR_MOSFET_TEMP_NUM_OFFSET] >= 1) && (Motor_State_Storage[MOTOR_ADDR_MOSFET_TEMP_NUM_OFFSET] <= 3))
	{
		ntc_tmp[Motor_State_Storage[MOTOR_ADDR_MOSFET_TEMP_NUM_OFFSET]-1] = Motor_State_Storage[MOTOR_ADDR_MOSFET_TEMP_OFFSET] *10;
	}
	Temperature = (ntc_tmp[0]>ntc_tmp[1])?ntc_tmp[0]:ntc_tmp[1];
	Temperature = (Temperature>ntc_tmp[2])?Temperature:ntc_tmp[2];
	*p_Mos_Temperature = Temperature;
	// ��ǰ����
	*p_Motor_Reality_Power =  Motor_State_Storage[MOTOR_ADDR_MOTOR_POWER_OFFSET+1]<<8 | Motor_State_Storage[MOTOR_ADDR_MOTOR_POWER_OFFSET];
	// �������
 	*p_Motor_Fault_Static = Motor_State_Storage[MOTOR_ADDR_MOTOR_FAULT_OFFSET+1]<<8 | Motor_State_Storage[MOTOR_ADDR_MOTOR_FAULT_OFFSET];
	
	//----- ���ڴ�ӡ   ------------------------------------------------------------------------------
	DEBUG_PRINT("\n\n\
	��ǰת��:\t\t\t%d (rpm)\n\
	�����汾:\t%X\n\
	ĸ�ߵ�ѹ:\t%d.%d (V)\n\
	ĸ�ߵ���:\t%d.%d (A)\n\
	�������:\t%d.%d (A)\n\
	mosfet�¶�:\t\t%d.%d (��C)\n\
	10KNTC�¶�1 2 3:\t%d.%d (��C)\t%d.%d (��C)\t%d.%d (��C)\n\
	��ǰ����:\t%d (W)\n\
	�������:\t\t0x%X\n",

	*p_Motor_Reality_Speed,Driver_Software_Version_Read,
	*p_Motor_Bus_Voltage/10,*p_Motor_Bus_Voltage%10,
	*p_Motor_Bus_Current/100,*p_Motor_Bus_Current%100,
	*p_Motor_Current/100,*p_Motor_Current%100,
	Temperature/10,Temperature%10,
	ntc_tmp[0]/10,ntc_tmp[0]%10,ntc_tmp[1]/10,ntc_tmp[1]%10,ntc_tmp[2]/10,ntc_tmp[2]%10,
	*p_Motor_Reality_Power,*p_Motor_Fault_Static);
	//===============================================================================================

	CLEAN_MOTOR_FAULT(Motor_Fault_State);
	if(*p_Motor_Fault_Static > 0)
	{
		if(Motor_Fault_Timer_cnt < MOTOR_CHECK_FAULT_TIMER)
			Motor_Fault_Timer_cnt ++;
		else
		{
			result_fault = Change_Faule_To_Upper(*p_Motor_Fault_Static);
			Set_Motor_Fault_State(result_fault);
		}
	}
	else
	{
		Motor_Fault_Timer_cnt = 0;
	}
	
	// ���� 		��Ƶ	mos
	Check_Down_Conversion_MOS_Temperature(Temperature/10);
	// ������� 	��Ƶ
	Check_Down_Conversion_Motor_Current(*p_Motor_Current/100);
	// ������� 		��Ƶ
	Check_Down_Conversion_Motor_Power(p_Motor_Reality_Power);
}
//����������������������������������������������������������������������������������������������������
#endif

//================================================== �ڲ����ýӿ�

//-------------------- ����״̬����   ���ת�� ----------------------------
void Drive_Status_Inspection_Motor_Speed(void)
{
//�жϵ��ת��
#ifdef MOTOR_SPEED_ERROR_TIME
	static uint16_t Check_Motor_Speed_Cnt=0;		// ��ʱ��
	static uint8_t Number_Of_Fault_Alarms = 0;	//	���ϼ�����
	
	if(Motor_Speed_Is_Reach())	//��ת�������ȶ������������ж�
	{
		if(Check_Motor_Speed())
		{
			if(Check_Motor_Speed_Cnt > 0)
			{
				DEBUG_PRINT("\n[���ת����ȷ]\tĿ��ת��\t%d(rpm)\tʵ��ת��\t%d(rpm)\n",Motor_Speed_To_Rpm(Motor_Speed_Now),*p_Motor_Reality_Speed);
			}
			if(Number_Of_Fault_Alarms >= 3)//�Ѿ�������
			{
				//���ת�ٲ�׼ ���� 202 ��������
				ReSet_Motor_Fault_State(E202_MOTOR_DRIVER);
				DEBUG_PRINT("\n[������ϻָ�]\t  ʵ��ת��\t%d(rpm)\n",*p_Motor_Reality_Speed);
			}
				
			Check_Motor_Speed_Cnt = 0;
			Number_Of_Fault_Alarms = 0;
			
		}
		else
		{
			if(Check_Motor_Speed_Cnt <= MOTOR_SPEED_ERROR_TIME)
			{
				if(Check_Motor_Speed_Cnt == 0)
				{
					DEBUG_PRINT("\n[���ת�ٴ���]\tĿ��ת��\t%d(rpm)\tʵ��ת��\t%d(rpm)\t������\t%d\n",Motor_Speed_To_Rpm(Motor_Speed_Now),*p_Motor_Reality_Speed,Check_Motor_Speed_Cnt);
				}
				Check_Motor_Speed_Cnt++;
			}
			else
			{
				if(Number_Of_Fault_Alarms >= 3)
				{
					//���ת�ٲ�׼ ���� 202 ��������
					Set_Motor_Fault_State(E202_MOTOR_DRIVER);
					if(Number_Of_Fault_Alarms == 3)
					{
						Number_Of_Fault_Alarms ++ ;
						DEBUG_PRINT("\n[ERROR]\t���ת�ٲ�׼\tĿ��ת��\t%d(rpm)\tʵ��ת��\t%d(rpm)\n",Motor_Speed_To_Rpm(Motor_Speed_Now),*p_Motor_Reality_Speed);
					}
				}
				else
				{
					//�ѵ�ǰ�ٶ�ͬ�����������ٶ�
					Motor_Speed_Now = Motor_Rpm_To_Speed(*p_Motor_Reality_Speed);
					DEBUG_PRINT("\n[�ѵ�ǰ�ٶ�ͬ�����������ٶ�]\tת��\t%d(rpm)\t�ٷֱ�\t%d()\n",*p_Motor_Reality_Speed,Motor_Speed_Now);
				
					//Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);	//��Ȧ������˸
					Number_Of_Fault_Alarms ++ ;
					Check_Motor_Speed_Cnt = 0;
				}
			}
		}
	}
//	else
//	{
//		Check_Motor_Speed_Cnt = 0;
//	}
#endif
}

//-------------------- ����״̬����   ������� ----------------------------
void Drive_Status_Inspection_Motor_Current(void)
{
//�жϵ������
#ifdef MOTOR_CANNOT_START_TIME
		static uint16_t Check_Motor_Current_Cnt=0;	// ��ʱ��
	
	
		if(Check_Motor_Current())
		{
			Check_Motor_Current_Cnt = 0;
		}
		else
		{
			if(Check_Motor_Current_Cnt <= MOTOR_CANNOT_START_TIME)
			{
				Check_Motor_Current_Cnt ++;
				DEBUG_PRINT("\n[�жϵ������]\tĿ��ת��\t%d(rpm)\tʵ��ת��\t%d(rpm)\t������\t%d\n",Motor_Speed_To_Rpm(Motor_Speed_Now),*p_Motor_Reality_Speed,Check_Motor_Speed_Cnt);
			}
			else
			{
				//������� ���� 202 ��������
				Set_Motor_Fault_State(E202_MOTOR_DRIVER);
			}
		}
#endif

}

//-------------------- ���½�Ƶ  mos ----------------------------
// Temperature ��Ϊ����
void Check_Down_Conversion_MOS_Temperature(short int Temperature)
{
	if(Temperature >= (MOS_TEMP_ALARM_VALUE))				//-------------  ͣ��
	{
		if(Motor_TEMP_Timer_cnt < (MOTOR_DOWN_CONVERSION_TIMER*2))
			Motor_TEMP_Timer_cnt ++;
		else
		{
			Set_Motor_Fault_State(E101_TEMPERATURE_MOS);
		}
	}
	else if(Temperature >= (MOS_TEMP_REDUCE_SPEED))				//-------------  ����
	{
		if(Motor_TEMP_Timer_cnt < MOTOR_DOWN_CONVERSION_TIMER)
			Motor_TEMP_Timer_cnt ++;
		else
		{
			//Ԥ�� ����
			if(Get_Temp_Slow_Down_State() == MOTOR_DOWN_CONVERSION_NO)
				Set_Temp_Slow_Down_State(MOTOR_DOWN_CONVERSION_MOS_TEMPER);
		}
	}
	else if(Temperature <= (MOS_TEMP_RESTORE_SPEED))				//-------------  �ָ�
	{
		if(Motor_TEMP_Timer_cnt > 0)
			Motor_TEMP_Timer_cnt = 0;
		else
		{
			if(Get_Temp_Slow_Down_State() == MOTOR_DOWN_CONVERSION_MOS_TEMPER)
				Set_Temp_Slow_Down_State(MOTOR_DOWN_CONVERSION_NO);
		}
	}
}

//-------------------- ���ʽ�Ƶ   ----------------------------
void Check_Down_Conversion_Motor_Power(uint16_t power)
{
	if(power >= (MOTOR_POWER_ALARM_VALUE))				//-------------  ͣ��
	{
		if(Motor_Power_Timer_cnt < (MOTOR_DOWN_CONVERSION_TIMER*2))
			Motor_Power_Timer_cnt ++;
		else
		{
			Set_Motor_Fault_State(E002_BUS_CURRENT_ABNORMAL);
		}
	}
	else if(power >= (MOTOR_POWER_REDUCE_SPEED))				//-------------  ����
	{
		if(Motor_Power_Timer_cnt < MOTOR_DOWN_CONVERSION_TIMER)
			Motor_Power_Timer_cnt ++;
		else
		{
			//Ԥ�� ����
			if(Get_Temp_Slow_Down_State() == MOTOR_DOWN_CONVERSION_NO)
				Set_Temp_Slow_Down_State(MOTOR_DOWN_CONVERSION_OUT_POWER);
		}
	}
	else if(power <= (MOTOR_POWER_RESTORE_SPEED))				//-------------  �ָ�
	{
		if(Motor_Power_Timer_cnt > 0)
			Motor_Power_Timer_cnt = 0;
		else
		{
			if(Get_Temp_Slow_Down_State() == MOTOR_DOWN_CONVERSION_OUT_POWER)
				Set_Temp_Slow_Down_State(MOTOR_DOWN_CONVERSION_NO);
		}
	}
}

//-------------------- ������Ƶ   ----------------------------
void Check_Down_Conversion_Motor_Current(uint32_t Current)
{
	if(Current >= (MOTOR_CURRENT_ALARM_VALUE))				//-------------  ͣ��
	{
		if(Motor_Current_Timer_cnt < (MOTOR_DOWN_CONVERSION_TIMER*2))
			Motor_Current_Timer_cnt ++;
		else
		{
			Set_Motor_Fault_State(E002_BUS_CURRENT_ABNORMAL);
		}
	}
	else if(Current >= (MOTOR_CURRENT_REDUCE_SPEED))				//-------------  ����
	{
		if(Motor_Current_Timer_cnt < MOTOR_DOWN_CONVERSION_TIMER)
			Motor_Current_Timer_cnt ++;
		else
		{
			//Ԥ�� ����
			if(Get_Temp_Slow_Down_State() == MOTOR_DOWN_CONVERSION_NO)
				Set_Temp_Slow_Down_State(MOTOR_DOWN_CONVERSION_OUT_CURRENT);
		}
	}
	else if(Current <= (MOTOR_CURRENT_RESTORE_SPEED))				//-------------  �ָ�
	{
		if(Motor_Current_Timer_cnt > 0)
			Motor_Current_Timer_cnt = 0;
		else
		{
			if(Get_Temp_Slow_Down_State() == MOTOR_DOWN_CONVERSION_OUT_CURRENT)
				Set_Temp_Slow_Down_State(MOTOR_DOWN_CONVERSION_NO);
		}
	}
}

//-------------------- ��Ƶ ״̬�ָ�   ----------------------------
void Down_Conversion_State_Clea(void)
{
	// ������ ����
	Motor_Fault_Timer_cnt = 0;			// ���ϼ�����
	Motor_TEMP_Timer_cnt = 0;				// ���� ������
	Motor_Current_Timer_cnt = 0;		// ���� ������
	Motor_Power_Timer_cnt = 0;			// ���� ������
	
	Set_Temp_Slow_Down_State(MOTOR_DOWN_CONVERSION_NO);
}

//-------------------- ��ȡ�������״̬ ----------------------------
uint16_t Get_Motor_Fault_State(void)
{
	
	return Motor_Fault_State;
}

//-------------------- ���õ������״̬ ----------------------------
void Set_Motor_Fault_State(uint16_t fault_bit)
{
	 Motor_Fault_State |= fault_bit;
}

//-------------------- ����������״̬ ----------------------------
void ReSet_Motor_Fault_State(uint16_t fault_bit)
{
	 Motor_Fault_State &= ~fault_bit;
}

//-------------------- ���� ----------------------------
void Motor_UART_Send(uint8_t* p_buff, uint8_t len)
{
#ifdef MOTOR_MODULE_HUART
	HAL_UART_Transmit(p_huart_motor, p_buff, len, 0xFFFF);
	
#endif
}
//-------------------- ���� ----------------------------
void Motor_RxData(uint8_t len)
{
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
//����������������������������������������������������������������������������������������������������
	uint16_t crc_value=0;
	
	// ��� ����
	if((Motor_DMABuff[1]+5) != len)
	{
		DEBUG_PRINT("[ERROR]\t���ճ��ȴ���:\t�յ�:\t%d   ʵ��:\t%d\n",Motor_DMABuff[1]+5, len);
		return;
	}
	
	//���crc
	crc_value = CRC16_XMODEM_T(&Motor_DMABuff[2], Motor_DMABuff[1] );
	//crc_read = ((Motor_DMABuff[len-2]<<8) & Motor_DMABuff[len-1]);
	if(crc_value != ((Motor_DMABuff[len-3]<<8) | Motor_DMABuff[len-2]))
	{
		DEBUG_PRINT("[ERROR]\tcrcУ�����:\t����õ�:\t%d   �յ���:\t%d\n",crc_value, ((Motor_DMABuff[len-3]<<8) & Motor_DMABuff[len-2]));
		return;
	}
	
	memcpy(Motor_State_Storage, &Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET], MOTOR_PROTOCOL_ADDR_MAX);
	Motor_State_Analysis();
//����������������������������������������������������������������������������������������������������
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
//����������������������������������������������������������������������������������������������������
	uint8_t crc_value=0;
	
	// ��� ����
	if(len != 13)
	{
		DEBUG_PRINT("[ERROR]\t���ճ��ȴ���:\t�յ�:\t%d  \n", len);
		return;
	}
	
	//���crc
	crc_value = CRC8_ADD(&Motor_DMABuff[0], 12);
	//crc_read = ((Motor_DMABuff[len-2]<<8) & Motor_DMABuff[len-1]);
	if(crc_value != Motor_DMABuff[12])
	{
		DEBUG_PRINT("[ERROR]\tcrcУ�����:\t����õ�:\t%X   �յ���:\t%X\n",crc_value, (Motor_DMABuff[12]));
		return;
	}
	
	memcpy(Motor_State_Storage, &Motor_DMABuff[MOTOR_PROTOCOL_HEADER_OFFSET], MOTOR_PROTOCOL_ADDR_MAX);
	//���ձ�־
	Rx_State = 1;

//����������������������������������������������������������������������������������������������������
#endif
}



/*-------------------- ���ٷ������� ----------------------------------------------*/
//------------------- ���õ��ת�� ----------------------------
void Motor_Speed_Send(uint32_t speed_rpm)
{
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
//����������������������������������������������������������������������������������������������������
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
//����������������������������������������������������������������������������������������������������
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
//����������������������������������������������������������������������������������������������������
	uint8_t buffer[13]={0x50,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xB1,0x00};
	
	//buffer[1] = Motor_Speed_To_Frequency(speed_rpm);
	buffer[1] = speed_rpm & 0xFF;
	buffer[2] = (speed_rpm>>8) & 0xFF;
	buffer[12] =  CRC8_ADD(buffer, 12);
	
	Motor_UART_Send(buffer, 13);

//����������������������������������������������������������������������������������������������������
#endif
}


//------------------- ���� ----------------------------
void Motor_Heartbeat_Send(void)
{
	uint8_t buffer[10]={0x02,0x01,0x1E,0xF3,0xFF,0x03};

	Motor_UART_Send(buffer, 6);
}


//-------------------- ���Ĵ��� ( ȫ�� )----------------------------
void Motor_Read_Register(void)
{
	uint8_t buffer[10]={0x02,0x01,0x04,0x40,0x84,0x03};

	Motor_UART_Send(buffer, 6);
}



//-------------------- ��������� ----------------------------
uint8_t Check_Motor_Current(void)
{
#ifdef MOTOR_CURRENT_MIX
	if(Motor_Speed_Now >= MOTOR_PERCENT_SPEED_MIX)
	{
		if(*p_Motor_Current < MOTOR_CURRENT_MIX)
			return 0;
	}
#endif
	return 1;
}

//-------------------- �����ת�� ----------------------------
uint8_t Check_Motor_Speed(void)
{
#ifdef MOTOR_SPEED_VIBRATION_RANGE
	uint32_t rpm = Motor_Speed_To_Rpm(Motor_Speed_Now);
	
	if(rpm  > *p_Motor_Reality_Speed)
	{
		if((rpm - *p_Motor_Reality_Speed) > MOTOR_SPEED_VIBRATION_RANGE)
			return 0;
	}
	else
	{
		if((*p_Motor_Reality_Speed - rpm) > MOTOR_SPEED_VIBRATION_RANGE)
			return 0;
	}
#endif
	return 1;
}
