/**
******************************************************************************
* @file    		iap.h
* @brief   		iap�ӿڼ�flash��ַ����
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

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.

#define FLASH_ADDR_OFFSET_SIZE			(4)  					// ÿ�����ݴ洢ƫ����
#define STM_FLASH_SECTOR_SIZE				(2048)

#define FLASH_APP_PROGRAM_ADDR			(0x08008000)  	// app ���� ��ʼ��ַ(�����FLASH)
#define FLASH_APP_PROGRAM_PAGE			(56)  					// app ���������� ��С (ҳ�� n*2048)

#define FLASH_APP_PATCH_ADDR				(0x08024000)  	// app ���������� ��ʼ��ַ(�����FLASH)
#define FLASH_APP_PARAM_ADDR				(0x0803E800)  	// app ������� ��ʼ��ַ(�����FLASH)

#define USER_FLASH_ADDR_SOFTWARE_VERSION			(USER_FLASH_ADDR_BAUD_RATE + (21*FLASH_ADDR_OFFSET_SIZE))		//	����汾


#define FLASH_BOOT_PROGRAM_ADDR			(0x08000000)  	// boot ���� ��ʼ��ַ(�����FLASH)
#define FLASH_BOOT_PROGRAM_PAGE			(15)  

#define FLASH_BOOT_PARAM_ADDR  			(0x08007800)  	// boot ������� ��ʼ��ַ(�����FLASH)

//	OTA��ʶ
#define BOOT_FLASH_ADDR_OTA_PASSWORD						(FLASH_BOOT_PARAM_ADDR)
//	������
#define BOOT_FLASH_ADDR_OTA_PACK_LEN						(BOOT_FLASH_ADDR_OTA_PASSWORD + FLASH_ADDR_OFFSET_SIZE)

//	boot�汾��
#define BOOT_FLASH_ADDR_BOOT_VERSION						(BOOT_FLASH_ADDR_OTA_PACK_LEN + FLASH_ADDR_OFFSET_SIZE)
#define FLASH_ADDR_OFFSET_BOOT_VERSION					(16)  					// ���ݴ洢ƫ����

//	���������� ��ַ
#define BOOT_FLASH_ADDR_OTA_PACK_ADDR						(BOOT_FLASH_ADDR_BOOT_VERSION + FLASH_ADDR_OFFSET_BOOT_VERSION)

//	���������� ��С
#define BOOT_FLASH_ADDR_OTA_PACK_SIZE						(BOOT_FLASH_ADDR_OTA_PACK_ADDR + FLASH_ADDR_OFFSET_SIZE)

//	app ������� ��ַ
#define BOOT_FLASH_ADDR_APP_PARAM_ADDR					(BOOT_FLASH_ADDR_OTA_PACK_SIZE + FLASH_ADDR_OFFSET_SIZE)



#define RANGE_OTA_PACK_LEN_MIN						(FLASH_BOOT_PROGRAM_PAGE * STM_FLASH_SECTOR_SIZE)		//	������ ��С
#define RANGE_OTA_PACK_LEN_MAX						(FLASH_APP_PROGRAM_PAGE * STM_FLASH_SECTOR_SIZE)		//	������ ���

#define RANGE_BOOT_PACK_LEN_MIN						(STM_FLASH_SECTOR_SIZE)		//	������ ��С
#define RANGE_BOOT_PACK_LEN_MAX						(FLASH_BOOT_PROGRAM_PAGE * STM_FLASH_SECTOR_SIZE)		//	������ ���

#define RECORD_LEN_PAGE_MAX		238

#define PRODUCT_APP_PASSWORD 				(0xFee1Dead)  	// feel dead  ֱ������
#define PRODUCT_BOOT_PASSWORD 			(0x9070B007)  	// go to boot  ��������


void iap_load_app(uint32_t appxaddr);			//��ת��APP����ִ��
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//��ָ����ַ��ʼ,д��bin
void iap_Process(void);
void iap_Bootloader_Process(void);


#endif







































