#ifndef _DELAY_H
#define _DELAY_H
#include "mini51series.h"
void delay_init(u8 SYSCLK);
void delay_us(u32 xus);
void delay_ms(u16 xms);
void delay_start(u16 xms);
void delay_end(void);
#endif
