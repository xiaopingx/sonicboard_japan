#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
struct TIM_Type
{
    void (*init)(u8,u32);
    u32 time0count; //定时器的计数只，每中断一次就计数一次
    u32 sendplus_time; //脉冲发出时间计数    
	  u32 sendplus_counter;  //脉冲发出后 时间计数
	  u32 cupcount[2];//加水计数
    u32 heatcount[2];
};
extern struct TIM_Type TIME_dev;
void TIM0_init(u8 pre,u32 tcmp);
void TIM1_init(u8 pre,u32 tcmp);
#endif

