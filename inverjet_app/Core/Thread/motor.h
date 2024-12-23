/**
******************************************************************************
* @file    		motor.h
* @brief   		��� ���Э��  ����ת������ 200ms
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-6-15
******************************************************************************
*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_H__
#define __MOTOR_H__


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "usart.h"
//#include "display.h"			// ��ʾģ��
//#include "port.h"
//#include "mbcrc.h"				// crc
#include "macro_definition.h"				// ͳһ�궨��
/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/



/* Exported macro ------------------------------------------------------------*/
#ifndef __MACRO_DEFINITION_H__

#define MOTOR_THREAD_LIFECYCLE								50				// ������������ 50ms

// ���� ���� 500s 
#define MOTOR_HEARTBEAT_CYCLE							(500/(MOTOR_THREAD_LIFECYCLE))				// 500 ����
// ���� ���� 50 ms
#define MOTOR_COMMAND_CYCLE								(200/(MOTOR_THREAD_LIFECYCLE))				// 5��: 50 ����     20�� : 200ms
// ��״̬ ���� 1s 
#define MOTOR_READ_STATIC_CYCLE						(1000/(MOTOR_THREAD_LIFECYCLE))				// 1 ��

//�������
#define	MOTOR_POLE_NUMBER				(5)

//���ת�� 100%
#define	MOTOR_RPM_SPEED_MAX				(1950*MOTOR_POLE_NUMBER)		//9750
//���ת�� 100%
#define	MOTOR_RPM_SPEED_MIX				(700*MOTOR_POLE_NUMBER)		//3500

// 700  1012   1324  1636   1948
//ÿ 1% ת�� 
#define	MOTOR_RPM_CONVERSION_COEFFICIENT				((MOTOR_RPM_SPEED_MAX-MOTOR_RPM_SPEED_MIX)/80)			//15.6

//ÿ 20% ת�� 
#define	MOTOR_RPM_CONVERSION_COEFFICIENT_20				((MOTOR_RPM_SPEED_MAX-MOTOR_RPM_SPEED_MIX)/4)			//312.5     


//-------------- MOS �¶ȱ���ֵ 90��C -------------------
#define MOS_TEMP_ALARM_VALUE								90
//-------------- MOS �¶� ���� 80��C -------------------
#define MOS_TEMP_REDUCE_SPEED								80		// ���� �¶�
#define MOS_TEMP_RESTORE_SPEED							75		// �ָ� �¶�
 
//ͨѶ���� ����ʱ��
#define FAULT_MOTOR_LOSS_TIME						(30000/(MOTOR_THREAD_LIFECYCLE))				// 30 ��


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
#if (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_AQPED002)
//����������������������������������������������������������������������������������������������������
// ��������С
#define MOTOR_RS485_TX_BUFF_SIZE			16
#define MOTOR_RS485_RX_BUFF_SIZE			256


//��������� ��־λ
#define MOTOR_FAULT_SIGN_BIT						(E102_TEMPERATURE_AMBIENT | E205_WIFI_HARDWARE | E206_BT_HARDWARE | E207_RS485_HARDWARE)
#define CLEAN_MOTOR_FAULT(n)						(n &= MOTOR_FAULT_SIGN_BIT)

#define	MOTOR_PROTOCOL_HEADER_OFFSET						3
#define	MOTOR_PROTOCOL_ADDR_MAX									73

#define	MOTOR_ADDR_MOSFET_TEMP_OFFSET						0
#define	MOTOR_ADDR_MOTOR_TEMP_OFFSET						2
#define	MOTOR_ADDR_MOTOR_CURRENT_OFFSET					4
#define	MOTOR_ADDR_MOTOR_SPEED_OFFSET						22
#define	MOTOR_ADDR_BUS_VOLTAGE_OFFSET						26

#define	MOTOR_ADDR_MOTOR_FAULT_OFFSET						52
#define	MOTOR_ADDR_NTC1_TEMP_OFFSET							58
#define	MOTOR_ADDR_NTC2_TEMP_OFFSET							60
#define	MOTOR_ADDR_NTC3_TEMP_OFFSET							62


//======= ������ϴ��� ================================================================================
#define	MOTOR_FAULT_NONE																									0						//
#define	MOTOR_FAULT_OVER_VOLTAGE																					1						//	�����ѹС�� ��С����ֵ��5%
#define	MOTOR_FAULT_UNDER_VOLTAGE																					2						//	�����ѹ���� �������ֵ��5%
#define	MOTOR_FAULT_DRV																										3						//	Ӳ����·����
#define	MOTOR_FAULT_ABS_OVER_CURRENT																			4						//	SQRT(iq*iq+id*id)����������
#define	MOTOR_FAULT_OVER_TEMP_FET																					5						//	MOSFET�¶ȴ�����������¶�-0.1ʱ�����¹���
#define	MOTOR_FAULT_OVER_TEMP_MOTOR																				6						//	xxx
#define	MOTOR_FAULT_GATE_DRIVER_OVER_VLOTAGE															7						//	xxx
#define	MOTOR_FAULT_GATE_DRIVER_UNDER_VLOTAGE															8						//	xxx
#define	MOTOR_FAULT_MCU_UNDER_VLOTAGE																			9						//	 mcu��ѹ���ȹ���
#define	MOTOR_FAULT_BOOTING_FROM_WATCHDOG_RESET														10					//	�����������Ź���λ����
#define	MOTOR_FAULT_ENCODER_SPI																						11					//	xxx
#define	MOTOR_FAULT_ENCODER_SINCOS_BELOW_MIN_AMPLITUDE										12					//	xxx
#define	MOTOR_FAULT_ENCODER_SINCOS_ABOVE_MAN_AMPLITUDE										13					//	xxx
#define	MOTOR_FAULT_FLASH_CORRUPTION																			14					//	lash�𻵣������洢����
#define	MOTOR_FAULT_HIGH_OFFSET_CURRENT_SENSOR_1													15					//	1.65+/-0.5V��ƫ��
#define	MOTOR_FAULT_HIGH_OFFSET_CURRENT_SENSOR_2													16					//	1.65+/-0.5V��ƫ��
#define	MOTOR_FAULT_HIGH_OFFSET_CURRENT_SENSOR_3													17					//	1.65+/-0.5V��ƫ��
#define	MOTOR_FAULT_UNBALANCED_CURRENTS																		18					//	���������ƽ�����
#define	MOTOR_FAULT_BRK																										19					//	xxx
#define	MOTOR_FAULT_RESOLVER_LOT																					20					//	xxx
#define	MOTOR_FAULT_RESOLVER_DOS																					21					//	xxx
#define	MOTOR_FAULT_RESOLVER_LOS																					22					//	xxx
#define	MOTOR_FAULT_FLASH_CURRUPTION_APP_CFG															23					//	Ӧ�ò���flashУ�����-����Ĭ�ϲ�������
#define	MOTOR_FAULT_FLASH_CURRUPTION_MC_CFG																24					//	�������flashУ�����-����Ĭ�ϲ�������
#define	MOTOR_FAULT_ENCODER_NO_MAGNET																			25					//	xxx
#define	MOTOR_FAULT_ENCODER_MAGNET_TOO_STRONG															26					//	xxx


#define	MOTOR_FAULT_OUTPUT_PHASE_A_SENSOR																	27					//	A���ѹ��������
#define	MOTOR_FAULT_OUTPUT_PHASE_B_SENSOR																	28					//	B���ѹ��������
#define	MOTOR_FAULT_OUTPUT_PHASE_C_SENSOR																	29					//	C���ѹ��������
#define	MOTOR_FAULT_OUTPUT_PHASE_A_LOSS_POWER_ON													30					//	A���ϵ�ȱ��
#define	MOTOR_FAULT_OUTPUT_PHASE_B_LOSS_POWER_ON													31					//	B���ϵ�ȱ��
#define	MOTOR_FAULT_OUTPUT_PHASE_C_LOSS_POWER_ON													32					//	C���ϵ�ȱ��
#define	MOTOR_FAULT_OUTPUT_PHASE_2_AND_3_LOSS_POWER_ON										33					//	�ϵ�ȱ2/3��
#define	MOTOR_FAULT_OUTPUT_PHASE_A_LOSS_RUNNING														34					//	������ ȱA��
#define	MOTOR_FAULT_OUTPUT_PHASE_B_LOSS_RUNNING														35					//	������ ȱA��
#define	MOTOR_FAULT_OUTPUT_PHASE_C_LOSS_RUNNING														36					//	������ ȱA��
#define	MOTOR_FAULT_OUTPUT_PHASE_2_AND3_LOSS_RUNNING											37					//	������ ȱ2/3��
#define	MOTOR_FAULT_OUTPUT_LOCKROTOR																			38					//	��ת

#define	MOTOR_FAULT_MOSFET_NTC_ERR																				39					//	Mos�¶ȴ���������


#define	MOTOR_FAULT_CODE_START							(MOTOR_FAULT_OVER_VOLTAGE)
#define	MOTOR_FAULT_CODE_END								(MOTOR_FAULT_MOSFET_NTC_ERR)
//====================================================================================================

//����������������������������������������������������������������������������������������������������
#elif (MOTOR_DEVICE_PROTOCOL_VERSION == MOTOR_DEVICE_HARDWARE_TEMP001)
//����������������������������������������������������������������������������������������������������
// ��������С
#define MOTOR_RS485_TX_BUFF_SIZE			16
#define MOTOR_RS485_RX_BUFF_SIZE			32


//��������� ��־λ
#define MOTOR_FAULT_SIGN_BIT						E102_TEMPERATURE_AMBIENT
#define CLEAN_MOTOR_FAULT(n)						(n &= ~MOTOR_FAULT_SIGN_BIT)

#define	MOTOR_PROTOCOL_HEADER_OFFSET						1
#define	MOTOR_PROTOCOL_ADDR_MAX									11

#define	MOTOR_ADDR_MOTOR_SPEED_OFFSET							0
#define	MOTOR_ADDR_MOTOR_VERSION_OFFSET						1
#define	MOTOR_ADDR_BUS_VOLTAGE_OFFSET							2
#define	MOTOR_ADDR_BUS_CURRENT_OFFSET							3
#define	MOTOR_ADDR_MOTOR_CURRENT_OFFSET						4
#define	MOTOR_ADDR_MOSFET_TEMP_NUM_OFFSET					5
#define	MOTOR_ADDR_MOSFET_TEMP_OFFSET							6

#define	MOTOR_ADDR_MOTOR_POWER_OFFSET							7
#define	MOTOR_ADDR_MOTOR_FAULT_OFFSET							9



#define MOTOR_FAULT_BUS_VOLTAGE_UNDER					0x1
#define MOTOR_FAULT_BUS_CURRENT_OVER					0x2
#define MOTOR_FAULT_SAMPLE_ERROR							0x4
#define MOTOR_FAULT_HARDWARE_OVERCURRENT			0x8
#define MOTOR_FAULT_OUT_STEP_FAULT						0x10
#define MOTOR_FAULT_STARTUP_FAILED						0x20
#define MOTOR_FAULT_TIME_OUT									0x40
#define MOTOR_FAULT_CRC_ERROR									0x80
#define MOTOR_FAULT_OUTPUT_OVERCURRENT				0x100
#define MOTOR_FAULT_LACK_PHASE								0x200

//����������������������������������������������������������������������������������������������������
#endif
/* Exported functions prototypes ---------------------------------------------*/
void Metering_Receive_Init(void);
// ����
void Motor_Usart_Restar(void);
// �������
void Clean_Motor_OffLine_Timer(void);
//------------------- ��ѭ������  ----------------------------
void App_Motor_Handler(void);
	
//------------------- ���ת�ٸ��� ----------------------------
extern uint8_t Motor_Speed_Update(void);
//------------------- ���ת���Ƿ�ﵽĿ��ֵ ----------------------------
extern uint8_t Motor_Speed_Is_Reach(void);
//------------------- ���ת�� Ŀ��ֵ ���� ----------------------------
extern void Motor_Speed_Target_Set(uint8_t speed);
//------------------- ��� ����ֹͣ ----------------------------
extern void Motor_Quick_Stop(void);
//------------------- ���ת�� Ŀ��ֵ ���� ----------------------------
extern uint8_t Motor_Speed_Target_Get(void);
//------------------- �ٷֱ� ת ת�� ----------------------------
extern uint32_t Motor_Speed_To_Rpm(uint8_t speed);
//------------------- ת�� ת �ٷֱ� ----------------------------
extern uint8_t Motor_Rpm_To_Speed(uint32_t speed_rpm);

//================================================== �ڲ����ýӿ�
//-------------------- ����״̬����   ���ת�� ----------------------------
extern void Drive_Status_Inspection_Motor_Speed(void);
//-------------------- ����״̬����   ������� ----------------------------
extern void Drive_Status_Inspection_Motor_Current(void);

//-------------------- ���½���  mos ----------------------------
extern void Check_Down_Conversion_MOS_Temperature(short int Temperature);
//-------------------- ���ʽ�Ƶ   ----------------------------
extern void Check_Down_Conversion_Motor_Power(uint16_t power);
//-------------------- ������Ƶ   ----------------------------
extern void Check_Down_Conversion_Motor_Current(uint32_t Current);
//-------------------- ��Ƶ ״̬�ָ�   ----------------------------
extern void Down_Conversion_State_Clea(void);

//-------------------- ��ȡ�������״̬ ----------------------------
uint16_t Get_Motor_Fault_State(void);

//-------------------- ���õ������״̬ ----------------------------
void Set_Motor_Fault_State(uint16_t fault_bit);
//-------------------- ����������״̬ ----------------------------
void ReSet_Motor_Fault_State(uint16_t fault_bit);
//-------------------- ���ع��� ----------------------------
uint8_t Motor_Is_Serious_Fault(uint16_t fault_bit);
//-------------------- һ����� ----------------------------
uint8_t Motor_Is_Ordinary_Fault(uint16_t fault_bit);
//-------------------- ָ������ ----------------------------
uint8_t Motor_Is_Specified_Fault(uint16_t fault_bit, uint16_t specified_bit);

//------------------- ���� ----------------------------
extern void Motor_Speed_Send(uint32_t speed_rpm);
//------------------- ���� ----------------------------
extern void Motor_Heartbeat_Send(void);
//-------------------- ���Ĵ��� ----------------------------
extern void Motor_Read_Register(void);

/*-------------------- �շ����� ----------------------------------------------*/
void Motor_State_Analysis(void);
//-------------------- ���� ----------------------------
extern void Motor_UART_Send(uint8_t* p_buff, uint8_t len);
//-------------------- ���� ----------------------------
extern void Motor_RxData(uint8_t len);

//-------------------- ��������� ----------------------------
extern uint8_t Check_Motor_Current(void);
//-------------------- �����ת�� ----------------------------
extern uint8_t Check_Motor_Speed(void);


/* Private defines -----------------------------------------------------------*/

extern uint8_t Motor_Speed_Now;			// ���ת�� 

//**************** �շ�������
extern uint8_t Motor_DMABuff[MOTOR_RS485_RX_BUFF_SIZE];//����һ�����ջ�����
extern uint8_t Motor_TxBuff[MOTOR_RS485_TX_BUFF_SIZE];//����һ�����ͻ�����


extern DMA_HandleTypeDef hdma_usart3_rx;


#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */

