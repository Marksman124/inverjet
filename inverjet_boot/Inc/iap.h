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

#define FLASH_APP_PARAM_ADDR				(0x08007C00)  	// app ������� ��ʼ��ַ(�����FLASH)
#define USER_FLASH_ADDR_SOFTWARE_VERSION			(USER_FLASH_ADDR_BAUD_RATE + (21*FLASH_ADDR_OFFSET_SIZE))		//	����汾

//ֻ�����ڶ�
#define USER_FLASH_ADDR_LOCAL_ADDR						(FLASH_APP_PARAM_ADDR)			//	���ص�ַ
#define USER_FLASH_ADDR_BAUD_RATE							(USER_FLASH_ADDR_LOCAL_ADDR + 2)		//	������



#define FLASH_BOOT_PARAM_ADDR  			(0x08007800)  	// boot ������� ��ʼ��ַ(�����FLASH)
#define BOOT_FLASH_ADDR_OTA_PASSWORD						(FLASH_BOOT_PARAM_ADDR)			//	OTA��ʶ
#define BOOT_FLASH_ADDR_OTA_PACK_LEN						(FLASH_BOOT_PARAM_ADDR + FLASH_ADDR_OFFSET_SIZE)		//	������

#define RANGE_OTA_PACK_LEN_MIN						(10 * STM_FLASH_SECTOR_SIZE)		//	������ ��С
#define RANGE_OTA_PACK_LEN_MAX						(FLASH_APP_PROGRAM_PAGE * STM_FLASH_SECTOR_SIZE)		//	������ ���

//#define RECORD_LEN_PAGE_MAX		238

#define PRODUCT_APP_PASSWORD 				(0xFee1Dead)  	// feel dead  ֱ������
#define PRODUCT_BOOT_PASSWORD 			(0x9070B007)  	// go to boot  ��������


void iap_load_app(uint32_t appxaddr);			//��ת��APP����ִ��
void iap_write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//��ָ����ַ��ʼ,д��bin
void iap_Process(void);

#endif







































