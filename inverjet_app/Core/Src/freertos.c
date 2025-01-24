/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "iap.h"
#include "iwdg.h"
#include "gpio.h"
#include "pump.h"
#include "dev.h"
#include "modbus.h"
#include "subsystem.h"
#include "display.h"
#include "Breath_light.h"
#include "timing.h"
#include "key.h"
#include "motor.h"
#include "debug_protocol.h"	///////////////////////	串口调试

#include "macro_definition.h"				// 统一宏定义
#include "wifi_thread.h"				// wifi模组
#include "wifi.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId Breath_Light_TaHandle;
osThreadId Rs485_Modbus_TaHandle;
osThreadId Main_TaskHandle;
osThreadId Key_Button_TaskHandle;
osThreadId Motor_TaskHandle;
osThreadId wifi_moduleHandle;
osThreadId BT_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Breath_Light_Handler(void const * argument);
void Rs485_Modbus_Handler(void const * argument);
void Main_Handler(void const * argument);
void Key_Button_Handler(void const * argument);
void Motor_Handler(void const * argument);
void wifi_module_Handler(void const * argument);
void BT_Handler(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationTickHook(void);

/* USER CODE BEGIN 3 */
__weak void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Breath_Light_Ta */
  osThreadDef(Breath_Light_Ta, Breath_Light_Handler, osPriorityLow, 0, 128);
  Breath_Light_TaHandle = osThreadCreate(osThread(Breath_Light_Ta), NULL);

  /* definition and creation of Rs485_Modbus_Ta */
  osThreadDef(Rs485_Modbus_Ta, Rs485_Modbus_Handler, osPriorityHigh, 0, 128);
  Rs485_Modbus_TaHandle = osThreadCreate(osThread(Rs485_Modbus_Ta), NULL);

  /* definition and creation of Main_Task */
  osThreadDef(Main_Task, Main_Handler, osPriorityRealtime, 0, 128);
  Main_TaskHandle = osThreadCreate(osThread(Main_Task), NULL);

  /* definition and creation of Key_Button_Task */
  osThreadDef(Key_Button_Task, Key_Button_Handler, osPriorityIdle, 0, 128);
  Key_Button_TaskHandle = osThreadCreate(osThread(Key_Button_Task), NULL);

  /* definition and creation of Motor_Task */
  osThreadDef(Motor_Task, Motor_Handler, osPriorityRealtime, 0, 256);
  Motor_TaskHandle = osThreadCreate(osThread(Motor_Task), NULL);

  /* definition and creation of wifi_module */
  osThreadDef(wifi_module, wifi_module_Handler, osPriorityHigh, 0, 640);
  wifi_moduleHandle = osThreadCreate(osThread(wifi_module), NULL);

  /* definition and creation of BT_Task */
  osThreadDef(BT_Task, BT_Handler, osPriorityAboveNormal, 0, 64);
  BT_TaskHandle = osThreadCreate(osThread(BT_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Breath_Light_Handler */
/**
  * @brief  Function implementing the Breath_Light_Ta thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Breath_Light_Handler */
void Breath_Light_Handler(void const * argument)
{
  /* USER CODE BEGIN Breath_Light_Handler */
	osDelay(POWER_ON_WAITE_TIME_TASK);
	App_Breath_light_Init();
  /* Infinite loop */
  while(1)
  {
		HAL_IWDG_Refresh(&hiwdg);
		
		App_Breath_light_Handler();
		
		osDelay(THREAD_PERIOD_BREATH_LIGHT_TASK);
  }
  /* USER CODE END Breath_Light_Handler */
}

/* USER CODE BEGIN Header_Rs485_Modbus_Handler */
/**
* @brief Function implementing the Rs485_Modbus_Ta thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Rs485_Modbus_Handler */
void Rs485_Modbus_Handler(void const * argument)
{
  /* USER CODE BEGIN Rs485_Modbus_Handler */
	uint16_t time_cnt=0;
	
	Modbus_Init();
	App_Data_Init();
	osDelay(1000);
  /* Infinite loop */
  while(1)
  {
		HAL_IWDG_Refresh(&hiwdg);
		
		Modbus_Handle_Task();
				
		if(System_is_OTA())
		{
			if(time_cnt++ > MODBUS_THREAD_ONE_SECOND)
			{
				time_cnt = 0;
				OTA_Time_Out();
			}
		}
		
		osDelay(THREAD_PERIOD_RS485_MODBUS_TASK);
  }
  /* USER CODE END Rs485_Modbus_Handler */
}

/* USER CODE BEGIN Header_Main_Handler */
/**
* @brief Function implementing the Main_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Main_Handler */
void Main_Handler(void const * argument)
{
  /* USER CODE BEGIN Main_Handler */

	Set_Software_Version();
	App_Timing_Init();		//5s
	//mcu_get_system_time();
	osDelay(20);
	System_Check_Timer_Clean();
	//osDelay(POWER_ON_WAITE_TIME_TASK);
  /* Infinite loop */
  while(1)
  {		
		HAL_IWDG_Refresh(&hiwdg);
		App_Timing_Handler();
				
		osDelay(THREAD_PERIOD_MAIN_TASK);
		
  }
  /* USER CODE END Main_Handler */
}

/* USER CODE BEGIN Header_Key_Button_Handler */
/**
* @brief Function implementing the Key_Button_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Key_Button_Handler */
void Key_Button_Handler(void const * argument)
{
  /* USER CODE BEGIN Key_Button_Handler */

	App_Key_Init();
	//osDelay(POWER_ON_WAITE_TIME_TASK);
  /* Infinite loop */
  while(1)
  {
		HAL_IWDG_Refresh(&hiwdg);
//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);		// 1
		App_Key_Handler();
//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);	// 0	
		osDelay(THREAD_PERIOD_KEY_BUTTON_TASK);
  }
  /* USER CODE END Key_Button_Handler */
}

/* USER CODE BEGIN Header_Motor_Handler */
/**
* @brief Function implementing the Motor_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Motor_Handler */
void Motor_Handler(void const * argument)
{
  /* USER CODE BEGIN Motor_Handler */
	
	Metering_Receive_Init();
	Debug_Protocol_Init();
	//osDelay(POWER_ON_WAITE_TIME_TASK);
  /* Infinite loop */
  for(;;)
  {
//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);		// 1
		App_Motor_Handler();
//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);	// 0	
    osDelay(THREAD_PERIOD_MOTOR_TASK);
  }
  /* USER CODE END Motor_Handler */
}

/* USER CODE BEGIN Header_wifi_module_Handler */
/**
* @brief Function implementing the wifi_module thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_wifi_module_Handler */
void wifi_module_Handler(void const * argument)
{
  /* USER CODE BEGIN wifi_module_Handler */
	uint16_t time_cnt=0;
	
	wifi_Module_Init();
	osDelay(POWER_ON_WAITE_TIME_TASK);
  /* Infinite loop */
  for(;;)
  {
//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);		// 1
		Wifi_Module_Handler();
		HAL_IWDG_Refresh(&hiwdg);
		
		if(System_is_OTA())
		{
			if(time_cnt++ > WIFI_THREAD_ONE_SECOND)
			{
				time_cnt = 0;
				OTA_Time_Out();
			}
		}
//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);	// 0	
    osDelay(THREAD_PERIOD_WIFI_TASK);
  }
  /* USER CODE END wifi_module_Handler */
}

/* USER CODE BEGIN Header_BT_Handler */
/**
* @brief Function implementing the BT_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BT_Handler */
void BT_Handler(void const * argument)
{
  /* USER CODE BEGIN BT_Handler */
	BT_Modbus_Config_Init();
	osDelay(1000);
	BT_Module_AT_Init();// 6s
	//osDelay(1000);
	
  /* Infinite loop */
  for(;;)
  {
		HAL_IWDG_Refresh(&hiwdg);
		BT_MsTimeout();
				
		BT_Read_Handler();
		
		osDelay(THREAD_PERIOD_BT_TASK);
  }
  /* USER CODE END BT_Handler */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

