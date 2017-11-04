#include "delay.h"
static u8  fac_us=0;            //us延时倍乘数
extern u16 fac_ms=0;            //ms延时倍乘数
void delay_init(u8 SYSCLK)      //系统运行的频率
{
    SysTick->CTRL&=~(1<<2);     //使用CPU的时钟作为时钟源22/2=11Mhz
    fac_us=SYSCLK/2;            //此处是和其他的不同，例如NANO100系列为8分频
    fac_ms=(u16)fac_us*1000;
}
void delay_us(u32 xus)
{
    u32 temp;	    	 
	SysTick->LOAD=xus*fac_us; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;      //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;                 //关闭计数器
	SysTick->VAL =0X00;                 //清空计数器
}
void delay_ms(u16 xms)
{
	u32 temp;		   
	SysTick->LOAD=(u32)xms*fac_ms;      //时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;                 //清空计数器
	SysTick->CTRL=0x01 ;                //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;                 //关闭计数器
	SysTick->VAL =0X00;                 //清空计数器	
}
void delay_start(u16 xms)
{
	SysTick->LOAD=(u32)xms*fac_ms;      //时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;                 //清空计数器
	SysTick->CTRL=0x01 ;                //开始倒数  
}
void delay_end(void)
{		
    u32 temp;	
	do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;                 //关闭计数器
	SysTick->VAL =0X00;                 //清空计数器	
}

