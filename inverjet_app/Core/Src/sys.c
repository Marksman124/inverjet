/**
******************************************************************************
* @file    		sys.c
* @brief   		系统底层驱动
*
*
* @author			WQG
* @versions   v1.0
* @date   		2024-1-5
******************************************************************************
*/
#include "sys.h"

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WfiSet(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void IntxDisable(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void IntxEnable(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MsrMsp(uint32_t addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
//软件复位  
void SysSoftReset(void)
{   
	SCB->AIRCR =0X05FA0000|(uint32_t)0x04;	  
} 	

