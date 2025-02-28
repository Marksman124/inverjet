/**
******************************************************************************
* @file    		iap.c
* @brief   		iap��أ�Bootloader �� app ����
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/	
#include "iap.h"
#include "cmsis_os.h"
#include "mbcrc.h"

uint32_t  ProductAppPassword  =     PRODUCT_APP_PASSWORD;  //��ƷӦ����Կ


iapfun jump2app; 
uint16_t iapbuf[1024];   
//appxaddr:Ӧ�ó������ʼ��ַ
//appbuf:Ӧ�ó���CODE.
//appsize:Ӧ�ó����С(�ֽ�).
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t appsize)
{
	uint16_t t;
	uint16_t i=0;
	uint16_t temp;
	uint32_t fwaddr=appxaddr;//��ǰд��ĵ�ַ
	uint8_t *dfu=appbuf;
	portDISABLE_INTERRUPTS();			//�ر��ж�
	for(t=0;t<appsize;t++)
	{
		temp=(uint16_t)dfu[1]<<8;
		temp+=(uint16_t)dfu[0];	  
		dfu+=2;//ƫ��2���ֽ�
		iapbuf[i++]=temp;	    
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//ƫ��2048  16=2*8.����Ҫ����2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//������һЩ�����ֽ�д��ȥ.  
	portENABLE_INTERRUPTS();			//�����ж�
}

//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
void iap_load_app(uint32_t appxaddr)
{
	if(((*(uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(uint32_t*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MsrMsp(*(uint32_t*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}		 


uint8_t Check_Pack_CRC(void)
{
	uint32_t len_sum=0;
	static uint16_t crc_read, crc_calculate;
	
	len_sum = *(uint32_t *)BOOT_FLASH_ADDR_OTA_PACK_LEN;
	if( ( len_sum < RANGE_BOOT_PACK_LEN_MIN ) || ( len_sum > RANGE_BOOT_PACK_LEN_MAX  ))
	{
		return 0;
	}
	crc_read = *(uint8_t*)(FLASH_APP_PATCH_ADDR + len_sum - 2)<<8 | *(uint8_t*)(FLASH_APP_PATCH_ADDR + len_sum - 1);

	crc_calculate = usMBCRC16( ( uint8_t * ) FLASH_APP_PATCH_ADDR, len_sum-2 );
	//crc_calculate = HAL_CRC_Accumulate(&hcrc, (uint32_t *)FLASH_APP_PATCH_ADDR, len_sum-2);
	
	if(crc_read == crc_calculate)
	{
		return 1;
	}
	return 0;
}

////����
void iap_Process(void)
{
	uint16_t buff[1024];//����
	uint8_t i=0;
	uint32_t writeAddr=0,readAddr=0;
	writeAddr=FLASH_APP_PROGRAM_ADDR;
	readAddr=FLASH_APP_PATCH_ADDR;
	
	for(i=0; i<FLASH_APP_PROGRAM_PAGE; i++)
	{
		STMFLASH_Read(readAddr,buff,1024);
		STMFLASH_Write(writeAddr,buff,1024);
		writeAddr+=2048;
		readAddr+=2048;
	}
	
	ProductAppPassword=PRODUCT_APP_PASSWORD;
  STMFLASH_Write(FLASH_BOOT_PARAM_ADDR,(uint16_t*)&ProductAppPassword,2);
}



////����
void iap_Bootloader_Process(void)
{
	uint16_t buff[1024];//����
	uint8_t i=0;
	uint32_t writeAddr=0,readAddr=0;
	
	writeAddr=FLASH_BOOT_PROGRAM_ADDR;
	readAddr=FLASH_APP_PATCH_ADDR;
	
	if(Check_Pack_CRC())
	{
		IntxDisable(); // ���������ж�
		for(i=0; i<FLASH_BOOT_PROGRAM_PAGE; i++)
		{
			STMFLASH_Read(readAddr,buff,1024);
			STMFLASH_Write(writeAddr,buff,1024);
			writeAddr+=2048;
			readAddr+=2048;
		}
		IntxEnable();// ���ж�
	}
	else{
		
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);}
	ProductAppPassword = PRODUCT_APP_PASSWORD;
	STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PASSWORD,(uint16_t*)&ProductAppPassword,2);
}







