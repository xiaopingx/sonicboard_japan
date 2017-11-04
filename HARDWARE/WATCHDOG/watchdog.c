#include "watchdog.h"
#include "sys.h"
//���Ź���ʼ������
//����wtis(Watchdog Timeout Interval Select)��ȡֵ��ΧΪ0~7
//����clk����ȡֵΪ10K
//����ֵ1��ʧ�ܡ�0���ɹ�
u8 WATCHDOG_init(u8 wtis,u8 clk)
{
    if(wtis>8)
        return 1;
    SYS_UnlockReg();//����
    CLK->APBCLK|=1<<0;//ʹ�ܿ��Ź�ʱ��
    CLK->CLKSEL1&=~(3<<0);//���ԭ����ѡ��
    switch(clk)
    {
         case WDT_EX_XTAL:case WDT_EX_32K:
             CLK->CLKSEL1|=0<<0;
         break;
             
         case WDT_HCLKDIV2048://ʱ��Դ����HCLK/2048
             CLK->CLKSEL1|=2<<0;
         break;
         case IN_10K://ʱ��Դ�����ڲ�10K����
             CLK->CLKSEL1|=3<<0;
         break;         

    }
    WDT->WTCR&=~(1<<2);
    WDT->WTCR&=~(1<<3);
    WDT->WTCR&=~(7<<8);
    WDT->WTCR|=wtis<<8;
    WDT->WTCR|=1<<1;//���Ź���ʱ����λʹ��
    WDT->WTCR|=1<<7;//�������Ź�
    SYS_LockReg();//����
    return 0;//�ɹ�
    
}
void WATCHDOG_feeddog(void)
{
    SYS_UnlockReg();//����
    WDT->WTCR|=1<<0;//������Ź���������ֵ
    SYS_LockReg();//����
}




