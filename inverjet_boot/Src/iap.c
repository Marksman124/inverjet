
#include "iap.h"
#include "stdint.h"  
#include "stmflash.h"
#include "gpio.h"
#include "mbcrc.h"
//#include "wwdg.h"


iapfun jump2app; 
uint16_t iapbuf[1024];   
uint16_t buff[1024];//缓冲
uint16_t pack_crc=0;

//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t appsize)
{
	uint16_t t;
	uint16_t i=0;
	uint16_t temp;
	uint32_t fwaddr=appxaddr;//当前写入的地址
	uint8_t *dfu=appbuf;
	for(t=0;t<appsize;t+=2)
	{						    
		temp=(uint16_t)dfu[1]<<8;
		temp+=(uint16_t)dfu[0];	  
		dfu+=2;//偏移2个字节
		iapbuf[i++]=temp;
		if(i==1024)
		{
			i=0;
			STMFLASH_Write(fwaddr,iapbuf,1024);	
			fwaddr+=2048;//偏移2048  16=2*8.所以要乘以2.
		}
	}
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.  
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(uint32_t appxaddr)
{
	uint32_t jump_address;
	
	if(((*(uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		HAL_DeInit();
		__HAL_RCC_SYSCFG_CLK_DISABLE();
		
		IntxDisable();
		__disable_irq();
		
		//HAL_Delay(500);
		
		
		jump_address = *(uint32_t*)(appxaddr + 4);
		jump2app=(iapfun)jump_address;		//用户代码区第二个字为程序开始地址(复位地址)		
		MsrMsp(*(uint32_t*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}

//未验证
uint8_t Check_Pack_CRC(void)
{
	uint32_t len_sum=0;
	static uint16_t crc_read, crc_calculate;
	
	len_sum = *(uint32_t *)BOOT_FLASH_ADDR_OTA_PACK_LEN;
	if( ( len_sum < RANGE_OTA_PACK_LEN_MIN ) || ( len_sum > RANGE_OTA_PACK_LEN_MAX  ))
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

////升级
void iap_Process(void)
{
	uint8_t i=0;
	uint32_t writeAddr=0,readAddr=0,readSize=0;
	uint32_t sign=0;
	
	writeAddr=FLASH_APP_PROGRAM_ADDR;
	
	readAddr = *(uint32_t *)BOOT_FLASH_ADDR_OTA_PACK_ADDR;
	//readAddr = FLASH_APP_PATCH_ADDR;
	
	readSize = *(uint32_t *)BOOT_FLASH_ADDR_OTA_PACK_SIZE;
	if(readSize > FLASH_APP_PROGRAM_PAGE)
		return;
	
	//HAL_WWDG_Refresh(&hwwdg);
	if(Check_Pack_CRC())
	{
		IntxDisable(); // 禁用所有中断
		for(i=0; i<readSize; i++)
		{
			STMFLASH_Read(readAddr,buff,1024);
			STMFLASH_Write(writeAddr,buff,1024);
			writeAddr+=2048;
			readAddr+=2048;
		}
		IntxEnable();// 开中断
	}
	else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);;}
	sign = PRODUCT_APP_PASSWORD;
	STMFLASH_Write(BOOT_FLASH_ADDR_OTA_PASSWORD,(uint16_t*)&sign,2);

}











