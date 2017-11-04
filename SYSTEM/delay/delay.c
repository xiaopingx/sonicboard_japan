#include "delay.h"
static u8  fac_us=0;            //us��ʱ������
extern u16 fac_ms=0;            //ms��ʱ������
void delay_init(u8 SYSCLK)      //ϵͳ���е�Ƶ��
{
    SysTick->CTRL&=~(1<<2);     //ʹ��CPU��ʱ����Ϊʱ��Դ22/2=11Mhz
    fac_us=SYSCLK/2;            //�˴��Ǻ������Ĳ�ͬ������NANO100ϵ��Ϊ8��Ƶ
    fac_ms=(u16)fac_us*1000;
}
void delay_us(u32 xus)
{
    u32 temp;	    	 
	SysTick->LOAD=xus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL=0x01 ;      //��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;                 //�رռ�����
	SysTick->VAL =0X00;                 //��ռ�����
}
void delay_ms(u16 xms)
{
	u32 temp;		   
	SysTick->LOAD=(u32)xms*fac_ms;      //ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;                 //��ռ�����
	SysTick->CTRL=0x01 ;                //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;                 //�رռ�����
	SysTick->VAL =0X00;                 //��ռ�����	
}
void delay_start(u16 xms)
{
	SysTick->LOAD=(u32)xms*fac_ms;      //ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;                 //��ռ�����
	SysTick->CTRL=0x01 ;                //��ʼ����  
}
void delay_end(void)
{		
    u32 temp;	
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;                 //�رռ�����
	SysTick->VAL =0X00;                 //��ռ�����	
}

