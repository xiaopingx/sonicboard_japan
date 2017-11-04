#ifndef _NVIC_H
#define _NVIC_H
// #include "core_cm0.h"//此头文件中定义了NVIC的结构体
#include "Mini51series.h"//此头文件中定义了NVIC的中断号码
/**********外部中断专用定义*********************/
#define P_0 0
#define P_1 1
#define P_2 2
#define P_3 3
#define P_4 4
#define P_5 5
#define FTIR   1  //下降沿触发
#define RTIR   2  //上升沿触发
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);
void Ex_NVIC_EINT0(u8 TRIM);
#endif


