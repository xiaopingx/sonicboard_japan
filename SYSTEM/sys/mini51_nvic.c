#include "mini51_nvic.h"
/*******外部中断专用中断******************/
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM)
{
    switch(GPIOx)
    {
        case 0://GPIOA
            P0->IMD&=~(1<<BITx);//边沿触发方式
            if(TRIM&0X01)//下降沿触发
                P0->IEN|=(1<<BITx);
            else if(TRIM&0X02)//上升沿触发
                P0->IEN|=(1<<(BITx+16));
        break;
            
        case 1://GPIOB
            P1->IMD&=~(1<<BITx);//边沿触发方式
            if(TRIM&0X01)//下降沿触发
                P1->IEN|=1<<BITx;
            else if(TRIM&0X02)//上升沿触发
                P1->IEN|=(1<<(BITx+16));  
        break;
            
        case 2://GPIOC
            P2->IMD&=~(1<<BITx);//边沿触发方式
            if(TRIM&0X01)//下降沿触发
                P2->IEN|=1<<BITx;
            else if(TRIM&0X02)//上升沿触发
                P2->IEN|=(1<<(BITx+16));  
        break;   
            
        case 3://GPIOD
            P3->IMD&=~(1<<BITx);//边沿触发方式
            if(TRIM&0X01)//下降沿触发
                P3->IEN|=1<<BITx;
            else if(TRIM&0X02)//上升沿触发
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
            P4->IMD&=~(1<<BITx);//边沿触发方式
            if(TRIM&0X01)//下降沿触发
                P4->IEN|=1<<BITx;
            else if(TRIM&0X02)//上升沿触发
                P4->IEN|=(1<<(BITx+16));  
        break;   
            
        case 5://GPIOF
            P5->IMD&=~(1<<BITx);//边沿触发方式
            if(TRIM&0X01)//下降沿触发
                P5->IEN|=1<<BITx;
            else if(TRIM&0X02)//上升沿触发
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


