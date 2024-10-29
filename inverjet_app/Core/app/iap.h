/**
******************************************************************************
* @file    		iap.h
* @brief   		iap接口及flash地址定义
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
#ifndef __IAP_H__
#define __IAP_H__

#include "stmflash.h"

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.

#define FLASH_ADDR_OFFSET_SIZE			(4)  					// 每个数据存储偏移量
#define STM_FLASH_SECTOR_SIZE				(2048)

#define FLASH_APP_PROGRAM_ADDR			(0x08008000)  	// app 程序 起始地址(存放在FLASH)
#define FLASH_APP_PROGRAM_PAGE			(55)  					// app 程序升级包 大小 (页数 n*2048)
#define FLASH_APP_PATCH_ADDR				(0x08023800)  	// app 程序升级包 起始地址(存放在FLASH)

#define FLASH_APP_PARAM_ADDR				(0x0803F000)  	// app 程序参数 起始地址(存放在FLASH)
#define USER_FLASH_ADDR_SOFTWARE_VERSION			(USER_FLASH_ADDR_BAUD_RATE + (21*FLASH_ADDR_OFFSET_SIZE))		//	软件版本

//只可用于读
#define USER_FLASH_ADDR_LOCAL_ADDR						(FLASH_APP_PARAM_ADDR)			//	本地地址
#define USER_FLASH_ADDR_BAUD_RATE							(USER_FLASH_ADDR_LOCAL_ADDR + 2)		//	波特率



#define FLASH_BOOT_PARAM_ADDR  			(0x08007800)  	// boot 程序参数 起始地址(存放在FLASH)
#define BOOT_FLASH_ADDR_OTA_PASSWORD						(FLASH_BOOT_PARAM_ADDR)			//	OTA标识
#define BOOT_FLASH_ADDR_OTA_PACK_LEN						(FLASH_BOOT_PARAM_ADDR + FLASH_ADDR_OFFSET_SIZE)		//	包长度

#define RANGE_OTA_PACK_LEN_MIN						(10 * STM_FLASH_SECTOR_SIZE)		//	包长度 最小
#define RANGE_OTA_PACK_LEN_MAX						(FLASH_APP_PROGRAM_PAGE * STM_FLASH_SECTOR_SIZE)		//	包长度 最大

#define RECORD_LEN_PAGE_MAX		238

#define PRODUCT_APP_PASSWORD 				(0xFee1Dead)  	// feel dead  直接重启
#define PRODUCT_BOOT_PASSWORD 			(0x9070B007)  	// go to boot  进入升级


void iap_load_app(uint32_t appxaddr);			//跳转到APP程序执行
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//在指定地址开始,写入bin
void iap_Process(void);

#endif







































