#ifndef _WATCHDOG_H
#define _WATCHDOG_H
#include "sys.h"

#define WDT_2POW4      0          
#define WDT_2POW6      1          
#define WDT_2POW8      2         
#define WDT_2POW10     3          
#define WDT_2POW12     4          
#define WDT_2POW14     5         
#define WDT_2POW16     6     
#define WDT_2POW18     7
#define WDT_EX_XTAL             0
#define WDT_EX_32K              1
#define WDT_HCLKDIV2048         2
#define IN_10K                  3
u8 WATCHDOG_init(u8 wtis,u8 clk);
void WATCHDOG_feeddog(void);
#endif


