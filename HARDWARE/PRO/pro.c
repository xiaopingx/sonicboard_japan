#include "pro.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

TypeDef_PRO PRO_dev=
{
    0,
    0,
	0,
    0,
    0,
    0,
    0x01,	
    0,

};
/***********************键初始化函数******************************/
void key_init(void)
{
		P0->PMD=0x00005500; 		    //P0.7~P0.4=OUT  01010101
		P1->PMD=0x00000040;             //P1.5=IN P1.4=IN P1.3=OUT(TX) P1.2=IN(RX)
		P2->PMD=0x00000500;             //P2.5=OUT P2.4=OUT 
		P3->PMD=0x00000400;             //P3.5=OUT P3.4=IN P3.2=IN		
		P5->PMD=0x00000005;             //P5.1=OUT P50		
}
/***************************************************************/

/********************键值读入函数**********************************/
u32 injz(u16 jpn)  
{
    u32 i=0xffffffff;  
    u32 temp=0x00; 
    P24=0,P35=0,P51=0;//先灯关闭  
    if(P14==0) 
              i=i&~0x01;
    if(P15==0) 
             i=i&~0x02;
    if(P32==0) 
              i=i&~0x04;
    if(P34==0) 
              i=i&~0x08;
    temp=~i;
    temp=(temp<<(jpn*4))&0x000fffff;	
    outdz(jpn);
    i=~temp;
    i&=0x000fffff;
    return(i);//返回值相应位为1
}
/*************************************************************/

/***********************等值输入函数**************************/
//jpn为需要列，范围为0~4
void outdz(u16 jpn)  
{
	    u16 i=0;	
        u16 dz[5]={0}; 
	    dz[jpn]=(PRO_dev.DZ>>jpn*4)&0x0f;   //读取灯值     
    	P04=1,P05=1,P06=1,P07=1,P25=1;      //先灯关闭 ,列线 
		P24=0,P35=0,P51=0;                  //行线
	   	i=dz[jpn];
		if(i&0x02) P24=1;
		if(i&0x04) P35=1;
		if(i&0x08) P51=1;		 		
		switch(jpn)//列线
		{
			case 0:
				  P04=0;
				  break;
			case 1:
				  P05=0;
				  break;    
			case 2:
				  P06=0;
				  break;
			case 3:
				  P07=0;
				  break;   
			case 4:
				  P25=0;
				  break;   			
		 }
}
/***************************************************************/

//主机主动发送通信请求
//返回值0，说明没有通信请求或者通信失败
//返回值1，说明通信成功
u8 main_to_slave(void)
{
//     u8 dz[6]={0};
    u8 jz[6]={0};
    jz[0]=PRO_dev.JZ&0xff;
    jz[1]=(PRO_dev.JZ>>8)&0xff;
    jz[2]=(PRO_dev.JZ>>16)&0xff;
    jz[3]=(jz[0] ^ jz[1] ^ jz[2]);
    
//     dz[0]=PRO_dev.DZ&0xff;
//     dz[1]=(PRO_dev.DZ>>8)&0xff;
//     dz[2]=(PRO_dev.DZ>>16)&0xff;
//     dz[3]=(dz[0] ^ dz[1] ^ dz[2]);
 //   temp=PRO_dev.DZ;

    jz[4]=0x0d;
    jz[5]=0x0a;
    UART_send(jz,6);//发送数据函数
    return 1;//通讯成功
}
//从机主动发送通信请求
//返回值0，说明没有通信请求或者通信失败
//返回值1，说明通信成功
u8 slave_to_main(void)
{
    u16 i=0;
    u32 temp=0;
    u8 jz[4]={0};
    KEY1_1_INMODE();//设置为输入模式
    KEY1_2_OUTMODE();//设置为输出模式
    if(KEY1_1==0)   
    {
        KEY1_2=0;//拉低应答
        delay_us(10);
        while(!KEY1_1);//等待拉高，若在500ms内未出现拉高，那么就认定此次通信无效
        //开始接受数据
        KEY1_2=1;
        delay_us(10);
        for(i=0;i<32;i++)
        {
            KEY1_2=1;
            delay_us(100);            
            KEY1_2=0;
            if(KEY1_1)
                temp|=1<<i;
            else
                temp|=0<<i;
            delay_us(100); 
        }
        jz[0]=temp&0xff;
        jz[1]=(temp>>8)&0xff;
        jz[2]=(temp>>16)&0xff;
        jz[3]=(temp>>24)&0xff;
        if(jz[3]==(jz[0] ^ jz[1] ^ jz[2]))//判断是否接收正确
        {
            PRO_dev.JZ=temp&0x00ffffff;         
            KEY1_1_INMODE();
            KEY1_2_OUTMODE();
            KEY1_1=1;//完成后把两根线都拉高
            KEY1_2=1;
            PRO_dev.flag|=0x02;//已接收到数据标志位
            return 1;//通讯成功
        }
    }  
    return 0;//通讯失败或者没有请求
}
void reciv_deal(void)
{
    u32 temp=PRO_dev.JZ;
    u8 t=0;
    u8 sum=0;
    u8 position=0;//记录0所在的位置
    for(t=0;t<24;t++)//统计零的个数
    {
        if((temp&0x01)==0)
        {
            sum++;
        }
        temp>>=1;
    }
    if(PRO_dev.JZ!=PRO_dev.JZ0)//判断本次接收的数据和上一次接受的数据不一致
    {
        PRO_dev.JZ0=PRO_dev.JZ;//JZ0为上一次接受到的数据，JZ为本次接收到的数据
        PRO_dev.notsame_flag=1;//不同的标志位
    }
    else//相同不进行处理，直接返回
        return;
  
    if(PRO_dev.notsame_flag==1)//已看到本次接受的数据不同
    {
        if(sum!=1)          //接受到的数据零的个数不为1，无效
            return;         //直接返回
        temp=PRO_dev.JZ;          
        for(t=0;t<24;t++)//统计零的个数
        {
            if((temp&0x01)==0)
            {
                position=t;//记录0所在的位置
                break;
            }
            temp>>=1;
        }        
        switch(position)
        {
            case 0:
                if(PRO_dev.stat_flag&(1<0))//之前灯为亮，本次按下后灯灭
                { 
                    PRO_dev.DZ&=~LIGHT_A;
                    main_to_slave();
                    delay_ms(2);
                    main_to_slave();   
                    delay_ms(2);
                    main_to_slave(); 
                    delay_ms(2);
                    main_to_slave(); 
                    delay_ms(2);
                    PRO_dev.stat_flag&=~(1<0);
                }
                else        //之前灯灭，本次按下后灯亮
                {   
                    PRO_dev.DZ|=LIGHT_A;
                    main_to_slave();
                    delay_ms(2);
                    main_to_slave();   
                    delay_ms(2);
                    main_to_slave(); 
                    delay_ms(2);
                    main_to_slave(); 
                    delay_ms(2);
                    PRO_dev.stat_flag|=1<0;
                }
            break;
            case 1:
                if(PRO_dev.stat_flag&(1<1))
                { 
                    PRO_dev.DZ&=~LIGHT_CUP;
                    main_to_slave();
                    delay_ms(2);
                    main_to_slave();   
                    delay_ms(2);
                    main_to_slave(); 
                    delay_ms(2);
                    main_to_slave(); 
                    delay_ms(2);  
                    PRO_dev.stat_flag&=~(1<1);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_CUP;
                    main_to_slave();
                    delay_ms(2);
                    main_to_slave();   
                    delay_ms(2);
                    main_to_slave(); 
                    delay_ms(2);
                    main_to_slave(); 
                    delay_ms(2);
                    PRO_dev.stat_flag|=1<1;
                }                        
            break;
            case 2:
                if(PRO_dev.stat_flag&(1<2))//
                { 
                    PRO_dev.DZ&=~LIGHT_S1;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<2);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_S1;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<2;
                }                        
            break;
            case 3:
                if(PRO_dev.stat_flag&(1<3))//
                { 
                    PRO_dev.DZ&=~LIGHT_SPITE;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<3);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_SPITE;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<3;
                }                                      
            break;
            case 4:
                if(PRO_dev.stat_flag&(1<4))//
                { 
                    PRO_dev.DZ&=~LIGHT_B;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<4);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_B;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<4;
                }                
            break;
            case 5:
                if(PRO_dev.stat_flag&(1<5))//
                { 
                    PRO_dev.DZ&=~LIGHT_WATER;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<5);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_WATER;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<5;
                }                
            break;
            case 6:
                if(PRO_dev.stat_flag&(1<6))//
                { 
                    PRO_dev.DZ&=~LIGHT_S2;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<6);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_S2;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<6;
                }                 
            break;
            /*****************************************************/
            case 8:
                if(PRO_dev.stat_flag&(1<8))//
                { 
                    PRO_dev.DZ&=~LIGHT_A;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<8);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_A;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<8;
                } 
            break;
            case 9:
                if(PRO_dev.stat_flag&(1<9))//
                { 
                    PRO_dev.DZ&=~LIGHT_HEAT;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<9);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_HEAT;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<9;
                } 
            break;                
            case 10:
                 if(PRO_dev.stat_flag&(1<10))//
                { 
                    PRO_dev.DZ&=~LIGHT_S3;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<10);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_S3;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<10;
                } 
            break;
            case 12:
                if(PRO_dev.stat_flag&(1<12))//
                { 
                    PRO_dev.DZ&=~LIGHT_B;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<12);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_B;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<12;
                } 
            break;
            case 13:
                if(PRO_dev.stat_flag&(1<13))//
                { 
                    PRO_dev.DZ&=~LIGHT_S0;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<13);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_S0;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<13;
                } 
            break;
            case 14:
                if(PRO_dev.stat_flag&(1<14))//
                { 
                    PRO_dev.DZ&=~LIGHT_WATER;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<14);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_WATER;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<14;
                } 
            break;
            case 15:
                if(PRO_dev.stat_flag&(1<15))//
                { 
                    PRO_dev.DZ&=~LIGHT_C;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<15);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_C;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<15;
                } 
            break;
             /*****************************************************/
            case 16:
                if(PRO_dev.stat_flag&(1<16))//
                { 
                    PRO_dev.DZ&=~LIGHT_C;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<16);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_C;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<16;
                }                   
            break;
            case 17:
                if(PRO_dev.stat_flag&(1<17))//
                { 
                    PRO_dev.DZ&=~LIGHT_SET;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<17);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_SET;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<17;
                }                   
            break;
            case 18:
                if(PRO_dev.stat_flag&(1<18))//
                { 
                    PRO_dev.DZ&=~LIGHT_LIGHT;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<18);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_LIGHT;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<18;
                }                   
            break;              
            case 19:
                if(PRO_dev.stat_flag&(1<19))//
                { 
                    PRO_dev.DZ&=~LIGHT_C;
                    main_to_slave();  
                    PRO_dev.stat_flag&=~(1<19);
                }
                else
                {     
                    PRO_dev.DZ|=LIGHT_C;
                    main_to_slave();
                    PRO_dev.stat_flag|=1<19;
                }                  
            break; 
            case 20:case 21:case 22:case 23:case 11:case 7:   
            break;
        }
    }
    PRO_dev.notsame_flag=0;
}






