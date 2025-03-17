
#ifndef _MODBUSSLAVE_H
#define _MODBUSSLAVE_H

#define MODBUS_SLAVE_TX_RX_MAX_LEN  256

typedef struct
{
	unsigned char txBuff[MODBUS_SLAVE_TX_RX_MAX_LEN];
	unsigned char rxBuff[MODBUS_SLAVE_TX_RX_MAX_LEN];
	unsigned short rxTimerCnt;
	unsigned char sendCount;
	unsigned char rxTimerEnable;
	unsigned char rxRecvLen;
	unsigned char rxWriteIdx;
	unsigned char rxWriteLock;
	unsigned char slaveId;
	unsigned short timeout;
	unsigned char *reg01Ptr;
	unsigned char *reg02Ptr;
	short * reg03Ptr;
	short * reg04Ptr;
	short * reg05Ptr;
	short * reg06Ptr;
	short * reg10Ptr;
	unsigned char* reg0FPtr;
	unsigned short reg01Num;
	unsigned short reg02Num;
	unsigned short reg03Num;
	unsigned short reg04Num;
	unsigned short reg05Num;
	unsigned short reg06Num;
	unsigned short reg10Num;
	unsigned short reg0FNum;
	void (*SerialWrite)(unsigned char *buff,int length);
}ModbusSlaveObj_t;

void Chenk_BT_State(unsigned char* p_buffer, unsigned char len);

//主循环运行
void MsProcess(ModbusSlaveObj_t * pObj);
//对象初始化
void MsInit(ModbusSlaveObj_t *pObj,unsigned char slaveId,unsigned short timeout,void (*SerialWrite)(unsigned char *buff,int length));
//串口读取
void MsSerialRead(ModbusSlaveObj_t * pObj,unsigned char *buff,int length);
//超时判断
void MsTimeout(ModbusSlaveObj_t * pObj);
//设置寄存器
void MsConfigureRegister(ModbusSlaveObj_t *pObj,unsigned char cmd,void *reg,unsigned short count);
//错误码定义
typedef enum
{
	MS_OK                          = 0x00,
	MS_Illegal_function 	    = 0x01,
	MS_Illegal_data_address = 0x02,
	MS_Illegal_data_value 	= 0x03,
	MS_Slave_device_failure = 0x04,
	MS_Slave_device_busy 	= 0x06,
	MS_CRC_check_failure 	= 0x0C,
}MsState;
#endif
