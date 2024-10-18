/**
******************************************************************************
* @file    		macro_definition.h
* @brief   		ϵͳ�����궨��
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-8-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MACRO_DEFINITION_H__
#define __MACRO_DEFINITION_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "model_parameter.h"


/* Exported types ------------------------------------------------------------*/

// ��Ƶ ����
typedef enum 
{
	MOTOR_DOWN_CONVERSION_NO = 0,						//	��
	MOTOR_DOWN_CONVERSION_MOS_TEMPER,				//	mos ����
	MOTOR_DOWN_CONVERSION_BOX_TEMPER,				//	���� ����
	MOTOR_DOWN_CONVERSION_OUT_CURRENT,			//	��� ����
	MOTOR_DOWN_CONVERSION_OUT_POWER,				//	��� ����
} Motor_Down_Conversion_Type_enum;



/* Exported macro ------------------------------------------------------------*/


/*========================================== <main.h> macro ====================================================*/

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------

// ����汾
#define	MACRO_SOFTWARE_VERSION_UINT32									"0.0.4"

/**
******************************************************************************
* ϵͳ�궨��
******************************************************************************
*/

//#define SYSTEM_DEBUG_MODE								1	// ����ģʽ
//#define UART_PRINTF_LOG									1	// ��ӡ��־
//#define UART_DEBUG_SEND_CTRL						1	// ͨ�� ���Դ��� ����ָ��
//#define SYSTEM_LONG_RUNNING_MODE				1	// �ϻ�ģʽ
//*******************************************************

//******************  ������ �ͺ�ѡ�� **************************
#define MOTOR_DEVICE_HARDWARE_AQPED002					(1)			//	���� ��
#define MOTOR_DEVICE_HARDWARE_TEMP001						(2)			//	���� ��

//*******************************************************

#define MOTOR_DEVICE_PROTOCOL_VERSION						(MOTOR_DEVICE_HARDWARE_AQPED002)				// ����Э��汾

//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
/*==============================================================================================================*/
// ����1 --> �п�Modbus 	(485)
// ����2 --> wifi 			(ttl)
// ����3 --> ������ 			(ttl)
// ����4 --> ���� debug 	(ttl)
// ����5 --> ����				(ttl)

#define MACRO_MODBUS_USART								1
#define	MACRO_WIFI_USART									2
#define	MACRO_DRIVER_USART								3
#define	MACRO_DEBUG_USART									4
#define	MACRO_BLUETOOTH_USART							5

//��������
#define	MACRO_SYSTEM_USER_USART_MAX										(5)


#define MACRO_POWER_ON_WAITE_TIME_TASK								(4000)			//�ϵ�ȴ�ʱ�� ������ʾ�������� �����룩

/*==============================================================================================================*/
/*==============================================================================================================*/
	
/*========================================== <Breath_light.h> macro ============================================*/
/*==============================================================================================================*/
#define BREATH_LIGHT_THREAD_TURN_ON					1

#if(BREATH_LIGHT_THREAD_TURN_ON)
//�߳�����
#define BREATH_LIGHT_THREAD_LIFECYCLE				(20)				// ms
//��Ȧ pwm ͨ����
#define BREATH_LIGHT_PWM_CHANNEL						(TIM_CHANNEL_1)

//******************  ����ģʽ **************************
#ifdef SYSTEM_DEBUG_MODE
#define LIGHT_BRIGHTNESS_MIX					(0)			// �������  
#define LIGHT_BRIGHTNESS_MAX					(50)		// �������  0~500
#else
#define LIGHT_BRIGHTNESS_MIX					(0)			// �������  
#define LIGHT_BRIGHTNESS_MAX					(500)		//(*p_Breath_Light_Max)			// �������  0~500
#endif
//*******************************************************
//��λ
#define BREATH_LIGHT_GEAR_POSITION						(1000/BREATH_LIGHT_THREAD_LIFECYCLE)				// ��λ 50
//����
#define LIGHT_BRIGHTNESS_STEP									((LIGHT_BRIGHTNESS_MAX-LIGHT_BRIGHTNESS_MIX)/BREATH_LIGHT_GEAR_POSITION)

//---------- ��ͣ�� 5������
//��λ
#define BREATH_LIGHT_GEAR_POSITION_LOW				(2500/BREATH_LIGHT_THREAD_LIFECYCLE)				// ��λ 75
//����
#define LIGHT_BRIGHTNESS_STEP_LOW							((LIGHT_BRIGHTNESS_MAX-LIGHT_BRIGHTNESS_MIX)/BREATH_LIGHT_GEAR_POSITION_LOW)


#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <key.h> macro =====================================================*/
/*==============================================================================================================*/
#define KEY_THREAD_TURN_ON					1

#if(KEY_THREAD_TURN_ON)
//�߳�����
#define KEY_THREAD_LIFECYCLE								(20)	// ������������ 200ms

#define KEY_LONG_PRESS_TIME									(2000/KEY_THREAD_LIFECYCLE)			//����ʱ�� 2s
#define KEY_LONG_PRESS_TIME_SHORT						(1000/KEY_THREAD_LIFECYCLE)			//��һ��� ����ʱ��  1s
//-------------- ������ ���� -------------------
#define KEY_BUZZER_TIME								((200/KEY_THREAD_LIFECYCLE)-2)					//����  KEY_THREAD_LIFECYCLE ����
#define KEY_BUZZER_TIME_LONG					((400/KEY_THREAD_LIFECYCLE)-2)					//����  KEY_THREAD_LIFECYCLE ����
#define KEY_BUZZER_TIME_LONG_32				((800/KEY_THREAD_LIFECYCLE)-2)					//����  KEY_THREAD_LIFECYCLE ����
/* ������ ����  50���  ------------------------------------------------------------*/
//******************  ����ģʽ **************************
#ifdef SYSTEM_DEBUG_MODE
#define BUZZER_FREQUENCY					1
#else
#define BUZZER_FREQUENCY					50					// wuqingguang
#endif
//*******************************************************

#define KEY_VALUE_SHAKE_TIME					(1)		//ȥ���� ����

//��Ļ���� pwm ���� 
//#define LCD_BACK_LIGHT_PWM_CTRL							1

//��Ļ���� pwm ͨ����
#define LCD_BACK_LIGHT_PWM_CHANNEL						(TIM_CHANNEL_2)
//��Ļ���� �������
#define BACK_LIGHT_BRIGHTNESS_MAX						(500)			// �������  0~500
//����ʱ��(Ŀǰ��Ч)
#define KEY_FOR_SLEEP_TIME_SHORT						(3000/KEY_THREAD_LIFECYCLE)			//5 min  300000

// ����ٶ�ÿ�������� 5��
#define	KEY_SPEED_INCREASE_20_GEAR										(1)
// ����ٶ�ÿ��������	100��
#define	KEY_SPEED_INCREASE_100_GEAR										(20)


#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <timing.h> macro ==================================================*/
/*==============================================================================================================*/
#define TIMING_THREAD_TURN_ON					1

#if(TIMING_THREAD_TURN_ON)
#define TIMING_THREAD_LIFECYCLE				(24)				// ms

//����������
#define TIMING_THREAD_HALF_SECOND			(480/TIMING_THREAD_LIFECYCLE)				// 0.5 s   16
//1��������
#define TIMING_THREAD_ONE_SECOND			(2)				// 1 s
//******************  ����ģʽ **************************
#ifdef SYSTEM_DEBUG_MODE
//����ʱ��
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(60*TIMING_THREAD_ONE_SECOND)				// 6 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(60*TIMING_THREAD_ONE_SECOND)				// 6 s
//�����Իָ�
#define SYSTEM_FAULT_TIME_CALLOUT							(20*TIMING_THREAD_ONE_SECOND)				// 20 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 �ι���
#define SYSTEM_FAULT_RECOVERY_TIME						(60*TIMING_THREAD_ONE_SECOND)				// 1 ����  60 s
//�Զ��ػ�
#define AUTOMATIC_SHUTDOWN_TIME								(600*TIMING_THREAD_ONE_SECOND)				// 10 min

#else
//����ʱ��
#define WIFI_DISTRIBUTION_TIME_CALLOUT				(60*TIMING_THREAD_ONE_SECOND)				// 60 s
#define BT_DISTRIBUTION_TIME_CALLOUT					(60*TIMING_THREAD_ONE_SECOND)				// 60 s
//�����Իָ�
#define SYSTEM_FAULT_TIME_CALLOUT							(120*TIMING_THREAD_ONE_SECOND)				// 120 s
#define SYSTEM_FAULT_RECOVERY_MAX							(3)				// 3 �ι���
#define SYSTEM_FAULT_RECOVERY_TIME						(3600*TIMING_THREAD_ONE_SECOND)				// 1 Сʱ��  3600 s
//�Զ��ػ�
#define AUTOMATIC_SHUTDOWN_TIME								(1800*TIMING_THREAD_ONE_SECOND)				// 0.5 Сʱ��  1800 s

#endif


//-------------------------------------------------------------------------------------------------
//*********************************************************************************************
//-------------- �����¶ȱ���ֵ 90��C -------------------
#define AMBIENT_TEMP_ALARM_VALUE									(80)
//-------------- �����¶� ���� 70��C -------------------
#define AMBIENT_TEMP_REDUCE_SPEED									(70)		// ���� �¶�
#define AMBIENT_TEMP_RESTORE_SPEED								(65)		// �ָ� �¶�
//*********************************************************************************************
//-------------------------------------------------------------------------------------------------

//-------------- ���ټ��ʱ�� -------------------
#define TIME_SLOW_DOWN_TIME													(120)		//2 min  120*2 ������,���߳��������
//-------------- ���� ��λ -------------------
#define TIME_SLOW_DOWN_SPEED_01											(10)		//��һ�� ����
#define TIME_SLOW_DOWN_SPEED_02											(5)			//�ڶ��� ����
//-------------- ���� ����ٶ� -------------------
#define TIME_SLOW_DOWN_SPEED_MIX										(20)		//��ͽ��� 20%
#define TIME_SLOW_DOWN_SPEED_MAX										(100)		//�ָ��ٶ���߻ָ��� 100%

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


/*========================================== <modbus.h> macro ==================================================*/
/*==============================================================================================================*/
#define MODBUS_THREAD_TURN_ON					1

#if(MODBUS_THREAD_TURN_ON)
#define MODBUS_THREAD_LIFECYCLE							(40)				// ms ��ʱ����

//1��������
#define MODBUS_THREAD_ONE_SECOND						(1000/MODBUS_THREAD_LIFECYCLE)				// 1 s

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/



/*========================================== <motor.h> macro ===================================================*/
/*==============================================================================================================*/
#define MOTOR_THREAD_TURN_ON					1

#if(MOTOR_THREAD_TURN_ON)

#define MOTOR_THREAD_LIFECYCLE						(20)				// ������������ 50ms

// ���� ���� 200ms 
#define MOTOR_POLLING_PERIOD							(200/MOTOR_THREAD_LIFECYCLE)

// ���� ���� 200ms 
#define MOTOR_HEARTBEAT_CYCLE							(MOTOR_POLLING_PERIOD/3)
// ���� ���� 200ms
#define MOTOR_COMMAND_CYCLE								((MOTOR_POLLING_PERIOD/3)*2)
// ��״̬ ���� 200ms 
#define MOTOR_READ_STATIC_CYCLE						(0)


// ����״̬����   ������� ����ʱ��  ------------------
//#define MOTOR_CANNOT_START_TIME						(5000 / MOTOR_POLLING_PERIOD / MOTOR_THREAD_LIFECYCLE)		// wuqingguang
// ����״̬����   ���ת�� ����ʱ��  ------------------
//#define MOTOR_SPEED_ERROR_TIME						(5000 / MOTOR_POLLING_PERIOD/ MOTOR_THREAD_LIFECYCLE)			// wuqingguang


//*****************************************************************************
//�������
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
#define	MOTOR_POLE_NUMBER									(5)
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
#define	MOTOR_POLE_NUMBER									(1)
#endif
//*****************************************************************************

// 700  1012   1324  1637   1950
//ÿ 1% ת��
#define	MOTOR_RPM_CONVERSION_COEFFICIENT				((MOTOR_RPM_SPEED_MAX - MOTOR_RPM_SPEED_MIX) /80)			//15.6			78

//ÿ 20% ת��
#define	MOTOR_RPM_CONVERSION_COEFFICIENT_20				((MOTOR_RPM_SPEED_MAX - MOTOR_RPM_SPEED_MIX) /4)			//312.5     1562

//�������ٶ�  �ٷֱ�  100%
#define	MOTOR_PERCENT_SPEED_MAX										(100)
//�������ٶ�  �ٷֱ�  20%
#define	MOTOR_PERCENT_SPEED_MIX										(20)

#ifdef MOTOR_CANNOT_START_TIME
//�����͵���
#define	MOTOR_CURRENT_MIX									(1000)				//10A
#endif

#ifdef MOTOR_SPEED_ERROR_TIME
//���ת����Χ
#define	MOTOR_SPEED_VIBRATION_RANGE					(100*MOTOR_POLE_NUMBER)				//���Ե������
#endif

//������ٶ�
#define	MOTOR_ACCELERATION										(1)

//-------------------------------------------------------------------------------------------------
//*********************************************************************************************
//-------------- MOS �¶ȱ���ֵ 90��C -------------------
#define MOS_TEMP_ALARM_VALUE								(90)
//-------------- MOS �¶� ���� 80��C -------------------
#define MOS_TEMP_REDUCE_SPEED								(80)		// ���� �¶�
#define MOS_TEMP_RESTORE_SPEED							(75)		// �ָ� �¶�
//*********************************************************************************************
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//*********************************************************************************************

//-------------------------------------------------------------------------------------------------

//ͨѶ���� ����ʱ��
#define FAULT_MOTOR_LOSS_TIME							(30000/(MOTOR_THREAD_LIFECYCLE))				// 30 ��
//ͨѶ���� �������� ʱ��
#define FAULT_MOTOR_TRY_RESTAR_TIME				(FAULT_MOTOR_LOSS_TIME/5)								// 6��

/*------------------- IO define ----------------------------------------------*/
#define	MOTOR_MODULE_HUART				DRIVER_USART		//

#if (MOTOR_MODULE_HUART == 1)
#define MOTOR_RS485_TX_EN_PORT		RS48501_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48501_RE_Pin
#elif (MOTOR_MODULE_HUART == 4)
#define MOTOR_RS485_TX_EN_PORT		RS48504_RE_GPIO_Port
#define MOTOR_RS485_TX_EN_PIN			RS48504_RE_Pin
#endif

#endif
/*==============================================================================================================*/
/*==============================================================================================================*/



/*========================================== <wifi_thread.h> macro ==================================================*/
/*==============================================================================================================*/
#define WIFI_THREAD_TURN_ON					1

#if(WIFI_THREAD_TURN_ON)
#define WIFI_THREAD_LIFECYCLE											(40)				// ms ��ʱ����

//1��������
#define WIFI_THREAD_ONE_SECOND									(1000/WIFI_THREAD_LIFECYCLE)				// 1 s

#define OTA_SHUTDOWN_TIME_OUT										(1800)				//  s

#define WIFI_DATE_UPLOAD_TIME_NORMAL							(1 * WIFI_THREAD_ONE_SECOND)				// ��ͨ���� ʱ�� 1s

#ifdef SYSTEM_LONG_RUNNING_MODE
//********* �ϻ���װ ***********************************************
// �ϻ���װ �����ϱ� ����鿴����״̬
#define WIFI_DATE_UPLOAD_TIME											(1 * WIFI_THREAD_ONE_SECOND)				// 1s
#else
#define WIFI_DATE_UPLOAD_TIME											(10 * WIFI_THREAD_ONE_SECOND)				// ���������� ʱ�� 10s
//******************************************************************
#endif

//wifi ���� �ж�ֵ
#define WIFI_RSSI_ERROR_VAULE										(50)

// ����ͳ�� �ϴ� ���ʱ�� -- ���� 3min ������ͳ��  ��λ(��)
#define WIFI_STATISTICE_UPLOAD_MINIMUM_TIME										(180)


#endif
/*==============================================================================================================*/
/*==============================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* __MACRO_DEFINITION_H__ */

