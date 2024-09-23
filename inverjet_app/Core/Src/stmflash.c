/**
******************************************************************************
* @file    		stmflash.c
* @brief   		flash �ӿ�
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
 
//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(uint16_t*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)   
{
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);
	  WriteAddr+=2;//��ַ����2.
	}
} 
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif

uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
/**
*@brief     ��ָ����ַ��ʼд��ָ�����ȵ�����
*@param     WriteAddr : ��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
*           pBuffer   : ����ָ��
*           NumToWrite: ����(16λ)��(����Ҫд���16λ���ݵĸ���.)
*@return    ��
*/
void STMFLASH_Write(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite)
{
    uint32_t  PageAddr;                                                                                            // Flashҳ��ַ
    uint32_t  WordAddr;                                                                                            // Ҫд��ĵ�ַ��Flashҳ�е�λ��(16λ�ּ���)
    uint32_t  WordRemainder;                                                                                       // Flashҳ�е�ʣ���ַ(16λ�ּ���)
    uint32_t  ShiftingAddr;                                                                                        // ȥ��0X08000000��ĵ�ַ
    uint32_t  i;    
    
    /* ��ָ����ʼ��ַС��STM32_FLASH_BASE (0x0800000) ���ߴ���оƬ�����Flash����ʱ��д���ַ��Ч���������� */
    if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
    {
        return;                                                                                                    // �Ƿ���ַ
    }
    HAL_FLASH_Unlock();     // ����
		
    ShiftingAddr  = WriteAddr - STM32_FLASH_BASE;                                                                  // ʵ��ƫ�Ƶ�ַ.Ҫд��������ʼ��ַ��λ��
    PageAddr      =  ShiftingAddr / FLASH_PAGE_SIZE;                                                               // Ҫд��ĵ�ַ���ڵ�Flashҳ��0~256��
    WordAddr      = (ShiftingAddr % FLASH_PAGE_SIZE) / 2;                                                          // ��Flashҳ�ڵ�ƫ��(2���ֽ�Ϊ������λ.)
    WordRemainder = FLASH_PAGE_SIZE/2 - WordAddr;                                                                  // Flashҳʣ��ռ��С
    if(NumToWrite <= WordRemainder)
    {
        WordRemainder = NumToWrite;                                                                                // �����ڸ�Flashҳ��Χ
    }
 
    while(1) 
    {
			STMFLASH_Read(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);//������������������
        //STMFLASH_ReadMultipleBytes(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // ��������Flashҳ�����ݴ�ŵ�STMFLASH_BUF��
        
        /* �������ݣ���flashҳ�Ƿ���Ҫ���� */
        for(i = 0;i < WordRemainder;i++)                                                                           // У������
        {
            if(STMFLASH_BUF[WordAddr + i] != 0XFFFF)
            {
                break;                                                                                             // ��Ҫ����
            }     
        }
        /* ���Ҫд�����ݵ�Flashҳ���ϣ����е��ֶ�����0XFFFF����ô�������ѭ��֮��i = WordRemainder*/
        if(i < WordRemainder)                                                                                      // ��Ҫ����
        {
            FLASH_PageErase(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE);                                      // �������Flashҳ
            for(i = 0;i < WordRemainder;i++)                                                                       // ����
            {
                STMFLASH_BUF[i + WordAddr] = pBuffer[i];
            }
            STMFLASH_Write_NoCheck(PageAddr*FLASH_PAGE_SIZE + STM32_FLASH_BASE,STMFLASH_BUF,FLASH_PAGE_SIZE/2);    // д������ҳ
        }
        /* i = WordRemainder */
        else
        {
            STMFLASH_Write_NoCheck(WriteAddr,pBuffer,WordRemainder);                                               // д�Ѿ������˵�,ֱ��д������ʣ������. 
        }
 
        if(NumToWrite == WordRemainder)
        {
            break;                                                                                                 // д�������
        }
        else                                                                                                       // д��δ����
        {
            PageAddr++;                                                                                            // ҳ��ַ����1
            WordAddr    = 0;                                                                                       // ƫ��λ��Ϊ0     
            pBuffer    += WordRemainder;                                                                           // ָ��ƫ��
            WriteAddr  += WordRemainder*2;                                                                         // д��ַƫ��(16λ����ַ,��Ҫ*2)
            NumToWrite -= WordRemainder;                                                                           // �ֽ�(16λ)���ݼ�
            if(NumToWrite > (FLASH_PAGE_SIZE/2))
            {
							WordRemainder = FLASH_PAGE_SIZE/2;                                                                 // ��һ��Flashҳ����д����
            }
            else
						{
							WordRemainder = NumToWrite;                                                                       // ��һ��Flashҳ����д����
						}
        }
    };
    HAL_FLASH_Lock();                                                                                                  // ����
}

#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)   	
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}

















