/**
******************************************************************************
* @file				data.c
* @brief			���ݹ���ģ��
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "data.h"
#include "modbus.h"
#include <stdlib.h>
#include <string.h>
#include "dev.h"
#include "down_conversion.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

Operating_Parameters OP_ShowNow;

Operating_Parameters* p_OP_ShowLater;

// ѵ��ģʽ ��ǰ״̬
uint8_t PMode_Now = 0;

uint8_t Period_Now = 0;

// ��ģʽ ���� ��ʼֵ
Operating_Parameters OP_Init_Free = { 40 , 0};

Operating_Parameters OP_Init_Timing = { 40 , 1800};

Operating_Parameters OP_Init_PMode[TRAINING_MODE_NUMBER_MAX][TRAINING_MODE_PERIOD_MAX] = {
{{20,120},{30,300}, {20,360},{35,540},{20,600},{30,780}, {20,900} },
{{45,180},{55,360}, {45,480},{70,720},{45,780},{55,1020},{45,1200}},
{{70,300},{80,540}, {70,600},{85,840},{70,900},{80,1200},{70,1500}},
{{45,420},{65,1440},{45,1800}},
{{30,00}},
};

//--------------------------- ϵͳ����
uint16_t* p_System_State_Machine;			// ״̬��
uint16_t* p_PMode_Now;								// ��ǰģʽ
uint16_t* p_OP_ShowNow_Speed;					// ��ǰ�ٶ�
uint16_t* p_OP_ShowNow_Time;					// ��ǰʱ��

//--------------------------- ��ʱ ���ڹ��ϵȽ����¼����ֵ
uint16_t* p_System_State_Machine_Memory;			// ״̬��
uint16_t* p_PMode_Now_Memory;									// ��ǰģʽ
uint16_t* p_OP_ShowNow_Speed_Memory;					// ��ǰ�ٶ�
uint16_t* p_OP_ShowNow_Time_Memory;						// ��ǰʱ��

System_Ctrl_Mode_Type_enum Ctrl_Mode_Type = CTRL_FROM_KEY;				// ���Ʒ�ʽ

// ��ģʽ ����
Operating_Parameters* p_OP_Free_Mode;

Operating_Parameters* p_OP_Timing_Mode;

Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX] = OP_Init_PMode;

//==========================================================
//--------------------------- �������ȡ��Ϣ
//==========================================================

uint16_t Driver_Software_Version_Read;		// ����������汾	 ��������ԭʼֵ

uint16_t* p_Motor_Fault_Static;						// ����״̬		������

uint32_t* p_Motor_Reality_Speed;					// ��� ʵ�� ת��
uint32_t* p_Motor_Reality_Power;					// ��� ʵ�� ����

int16_t* p_Mos_Temperature;							// mos �¶�
uint32_t* p_Motor_Current;								// ��� ����		���
uint16_t* p_Motor_Bus_Voltage;						// ĸ�� ��ѹ		����
uint16_t* p_Motor_Bus_Current;						// ĸ�� ����  	����

//==========================================================
//--------------------------- ������Ϣ
//==========================================================
uint16_t* p_System_Fault_Static;					// ����״̬		����
int16_t* p_Box_Temperature;							// ���� �¶�
uint32_t* p_Send_Reality_Speed;						// �·� ʵ�� ת��


uint16_t* p_Support_Control_Methods;	//���ο��Ʒ�ʽ
uint16_t* p_Motor_Pole_Number;				//�������
uint16_t* p_Breath_Light_Max;					//��Ȧ����  
	
uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX]={0};//���״̬

//================= ����ģʽ ȫ�� ���� ================================
// ----------------------------------------------------------------------------------------------
uint16_t* p_Surf_Mode_Info_Acceleration;  		//	����ģʽ -- ���ٶ�
uint16_t* p_Surf_Mode_Info_Prepare_Time;  		//	����ģʽ -- ׼��ʱ��
uint16_t* p_Surf_Mode_Info_Low_Speed;  				//	����ģʽ -- ���ٵ� -- �ٶ�
uint16_t* p_Surf_Mode_Info_Low_Time;					//	����ģʽ -- ���ٵ� -- ʱ��
uint16_t* p_Surf_Mode_Info_High_Speed;  			//	����ģʽ -- ���ٵ� -- �ٶ�
uint16_t* p_Surf_Mode_Info_High_Time;  				//	����ģʽ -- ���ٵ� -- ʱ��
// ----------------------------------------------------------------------------------------------

uint16_t *p_WIFI_Rssi;
uint16_t *p_BLE_Rssi;

uint16_t* p_Analog_key_Value;					// ���ⰴ��

uint8_t System_PowerUp_Finish = 0;

uint16_t MB_Buffer_Write_Timer = 0;

//================= ����ʹ��  ʱ�� ================================

uint32_t* p_System_Runing_Second_Cnt;			// ϵͳʱ��
uint32_t* p_No_Operation_Second_Cnt;			// ���˲���ʱ��
uint32_t* p_System_Startup_Second_Cnt;		// ���� ʱ��


//==========================================================
//--------------------------- ���ͳ�� (APPҪ)
//==========================================================
uint16_t* p_Finish_Statistics_Time;					//	���ͳ�� --> ʱ��
uint16_t* p_Finish_Statistics_Speed;				//	���ͳ�� --> ǿ��
uint32_t* p_Finish_Statistics_Distance;			//	���ͳ�� --> ��Ӿ����
uint16_t* p_Preparation_Time_BIT;						//	׼��ʱ�� Bit: ��ʱģʽ P1-P6


uint16_t* p_Thread_Activity_Sign;					//	�߳� � ��־λ

uint32_t* p_Wifi_Timing_Value;						// wifi ϵͳʱ��	
uint32_t* p_Wifi_Timing_Value_Old;				// ��һ��ʱ��

uint16_t* p_Check_Timing_Add_More;				// ������,��ʱ

uint16_t* p_Check_Timing_Minus_More;			// �߿���, ��ʱ

uint16_t* p_Check_Timing_Error_Cnt;				// wifiģ�� Уʱ���������

uint16_t* p_Wifi_DP_Upload_Level;					// wifiģ�� dp���ϱ��ȼ�
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- Ӳ�� & ���� ----------------------------

// ��ʼ�� ����ģʽ ����
void Surf_Mode_Info_Data_Init(void)
{
	//================= ����ģʽ ȫ�� ���� ================================
	// ----------------------------------------------------------------------------------------------
	*p_Surf_Mode_Info_Acceleration	=	2;  			//	����ģʽ -- ���ٶ�
	*p_Surf_Mode_Info_Prepare_Time	=	10;  			//	����ģʽ -- ׼��ʱ��
	*p_Surf_Mode_Info_Low_Speed			=	30;  			//	����ģʽ -- ���ٵ� -- �ٶ�
	*p_Surf_Mode_Info_Low_Time			=	15;				//	����ģʽ -- ���ٵ� -- ʱ��
	*p_Surf_Mode_Info_High_Speed		=	100;  		//	����ģʽ -- ���ٵ� -- �ٶ�
	*p_Surf_Mode_Info_High_Time			=	15;  			//	����ģʽ -- ���ٵ� -- ʱ��
	// ----------------------------------------------------------------------------------------------
}

// ��ʼ�� ����ִ��
uint8_t Check_Data_Init(void)
{
	uint8_t x=0,y=0;
	uint8_t result = 0;
	
	//MB_HoldBuffer_Temp_Clean();
	
	if(Is_Speed_Legal(p_OP_Free_Mode->speed) == 0)
	{
		*p_OP_Free_Mode = OP_Init_Free;
		result = 1;
	}
	
	if((Is_Speed_Legal(p_OP_Timing_Mode->speed) == 0) || (Is_Time_Legal(p_OP_Timing_Mode->time) == 0))
	{
		*p_OP_Timing_Mode = OP_Init_Timing;
		result = 1;
	}

	if( ( *p_Motor_Pole_Number > MOTOR_RPM_MAX_OF_POLES) || ( *p_Motor_Pole_Number < MOTOR_RPM_MIX_OF_POLES))
	{
		*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
		result = 1;
	}
	
	if(result == 1)
	{
		*p_Local_Address 					= MODBUS_LOCAL_ADDRESS;
		//*p_Baud_Rate 							= MODBUS_BAUDRATE_DEFAULT;
		*p_Motor_Pole_Number 			= MOTOR_RPM_NUMBER_OF_POLES;				// �������		5
		*p_Support_Control_Methods 	= 0;			//	���Ʒ�ʽ	
	}
	
	//================= ����ģʽ ȫ�� ���� ================================
	Surf_Mode_Info_Data_Init();
	
	for(x=0; x<TRAINING_MODE_NUMBER_MAX; x++)
	{
		for(y=0; y<TRAINING_MODE_PERIOD_MAX; y++)
		{
			if((Is_Speed_Legal(p_OP_PMode[x][y].speed) == 0) || (Is_Time_Legal(p_OP_PMode[x][y].time) == 0))
			{
				memcpy(p_OP_PMode, OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
				result = 1;
				break;
			}
			if(y>0)
			{
				if(p_OP_PMode[x][y].time <= p_OP_PMode[x][y-1].time)
				{
					memcpy(p_OP_PMode, &OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
					result = 1;
					break;
				}
			}
		}
	}
	
	return result;
}


// ��ʼ��
void App_Data_Init(void)
{
	Read_OPMode();
	// ��ȡӳ��  flash�Ѷ�
	MB_Get_Mapping_Register();
	//
	MB_InputBuffer_Init();
	// ����ģʽ ����
	if(Check_Data_Init())
	{
		//App_Data_ReInit();
		Write_MbBuffer_Now();
	}
	// ��Ļ��ʼ��
	TM1621_LCD_Init();
	
	TM1621_Buzzer_Init();
	
	//test ���Թ���Ļ����pwm �ǵ�ɾ  wuqingguang 2024-09-09
	TM1621_light_Off();
}


// �ָ� ��ʼ��
void App_Data_ReInit(void)
{
	memset(p_Local_Address,0,REG_HOLDING_NREGS*2);
	
	*p_Local_Address 					= MODBUS_LOCAL_ADDRESS;
	*p_Baud_Rate 							= MODBUS_BAUDRATE_DEFAULT;
	*p_Motor_Pole_Number 			= MOTOR_RPM_NUMBER_OF_POLES;				// �������		5
	//*p_Preparation_Time_BIT 	= 0;				// Ԥ��ʱ��
	
	// ѵ��ģʽ ��ǰ״̬
//	*p_System_State_Machine 	= 0;			// ״̬��
//	*p_PMode_Now 							= 0;			// ��ǰģʽ
//	*p_OP_ShowNow_Speed 			= 0;			// ��ǰ�ٶ�
//	*p_OP_ShowNow_Time 				= 0;			// ��ǰʱ��
	Set_Pmode_Period_Now(0);
	
	// ��ģʽ ����
	*p_OP_Free_Mode = OP_Init_Free;
	*p_OP_Timing_Mode = OP_Init_Timing;

	//================= ����ģʽ ȫ�� ���� ================================
	Surf_Mode_Info_Data_Init();
	//================= ��� ���� ================================
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER , MB_MOTOR_DRIVE_MODE, MOTOR_DRIVE_MODE_POLES);	// ����ģʽ
	Set_DataAddr_Value(MB_FUNC_READ_HOLDING_REGISTER , MB_MOTOR_MODEL_CODE, MOTOR_MODEL_CODE_POLES);	// ����ͺ�
	
	memcpy(&p_OP_PMode[0][0], &OP_Init_PMode[0][0], sizeof(OP_Init_PMode[0]));
	memcpy(&p_OP_PMode[1][0], &OP_Init_PMode[1][0], sizeof(OP_Init_PMode[1]));
	memcpy(&p_OP_PMode[2][0], &OP_Init_PMode[2][0], sizeof(OP_Init_PMode[2]));
	memcpy(&p_OP_PMode[3][0], &OP_Init_PMode[3][0], sizeof(OP_Init_PMode[3]));
	memcpy(&p_OP_PMode[4][0], &OP_Init_PMode[4][0], sizeof(OP_Init_PMode[4]));
	
	//�洢  ��һ�� ���� ������
	Write_MbBuffer_Now();
}

// �� flash
uint8_t Read_OPMode(void)
{
	MB_Flash_Buffer_Read();
	return 1;
}

void MB_Write_Timer_CallOut(void)
{
	if(MB_Buffer_Write_Timer > 0)
	{
		MB_Buffer_Write_Timer ++;
		if(MB_Buffer_Write_Timer > 20) // 20:5s   60000:25min
		{
			MB_Flash_Buffer_Write();
			MB_Buffer_Write_Timer = 0;
		}
	}
}


// �� flash
uint8_t Write_MbBuffer_Later(void)
{
	MB_Buffer_Write_Timer = 1;
	return 1;
}


// ������ flash
uint8_t Write_MbBuffer_Now(void)
{
	MB_Flash_Buffer_Write();
	MB_Buffer_Write_Timer = 0;

	return 1;
}

//�洢 �� �ٶ�
void Update_OP_Speed(void)
{
	if(System_is_Pause())	// ��ͣ
		return;
	
	if(System_Mode_Free())	// ����
	{
		if(*p_OP_ShowNow_Speed < MOTOR_PERCENT_SPEED_MIX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MIX;
		else if(*p_OP_ShowNow_Speed > MOTOR_PERCENT_SPEED_MAX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MAX;
			
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
		Write_MbBuffer_Later();//��flash
	}
	else if(System_Mode_Time())	// ��ʱ
	{
		if(*p_OP_ShowNow_Speed < MOTOR_PERCENT_SPEED_MIX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MIX;
		else if(*p_OP_ShowNow_Speed > MOTOR_PERCENT_SPEED_MAX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MAX;
		
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		Write_MbBuffer_Later();//��flash
	}
}

//�洢 �� ʱ��
void Update_OP_Time(void)
{
	if(System_Mode_Time())	// ��ʱ
	{
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
		Write_MbBuffer_Later();//��flash
	}
}

//�洢 �� �ٶ� & ʱ��
void Update_OP_All(void)
{
	if(System_Mode_Free())	// ����
	{
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
		Write_MbBuffer_Later();//��flash
	}
	else if(System_Mode_Time())	// ��ʱ
	{
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
		Write_MbBuffer_Later();//��flash
	}
}

extern void System_Power_Off(void);
extern void Clean_Timing_Timer_Cnt(void);
extern void Clean_Automatic_Shutdown_Timer(void);

void OP_Update_Mode(void)
{
	if(System_is_Power_Off())
		System_Power_Off();
	else if(System_is_Stop())
	{
		if(System_Mode_Train())
			*p_OP_ShowNow_Speed = p_OP_PMode[Get_System_State_Mode()-1][0].speed;
		else if(System_Mode_Time())
			*p_OP_ShowNow_Time = p_OP_Timing_Mode->time - *p_OP_ShowNow_Time;
				
		//Motor_Speed_Target_Set(0);
		Clean_Automatic_Shutdown_Timer();
		Clean_Timing_Timer_Cnt();
	}
	else if(System_is_Pause())
	{
		p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
		*p_OP_ShowNow_Speed = 0;
		Data_Set_Current_Speed(0);//ע��,��Ҫ����������״̬���������ٶ�,��"����"
	}
	else
	{
		// �ٶ�
		if(System_Mode_Free())
		{
			*p_OP_ShowNow_Speed = p_OP_Free_Mode->speed;
		}
		else if(System_Mode_Time())
		{
			*p_OP_ShowNow_Speed = p_OP_Timing_Mode->speed;
		}
		else if(System_Mode_Train())
		{
			//if((p_OP_ShowLater->speed < MOTOR_PERCENT_SPEED_MIX)||(p_OP_ShowLater->speed > MOTOR_PERCENT_SPEED_MAX))
				*p_OP_ShowNow_Speed = p_OP_PMode[Get_System_State_Mode()-1][Period_Now].speed;
			//else
				//*p_OP_ShowNow_Speed = p_OP_ShowLater->speed ;
		}
	}
	
	if(Motor_is_Start())
	{
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//��Ȧ�Զ��ж�
		Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
	}
	else
		Motor_Speed_Target_Set(0);
}

//��� �� �ٶ� & ʱ��  ��ֹ���
void Check_OP_All(void)
{
	if(System_is_Power_Off())
	{
		System_Power_Off();
	}
	else if(System_is_Pause())
	{
		p_OP_ShowLater->speed = *p_OP_ShowNow_Speed;
		Data_Set_Current_Speed(0);//ע��,��Ҫ����������״̬���������ٶ�,��"����"
	}
	else
	{
		// �ٶ�
		if(System_Mode_Free())
		{
			*p_OP_ShowNow_Speed = p_OP_Free_Mode->speed;
			*p_OP_ShowNow_Time = 0;
		}
		else if(System_Mode_Time())
		{
			*p_OP_ShowNow_Speed = p_OP_Timing_Mode->speed;
			*p_OP_ShowNow_Time = p_OP_Timing_Mode->time;
		}
		else if(System_Mode_Train())
		{
			if(Is_Mode_Legal(Get_System_State_Mode()) == 0)
				Set_System_State_Mode(SYSTEM_MODE_TRAIN_P1);
			Set_Pmode_Period_Now(0);
			*p_OP_ShowNow_Speed = p_OP_PMode[Get_System_State_Mode()-1][0].speed;
			
			if(System_is_Stop() == 0)
				*p_OP_ShowNow_Time = 0;
		}
		
		if(Motor_is_Start())
		{
			Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);//��Ȧ�Զ��ж�
			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
		}
		else
			Motor_Speed_Target_Set(0);
	}
}

//------------------- �ж� ģʽ �Ϸ� ----------------------------
uint8_t Is_Mode_Legal(uint8_t mode)
{
	if((mode > 0) && (mode <= TRAINING_MODE_NUMBER_MAX) )
	{
		return 1;
	}
	else
		return 0;
}
//------------------- �ж��ٶ� �Ϸ� ----------------------------
uint8_t Is_Speed_Legal(uint16_t speed)
{
	if((speed >= SPEED_LEGAL_MIN) && (speed <= SPEED_LEGAL_MAX))
	{
		return 1;
	}
	else
		return 0;
}

//------------------- �ж�ʱ�� �Ϸ� ----------------------------
uint8_t Is_Time_Legal(uint16_t time)
{
	if((time >= TIME_LEGAL_MIN) && (time <= TIME_LEGAL_MAX))
	{
		return 1;
	}
	else
		return 0;
}

//------------------- ���� ��ǰ �ٶ� ----------------------------
void Data_Set_Current_Speed(uint8_t speed)
{
	//if(System_is_Starting())
		//return;
	if(Special_Status_Get( SPECIAL_BIT_SKIP_INITIAL))// ���� �Զ�����
		return;
	
	*p_OP_ShowNow_Speed = speed;	
	
	/*if(Get_Temp_Slow_Down_State())
	{
		if(speed > Get_Down_Conversion_Speed_Now())
		{
			*p_OP_ShowNow_Speed = Get_Down_Conversion_Speed_Now();
		}
	}*/
	
	Motor_Speed_Target_Set(speed);
}

//------------------- ���� ��ǰ ʱ�� ----------------------------
void Data_Set_Current_Time(uint16_t time)
{
	//if(System_is_Starting())
		//return;
	
	*p_OP_ShowNow_Time = time;
}

//------------------- ���� ѵ��ʱ�� ----------------------------
void Set_Pmode_Period_Now(uint16_t value)
{
	Period_Now = value;
}


//------------------- �Ƿ�����ⲿ���� ----------------------------
uint8_t If_Accept_External_Control(uint8_t mode)
{
	if(ERROR_DISPLAY_STATUS == Get_System_State_Machine())
		return 0;
	
	//if((*p_Support_Control_Methods & mode) != 0)
		//return 0;
	

	return 1;
}

//------------------- ��ȡ����汾��  �ַ���ת uint32 ----------------------------
void get_uint3_version(char * buffer)
{
	char str[32];
	char *tmp;

	strncpy(str, (const char *)buffer, strlen(buffer));
	
	tmp = strtok(str, ".");
	if (tmp != NULL) {
			*p_Software_Version_high = (uint8_t)atoi(tmp);

			// ����Ӱ汾�ų�ʼ��
			tmp = strtok(NULL, ".");
			if (tmp != NULL) {
				*p_Software_Version_low = (uint8_t)atoi(tmp)<<8;
					tmp = strtok(NULL, ".");
					if (tmp != NULL) {
							*p_Software_Version_low |= (uint8_t)atoi(tmp);
					}
			}
	}
}

//------------------- ���wifi��־ ----------------------------
void System_Wifi_State_Clean(void)
{
	*p_WIFI_Rssi = 0xFF;
}
//------------------- ���������־ ----------------------------
void System_BT_State_Clean(void)
{
	*p_BLE_Rssi = 0;
}
extern void LCD_Refresh_Restore(void);

//------------------- ���ÿ��Ʒ�ʽ ----------------------------
void Set_Ctrl_Mode_Type(System_Ctrl_Mode_Type_enum type)
{
	LCD_Refresh_Restore();//�ָ�ˢ��
	Ctrl_Mode_Type = type;
}
	
//------------------- ��ȡ���Ʒ�ʽ ----------------------------
System_Ctrl_Mode_Type_enum Get_Ctrl_Mode_Type(void)
{
	return Ctrl_Mode_Type;
}

//------------------- OTA �Զ��˳� 1���һ�� ----------------------------
void OTA_Time_Out(void)
{
	static uint32_t time_out_cnt=0;
	
	time_out_cnt ++;
	if(time_out_cnt > OTA_SHUTDOWN_TIME_OUT)
	{
		SysSoftReset();// �����λ
	}
}

//------------------- ��ʾ����ӳ�� ----------------------------
void LCD_TO_Mapping(void)
{
	
}

// �������ͳ��
//*********************************************************************************************
//-------------- ���� ���ͳ��  -------------------
//	count ��
void Finish_Statistics_Count(uint8_t count)
{

	* p_Finish_Statistics_Time += count;								//	���ͳ�� --> ʱ��
	if(*p_OP_ShowNow_Speed >= MOTOR_PERCENT_SPEED_MIX)
		* p_Finish_Statistics_Speed = *p_OP_ShowNow_Speed;	//	���ͳ�� --> ǿ��
	* p_Finish_Statistics_Distance += (Motor_Speed_Now * EVERY_1PERCENT_DISTANCE_PER_SECOND / 100);			//	���ͳ�� --> ��Ӿ����
}


//-------------- ��� ���ͳ��  -------------------
void Finish_Statistics_Clean( void )
{
	* p_Finish_Statistics_Time = 0;				//	���ͳ�� --> ʱ��
	* p_Finish_Statistics_Speed = 0;			//	���ͳ�� --> ǿ��
	* p_Finish_Statistics_Distance = 0;		//	���ͳ�� --> ��Ӿ����
}


//-------------- �ϴ� ���ͳ��  -------------------
void Finish_Statistics_Upload( void )
{
	WIFI_Finish_Statistics_Upload();
}


//-------------- �߳� � ��־����  -------------------
void Thread_Activity_Sign_Clean( void )
{
	*p_Thread_Activity_Sign = 0;
}

//-------------- �߳� � ����  -------------------
void Thread_Activity_Sign_Set( uint16_t val )
{
	*p_Thread_Activity_Sign |= val;
}







//---------------------------------------------------------------------------------------------------------------------------------------------------






