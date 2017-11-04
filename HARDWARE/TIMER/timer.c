#include "timer.h"
#include "sys.h"
#include "hardware.h"
#include "mini51eeprom.h"
struct TIM_Type TIME_dev={
    TIM0_init,
    0,      //��ʱ���ļ���ֻ��ÿ�ж�һ�ξͼ���һ��
    0,0,    //cup����
    0,0,    //heatcount
};
//****T=1/22*(8Ԥ��Ƶ+1��*��24bitTcmp)
void TIM0_init(u8 pre,u32 tcmp)
{
    CLK->CLKSEL1&=~(7<<8);      //�����ʱ��ʱ��Դ����
    CLK->CLKSEL1|=2<<8;         //�Ѷ�ʱ��0ʱ��Դ����ΪHCLK      
    CLK->APBCLK|=1<<2;          //�򿪶�ʱ��0ʱ��
    TIMER0->TCSR&=~(3<<27);
    TIMER0->TCSR|=1<<27;        //����ģʽ   
    TIMER0->TCSR|=1<<29;        //�����ж�    
    TIMER0->TCSR|=pre;          //Ԥ��Ƶ������
    TIMER0->TCMPR=tcmp;         //��������
    MY_NVIC_Init(TMR0_IRQn,3);  //�����ж����ȼ�
    TIMER0->TCSR|=1<<30;        //������ʱ��0
}
void TIM1_init(u8 pre,u32 tcmp)
{
    CLK->CLKSEL1&=~(7<<12);      //�����ʱ��ʱ��Դ����
    CLK->CLKSEL1|=2<<12;         //�Ѷ�ʱ��0ʱ��Դ����ΪHCLK      
    CLK->APBCLK|=1<<3;          //�򿪶�ʱ��0ʱ��
    TIMER1->TCSR&=~(3<<27);
    TIMER1->TCSR|=1<<27;        //����ģʽ   
    TIMER1->TCSR|=1<<29;        //�����ж�    
    TIMER1->TCSR|=pre;          //Ԥ��Ƶ������
    TIMER1->TCMPR=tcmp;         //��������
    MY_NVIC_Init(TMR1_IRQn,3);  //�����ж����ȼ�
    TIMER1->TCSR|=1<<30;        //������ʱ��0
}

//*50usһ��
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
    TIMER1->TISR|=1<<0;//�����־λ
}
//********************************************************************************************//
