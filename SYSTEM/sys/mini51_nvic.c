#include "mini51_nvic.h"
/*******�ⲿ�ж�ר���ж�******************/
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM)
{
    switch(GPIOx)
    {
        case 0://GPIOA
            P0->IMD&=~(1<<BITx);//���ش�����ʽ
            if(TRIM&0X01)//�½��ش���
                P0->IEN|=(1<<BITx);
            else if(TRIM&0X02)//�����ش���
                P0->IEN|=(1<<(BITx+16));
        break;
            
        case 1://GPIOB
            P1->IMD&=~(1<<BITx);//���ش�����ʽ
            if(TRIM&0X01)//�½��ش���
                P1->IEN|=1<<BITx;
            else if(TRIM&0X02)//�����ش���
                P1->IEN|=(1<<(BITx+16));  
        break;
            
        case 2://GPIOC
            P2->IMD&=~(1<<BITx);//���ش�����ʽ
            if(TRIM&0X01)//�½��ش���
                P2->IEN|=1<<BITx;
            else if(TRIM&0X02)//�����ش���
                P2->IEN|=(1<<(BITx+16));  
        break;   
            
        case 3://GPIOD
            P3->IMD&=~(1<<BITx);//���ش�����ʽ
            if(TRIM&0X01)//�½��ش���
                P3->IEN|=1<<BITx;
            else if(TRIM&0X02)//�����ش���
                P3->IEN|=(1<<(BITx+16));  
            if(BITx==2)
            {
                SYS->P3_MFP&=~(1<<2);
                SYS->P3_MFP&=~(1<<10);
                SYS->P3_MFP|=1<<10;
                SYS->P3_MFP|=0<<2;
            }
        break;        
            
        case 4://GPIOE
            P4->IMD&=~(1<<BITx);//���ش�����ʽ
            if(TRIM&0X01)//�½��ش���
                P4->IEN|=1<<BITx;
            else if(TRIM&0X02)//�����ش���
                P4->IEN|=(1<<(BITx+16));  
        break;   
            
        case 5://GPIOF
            P5->IMD&=~(1<<BITx);//���ش�����ʽ
            if(TRIM&0X01)//�½��ش���
                P5->IEN|=1<<BITx;
            else if(TRIM&0X02)//�����ش���
                P5->IEN|=(1<<(BITx+16));  
            if(BITx==2)
            {
                SYS->P5_MFP&=~(1<<2);
                SYS->P5_MFP&=~(1<<10);
                SYS->P5_MFP|=1<<10;
                SYS->P5_MFP|=0<<2;
            }            
        break;        
            
    }
}


