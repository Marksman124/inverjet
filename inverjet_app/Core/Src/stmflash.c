/**
******************************************************************************
* @file    		stmflash.c
* @brief   		flash 接口
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/	
#include "stmflash.h"
#include "FreeRTOS.h"

extern void    FLASH_PageErase(uint32_t PageAddress);
 
//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)   
{
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//地址增加2.
	}
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif

uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
/**
*@brief     从指定地址开始写入指定长度的数据
*@param     WriteAddr : 起始地址(此地址必须为2的倍数!!)
*           pBuffer   : 数据指针
*           NumToWrite: 半字(16位)数(就是要写入的16位数据的个数.)
*@return    无
*/
void STMFLASH_Write(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)
{
    uint32_t  PageAddr;                                                                                            // Flash页地址
    uint32_t  WordAddr;                                                                                            // 要写入的地址在Flash页中的位置(16位字计算)
    uint32_t  WordRemainder;                                                                                       // Flash页中的剩余地址(16位字计算)
    uint32_t  ShiftingAddr;                                                                                        // 去掉0X08000000后的地址
    uint32_t  i;    
    
    /* 当指定起始地址小于STM32_FLASH_BASE (0x0800000) 或者大于芯片本身的Flash容量时，写入地址无效，跳出函数 */
    if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
    {
        return;                                                                                                    // 非法地址
    }
    HAL_FLASH_Unlock();     // 解锁
		
    ShiftingAddr  = WriteAddr - STM32_FLASH_BASE;                                                                  // 实际偏移地址.要写入数据起始地址的位置
    PageAddr      =  ShiftingAddr / FLASH_PAGE_SIZE;                                                               // 要写入的地址所在的Flash页（0~256）
    WordAddr      = (ShiftingAddr % FLASH_PAGE_SIZE) / 2;                                                          // 在Flash页内的偏移(2个字节为基本单位.)
    WordRemainder = FLASH_PAGE_SIZE/2 - WordAddr;                                                                  // Flash页剩余空间大小
    if(NumToWrite <= WordRemainder)
    {
        WordRemainder = NumToWrite;                                                                                // 不大于该Flash页范围
    }
 
    while(1) 
    {
			STMFLASH_Read(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);//读出整个扇区的内容
        //STMFLASH_ReadMultipleBytes(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // 读出整个Flash页的内容存放到STMFLASH_BUF中
        
        /* 查验数据，看flash页是否需要擦除 */
        for(i = 0;i < WordRemainder;i++)                                                                           // 校验数据
        {
            if(STMFLASH_BUF[WordAddr + i] != 0XFFFF)
            {
                break;                                                                                             // 需要擦除
            }     
        }
        /* 如果要写入数据的Flash页面上，所有的字都等于0XFFFF，那么在上面的循环之后，i = WordRemainder*/
        if(i < WordRemainder)                                                                                      // 需要擦除
        {
            FLASH_PageErase(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE);                                      // 擦除这个Flash页
            for(i = 0;i < WordRemainder;i++)                                                                       // 复制
            {
                STMFLASH_BUF[i + WordAddr] = pBuffer[i];
            }
            STMFLASH_Write_NoCheck(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // 写入整个页
        }
        /* i = WordRemainder */
        else
        {
            STMFLASH_Write_NoCheck(WriteAddr,pBuffer,WordRemainder);                                               // 写已经擦除了的,直接写入扇区剩余区间. 
        }
 
        if(NumToWrite == WordRemainder)
        {
            break;                                                                                                 // 写入结束了
        }
        else                                                                                                       // 写入未结束
        {
            PageAddr++;                                                                                            // 页地址增加1
            WordAddr    = 0;                                                                                       // 偏移位置为0     
            pBuffer    += WordRemainder;                                                                           // 指针偏移
            WriteAddr  += WordRemainder*2;                                                                         // 写地址偏移(16位数据址,需要*2)
            NumToWrite -= WordRemainder;                                                                           // 字节(16位)数递减
            if(NumToWrite > (FLASH_PAGE_SIZE/2))
            {
							WordRemainder = FLASH_PAGE_SIZE/2;                                                                 // 下一个Flash页还是写不完
            }
            else
						{
							WordRemainder = NumToWrite;                                                                       // 下一个Flash页可以写完了
						}
        }
    };
    HAL_FLASH_Lock();                                                                                                  // 上锁
}

#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

















