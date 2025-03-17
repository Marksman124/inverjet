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
#define FLASH_APP_PROGRAM_PAGE			(56)  					// app 程序升级包 大小 (页数 n*2048)

#define FLASH_APP_PATCH_ADDR				(0x08024000)  	// app 程序升级包 起始地址(存放在FLASH)
#define FLASH_APP_PARAM_ADDR				(0x0803E800)  	// app 参数 起始地址


#define FLASH_BOOT_PROGRAM_ADDR			(0x08000000)  	// boot 起始地址(存放在FLASH)
#define FLASH_BOOT_PROGRAM_PAGE			(15)  

#define FLASH_BOOT_PARAM_ADDR  			(0x08007800)  	// boot 程序参数 起始地址(存放在FLASH)

//	OTA 标志
#define BOOT_FLASH_ADDR_OTA_PASSWORD						(FLASH_BOOT_PARAM_ADDR)
//	升级包 大小
#define BOOT_FLASH_ADDR_OTA_PACK_LEN						(BOOT_FLASH_ADDR_OTA_PASSWORD + FLASH_ADDR_OFFSET_SIZE)

//	boot 软件版本
#define BOOT_FLASH_ADDR_BOOT_VERSION						(BOOT_FLASH_ADDR_OTA_PACK_LEN + FLASH_ADDR_OFFSET_SIZE)
#define FLASH_ADDR_OFFSET_BOOT_VERSION					(16)  					// 长度  （字符串存储）

//	app 程序升级包 起始地址
#define BOOT_FLASH_ADDR_OTA_PACK_ADDR						(BOOT_FLASH_ADDR_BOOT_VERSION + FLASH_ADDR_OFFSET_BOOT_VERSION)

//	app 程序升级包 大小
#define BOOT_FLASH_ADDR_OTA_PACK_SIZE						(BOOT_FLASH_ADDR_OTA_PACK_ADDR + FLASH_ADDR_OFFSET_SIZE)

//	app 参数 起始地址
#define BOOT_FLASH_ADDR_APP_PARAM_ADDR					(BOOT_FLASH_ADDR_OTA_PACK_SIZE + FLASH_ADDR_OFFSET_SIZE)

//	app 下载包大小
#define BOOT_FLASH_ADDR_DOWNLOAD_PACK_SIZE			(BOOT_FLASH_ADDR_APP_PARAM_ADDR + FLASH_ADDR_OFFSET_SIZE)


#define RANGE_OTA_PACK_LEN_MIN						(FLASH_BOOT_PROGRAM_PAGE * STM_FLASH_SECTOR_SIZE)		//	APP 升级包 最小值
#define RANGE_OTA_PACK_LEN_MAX						(FLASH_APP_PROGRAM_PAGE * STM_FLASH_SECTOR_SIZE)		//	APP 升级包 最大值

#define RANGE_BOOT_PACK_LEN_MIN						(STM_FLASH_SECTOR_SIZE)															//	boot 升级包 最大值
#define RANGE_BOOT_PACK_LEN_MAX						(FLASH_BOOT_PROGRAM_PAGE * STM_FLASH_SECTOR_SIZE)		//	boot 升级包 最大值

#define RECORD_LEN_PAGE_MAX		238

#define PRODUCT_APP_PASSWORD 				(0xFee1Dead)  	// feel dead  直接重启
#define PRODUCT_BOOT_PASSWORD 			(0x9070B007)  	// go to boot  进入升级


void iap_load_app(uint32_t appxaddr);			//跳转到APP程序执行
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//在指定地址开始,写入bin
void iap_Process(void);
void iap_Bootloader_Process(void);


#endif







































