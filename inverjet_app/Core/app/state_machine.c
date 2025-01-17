/**
******************************************************************************
* @file				state_machine.c
* @brief			״̬�� 
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "state_machine.h"
#include "timing.h"
#include "key.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

// �ر�״̬ λ
uint8_t Special_Status_Bit = 0;


/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- Ӳ�� & ���� ----------------------------
// ��ʼ��
void App_State_Machine_Init(void)
{
	*p_System_State_Machine = POWER_OFF_STATUS;  //״̬��
	
	Special_Status_Bit = 0;
}

//-----------------------------------------------------------------------------------------------
//******************************************************************************************
//------------- ģʽ�����л� ------------------------------------
// machine: ��״̬��
void Check_Mode_Change(uint16_t machine)
{
	if(Get_System_State_Machine() != machine)
	{
		Clean_Automatic_Shutdown_Timer();	// ��� �Զ��ػ���ʱ��
		Clean_Timing_Timer_Cnt();					// �������(timing) ��ʱ��
		//Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
		//Down_Conversion_State_Update();		// ��� ��Ƶ״̬
		
		//	�л�ģʽʱ
		if(Is_Change_System_Mode(machine))
		{
			Finish_Statistics_Upload();			// �ϴ�<ͳ������>
		}
	}
}

//------------------- ����״̬��  ----------------------------

uint8_t Set_System_State_Machine(uint8_t val)
{
	if(val >= SYSTEM_STATE_END) //���
		return 0;
//-----------------  չʾ���� -------------------------
#ifdef SYSTEM_SHOW_MODEL_MACHINE
	if(val == ERROR_DISPLAY_STATUS)
		return 0;
#endif
	
	Check_Mode_Change(val);
	
	*p_System_State_Machine = val;
	
	// ģʽ
	if(System_Mode_Train())
	{
		if(Get_System_State_Mode() == 0)
		{
			Set_System_State_Mode(SYSTEM_MODE_TRAIN_P1);
			Set_Pmode_Period_Now(0);
		}
	}
	
	return 1;
}


//------------------- ��ȡ״̬��  ----------------------------

uint8_t Get_System_State_Machine(void)
{
	return *p_System_State_Machine;
}

//******************************************************************************************
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
//******************************************************************************************
//------------------- ���� ģʽ(Pn)  ----------------------------

uint8_t Set_System_State_Mode(uint8_t val)
{
	if(val > TRAINING_MODE_NUMBER_MAX) //���
		return 0;
		
	//	�л�ģʽʱ
	if(val != *p_PMode_Now)
	{
		Set_Pmode_Period_Now(0);
		Finish_Statistics_Upload();			// �ϴ�<ͳ������>
		//Down_Conversion_State_Update();		// ��� ��Ƶ״̬
		Clean_Timing_Timer_Cnt();				// �������(timing) ��ʱ��
	}
	
	*p_PMode_Now = val;
	
	return 1;
}


//------------------- ��ȡ ģʽ  ----------------------------

uint8_t Get_System_State_Mode(void)
{
	return *p_PMode_Now;
}

//******************************************************************************************
//-----------------------------------------------------------------------------------------------


//------------------- ���ٻ�ȡ״̬  ----------------------------

// �������״̬
uint8_t Motor_is_Start(void)
{
	if((*p_System_State_Machine == FREE_MODE_STARTING) || (*p_System_State_Machine == FREE_MODE_RUNNING) 
			|| (*p_System_State_Machine == TIMING_MODE_STARTING) || (*p_System_State_Machine == TIMING_MODE_RUNNING)
			|| (*p_System_State_Machine == TRAINING_MODE_STARTING) || (*p_System_State_Machine == TRAINING_MODE_RUNNING))
	return 1;
	else
		return 0;
}

// ��������״̬
uint8_t System_is_Normal_Operation(void)
{
	if((*p_System_State_Machine >= FREE_MODE_INITIAL) && (*p_System_State_Machine <= TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}

// ��ʼ״̬
uint8_t System_is_Initial(void)
{
	if((*p_System_State_Machine == FREE_MODE_INITIAL) || (*p_System_State_Machine == TIMING_MODE_INITIAL) || (*p_System_State_Machine == TRAINING_MODE_INITIAL))
		return 1;
	else
		return 0;
}

// ����״̬
uint8_t System_is_Starting(void)
{
	if((*p_System_State_Machine == FREE_MODE_STARTING) || (*p_System_State_Machine == TIMING_MODE_STARTING) || (*p_System_State_Machine == TRAINING_MODE_STARTING))
		return 1;
	else
		return 0;
}

// ����״̬
uint8_t System_is_Running(void)
{
	if((*p_System_State_Machine == FREE_MODE_RUNNING) || (*p_System_State_Machine == TIMING_MODE_RUNNING) || (*p_System_State_Machine == TRAINING_MODE_RUNNING))
		return 1;
	else
		return 0;
}

// ��ͣ״̬
uint8_t System_is_Pause(void)
{
	if((*p_System_State_Machine == FREE_MODE_SUSPEND) || (*p_System_State_Machine == TIMING_MODE_SUSPEND) || (*p_System_State_Machine == TRAINING_MODE_SUSPEND))
		return 1;
	else
		return 0;
}

// ����״̬
uint8_t System_is_Stop(void)
{
	if((*p_System_State_Machine == FREE_MODE_STOP) || (*p_System_State_Machine == TIMING_MODE_STOP) || (*p_System_State_Machine == TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}

// �����˵�
uint8_t System_is_Operation(void)
{
	if(*p_System_State_Machine == OPERATION_MENU_STATUS)
		return 1;
	else
		return 0;
}

// ���Ͻ���
uint8_t System_is_Error(void)
{
	if(*p_System_State_Machine == ERROR_DISPLAY_STATUS)
		return 1;
	else
		return 0;
}

// �ػ�
uint8_t System_is_Power_Off(void)
{
	if(*p_System_State_Machine == POWER_OFF_STATUS)
		return 1;
	else
		return 0;
}

// OTA
uint8_t System_is_OTA(void)
{
	if(*p_System_State_Machine == OTA_UPGRADE_STATUS)
		return 1;
	else
		return 0;
}


// ���� ģʽ
uint8_t System_Mode_Free(void)
{
	if( (*p_System_State_Machine >= FREE_MODE_INITIAL) && (*p_System_State_Machine <= FREE_MODE_STOP))
		return 1;
	else
		return 0;
}

// ��ʱ ģʽ
uint8_t System_Mode_Time(void)
{
	if( (*p_System_State_Machine >= TIMING_MODE_INITIAL) && (*p_System_State_Machine <= TIMING_MODE_STOP))
		return 1;
	else
		return 0;
}

// ѵ�� ģʽ
uint8_t System_Mode_Train(void)
{
	if( (*p_System_State_Machine >= TRAINING_MODE_INITIAL) && (*p_System_State_Machine <= TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}

// ���� ģʽ
uint8_t System_Mode_Surf(void)
{
	if( System_Mode_Train() && (Get_System_State_Mode() == SURFING_MODE_NUMBER_ID))
		return 1;
	else
		return 0;
}

//------------------- ����״̬ ���ı�ģʽ  ----------------------------
// --> ��ʼ״̬ ��������ģʽ��
void Arbitrarily_To_Initial(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	Clean_Timing_Timer_Cnt();
	
	if(System_is_Initial())
		return;
	else if(System_is_Starting())
		*p_System_State_Machine -= 1;
	else if(System_is_Running())
		*p_System_State_Machine -= 2;
	else if(System_is_Pause())
		*p_System_State_Machine -= 3;
	else if(System_is_Stop())
		*p_System_State_Machine -= 4;
	
	return;
}
	
// --> ����״̬ ��������ģʽ��
void Arbitrarily_To_Starting(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	Clean_Timing_Timer_Cnt();
	
	if(System_is_Initial())
		*p_System_State_Machine += 1;
	else if(System_is_Starting())
		return;
	else if(System_is_Running())
		*p_System_State_Machine -= 1;
	else if(System_is_Pause())
		*p_System_State_Machine -= 2;
	else if(System_is_Stop())
		*p_System_State_Machine -= 3;
	
	return;
}

// --> ����״̬ ��������ģʽ��
void Arbitrarily_To_Running(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	Clean_Timing_Timer_Cnt();
	
	if(System_is_Initial())
		*p_System_State_Machine += 2;
	else if(System_is_Starting())
		*p_System_State_Machine += 1;
	else if(System_is_Running())
		return;
	else if(System_is_Pause())
		*p_System_State_Machine -= 1;
	else if(System_is_Stop())
		*p_System_State_Machine -= 2;
	
	return;
}

// --> ��ͣ״̬ ��������ģʽ��
void Arbitrarily_To_Pause(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	Clean_Timing_Timer_Cnt();
	
	if(System_is_Initial())
		*p_System_State_Machine += 3;
	else if(System_is_Starting())
		*p_System_State_Machine += 2;
	else if(System_is_Running())
		*p_System_State_Machine += 1;
	else if(System_is_Pause())
		return;
	else if(System_is_Stop())
		*p_System_State_Machine -= 1;
	
	Clean_Automatic_Shutdown_Timer();
	return;
}

// --> ����״̬ ��������ģʽ��
void Arbitrarily_To_Stop(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
	Clean_Timing_Timer_Cnt();
	
	if(System_is_Initial())
		*p_System_State_Machine += 4;
	else if(System_is_Starting())
		*p_System_State_Machine += 3;
	else if(System_is_Running())
		*p_System_State_Machine += 2;
	else if(System_is_Pause())
		*p_System_State_Machine += 1;
	else if(System_is_Stop())
		return;
	
	Motor_Speed_Target_Set(0);
	Clean_Automatic_Shutdown_Timer();
	return;
}

//------------------- �ж� ״̬ & ģʽ  ----------------------------
// 	ģʽ �Ƿ����仯 
//	1:��  0:��
uint8_t Is_Change_System_Mode(uint16_t machine)
{
	uint8_t result = 1;
	
	switch(machine)
	{
		case POWER_OFF_STATUS:			//	�ػ�
			if(System_is_Power_Off())
				result = 0;
		break;
	//����ģʽ
		case FREE_MODE_INITIAL:				//	1		��ʼ״̬	
		case FREE_MODE_STARTING:			//			������
		case FREE_MODE_RUNNING:				//			������
		case FREE_MODE_SUSPEND:				//			��ͣ
		case FREE_MODE_STOP:					//	5		����
			if(System_Mode_Free())
				result = 0;
		break;
	//��ʱģʽ
		case TIMING_MODE_INITIAL:			//	6		��ʼ״̬
		case TIMING_MODE_STARTING:		//			������
		case TIMING_MODE_RUNNING:			//			������
		case TIMING_MODE_SUSPEND:			//			��ͣ
		case TIMING_MODE_STOP:				//	10	����
			if(System_Mode_Time())
				result = 0;
		break;
	//ѵ��ģʽ
		case TRAINING_MODE_INITIAL:		//	11	��ʼ״̬
		case TRAINING_MODE_STARTING:	//			������
		case TRAINING_MODE_RUNNING:		//			������
		case TRAINING_MODE_SUSPEND:		//			��ͣ
		case TRAINING_MODE_STOP:			//	15	����
			if(System_Mode_Train())
				result = 0;
		break;
	
	// ����״̬
		case OPERATION_MENU_STATUS:		//			�����˵�
			if(System_is_Operation())
				result = 0;
		break;
		case ERROR_DISPLAY_STATUS:		//			���Ͻ���
			if(System_is_Error())
				result = 0;
		break;
		case OTA_UPGRADE_STATUS:			//			OTA ����
			if(System_is_OTA())
				result = 0;
		break;
			
		default:
			break;
	}
	
	return result;
}


//------------------- �ر�״̬ ��  ----------------------------

// ��� �ر�״̬ ��־
void Special_Status_Add(uint8_t num)
{
	Special_Status_Bit |= num;
}


// ɾ�� �ر�״̬ ��־
void Special_Status_Delete(uint8_t num)
{
	Special_Status_Bit &= ~num;
}

// ��ȡ �ر�״̬ ��־
uint8_t Special_Status_Get(uint8_t num)
{
	return (Special_Status_Bit & num);
}

//------------------- ���в������ýӿ� ----------------------------
// �Ƿ���տ���
uint8_t System_Para_External_Control(System_Ctrl_Mode_Type_enum ctrler)
{
	switch(ctrler)
	{
		case CTRL_FROM_KEY:				//	����
			return 1;
		case CTRL_FROM_WIFI:						//	wifi
			if(If_Accept_External_Control(BLOCK_WIFI_CONTROL) == 0)
				return 0;
			break;
		case CTRL_FROM_BT:							//	����
			if(If_Accept_External_Control(BLOCK_BLUETOOTH_CONTROL) == 0)
				return 0;
			break;
		case CTRL_FROM_RS485:					//	modbus 485	
			if(If_Accept_External_Control(BLOCK_MODBUS_CONTROL) == 0)
				return 0;
			break;
	}
	return 1;
}

// ģʽ
void System_Para_Set_PMode(uint16_t pmode, System_Ctrl_Mode_Type_enum ctrler)
{
	if(System_Para_External_Control(ctrler))
	{
		Set_System_State_Mode(pmode);
		
		Set_Ctrl_Mode_Type(ctrler);//��ǿ�����Դ
	}
}

// ״̬��
void System_Para_Set_Status(uint16_t status, System_Ctrl_Mode_Type_enum ctrler)
{
	if(System_Para_External_Control(ctrler))
	{
		if(status <= TRAINING_MODE_STOP)
		{
			//	�л�ģʽʱ
			if(Is_Change_System_Mode(status))
			{
				Set_System_State_Machine(status);
				Check_OP_All();		// ȷ�������Ϸ�
			}
			else
			{
				Set_System_State_Machine(status);
				OP_Update_Mode();
			}
			
			Set_Ctrl_Mode_Type(ctrler);//��ǿ�����Դ
		}
	}
}

// �ٶ�
void System_Para_Set_Speed(uint16_t speed, System_Ctrl_Mode_Type_enum ctrler)
{
	if(System_Para_External_Control(ctrler))
	{
		if((speed < MOTOR_PERCENT_SPEED_MIX)&&(speed > 0))
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MIX;
		else if(speed > MOTOR_PERCENT_SPEED_MAX)
			*p_OP_ShowNow_Speed = MOTOR_PERCENT_SPEED_MAX;
		else
			*p_OP_ShowNow_Speed = speed;
		Set_Ctrl_Mode_Type(ctrler);//��ǿ�����Դ
		// ����
		Update_OP_Speed();
		
		if(Motor_is_Start())
		{
			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
		}
	}
}

// ʱ��
void System_Para_Set_Time(uint16_t time, System_Ctrl_Mode_Type_enum ctrler)
{
	if(System_Para_External_Control(ctrler))
	{
		if(time >= MOTOR_TIME_SHOW_MAX)
			time = MOTOR_TIME_SHOW_MAX - 1;
		
		*p_OP_ShowNow_Time = time;
		Set_Ctrl_Mode_Type(ctrler);//��ǿ�����Դ
		
		// ����
		Update_OP_Time();
	}
}



