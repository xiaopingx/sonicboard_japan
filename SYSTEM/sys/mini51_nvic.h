#ifndef _NVIC_H
#define _NVIC_H
// #include "core_cm0.h"//��ͷ�ļ��ж�����NVIC�Ľṹ��
#include "Mini51series.h"//��ͷ�ļ��ж�����NVIC���жϺ���
/**********�ⲿ�ж�ר�ö���*********************/
#define P_0 0
#define P_1 1
#define P_2 2
#define P_3 3
#define P_4 4
#define P_5 5
#define FTIR   1  //�½��ش���
#define RTIR   2  //�����ش���
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);
void Ex_NVIC_EINT0(u8 TRIM);
#endif


