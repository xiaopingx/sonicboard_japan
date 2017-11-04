#include "timer.h"
#include "sys.h"
#include "hardware.h"
#include "mini51eeprom.h"
struct TIM_Type TIME_dev={
    TIM0_init,
    0,      //定时器的计数只，每中断一次就计数一次
    0,0,    //cup计数
    0,0,    //heatcount
};
//****T=1/22*(8预分频+1）*（24bitTcmp)
void TIM0_init(u8 pre,u32 tcmp)
{
    CLK->CLKSEL1&=~(7<<8);      //清除定时器时钟源设置
    CLK->CLKSEL1|=2<<8;         //把定时器0时钟源设置为HCLK      
    CLK->APBCLK|=1<<2;          //打开定时器0时钟
    TIMER0->TCSR&=~(3<<27);
    TIMER0->TCSR|=1<<27;        //周期模式   
    TIMER0->TCSR|=1<<29;        //开启中断    
    TIMER0->TCSR|=pre;          //预分频器设置
    TIMER0->TCMPR=tcmp;         //计数周期
    MY_NVIC_Init(TMR0_IRQn,3);  //设置中断优先级
    TIMER0->TCSR|=1<<30;        //开启定时器0
}
void TIM1_init(u8 pre,u32 tcmp)
{
    CLK->CLKSEL1&=~(7<<12);      //清除定时器时钟源设置
    CLK->CLKSEL1|=2<<12;         //把定时器0时钟源设置为HCLK      
    CLK->APBCLK|=1<<3;          //打开定时器0时钟
    TIMER1->TCSR&=~(3<<27);
    TIMER1->TCSR|=1<<27;        //周期模式   
    TIMER1->TCSR|=1<<29;        //开启中断    
    TIMER1->TCSR|=pre;          //预分频器设置
    TIMER1->TCMPR=tcmp;         //计数周期
    MY_NVIC_Init(TMR1_IRQn,3);  //设置中断优先级
    TIMER1->TCSR|=1<<30;        //开启定时器0
}

//*50us一次
void TMR1_IRQHandler(void)
{ 
    if(TIMER1->TISR&(1<<0))
    {
//         TIME_dev.time0count++;
/*********************************************************************/
		if(CMD_CUP_BIT0 == 0){
			SONIC.cmd.detect.counter_bit0[0] ++;
			SONIC.cmd.detect.counter_bit0[1] = 0;
			if(SONIC.cmd.detect.counter_bit0[0] > 150){
				SONIC.cmd.detect.counter_bit0[0] = 0;
				SONIC.cmd.detect.bit[0] = 0;
			}
		}else{
			SONIC.cmd.detect.counter_bit0[1] ++;
			SONIC.cmd.detect.counter_bit0[0] = 0;
			if(SONIC.cmd.detect.counter_bit0[1] > 200){
				SONIC.cmd.detect.counter_bit0[1] = 0;
				SONIC.cmd.detect.bit[0] = 1;
			}
		}
		if(CMD_CUP_BIT1 == 0){
			SONIC.cmd.detect.counter_bit1[0] ++;
			SONIC.cmd.detect.counter_bit1[1] = 0;
			if(SONIC.cmd.detect.counter_bit1[0] > 150){
				SONIC.cmd.detect.counter_bit1[0] = 0;
				SONIC.cmd.detect.bit[1] = 0;
			}
		}else{
			SONIC.cmd.detect.counter_bit1[1] ++;
			SONIC.cmd.detect.counter_bit1[0] = 0;
			if(SONIC.cmd.detect.counter_bit1[1] > 200){
				SONIC.cmd.detect.counter_bit1[1] = 0;
				SONIC.cmd.detect.bit[1] = 1;
			}
		}
/*********************************************************************/		
		if(IS_PHYSIC_CUP_INPUT == 0){
			SONIC.physiccup.counter[0] ++;
			SONIC.physiccup.counter[1] = 0;
			if(SONIC.physiccup.counter[0] > 150){
				SONIC.physiccup.counter[0] = 0;
				SONIC.physiccup.exist = 0;
			}
		}else{
			SONIC.physiccup.counter[1] ++;
			SONIC.physiccup.counter[0] = 0;
			if(SONIC.physiccup.counter[1] > 200){
				SONIC.physiccup.counter[1] = 0;
				SONIC.physiccup.exist = 1;
			}
		}
/*********************************************************************/			
    }
    TIMER1->TISR|=1<<0;//清除标志位
}
//********************************************************************************************//
