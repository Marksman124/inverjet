#include "sys.h"

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WfiSet(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void IntxDisable(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void IntxEnable(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MsrMsp(uint32_t addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
//�����λ  
void SysSoftReset(void)
{   
	SCB->AIRCR =0X05FA0000|(uint32_t)0x04;	  
} 	

