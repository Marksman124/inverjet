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

uint16_t* p_Driver_Software_Version;			//����������汾

uint16_t* p_System_Fault_Static;			//����״̬
uint16_t* p_Motor_Fault_Static;				//����״̬
uint16_t* p_Mos_Temperature;					//mos �¶�
uint16_t* p_Box_Temperature;					//���� �¶�
uint32_t* p_Motor_Current;						//��� ����
uint32_t* p_Motor_Reality_Speed;			//��� ʵ�� ת��
uint32_t* p_Send_Reality_Speed;				//�·� ʵ�� ת��

uint16_t* p_Motor_Bus_Voltage;				//ĸ�� ��ѹ
	
uint16_t* p_Modbus_Node_Addr;					//��ַ
uint16_t* p_Modbus_Baud_Rate;					//������
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
uint32_t* p_System_Sleeping_Second_Cnt;		// ����ʱ��


/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- Ӳ�� & ���� ----------------------------

// ��ʼ�� ����ִ��
void Check_Data_Init(void)
{
	static uint8_t x=0,y=0;
	
	if(Is_Speed_Legal(p_OP_Free_Mode->speed) == 0)
	{
		*p_OP_Free_Mode = OP_Init_Free;
	}
	
	if((Is_Speed_Legal(p_OP_Timing_Mode->speed) == 0) || (Is_Time_Legal(p_OP_Timing_Mode->time) == 0))
	{
		*p_OP_Timing_Mode = OP_Init_Timing;
	}
	//*p_Modbus_Node_Addr = 21;
	//*p_Modbus_Baud_Rate = 4;
	//*p_Support_Control_Methods = 0;
	
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
	
	*p_System_Runing_Second_Cnt = 0; //����ʱ��
	*p_No_Operation_Second_Cnt = 0;	//���˲���ʱ��
	*p_System_Sleeping_Second_Cnt = 0;// ����ʱ��
	*p_Analog_key_Value = 0;	//���ⰴ��
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
uint8_t If_Accept_External_Control(void)
{
	if(ERROR_DISPLAY_STATUS != Get_System_State_Machine())
		return 1;
	else
		return 0;
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
	
	
//------------------- ���ÿ��Ʒ�ʽ ----------------------------
void Set_Ctrl_Mode_Type(System_Ctrl_Mode_Type_enum type)
{
	Ctrl_Mode_Type = type;
}
	
//------------------- ��ȡ���Ʒ�ʽ ----------------------------
System_Ctrl_Mode_Type_enum Get_Ctrl_Mode_Type(void)
{
	return Ctrl_Mode_Type;
}


