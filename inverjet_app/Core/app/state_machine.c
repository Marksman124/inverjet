/**
******************************************************************************
* @file				state_machine.c
* @brief			状态机 
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "state_machine.h"
#include "timing.h"

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

// 特别状态 位
uint8_t Special_Status_Bit = 0;


// 拨码开关
uint8_t System_Dial_Switch = 0;
/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//------------------- 硬件 & 驱动 ----------------------------
// 初始化
void App_State_Machine_Init(void)
{
	*p_System_State_Machine = POWER_OFF_STATUS;  //状态机
	
	Special_Status_Bit = 0;
}

//------------- 模式发生切换 ------------------------------------
// machine: 新状态机
void Check_Mode_Change(uint16_t machine)
{
	//	切换模式时上传<统计数据>
	if(Is_Change_System_Mode(machine))
	{
		Finish_Statistics_Upload();
	}
	
}

//------------------- 设置状态机  ----------------------------

uint8_t Set_System_State_Machine(uint8_t val)
{
	if(val >= SYSTEM_STATE_END) //溢出
		return 0;
		
	Check_Mode_Change(val);
	
	*p_System_State_Machine = val;
	
	return 1;
}


//------------------- 获取状态机  ----------------------------

uint8_t Get_System_State_Machine(void)
{
	return *p_System_State_Machine;
}

//------------------- 快速获取状态  ----------------------------

// 电机启动状态
uint8_t Motor_is_Start(void)
{
	if((*p_System_State_Machine == FREE_MODE_STARTING) || (*p_System_State_Machine == FREE_MODE_RUNNING) 
			|| (*p_System_State_Machine == TIMING_MODE_STARTING) || (*p_System_State_Machine == TIMING_MODE_RUNNING)
			|| (*p_System_State_Machine == TRAINING_MODE_STARTING) || (*p_System_State_Machine == TRAINING_MODE_RUNNING))
	return 1;
	else
		return 0;
}

// 正常工作状态
uint8_t System_is_Normal_Operation(void)
{
	if((*p_System_State_Machine >= FREE_MODE_INITIAL) && (*p_System_State_Machine <= TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}

// 初始状态
uint8_t System_is_Initial(void)
{
	if((*p_System_State_Machine == FREE_MODE_INITIAL) || (*p_System_State_Machine == TIMING_MODE_INITIAL) || (*p_System_State_Machine == TRAINING_MODE_INITIAL))
		return 1;
	else
		return 0;
}

// 启动状态
uint8_t System_is_Starting(void)
{
	if((*p_System_State_Machine == FREE_MODE_STARTING) || (*p_System_State_Machine == TIMING_MODE_STARTING) || (*p_System_State_Machine == TRAINING_MODE_STARTING))
		return 1;
	else
		return 0;
}

// 运行状态
uint8_t System_is_Running(void)
{
	if((*p_System_State_Machine == FREE_MODE_RUNNING) || (*p_System_State_Machine == TIMING_MODE_RUNNING) || (*p_System_State_Machine == TRAINING_MODE_RUNNING))
		return 1;
	else
		return 0;
}

// 暂停状态
uint8_t System_is_Pause(void)
{
	if((*p_System_State_Machine == FREE_MODE_SUSPEND) || (*p_System_State_Machine == TIMING_MODE_SUSPEND) || (*p_System_State_Machine == TRAINING_MODE_SUSPEND))
		return 1;
	else
		return 0;
}

// 结束状态
uint8_t System_is_Stop(void)
{
	if((*p_System_State_Machine == FREE_MODE_STOP) || (*p_System_State_Machine == TIMING_MODE_STOP) || (*p_System_State_Machine == TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}

// 操作菜单
uint8_t System_is_Operation(void)
{
	if(*p_System_State_Machine == OPERATION_MENU_STATUS)
		return 1;
	else
		return 0;
}

// 故障界面
uint8_t System_is_Error(void)
{
	if(*p_System_State_Machine == ERROR_DISPLAY_STATUS)
		return 1;
	else
		return 0;
}

// 关机
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


// 自由 模式
uint8_t System_Mode_Free(void)
{
	if( (*p_System_State_Machine >= FREE_MODE_INITIAL) && (*p_System_State_Machine <= FREE_MODE_STOP))
		return 1;
	else
		return 0;
}

// 定时 模式
uint8_t System_Mode_Time(void)
{
	if( (*p_System_State_Machine >= TIMING_MODE_INITIAL) && (*p_System_State_Machine <= TIMING_MODE_STOP))
		return 1;
	else
		return 0;
}

// 训练 模式
uint8_t System_Mode_Train(void)
{
	if( (*p_System_State_Machine >= TRAINING_MODE_INITIAL) && (*p_System_State_Machine <= TRAINING_MODE_STOP))
		return 1;
	else
		return 0;
}



//------------------- 更改状态 不改变模式  ----------------------------
// --> 初始状态 （不更改模式）
void Arbitrarily_To_Initial(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
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
	
// --> 启动状态 （不更改模式）
void Arbitrarily_To_Starting(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
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

// --> 运行状态 （不更改模式）
void Arbitrarily_To_Running(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
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

// --> 暂停状态 （不更改模式）
void Arbitrarily_To_Pause(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
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

// --> 结束状态 （不更改模式）
void Arbitrarily_To_Stop(void)
{
	Special_Status_Delete(SPECIAL_BIT_SKIP_INITIAL);
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

//------------------- 判断 状态 & 模式  ----------------------------
// 	模式 是否发生变化 
//	1:是  0:否
uint8_t Is_Change_System_Mode(uint16_t machine)
{
	uint8_t result = 1;
	
	switch(machine)
	{
		case POWER_OFF_STATUS:			//	关机
			if(System_is_Power_Off())
				result = 0;
		break;
	//自由模式
		case FREE_MODE_INITIAL:				//	1		初始状态	
		case FREE_MODE_STARTING:			//			启动中
		case FREE_MODE_RUNNING:				//			运行中
		case FREE_MODE_SUSPEND:				//			暂停
		case FREE_MODE_STOP:					//	5		结束
			if(System_Mode_Free())
				result = 0;
		break;
	//定时模式
		case TIMING_MODE_INITIAL:			//	6		初始状态
		case TIMING_MODE_STARTING:		//			启动中
		case TIMING_MODE_RUNNING:			//			运行中
		case TIMING_MODE_SUSPEND:			//			暂停
		case TIMING_MODE_STOP:				//	10	结束
			if(System_Mode_Time())
				result = 0;
		break;
	//训练模式
		case TRAINING_MODE_INITIAL:		//	11	初始状态
		case TRAINING_MODE_STARTING:	//			启动中
		case TRAINING_MODE_RUNNING:		//			运行中
		case TRAINING_MODE_SUSPEND:		//			暂停
		case TRAINING_MODE_STOP:			//	15	结束
			if(System_Mode_Train())
				result = 0;
		break;
	
	// 其它状态
		case OPERATION_MENU_STATUS:		//			操作菜单
			if(System_is_Operation())
				result = 0;
		break;
		case ERROR_DISPLAY_STATUS:		//			故障界面
			if(System_is_Error())
				result = 0;
		break;
		case OTA_UPGRADE_STATUS:			//			OTA 升级
			if(System_is_OTA())
				result = 0;
		break;
			
		default:
			break;
	}
	
	return result;
}


//------------------- 特别状态 机  ----------------------------

// 添加 特别状态 标志
void Special_Status_Add(uint8_t num)
{
	Special_Status_Bit |= num;
}


// 删除 特别状态 标志
void Special_Status_Delete(uint8_t num)
{
	Special_Status_Bit &= ~num;
}

// 获取 特别状态 标志
uint8_t Special_Status_Get(uint8_t num)
{
	return (Special_Status_Bit & num);
}






