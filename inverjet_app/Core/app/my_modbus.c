
#include "my_modbus.h"
#include <string.h>
#include "data.h"
#include "iap.h"
#include "modbus.h"
#include "key.h"

#define _MsGetUint16(A,B)  (unsigned short)((A[B] << 8) | A[B + 1])
#define _MsGetInt16(A,B)   (short)((A[B] << 8) | A[B + 1])
	

unsigned char BT_OTA_Mode=0;

unsigned long BT_OTA_Pack_Len=0;					// 总长度

unsigned long BT_Pack_Len=0;

unsigned char BT_OTA_Buffer[MODBUS_SLAVE_TX_RX_MAX_LEN]={0};

#define BT_MODLE_AT_MSG_LENGTH									23	

/* 连接成功  +EVENT:BLE_CONNECTED > */ 
const unsigned char connected_table[BT_MODLE_AT_MSG_LENGTH] = {
	0x2B, 0x45, 0x56, 0x45, 0x4E, 0x54, 0x3A, 0x42, 0x4C, 0x45, 
	0x5F, 0x43, 0x4F, 0x4E, 0x4E, 0x45, 0x43, 0x54, 0x45, 0x44, 
	0x0D, 0x0A, 0x3E};

/* 断开连接  +EVENT:BLE_DISCONNECT */ 
const unsigned char disconnected_table[BT_MODLE_AT_MSG_LENGTH] = {
	0x2B, 0x45, 0x56, 0x45, 0x4E, 0x54, 0x3A, 0x42, 0x4C, 0x45,
	0x5F, 0x44, 0x49, 0x53, 0x43, 0x4F, 0x4E, 0x4E, 0x45, 0x43, 
	0x54, 0x0D, 0x0A};

/* 进入透传  AT+TRANSENTER> */ 
const unsigned char transenter_table[16] = {
	0x41, 0x54, 0x2B, 0x54, 0x52, 0x41, 0x4E, 0x53, 0x45, 0x4E, 
	0x54, 0x45, 0x52, 0x0D, 0x0A, 0x3E};

	
void Chenk_BT_State(unsigned char* p_buffer, unsigned char len)
{
	if(len == BT_MODLE_AT_MSG_LENGTH)
	{
		if(memcmp(p_buffer,disconnected_table,BT_MODLE_AT_MSG_LENGTH) == 0)
		{
			BT_Set_Machine_State(BT_NO_CONNECT);
		}
		else if(memcmp(p_buffer,connected_table,BT_MODLE_AT_MSG_LENGTH) == 0)
		{
			BT_Set_Machine_State(BT_WORKING);
		}
	}
	else if(len == 16)
	{
		if(memcmp(p_buffer,transenter_table,16) == 0)
		{
			BT_Set_Machine_State(BT_WORKING);
		}
	}
}

/*******************************************************************************
*功能：解锁接收队列
*******************************************************************************/
void _MsRxQueueUnLock(ModbusSlaveObj_t * pObj)
{
    pObj->rxWriteLock = 0;
    pObj->rxWriteIdx = 0;
    pObj->rxRecvLen = 0;
		memset(pObj->rxBuff,0,MODBUS_SLAVE_TX_RX_MAX_LEN);
}
/*******************************************************************************
*功能：超时计数，放到定时器中运行，一般放到1ms中断足够满足要求
*******************************************************************************/
void MsTimeout(ModbusSlaveObj_t * pObj)
{
    if(pObj->rxTimerEnable)
    {
        if(pObj->rxTimerCnt < pObj->timeout)
        {
            pObj->rxTimerCnt++;
        }
        else
        {
            pObj->rxWriteLock = 1;
            pObj->rxTimerEnable = 0;
            pObj->rxTimerCnt = 0;
        }
    }
}
/*******************************************************************************
*功能：串口读取，放到中读取，一个字节读取也是可以的
*******************************************************************************/
void MsSerialRead(ModbusSlaveObj_t * pObj,unsigned char *buff,int length)
{
    int i = 0;
    if(pObj->rxWriteLock)
    {
        return;
    }
    length = length % MODBUS_SLAVE_TX_RX_MAX_LEN;
    pObj->rxTimerEnable = 1;
    pObj->rxTimerCnt = 0;
    for(i = 0; i < length; i++)
    {

        pObj->rxBuff[pObj->rxWriteIdx] = buff[i];
        if(pObj->rxWriteIdx < (MODBUS_SLAVE_TX_RX_MAX_LEN - 1))
        {
            pObj->rxWriteIdx++;
        }
    }
}
/* CRC余式表 */
const unsigned short crc_table[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

/*******************************************************************************
*功能：CRC校验
*******************************************************************************/
unsigned short _MsCRC16(unsigned char *ptr, int len)
{
    unsigned short crc = 0xFFFF;

    while(len--)
    {
        crc = (crc >> 8) ^ crc_table[(crc ^ *ptr++) & 0xff];
    }

    return (crc);
}
/*******************************************************************************
*功能：对象初始化
*参数：pObj 对象 slaveId 从机号 timeout一帧超时时间 SerialWrite 写接口
*注意：timeout根据MsTimeout(ModbusSlaveObj_t * pObj)中断时间进行计算,如中断时间50us,要求一帧超时时间为3.7ms timeout = 3700 / 50
*******************************************************************************/
void MsInit(ModbusSlaveObj_t *pObj,unsigned char slaveId,unsigned short timeout,void (*SerialWrite)(unsigned char *buff,int length))
{
	int i = 0;
	char *p = (char *)(pObj);
	for(i = 0;i < sizeof(ModbusSlaveObj_t);i++)
	{
		p[i] = 0;
	}
	pObj->slaveId = slaveId;
	pObj->timeout = timeout;
	pObj->SerialWrite = SerialWrite;
}
/*******************************************************************************
*功能：设置寄存器
*参数：pObj 对象 cmd 命令 reg寄存器类型  count 寄存器个数
*注意：线圈类型均为unsigned char类型 一个字节代表一位 寄存器为16位有符号
*******************************************************************************/
void MsConfigureRegister(ModbusSlaveObj_t *pObj,unsigned char cmd,void *reg,unsigned short count)
{
    switch(cmd)
    {
    case 0x01:
        pObj->reg01Num = count;
        pObj->reg01Ptr = (unsigned char*)(reg);
        break;
    case 0x02:
        pObj->reg02Num = count;
        pObj->reg02Ptr = (unsigned char*)(reg);
        break;
    case 0x03:
        pObj->reg03Num = count;
        pObj->reg03Ptr = (short*)(reg);
        break;
    case 0x04:
        pObj->reg04Num = count;
        pObj->reg04Ptr = (short*)(reg);
        break;
    case 0x05:
        pObj->reg05Num = count;
        pObj->reg05Ptr = (short*)(reg);
        break;
    case 0x06:
        pObj->reg06Num = count;
        pObj->reg06Ptr = (short*)(reg);
        break;
    case 0x10:
        pObj->reg10Num = count;
        pObj->reg10Ptr = (short*)(reg);
        break;
    case 0x0F:
        pObj->reg0FNum = count;
        pObj->reg0FPtr = (unsigned char*)(reg);
        break;
    }
	
}
/*******************************************************************************
*功能：解析0x01命令
*******************************************************************************/
MsState _MsAnalyzeCmd01(ModbusSlaveObj_t *pObj)
{
	unsigned short count,addr;
	int i,j,idx = 0;
	addr = _MsGetUint16(pObj->rxBuff,2);
	count = _MsGetUint16(pObj->rxBuff,4);
	pObj->txBuff[0] = pObj->rxBuff[0];
	pObj->txBuff[1] = pObj->rxBuff[1];
	pObj->txBuff[2] = (count >> 3) + ((count % 8) > 0);
	if(addr + count > pObj->reg01Num)
	{
		return MS_Illegal_data_address;
	}
	if(pObj->reg01Num == 0)
	{
		return MS_Illegal_function;
	}
	for(i = 0;i < pObj->txBuff[2];i++)
	{
		pObj->txBuff[i + 3] = 0;
		for(j = 0;j < 8;j++)
		{
			idx = (i << 3) + j;
			if(idx + addr > pObj->reg01Num)
			{
				break;
			}			
			if(pObj->reg01Ptr[addr + idx])
			{
				pObj->txBuff[i + 3] |= (0x01 << j);
			}
			else
			{
				pObj->txBuff[i + 3] &= ~(0x01 << j);
			}
		}
	}
	count = 3 + pObj->txBuff[2];
	addr =  _MsCRC16(pObj->txBuff,count);
	pObj->txBuff[count++] = (addr >> 0) & 0xFF;
	pObj->txBuff[count++] = (addr >> 8) & 0xFF;
	pObj->sendCount = count;
	return MS_OK;
}
/*******************************************************************************
*功能：解析0x02命令
*******************************************************************************/
MsState _MsAnalyzeCmd02(ModbusSlaveObj_t *pObj)
{
	unsigned short count,addr;
	int i,j,idx = 0;
	addr = _MsGetUint16(pObj->rxBuff,2);
	count = _MsGetUint16(pObj->rxBuff,4);
	pObj->txBuff[0] = pObj->rxBuff[0];
	pObj->txBuff[1] = pObj->rxBuff[1];
	pObj->txBuff[2] = (count >> 3) + ((count % 8) > 0);
	if(addr + count > pObj->reg02Num)
	{
		return MS_Illegal_data_address;
	}
	if(pObj->reg02Num == 0)
	{
		return MS_Illegal_function;
	}
	for(i = 0;i < pObj->txBuff[2];i++)
	{
		pObj->txBuff[i + 3] = 0;
		for(j = 0;j < 8;j++)
		{
			idx = (i << 3) + j;
			if(idx + addr > pObj->reg02Num)
			{
				break;
			}
			if(pObj->reg02Ptr[addr + idx] )
			{
				pObj->txBuff[i + 3] |= (0x01 << j);
			}
			else
			{
				pObj->txBuff[i + 3] &= ~(0x01 << j);
			}
		}
	}
	count = 3 + pObj->txBuff[2];
	addr =  _MsCRC16(pObj->txBuff,count);
	pObj->txBuff[count++] = (addr >> 0) & 0xFF;
	pObj->txBuff[count++] = (addr >> 8) & 0xFF;
	pObj->sendCount = count;
	return MS_OK;
}
/*******************************************************************************
*功能：解析0x10命令
*******************************************************************************/
MsState _MsAnalyzeCmd10(ModbusSlaveObj_t *pObj)
{
	unsigned short count,addr;
	int i,j;
	addr = _MsGetUint16(pObj->rxBuff,2);
	count = _MsGetUint16(pObj->rxBuff,4);

	if(addr + count > pObj->reg10Num)
	{
		return MS_Illegal_data_address;
	}
	if(pObj->reg10Num == 0)
	{
		return MS_Illegal_function;
	}
	j = 0;
	for(i = 0;i < count;i++)
	{
		pObj->reg10Ptr[addr + i] = _MsGetInt16(pObj->rxBuff,7 + j);
		j += 2;
	}
	
	if(addr == MB_SYSTEM_WORKING_MODE) //	系统工作模式  高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
	{
		if(Get_System_State_Mode() > 0)//P模式
		{
			Set_Pmode_Period_Now(0);
		}
		if(System_is_Power_Off())//状态机
		{
			System_Power_Off();
		}
		if(Motor_is_Start())
		{
			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
		}
		else
		{
			Motor_Speed_Target_Set(*p_OP_ShowNow_Speed);
		}
		//Ctrl_Set_System_Mode(usRegHoldingBuf[MB_SYSTEM_WORKING_MODE]);
		Set_Ctrl_Mode_Type(CTRL_FROM_BT);//标记控制来源
	}
			
	pObj->txBuff[0] = pObj->rxBuff[0];
	pObj->txBuff[1] = pObj->rxBuff[1];
	pObj->txBuff[2] = pObj->rxBuff[2];
	pObj->txBuff[3] = pObj->rxBuff[3];
	pObj->txBuff[4] = pObj->rxBuff[4];
	pObj->txBuff[5] = pObj->rxBuff[5];
	addr =  _MsCRC16(pObj->txBuff,6);
	pObj->txBuff[6] = (addr >> 0) & 0xFF;
	pObj->txBuff[7] = (addr >> 8) & 0xFF;
	pObj->sendCount = 8;
	return MS_OK;
}
/*******************************************************************************
*功能：解析0x0F命令
*******************************************************************************/
MsState _MsAnalyzeCmd0F(ModbusSlaveObj_t *pObj)
{
	unsigned short count,addr,byteCount;
	int i,j,idx = 0;
	addr = _MsGetUint16(pObj->rxBuff,2);
	count = _MsGetUint16(pObj->rxBuff,4);
	byteCount = pObj->rxBuff[6];
	if(addr + count > pObj->reg0FNum)
	{
		return MS_Illegal_data_address;
	}
	if(pObj->reg0FNum == 0)
	{
		return MS_Illegal_function;
	}
	for(i = 0;i < byteCount;i++)
	{
		
		for(j = 0;j < 8;j++)
		{
			idx = addr + (i << 3) + j;
			if(idx > (pObj->reg0FNum - 1))
			{
				break;
			}
			else
			{
				if((pObj->rxBuff[i + 7]) & (0x01 << j))
				{
					pObj->reg0FPtr[idx] = 1;
				}
				else
				{
					pObj->reg0FPtr[idx] = 0;
				}
			}
		}
	}
	pObj->txBuff[0] = pObj->rxBuff[0];
	pObj->txBuff[1] = pObj->rxBuff[1];
	pObj->txBuff[2] = pObj->rxBuff[2];
	pObj->txBuff[3] = pObj->rxBuff[3];
	pObj->txBuff[4] = pObj->rxBuff[4];
	pObj->txBuff[5] = pObj->rxBuff[5];
	pObj->txBuff[6] = pObj->rxBuff[6];
	addr =  _MsCRC16(pObj->txBuff,7);
	pObj->txBuff[7] = (addr >> 0) & 0xFF;
	pObj->txBuff[8] = (addr >> 8) & 0xFF;
	pObj->sendCount = 9;
	return MS_OK;
}
/*******************************************************************************
*功能：解析0x03命令
*******************************************************************************/
MsState _MsAnalyzeCmd03(ModbusSlaveObj_t *pObj)
{
	unsigned short count,addr;
	int i,idx = 0;
	addr = _MsGetUint16(pObj->rxBuff,2);
	count = _MsGetUint16(pObj->rxBuff,4);
	pObj->txBuff[idx++] = pObj->rxBuff[0];
	pObj->txBuff[idx++] = pObj->rxBuff[1];
	pObj->txBuff[idx++] = (count << 1) % 0xFF;
	if(addr + count > pObj->reg03Num)
	{
		return MS_Illegal_data_address;
	}
	if(pObj->reg03Num == 0)
	{
		return MS_Illegal_function;
	}
	for(i = 0;i < count;i++)
	{
		 pObj->txBuff[idx++] = pObj->reg03Ptr[addr + i] >> 8;
		 pObj->txBuff[idx++] = pObj->reg03Ptr[addr + i] & 0xFF;
	}
	addr =  _MsCRC16(pObj->txBuff,idx);
	pObj->txBuff[idx++] = (addr >> 0) & 0xFF;
	pObj->txBuff[idx++] = (addr >> 8) & 0xFF;
	pObj->sendCount = idx;
	return MS_OK;
}
/*******************************************************************************
*功能：解析0x04命令
*******************************************************************************/
MsState _MsAnalyzeCmd04(ModbusSlaveObj_t *pObj)
{
	unsigned short count,addr;
	int i,idx = 0;
	addr = _MsGetUint16(pObj->rxBuff,2);
	count = _MsGetUint16(pObj->rxBuff,4);
	pObj->txBuff[idx++] = pObj->rxBuff[0];
	pObj->txBuff[idx++] = pObj->rxBuff[1];
	pObj->txBuff[idx++] = (count << 1) % 0xFF;
	if(addr + count > pObj->reg04Num)
	{
		return MS_Illegal_data_address;
	}
	if(pObj->reg04Num == 0)
	{
		return MS_Illegal_function;
	}
	for(i = 0;i < count;i++)
	{
		 pObj->txBuff[idx++] = pObj->reg04Ptr[addr + i] >> 8;
		 pObj->txBuff[idx++] = pObj->reg04Ptr[addr + i] & 0xFF;
	}
	addr =  _MsCRC16(pObj->txBuff,idx);
	pObj->txBuff[idx++] = (addr >> 0) & 0xFF;
	pObj->txBuff[idx++] = (addr >> 8) & 0xFF;
	pObj->sendCount = idx;
	return MS_OK;
}

/*******************************************************************************
*功能：解析0x05命令
*******************************************************************************/
MsState _MsAnalyzeCmd05(ModbusSlaveObj_t *pObj)
{
unsigned short addr;
	int count = 1;
	addr = _MsGetUint16(pObj->rxBuff,2);
	if(addr + count > pObj->reg05Num)
	{
		return MS_Illegal_data_address;
	}
	if(pObj->reg05Num == 0)
	{
		return MS_Illegal_function;
	}
	pObj->reg05Ptr[addr] = _MsGetInt16(pObj->rxBuff,4);
	pObj->txBuff[0] = pObj->rxBuff[0];
	pObj->txBuff[1] = pObj->rxBuff[1];
	pObj->txBuff[2] = pObj->rxBuff[2];
	pObj->txBuff[3] = pObj->rxBuff[3];
	pObj->txBuff[4] = pObj->rxBuff[4];
	pObj->txBuff[5] = pObj->rxBuff[5];
	pObj->txBuff[6] = pObj->rxBuff[6];
	pObj->txBuff[7] = pObj->rxBuff[7];
	pObj->sendCount = 8;
	return MS_OK;
}
/*******************************************************************************
*功能：解析0x06命令
*******************************************************************************/
MsState _MsAnalyzeCmd06(ModbusSlaveObj_t *pObj)
{
	unsigned short addr;
	int count = 1;
	addr = _MsGetUint16(pObj->rxBuff,2);
	if(addr + count > pObj->reg06Num)
	{
		return MS_Illegal_data_address;
	}
	if(pObj->reg06Num == 0)
	{
		return MS_Illegal_function;
	}
	pObj->reg06Ptr[addr] = _MsGetInt16(pObj->rxBuff,4);
	
	if(addr == MB_SYSTEM_WORKING_MODE) //	系统工作模式  高位::0：P1\2\3  低位:0：自由:1：定时:2：训练
	{
		System_Para_Set_PMode(pObj->reg06Ptr[addr], CTRL_FROM_BT);
	}
	else if(addr == MB_SYSTEM_WORKING_STATUS)
	{
		System_Para_Set_Status(pObj->reg06Ptr[addr], CTRL_FROM_BT);
	}
	else if(addr == MB_MOTOR_CURRENT_SPEED)
	{
		System_Para_Set_Speed(pObj->reg06Ptr[addr], CTRL_FROM_BT);
	}
	else if(addr == MB_MOTOR_CURRENT_TIME)
	{
		System_Para_Set_Time(pObj->reg06Ptr[addr], CTRL_FROM_BT);
	}

	pObj->txBuff[0] = pObj->rxBuff[0];
	pObj->txBuff[1] = pObj->rxBuff[1];
	pObj->txBuff[2] = pObj->rxBuff[2];
	pObj->txBuff[3] = pObj->rxBuff[3];
	pObj->txBuff[4] = pObj->rxBuff[4];
	pObj->txBuff[5] = pObj->rxBuff[5];
	pObj->txBuff[6] = pObj->rxBuff[6];
	pObj->txBuff[7] = pObj->rxBuff[7];
	pObj->sendCount = 8;
	return MS_OK;
}

/*******************************************************************************
*功能：解析0x15命令 升级
*******************************************************************************/
MsState _MsAnalyzeCmd15(ModbusSlaveObj_t *pObj)
{
//	unsigned short count,addr;
//	int i,j;
//	addr = _MsGetUint16(pObj->rxBuff,2);
//	count = _MsGetUint16(pObj->rxBuff,4);

//	if(addr + count > pObj->reg10Num)
//	{
//		return MS_Illegal_data_address;
//	}
//	if(pObj->reg10Num == 0)
//	{
//		return MS_Illegal_function;
//	}
//	j = 0;
//	for(i = 0;i < count;i++)
//	{
//		pObj->reg10Ptr[addr + i] = _MsGetInt16(pObj->rxBuff,7 + j);
//		j += 2;
//	}
//	pObj->txBuff[0] = pObj->rxBuff[0];
//	pObj->txBuff[1] = pObj->rxBuff[1];
//	pObj->txBuff[2] = pObj->rxBuff[2];
//	pObj->txBuff[3] = pObj->rxBuff[3];
//	pObj->txBuff[4] = pObj->rxBuff[4];
//	pObj->txBuff[5] = pObj->rxBuff[5];
//	addr =  _MsCRC16(pObj->txBuff,6);
//	pObj->txBuff[6] = (addr >> 0) & 0xFF;
//	pObj->txBuff[7] = (addr >> 8) & 0xFF;
//	pObj->sendCount = 8;
//	return MS_OK;
//	
//	unsigned long write_addr=0;						// flash 写入地址
//	unsigned long sign=0;
//	
//	Clean_ModbusTimerCnt();
//	
//	if( (( fileNumber <= REG_FILE_NUMBER_MAX ) || ( fileNumber == REG_FILE_NUMBER_END )) && ( fileLength <= REG_FILE_LENTH_MAX ) )
//	{

//			if(fileNumber == REG_FILE_NUMBER_STAR) // 起始包
//			{OTA_Pack_Len = 0;}
//			
//			taskENTER_CRITICAL();
//			write_addr = (FLASH_APP_PATCH_ADDR + OTA_Pack_Len);
//			iap_write_appbin(write_addr,pucRegBuffer,fileLength);
//			OTA_Pack_Len += (fileLength*2);
//			taskEXIT_CRITICAL();
//			
//			if(fileNumber == REG_FILE_NUMBER_END)//最后一帧
//			{
//				STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PACK_LEN,(uint16_t*)&OTA_Pack_Len,2); // 写包长度 (含crc)
//				sign = PRODUCT_BOOT_PASSWORD;
//				STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PASSWORD,(uint16_t*)&sign,2); // 进入OTA 

//				SysSoftReset();// 软件复位
//			}
//	}
//	else
//	{
//	}

	return MS_OK;
}

/*******************************************************************************
*功能：解析命令
*******************************************************************************/
void MsProcess(ModbusSlaveObj_t * pObj)
{
	MsState res;
	unsigned short crc;
	
	unsigned long write_addr=0;						// flash 写入地址
	unsigned long sign=0;
	unsigned long len=0;
	
	static uint8_t BT_Pack_Sum=0;
	static unsigned char first_error=0;
	
    if(!pObj->rxWriteLock)
    {
        return;
    }
		
		if(BT_OTA_Mode == 0xAA)
		{
			if((pObj->rxWriteIdx == 8)&&(pObj->rxBuff[0] == 0x15)&&(pObj->rxBuff[1] == 0x01)
				&&(pObj->rxBuff[2] == 0x00)&&(pObj->rxBuff[3] == 0x00)&&(pObj->rxBuff[4] == 0x00)
			&&(pObj->rxBuff[5] == 0x01)&&(pObj->rxBuff[6] == 0xBB)&&(pObj->rxBuff[7] == 0xBB)) {
					//固件数据发送完成
					STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PACK_LEN,(uint16_t*)&BT_OTA_Pack_Len,2); // 写包长度 (含crc)
					sign = PRODUCT_BOOT_PASSWORD;
					STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PASSWORD,(uint16_t*)&sign,2); // 进入OTA 

					SysSoftReset();// 软件复位
				
			}else {
				//固件数据处理
				if(memcmp(BT_OTA_Buffer,pObj->rxBuff,pObj->rxWriteIdx) == 0)
				{
					_MsRxQueueUnLock(pObj);
					first_error ++;
				}
				else
				{
					len = pObj->rxWriteIdx;
					memcpy(BT_OTA_Buffer,pObj->rxBuff,len);
					
////					if((BT_OTA_Pack_Len + len) > BT_Pack_Len)
////					{
////						len =  BT_Pack_Len - BT_OTA_Pack_Len;
////					}
					_MsRxQueueUnLock(pObj);
					taskENTER_CRITICAL();
					write_addr = (FLASH_APP_PATCH_ADDR + BT_OTA_Pack_Len);
					iap_write_appbin(write_addr,BT_OTA_Buffer,len/2);
					BT_OTA_Pack_Len += (len);
					taskEXIT_CRITICAL();
					
					Lcd_Show_Upgradation(BT_Pack_Sum,BT_OTA_Pack_Len*100/BT_Pack_Len);
				}
			}
		}
		else
		{

			if(pObj->rxWriteIdx < 4)
			{
					_MsRxQueueUnLock(pObj);
					return;
			}
			
			Chenk_BT_State(pObj->rxBuff , pObj->rxWriteIdx);
			
			if(pObj->slaveId != pObj->rxBuff[0])
			{
					_MsRxQueueUnLock(pObj);
					return;
			}
			if(_MsCRC16(pObj->rxBuff,pObj->rxWriteIdx) != 0)
			{
			res = MS_CRC_check_failure;
			pObj->txBuff[1] += 128; 
			pObj->txBuff[2]  = res;
			crc = _MsCRC16(&pObj->txBuff[0],3);
			pObj->txBuff[3] = crc & 0xFF;
			pObj->txBuff[4] = (crc >> 8) & 0xFF;
			pObj->SerialWrite(pObj->txBuff,5);
					_MsRxQueueUnLock(pObj);
					return;
			}
			BT_Set_Machine_State(BT_WORKING);
			BLE_Pair_Finish_Now = 1;
			
			switch(pObj->rxBuff[1])
			{
			case 0x01:
				res = _MsAnalyzeCmd01(pObj);
			//进入升级
			BT_OTA_Mode = 0xAA;
			BT_OTA_Pack_Len = 0;
			first_error = 0;
			BT_Pack_Len = pObj->rxBuff[2]<<24 | pObj->rxBuff[3]<<16| pObj->rxBuff[4]<<8| pObj->rxBuff[5];
			BT_Pack_Sum = BT_Pack_Len/1024;
			if(BT_Pack_Len % 1024)
				BT_Pack_Sum ++;
			System_To_OTA();
			Lcd_Show_Upgradation(BT_Pack_Sum,0);
			Freertos_TaskSuspend_BT();
				break;
			case 0x02:
				res = _MsAnalyzeCmd02(pObj);
				break;
			case 0x03:
				res = _MsAnalyzeCmd03(pObj);
				break;
			case 0x04:
				res = _MsAnalyzeCmd04(pObj);
				break;
			case 0x05:
				res = _MsAnalyzeCmd05(pObj);
				break;
			case 0x06:
				if(If_Accept_External_Control(BLOCK_BLUETOOTH_CONTROL))
				{
					res = _MsAnalyzeCmd06(pObj);
					//保存
					Write_MbBuffer_Later();
				}
				break;
			case 0x10:
				if(If_Accept_External_Control(BLOCK_BLUETOOTH_CONTROL))
				{
					res = _MsAnalyzeCmd10(pObj);
					//保存
					Write_MbBuffer_Later();
				}
				break;
			case 0x0F:
				res = _MsAnalyzeCmd0F(pObj);
				break;
			default :
				res = MS_Illegal_function;
				break;
			}
			if(res == MS_OK)
			{
				pObj->SerialWrite(pObj->txBuff,pObj->sendCount);
			}
			else
			{
				pObj->txBuff[1] += 128; 
				pObj->txBuff[2]  = res;
				crc = _MsCRC16(&pObj->txBuff[0],3);
				pObj->txBuff[3] = crc & 0xFF;
				pObj->txBuff[4] = (crc >> 8) & 0xFF;
				pObj->SerialWrite(pObj->txBuff,5);
			}
			 _MsRxQueueUnLock(pObj);
	}

}
