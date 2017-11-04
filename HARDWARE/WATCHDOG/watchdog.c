#include "watchdog.h"
#include "sys.h"
//开门狗初始化函数
//参数wtis(Watchdog Timeout Interval Select)的取值范围为0~7
//参数clk建议取值为10K
//返回值1：失败。0：成功
u8 WATCHDOG_init(u8 wtis,u8 clk)
{
    if(wtis>8)
        return 1;
    SYS_UnlockReg();//解锁
    CLK->APBCLK|=1<<0;//使能看门狗时钟
    CLK->CLKSEL1&=~(3<<0);//清除原来的选择
    switch(clk)
    {
         case WDT_EX_XTAL:case WDT_EX_32K:
             CLK->CLKSEL1|=0<<0;
         break;
             
         case WDT_HCLKDIV2048://时钟源来自HCLK/2048
             CLK->CLKSEL1|=2<<0;
         break;
         case IN_10K://时钟源来自内部10K振荡器
             CLK->CLKSEL1|=3<<0;
         break;         

    }
    WDT->WTCR&=~(1<<2);
    WDT->WTCR&=~(1<<3);
    WDT->WTCR&=~(7<<8);
    WDT->WTCR|=wtis<<8;
    WDT->WTCR|=1<<1;//看门狗定时器复位使能
    WDT->WTCR|=1<<7;//开启看门狗
    SYS_LockReg();//上锁
    return 0;//成功
    
}
void WATCHDOG_feeddog(void)
{
    SYS_UnlockReg();//解锁
    WDT->WTCR|=1<<0;//清除看门狗计数器的值
    SYS_LockReg();//上锁
}




