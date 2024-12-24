/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "usart.h"
#include "debug_protocol.h"
#include "wifi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
//extern uint8_t aRxBuffer3[RXBUFFERSIZE];	 //HAL库USART接收Buffer
//extern uint8_t aRxBuffer5[RXBUFFERSIZE];	 //HAL库USART接收Buffer
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
extern void prvvUARTTxReadyISR(void);
extern void prvvUARTRxISR(void);
extern void prvvTIMERExpiredISR( void );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel3 global interrupt.
  */
void DMA1_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel3_IRQn 0 */

  /* USER CODE END DMA1_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  /* USER CODE BEGIN DMA1_Channel3_IRQn 1 */

  /* USER CODE END DMA1_Channel3_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt.
  */
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */

  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */

  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
	
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
#if DEBUG_USART == 1
	uint32_t timeout = 0;
	
	HAL_UART_IRQHandler(&huart1);
	while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) //等待就绪
	{
		timeout++; ////超时处理
		if (timeout > 50)
			break;
	}

	timeout = 0;
	while (HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer1, 1) != HAL_OK) //一次处理完成之后，重新开启中断并设置RxXferCount为1
	{
		timeout++; //超时处理
		if (timeout > 50)
		{
			//解除忙状态（由ORE导致，清零ORE位）
			if(HAL_UART_Receive_IT(&huart3, (uint8_t *)aRxBuffer1, 1) == HAL_BUSY)
			{
				huart1.Lock=HAL_UNLOCKED;
				//重新开始接收
				HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer1,1);
			}
			else
			{
				__HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);
			}
			break;
		}
	}
#elif MODBUS_USART == 1
	//HAL_UART_IRQHandler(&huart1);
	if(__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE)!= RESET) 
		{
				prvvUARTRxISR();//接收中断
		}

	if(__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_TXE)!= RESET) 
		{
			prvvUARTTxReadyISR();//发送中断
		}
	
  HAL_NVIC_ClearPendingIRQ(USART1_IRQn);
#else
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */
#endif
  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	
	if((USART2->SR&UART_FLAG_RXNE) != 0)
	{
		DEBUG_LED1_ON();
		//Res=USART2->DR;
			uart_receive_input(USART2->DR);
		DEBUG_LED1_OFF();
	}
  /* USER CODE END USART2_IRQn 0 */
  //HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
#if (MOTOR_MODULE_HUART == 3)
	DEBUG_LED2_ON();
	uint8_t temp=0;
	if((__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE) != RESET))//如果是接收完成中断，idle标志被置位
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);//清除标志位
		HAL_UART_DMAStop(&huart3); //  停止DMA传输，防止
		temp  =  __HAL_DMA_GET_COUNTER(huart3.hdmarx);// 获取DMA中未传输的数据个数   
		Motor_RxData(MOTOR_RS485_RX_BUFF_SIZE-temp);
		
		memset(Motor_DMABuff,0,MOTOR_RS485_RX_BUFF_SIZE);    				//清空缓存区
		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, Motor_DMABuff, MOTOR_RS485_RX_BUFF_SIZE); // 接收完毕后重启
		__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		   // 手动关闭DMA_IT_HT中断
	 }
	DEBUG_LED2_OFF();
#elif MODBUS_USART == 3
	if(__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_RXNE)!= RESET) 
		{
			prvvUARTRxISR();//接收中断
		}

	if(__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_TXE)!= RESET) 
		{
			prvvUARTTxReadyISR();//发送中断
		}
	
  HAL_NVIC_ClearPendingIRQ(USART3_IRQn);
#endif
	HAL_UART_IRQHandler(&huart3);

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */
	if(__HAL_TIM_GET_FLAG(&htim5, TIM_FLAG_UPDATE))			// 更新中断标记被置位
	{
			__HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_UPDATE);		// 清除中断标记
			prvvTIMERExpiredISR();								// 通知modbus3.5个字符等待时间到
	}
  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */
#if MODBUS_USART == 4
	if(__HAL_UART_GET_IT_SOURCE(&huart4, UART_IT_RXNE)!= RESET) 
		{
			prvvUARTRxISR();//接收中断
		}

	if(__HAL_UART_GET_IT_SOURCE(&huart4, UART_IT_TXE)!= RESET) 
		{
			prvvUARTTxReadyISR();//发送中断
		}
	
  HAL_NVIC_ClearPendingIRQ(UART4_IRQn);
#elif (DEBUG_USART == 4)
#ifdef UART_DEBUG_SEND_CTRL
	uint8_t temp=0;
	if((__HAL_UART_GET_FLAG(&huart4,UART_FLAG_IDLE) != RESET))//如果是接收完成中断，idle标志被置位
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart4);//清除标志位
		HAL_UART_DMAStop(&huart4); //  停止DMA传输，防止
		temp  =  __HAL_DMA_GET_COUNTER(huart4.hdmarx);// 获取DMA中未传输的数据个数   
		To_Debug_Protocol_Analysis(DEBUG_PROTOCOL_RX_MAX - temp);
		
		memset(Debug_Read_Buffer,0,DEBUG_PROTOCOL_RX_MAX);    				//清空缓存区
		HAL_UARTEx_ReceiveToIdle_DMA(&huart4, Debug_Read_Buffer, DEBUG_PROTOCOL_RX_MAX); // 接收完毕后重启
		__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);		   // 手动关闭DMA_IT_HT中断
		
	 }
  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN UART4_IRQn 1 */
#endif
#endif
  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
  /* USER CODE BEGIN UART5_IRQn 0 */
	if((UART5->SR&UART_FLAG_RXNE) != 0)
	{
		//Res=USART2->DR;
			BT_Read_Data_Bit(UART5->DR);
	}
  /* USER CODE END UART5_IRQn 0 */
  //HAL_UART_IRQHandler(&huart5);
  /* USER CODE BEGIN UART5_IRQn 1 */
  /* USER CODE END UART5_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/**
  * @brief This function handles DMA2 channel3 global interrupt.
  */
void DMA2_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel3_IRQn 0 */

  /* USER CODE END DMA2_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart4_rx);
  /* USER CODE BEGIN DMA2_Channel3_IRQn 1 */

  /* USER CODE END DMA2_Channel3_IRQn 1 */
}

/* USER CODE BEGIN 1 */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//定时器中断回调函数，用于连接porttimer.c文件的函数
//{
//  /* NOTE : This function Should not be modified, when the callback is needed,
//            the __HAL_TIM_PeriodElapsedCallback could be implemented in the user file
//   */
//  	  if (htim->Instance == TIM4)
//  {
//    TM1621_Buzzer_Off();
//  }

//}

/* USER CODE END 1 */
