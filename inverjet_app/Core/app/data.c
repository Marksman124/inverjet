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
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

uint32_t Product_Model_Ccode[MODEL_DIAL_SWITCH_NUMBER] = 
{
	PRODUCT_MODEL_CODE_SJ230,
	PRODUCT_MODEL_CODE_SJ200,
	PRODUCT_MODEL_CODE_SJ160,
	PRODUCT_MODEL_CODE_SJ230
};

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

System_Ctrl_Mode_Type_enum Ctrl_Mode_Type = CTRL_FROM_KEY;				// ���Ʒ�ʽ

// ��ģʽ ����
Operating_Parameters* p_OP_Free_Mode;

Operating_Parameters* p_OP_Timing_Mode;

Operating_Parameters (*p_OP_PMode)[TRAINING_MODE_PERIOD_MAX] = OP_Init_PMode;

//==========================================================
//--------------------------- �������ȡ��Ϣ
//==========================================================
uint16_t* p_Driver_Software_Version;			// ����������汾
uint16_t* p_Motor_Fault_Static;						// ����״̬		������

uint32_t* p_Motor_Reality_Speed;					// ��� ʵ�� ת��
uint32_t* p_Motor_Reality_Power;					// ��� ʵ�� ����

uint16_t* p_Mos_Temperature;							// mos �¶�
uint32_t* p_Motor_Current;								// ��� ����		���
uint16_t* p_Motor_Bus_Voltage;						// ĸ�� ��ѹ		����
uint16_t* p_Motor_Bus_Current;						// ĸ�� ����  	����

//==========================================================
//--------------------------- ������Ϣ
//==========================================================
uint16_t* p_System_Fault_Static;					// ����״̬		����
uint16_t* p_Box_Temperature;							// ���� �¶�
uint32_t* p_Send_Reality_Speed;						// �·� ʵ�� ת��
uint16_t* p_Simulated_Swim_Distance;			// ģ����Ӿ����


uint16_t* p_Support_Control_Methods;	//���ο��Ʒ�ʽ
uint16_t* p_Motor_Pole_Number;				//�������
uint16_t* p_Breath_Light_Max;					//��Ȧ����  
	
uint8_t Motor_State_Storage[MOTOR_PROTOCOL_ADDR_MAX]={0};//���״̬

//================= ��ʱ����  ȫ�� ================================
uint16_t Temp_Data_P5_Acceleration = 2;				//P5 ���ٶ�
uint16_t Temp_Data_P5_100_Time = 15;					//P5 100% ʱ��	��
uint16_t Temp_Data_P5_0_Time = 15;						//P5 0% 	ʱ��	��

uint8_t WIFI_Rssi = 0xFF;

uint16_t* p_Analog_key_Value;					// ���ⰴ��

uint8_t System_PowerUp_Finish = 0;
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



/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- Ӳ�� & ���� ----------------------------

// ��ʼ�� ����ִ��
void Check_Data_Init(void)
{
	static uint8_t x=0,y=0;
	
	*p_System_Runing_Second_Cnt = 0; 		//���� ʱ��
	*p_No_Operation_Second_Cnt = 0;			//���˲��� ʱ��
	*p_System_Startup_Second_Cnt = 0;		// ���� ʱ��
	*p_Analog_key_Value = 0;						//���ⰴ��
	
	if(Is_Speed_Legal(p_OP_Free_Mode->speed) == 0)
	{
		*p_OP_Free_Mode = OP_Init_Free;
	}
	
	if((Is_Speed_Legal(p_OP_Timing_Mode->speed) == 0) || (Is_Time_Legal(p_OP_Timing_Mode->time) == 0))
	{
		*p_OP_Timing_Mode = OP_Init_Timing;
	}

	if( ( *p_Motor_Pole_Number > MOTOR_RPM_MAX_OF_POLES) || ( *p_Motor_Pole_Number < MOTOR_RPM_MIX_OF_POLES))
	{
		*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	}
	
	for(x=0; x<TRAINING_MODE_NUMBER_MAX; x++)
	{
		for(y=0; y<TRAINING_MODE_PERIOD_MAX; y++)
		{
			if((Is_Speed_Legal(p_OP_PMode[x][y].speed) == 0) || (Is_Time_Legal(p_OP_PMode[x][y].time) == 0))
			{
				memcpy(p_OP_PMode, OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
				return;
			}
			if(y>0)
			{
				if(p_OP_PMode[x][y].time <= p_OP_PMode[x][y-1].time)
				{
					memcpy(p_OP_PMode, &OP_Init_PMode, (sizeof(Operating_Parameters)*TRAINING_MODE_NUMBER_MAX*TRAINING_MODE_PERIOD_MAX));
					return;
				}
			}
		}
	}
}


extern void Get_Mapping_Register(void);
// ��ʼ��
void App_Data_Init(void)
{
	Read_OPMode();
	// ��ȡӳ��  flash�Ѷ�
	Get_Mapping_Register();
	
	// ѵ��ģʽ ��ǰ״̬
	*p_PMode_Now = 0;
	Period_Now = 0;
	
	// ��ģʽ ����
	Check_Data_Init();
	
	//�洢  ��һ�� ���� ������
	//Memset_OPMode();
	
	// ��Ļ��ʼ��
	TM1621_LCD_Init();
	
	TM1621_Buzzer_Init();
	
	//test ���Թ���Ļ����pwm �ǵ�ɾ  wuqingguang 2024-09-09
	TM1621_light_Off();
}


// �ָ� ��ʼ��
void App_Data_ReInit(void)
{
	
	*p_Local_Address = MODBUS_LOCAL_ADDRESS;
	*p_Baud_Rate = MODBUS_BAUDRATE_DEFAULT;
	*p_Support_Control_Methods = 0;
	
	
	// ѵ��ģʽ ��ǰ״̬
	*p_PMode_Now = 0;
	Period_Now = 0;
	
	// ��ģʽ ����
	*p_OP_Free_Mode = OP_Init_Free;
	*p_OP_Timing_Mode = OP_Init_Timing;
	
	*p_Motor_Pole_Number = MOTOR_RPM_NUMBER_OF_POLES;
	
	memcpy(&p_OP_PMode[0][0], &OP_Init_PMode[0][0], sizeof(OP_Init_PMode));
	
	//�洢  ��һ�� ���� ������
	Memset_OPMode();
}

// �� flash
uint8_t Read_OPMode(void)
{
	MB_Flash_Buffer_Read();
	return 1;
}

// �� flash
uint8_t Memset_OPMode(void)
{
	MB_Flash_Buffer_Write();
	return 1;
}

//�洢 �� �ٶ�
void Update_OP_Speed(void)
{
	if(System_Mode_Free())	// ����
	{
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
		Memset_OPMode();//��flash
	}
	else if(System_Mode_Time())	// ��ʱ
	{
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		Memset_OPMode();//��flash
	}
}

//�洢 �� ʱ��
void Update_OP_Time(void)
{
	if(System_Mode_Time())	// ��ʱ
	{
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
		Memset_OPMode();//��flash
	}
}

//�洢 �� �ٶ� & ʱ��
void Update_OP_All(void)
{
	if(System_Mode_Free())	// ����
	{
		p_OP_Free_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Free_Mode->time = 0;
		Memset_OPMode();//��flash
	}
	else if(System_Mode_Time())	// ��ʱ
	{
		p_OP_Timing_Mode->speed = *p_OP_ShowNow_Speed;
		p_OP_Timing_Mode->time = *p_OP_ShowNow_Time;
		Memset_OPMode();//��flash
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
	
	*p_OP_ShowNow_Speed = speed;	
	Motor_Speed_Target_Set(speed);
	
	//if(System_is_Running())
		Special_Status_Add(SPECIAL_BIT_SKIP_STARTING);
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
uint32_t get_uint3_version(char * buffer)
{
	char str[32];
	char *tmp;
	uint32_t version = 0;
	
	strncpy(str, (const char *)buffer, strlen(buffer));
	
	tmp = strtok(str, ".");
	if (tmp != NULL) {
			version = (uint8_t)atoi(tmp)<<16;

			// ����Ӱ汾�ų�ʼ��
			tmp = strtok(NULL, ".");
			if (tmp != NULL) {
					tmp = strtok(NULL, ".");
					if (tmp != NULL) {
							version |= (uint8_t)atoi(tmp);
					}
			}
	}
	return version;
}

//------------------- ���wifi��־ ----------------------------
void System_Wifi_State_Clean(void)
{
	WIFI_Rssi = 0xFF;
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


//------------------- ��ȡ������ ----------------------------
uint32_t Get_Model_Code_Num(void)
{
	System_Dial_Switch = Gpio_Get_Dial_Switch();
	
	if(System_Dial_Switch < MODEL_DIAL_SWITCH_NUMBER)
		return Product_Model_Ccode[System_Dial_Switch];
	else
		return PRODUCT_MODEL_CODE_SJ230;//Ĭ��
}

// ÿ %1 ÿ�� ������Ӿ���� �Ŵ�100��
uint32_t Get_Every_1Percent_Distance_Per_Second(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
	else
	{
		return SJ230_EVERY_1PERCENT_DISTANCE_PER_SECOND;
	}
}

//���ת�� 100%
uint32_t Get_Motor_Rpm_Speed_Max(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_RPM_SPEED_MAX;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_RPM_SPEED_MAX;
	}
	else
	{
		return SJ230_MOTOR_RPM_SPEED_MAX;
	}
}


//���ת�� 20%
uint32_t Get_Motor_Rpm_Speed_Mix(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_RPM_SPEED_MIX;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_RPM_SPEED_MIX;
	}
	else
	{
		return SJ230_MOTOR_RPM_SPEED_MIX;
	}
}

// ���� ��Ƶ
//*********************************************************************************************
//-------------- ������� ����ֵ  -------------------
uint32_t Get_Motor_Power_Alarm_Value(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_ALARM_VALUE;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_ALARM_VALUE;
	}
	else
	{
		return SJ230_MOTOR_POWER_ALARM_VALUE;
	}
}

//-------------- ������� ����  -------------------
uint32_t Get_Motor_Power_Reduce_Speed(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_REDUCE_SPEED;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_REDUCE_SPEED;
	}
	else
	{
		return SJ230_MOTOR_POWER_REDUCE_SPEED;
	}
}

//-------------- ������� �ָ�  -------------------
uint32_t Get_Motor_Power_Restore_Speed(void)
{
	uint32_t code=0;
	
	code = Get_Model_Code_Num();

	if(PRODUCT_MODEL_CODE_SJ160 == code)
	{
		return SJ160_MOTOR_POWER_RESTORE_SPEED;
	}
	else if(PRODUCT_MODEL_CODE_SJ200 == code)
	{
		return SJ200_MOTOR_POWER_RESTORE_SPEED;
	}
	else
	{
		return SJ230_MOTOR_POWER_RESTORE_SPEED;
	}
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
	Finish_Statistics_Clean();
}












//---------------------------------------------------------------------------------------------------------------------------------------------------






