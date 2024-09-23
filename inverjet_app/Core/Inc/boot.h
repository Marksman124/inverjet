/**
******************************************************************************
* @file    		boot.h
* @brief   		boot 接口
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
#ifndef BOOT_H
#define BOOT_H


#include "blt_conf.h"                          /* bootloader configuration     */
#include "stm32f1xx.h"                         /* STM32 registers and drivers  */
#include "timer.h"                             /* Timer driver                 */

/****************************************************************************************
* Function prototypes
****************************************************************************************/
void BootComInit(void);
void BootComCheckActivationRequest(void);
void BootActivate(void);


#endif /* BOOT_H */
/*********************************** end of boot.h *************************************/
