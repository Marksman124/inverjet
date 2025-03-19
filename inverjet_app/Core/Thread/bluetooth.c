/**
******************************************************************************
* @file				bluetooth.c
* @brief			bluetooth 模组
*
* @author			WQG
* @versions		v1.0
* @date				2024-6-15
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "bluetooth.h"
#include "my_modbus.h"
#include "modbus.h"
#include <stdio.h>
#include "fault.h"
#include "debug_protocol.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

static BT_STATE_MODE_E BT_State_Machine = BT_NO_CONNECT;
//串口接收
uint8_t BT_Uart_Read_Buffer;
//声明一个对
ModbusSlaveObj_t Ms_BT_Modbus;

//串口接对象
uint8_t BT_Read_Buffer_For_Test[64]={0};
uint16_t BT_Read_Cnt_For_Test=0;

static uint16_t BT_Halder_cnt = 0;


/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/
//串口发送接口
void SerialWrite(unsigned char *buff,int length)
{
	HAL_UART_Transmit(&huart5,buff,length,200);
}

//-------------------- 发送----------------------------
void BT_UART_Send(uint8_t* p_buff, uint8_t len)
{
	HAL_UART_Transmit(&huart5, p_buff, len, 200);
}
//串口接收
void BT_Read_Data_Bit(unsigned char vaule)
{
	//+EVENT:BLE_CONNECTED
	//+EVENT:BLE_DISCONNECT
	//char* disconnect = "+EVENT:BLE_DISCONNECT";
	//char* rssi = "MAC:123456789000\r\nrssi:-40\r\n";
//	char rssi[24] = {0x4D, 0x41, 0x43, 0x3A, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x30, 0x30, 0x0D, 0x0A, 0x72, 0x73, 0x73, 0x69, 0x3A, 0x2D};
	//MsSerialRead(&Ms_BT_Modbus,&vaule,1);
	if(IS_CHECK_ERROR_MODE())
	{
		if(vaule == '>')
		{
			*p_BLE_Rssi = 50;
		}
	}
	else
	{
		if(Ms_BT_Modbus.rxWriteLock)
		{
				return;
		}

		Ms_BT_Modbus.rxTimerEnable = 1;
		Ms_BT_Modbus.rxTimerCnt = 0;

		Ms_BT_Modbus.rxBuff[Ms_BT_Modbus.rxWriteIdx] =vaule;
		if(Ms_BT_Modbus.rxWriteIdx < (MODBUS_SLAVE_TX_RX_MAX_LEN - 1))
		{
				Ms_BT_Modbus.rxWriteIdx++;
		}
		else
		{
			Ms_BT_Modbus.rxWriteIdx = 0;
		}
	}
		
}

//中断调用
void Usart_IRQ_CallBack(uint8_t data)
{
	//static uint8_t buffer[8] = {0};
	//static uint8_t buffer_cnt=0;
	
	if(IS_CHECK_ERROR_MODE())
	{
		if(data == '>')
		{
			*p_BLE_Rssi = 50;
		}
	}
	else
	{
		/*if(BLE_Pair_Finish_Now == 1)
		{
			if(buffer[0] != 0x15)
			{
				buffer_cnt = 0;
			}
			if(buffer_cnt < 8)
				buffer[buffer_cnt++] = data;
			
			if(buffer_cnt >= 8)
			{
				MsSerialRead(&Ms_BT_Modbus,buffer,8);
				buffer_cnt = 0;
			}
		}
		else*/
		{
			BT_Uart_Read_Buffer = data;
			MsSerialRead(&Ms_BT_Modbus,&BT_Uart_Read_Buffer,1);
		}
	}
}


// AT 指令 设置 MAC
void BT_Set_MAC(void)
{
	char buff[32]={"AT+BLEMAC=123456789012\r\n"};
	
	//sprintf(buff,"AT+BLENAME=inverjet\r\n");
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(1000);
}

// AT 指令 设置名称  需重启
void BT_Set_Name(void)
{
	char buff_1[32]={"AT+BLENAME=inverjet\r\n"};
	char buff_0[32]={"AT+BLENAME=Anjie_Ble\r\n"};
	
	if(*p_BLE_Pair_Finish == 0xA5)
		SerialWrite((uint8_t*)buff_0,strlen(buff_0));
	else
		SerialWrite((uint8_t*)buff_1,strlen(buff_1));
	
	osDelay(1000);
}

// AT 指令 设置从机模式
void BT_Set_Mode(uint8_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEMODE=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(3000);
}

// AT 指令 开/关 广播
void BT_Set_Adven(uint8_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEADVEN=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(1000);
}

// AT 指令 设置 MTU
void BT_Set_MTU(uint8_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEMTU=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(1000);
}
// AT 指令 设置 功率
void BT_Set_Power(uint8_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLERFPWR=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(1000);
}
// AT 指令 设置广播间隔  ADVINTV
void BT_Set_Advintv(uint16_t data)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+ADVINTV=%d\r\n",data);
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(1000);
}
// AT 指令 设置配对码 AT+BLEAUTH=123456
void BT_Set_Auth(void)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEAUTH=123456\r\n");
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(1000);
}
// AT 指令 进入透传
void BT_Set_TRANSENTER(uint8_t data)
{
	char buff[32]={0};
	
	if(data == 0)//退出
	{
		SerialWrite((uint8_t*)"+++",3);
	}
	else
	{
		sprintf(buff,"AT+TRANSENTER\r\n");
	
		SerialWrite((uint8_t*)buff,strlen(buff));
	}
	
	osDelay(1000);
}
// AT 指令 开启扫描
void BT_Scan_Server(void)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLESCAN\r\n");
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(5000);
}

// AT 指令 模组 重启
void BT_Restar(void)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+RST\r\n");
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(3000);
}

// AT 指令 恢复出厂
void BT_Restore_Factory(void)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+RESTORE\r\n");
	
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(5000);
}

// AT 指令 连接 工装
void BT_Connect_TestServer(void)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLECONNECT=123456789000\r\n");
	//sprintf(buff,"AT+BLECONNECT=123456789876\r\n");
	//sprintf(buff,"AT+BLECONNECT=94c960ea4b2b\r\n");
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(3000);
}


// AT 指令 退出连接
void BT_Out_Connect(void)
{
	char buff[32]={0};
	
	sprintf(buff,"AT+BLEDISCON\r\n");
	SerialWrite((uint8_t*)buff,strlen(buff));
	
	osDelay(2000);
}

// 同步状态机
//void BT_State_Machine_Update(void)
//{
//	uint8_t buff[32]={0x15, 0x10, 0x00, 0x21, 0x00, 0x04, 0x08};
//	uint16_t crc_calculate;
//	//memcpy(&buff[7],p_System_State_Machine, 8 );

//	buff[7] = *p_PMode_Now >> 8;
//	buff[8] = *p_PMode_Now & 0xFF;
//	
//	buff[9] = *p_System_State_Machine >> 8;
//	buff[10] = *p_System_State_Machine & 0xFF;
//	
//	buff[11] = *p_OP_ShowNow_Speed >> 8;
//	buff[12] = *p_OP_ShowNow_Speed & 0xFF;
//	
//	buff[13] = *p_OP_ShowNow_Time >> 8;
//	buff[14] = *p_OP_ShowNow_Time & 0xFF;
//	
//	
//	crc_calculate = usMBCRC16( buff, 15);
//	buff[15] = (crc_calculate & 0xFF);
//	buff[16] = (crc_calculate >> 8);

//	BT_UART_Send(buff,17);
//}

void BT_Module_AT_Factory(void)
{
	BT_Set_TRANSENTER(0);
	BT_Out_Connect();
	BT_Restore_Factory();
}

// 上电初始化
void BT_Module_AT_Init(void)
{
	BT_Set_TRANSENTER(0);
	BT_Out_Connect();

	BT_Set_Name();
	BT_Set_Mode(0);
	BT_Set_Power(10);
	
	//重启
	BT_Restar();
	BT_Set_Advintv(160);
	BT_Set_MTU(243);
	//BT_Set_Auth();
	
	if(*p_BLE_Pair_Finish == 0xA5)
		BT_Set_Adven(1);
	else
		BT_Set_Adven(0);
	//BT_Set_TRANSENTER(1);// 进入透传
}

// 连接后断开
void BT_Module_AT_Disconnect(void)
{
	BT_Set_Name();
	
	//重启
	BT_Restar();
	BT_Set_Advintv(160);
	BT_Set_MTU(243);
	
	if(*p_BLE_Pair_Finish == 0xA5)
		BT_Set_Adven(1);
	else
		BT_Set_Adven(0);
	
}

// 配网
void BT_Module_AT_ReInit(void)
{
	BT_Set_TRANSENTER(0);
	BT_Out_Connect();
	
	//BT_Restore_Factory();
	*p_BLE_Pair_Finish=0;
	BLE_Pair_Finish_Now = 0;
	
	BT_Set_Name();
	Write_MbBuffer_Now();//
	//BT_Set_MAC();
	BT_Set_Mode(0);
	BT_Set_Power(10);
	//重启
	BT_Restar();
	BT_Set_Advintv(160);
	BT_Set_MTU(243);
	//BT_Set_Auth();//配对码
	//BT_Set_Adven(1);//开广播
}

void BT_Module_AT_NoConnect(void)
{
	//BT_Set_TRANSENTER(0);
	//BT_Out_Connect();
	
	BT_Set_Adven(0);
	//重启
	//BT_Restar();
}

//------------------- 蓝牙 进入工装 ----------------------------
void BT_Module_AT_InTest(void)
{
	BT_Set_TRANSENTER(0);
	BT_Set_Mode(1);
	
	BT_Connect_TestServer();
	//BT_Scan_Server();
	if(*p_BLE_Rssi == 0)//
	{
		BT_Connect_TestServer();
		//BT_Scan_Server();
	}
	BT_Set_TRANSENTER(0);
	BT_Out_Connect();
}


//------------------- 蓝牙 持续测试 ----------------------------
void BT_Module_AT_DoTest(void)
{
	BT_Connect_TestServer();
}

//------------------- 蓝牙 Modbus 配置初始化----------------------------
void BT_Modbus_Config_Init(void)
{
	//初始化
	MsInit(&Ms_BT_Modbus,21,2,SerialWrite);
	//设置01寄存器的参数,
	//MsConfigureRegister(&Ms_BT_Modbus,0x01,buff01,sizeof(buff01));
	//MsConfigureRegister(&Ms_BT_Modbus,0x0F,buff01,sizeof(buff01));
	MsConfigureRegister(&Ms_BT_Modbus,0x03,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x04,Get_DataAddr_Pointer(MB_FUNC_READ_INPUT_REGISTER,0),REG_INPUT_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x06,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x10,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	MsConfigureRegister(&Ms_BT_Modbus,0x05,Get_DataAddr_Pointer(MB_FUNC_READ_HOLDING_REGISTER,0),REG_HOLDING_NREGS);
	
}
/*******************************************************************************
*功能：超时计数，放到定时器中运行，一般放�?ms中断足够满足要求
*/
void BT_MsTimeout(void)
{
	MsTimeout(&Ms_BT_Modbus);
}

//------------------- 设置wifi状态机 ----------------------------
void BT_Set_Machine_State(BT_STATE_MODE_E para)
{
	if(para <= BT_ERROR)
	{
		BT_State_Machine = para;
	}
}
	
//------------------- 获取wifi状态机 ----------------------------
BT_STATE_MODE_E BT_Get_Machine_State(void)
{
	return BT_State_Machine;
}


//------------------- 接收处理函数 ----------------------------
void BT_Read_Handler(void)
{
	static uint8_t self_test_cnt=0;
	
	if(IS_CHECK_ERROR_MODE())
	{
		if(self_test_cnt == 0)
		{
			BT_Module_AT_InTest();
			self_test_cnt = 1;
		}
	}
	else
	{
		if(self_test_cnt != 0)
		{
			self_test_cnt = 0;
			BT_Module_AT_Init();
		}
		MsProcess(&Ms_BT_Modbus);
	}
}

//------------------- 进入配网 ----------------------------
void BT_Get_In_Distribution(void)
{
	BT_Set_Machine_State( BT_DISTRIBUTION );
	
	BT_Halder_cnt = 0;
	BT_Module_AT_ReInit();
}

//------------------- 进入故障 ----------------------------
void BT_Get_In_Error(void)
{
	BT_Set_Machine_State( BT_ERROR );
	
}

//------------------- 配网处理 0.5秒进一次 ----------------------------
void BT_Distribution_Halder(void)
{
	if(BT_Halder_cnt == 0)
		SerialWrite((uint8_t*)"+++",3);
	else if(BT_Halder_cnt == 2)
		SerialWrite((uint8_t*)"AT+BLENAME=inverjet\r\n",strlen("AT+BLENAME=inverjet\r\n"));
		//BT_Set_Name();
	//else if(BT_Halder_cnt == 4)
		//BT_Set_MAC();
	else if(BT_Halder_cnt == 9)
		SerialWrite((uint8_t*)"AT+BLEMODE=%d\r\n",strlen("AT+BLEMODE=%d\r\n"));
		//BT_Set_Mode(0);
	else if(BT_Halder_cnt == 16)
		SerialWrite((uint8_t*)"AT+BLEMTU=%d\r\n",strlen("AT+BLEMTU=%d\r\n"));
		//BT_Set_MTU(243);
	
	if(BT_Halder_cnt < 9999)
		BT_Halder_cnt ++;
	else
		BT_Halder_cnt = 0;
}

//------------------- 联网处理 0.5秒进一次 ----------------------------
//void BT_Online_Connect_Halder(void)
//{
//	if(BT_Halder_cnt == 0)
//		SerialWrite((uint8_t*)"+++",3);
//	else if(BT_Halder_cnt == 2)
//		BT_Set_Mode(1);
//	else if(BT_Halder_cnt == 16)
//		BT_Connect_OnlineServer();
//	
//	if(BT_Halder_cnt < 9999)
//		BT_Halder_cnt ++;
//	else
//		BT_Halder_cnt = 0;
//}

