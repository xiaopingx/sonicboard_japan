#include "hardware.h"
#include "sys.h"
#include "mini51eeprom.h"
#include "delay.h"
#include "timer.h"
#include "string.h"
void IOCUP_Init(void)
{	
	/*
	1、LED指示灯控制脚：U10-7:P3.4								输出
	2、与治疗机单片机的连接
		 超声波单片机   	治疗机             代号
		  U10-8  (P35)     	  P11	  ==>	SONIC_BIT3			输出  ==>指示是否加满
		  U10-16 (P07)		  P42	  ==> 	SONIC_BIT2			输出  ==>指示杯子是否存在
		  U10-13 (P25)		  P14	  ==> 	SONIC_BIT1			输入  ==>命令1
		  U10-12 (P24)		  P13	  ==> 	SONIC_BIT0			输入  ==>命令2
	3、超声波控制
		检测水杯是否存在	U11-1    ==> P1.2					输入
		脉冲触发(Trig)		U11-18	 ==> P0.5					输出
		电平检测(Echo)		U11-6	 ==> P3.2					输入

	*/
	/*****************************************/
	//LED
	P3->PMD &= ~(3 << 2*4);		//P34 
	P3->PMD |= 1 << 2*4;
	/*****************************************/
	P0->PMD &= ~(3 << 2*7); 	//P07
	P0->PMD |= 1 << 2*7;	
	P0->DOUT &= ~(1 << 7);
	P2->PMD &= ~(0XF << 2*4);	//P24 P25
	//水杯是否存在
	P3->PMD &= ~(3 << 5*2);		//P35
	P3->PMD |= 1 << 5*2;
	P3->DOUT &= ~(1 << 5);
	/*****************************************/
	P1->PMD &= ~(3 << 2*2);		//P12
    
//    P1->PMD &= ~(3 << 4*2);     //P14
//    P1->PMD |= 1 << 4*2;
//    P1->DOUT &= ~(1 << 4);
    P0->PMD &= ~(3 << 5*2);     //P05
    P0->PMD |= 1 << 5*2;
    P0->DOUT &= ~(1 << 5);
	/**********************************/
	//P32 ==> 定时器脉冲宽度检测(T0EX)
    SYS->P3_MFP |= 1 << (2 + 8);
    SYS->P3_MFP &= ~(1 << 2);
    CLK->CLKSEL1 &= ~(7 << 8);      //清除定时器时钟源设置
    CLK->CLKSEL1 |= 2 << 8;         //把定时器0时钟源设置为HCLK      
    CLK->APBCLK |= 1 << 2;          //打开定时器0时钟
    TIMER0->TCSR&=~(3<<27);
    TIMER0->TCSR|=1<<27;            //周期模式 
    TIMER0->TCSR |= 22;             //预分频器设置
    TIMER0->TCMPR = 1000*1000;      //计数周期
    TIMER0->TEXCON |= 1 << 8;       //Triger-Counting
    TIMER0->TEXCON |= 1 << 6;       //Enable De-bounce
    TIMER0->TEXCON |= 1 << 5;       //Enable EXT interrupt
    /*********************************/
    TIMER0->TEXCON &= ~(1 << 4);
    TIMER0->TEXCON |= 3 << 1;       //记录第一次上升和第二次下降的脉冲宽度
    /*********************************/
    TIMER0_start_cap();
    MY_NVIC_Init(TMR0_IRQn,0);
    TIMER0->TCSR |= 1<<30;
}
void TIMER0_start_cap(void)
{
    TIMER0->TEXCON |= 1 << 3;
}
void TIMER0_stop_cap(void)
{
    TIMER0->TEXCON &= ~(1 << 3);
}

s8 TIMER0_plus_width_read(u32 *value)
{
    s8 ret = -1;
    u32 temp = 0;
    temp = TIMER0->TCAP;
    if(temp > 0){
        ret = 1;
        *value = temp;
    }
    return ret;
}
void Sonic_cappre_finish_clear(void)
{
    TIMEPARAM.pre.capfinish = 0;
}
void Sonic_cappre_clear(void)
{
    memset(&TIMEPARAM.pre,0,sizeof(TIMEPARAM.pre));
}
void Sonic_timeout_clear(void)
{
    memset(&TIMEPARAM.timeout,0,sizeof(TIMEPARAM.timeout));
}
void Sonic_cappre_filter(void)
{
    u32 sum = 0,average = 0;
    u32 buf[SECOND_FILTER_TIMES] = {0},t = 0;
    u8 i = 0,j = 0;
    static u32 first[FIRST_FILTER_TIMES] = {0},first_index = 0;
    u32 first_buf[FIRST_FILTER_TIMES] = {0},first_out = 0;
    static u16 overtime = 0;
    if(TIMEPARAM.pre.capfinish == 0)
        return;
    if(TIMEPARAM.pre.filtered)
        return;
    if(TIMEPARAM.pre.precap < 20 || TIMEPARAM.pre.precap > 1000){
        overtime ++;
        if(overtime >= 50){
            overtime = 0;
            Sonic_cappre_clear();
            TIMEPARAM.level.level = SONIC_ERROR_VALUE;
        }
        TIMEPARAM.pre.filtered = 1;
        return; 
    }
    overtime = 0;  
    /****************************************/
    //第一次过滤
    if(first_index < FIRST_FILTER_TIMES){
        first[first_index] = TIMEPARAM.pre.precap;
        first_index ++;
        return;
    }else{
        for(i = 0;i < FIRST_FILTER_TIMES;i ++)
            first_buf[i] = first[first_index];
        for(i = 0;i < FIRST_FILTER_TIMES-1;i ++){
            for(j = i + 1;j < FIRST_FILTER_TIMES;j ++){
                if(first_buf[i] > first_buf[j]){
                    t = first_buf[i];first_buf[i] = first_buf[j];first_buf[j] = t;
                }
            }
        } 
    }
    first_out = first_buf[FIRST_FILTER_TIMES/2 + 1];
    /****************************************/
    //第二次过滤
    if(TIMEPARAM.pre.index < SECOND_FILTER_TIMES){
        i = TIMEPARAM.pre.index;
        TIMEPARAM.pre.count[i] = first_out;
        i++;
        TIMEPARAM.pre.index = i;
        for(j = 0;j < i;j ++)
            sum += TIMEPARAM.pre.count[j];
        average = sum / i;
        TIMEPARAM.level.level = average;
        return;
    }
    for(i = 0;i < SECOND_FILTER_TIMES - 1;i ++)
        TIMEPARAM.pre.count[i] = TIMEPARAM.pre.count[i + 1];
    TIMEPARAM.pre.count[i] =  TIMEPARAM.pre.precap;
    for(i = 0;i < SECOND_FILTER_TIMES;i ++)
        buf[i] = TIMEPARAM.pre.count[i];
    for(i = 0;i < SECOND_FILTER_TIMES-1;i ++){
        for(j = i+1;j < SECOND_FILTER_TIMES;j ++){
            if(buf[i] > buf[j]){
                t = buf[i];buf[i] = buf[j];buf[j] = t;
            }
        }
    }
    for(i = 6;i < 6 + 8; i++)
        sum += buf[i];
    average = sum >> 3;
    TIMEPARAM.level.level = average;
    TIMEPARAM.pre.filtered = 1;
    /******************************************************/    
}
//超过了时间
void Sonic_timeout(void)
{
    if(TIMEPARAM.pre.capfinish == 1){
        TIMEPARAM.pre.capfinish = 0;
        Sonic_timeout_clear();
        return;
    }
    TIMEPARAM.timeout.counter ++;
    if(TIMEPARAM.timeout.counter > 50){
        TIMEPARAM.timeout.counter = 0;
        Sonic_cappre_clear();
        TIMEPARAM.level.level = SONIC_ERROR_VALUE;
    }
}
void TMR0_IRQHandler(void)
{ 
    u32 value = 0;
    if(TIMER0->TEXISR & (1 << 0))
    {
        if(TIMER0_plus_width_read(&value) < 0){//Raise
            TIMEPARAM.pre.capfinish = 0;
        }else{//fail
            TIMEPARAM.pre.precap = value;
            TIMEPARAM.pre.capfinish = 1;
        }
    }
    TIMER0->TEXISR |= 1 << 0;
}
/***************************************/
TypeDef_FLAG FLAG = 
{
	0,//ywnocup;      //液位没到标志
    0,//setcup;   //设置加水标志
    0,//sendplus; //是否发出电平标志 
	0,//cupsetted;
};
Typedef_TIMEPARAM TIMEPARAM = 
{
	{0},
    {0},
    {0},
};
/**********************************************************************/
Typedef_SONIC SONIC =
{
	{
		0,//cmd
		{
			{0},//counter_bit0
			{0},//counter_bit1
			{0},//bit
		},//counter
		
	},//cmd
	{
		0,//exist
		{0},//counter
		
	},//physiccup
	{
		0,//send_cmd
	},
	{
		0,//ocurr
	}//interrupt
	
};
//三个信号检测，包括对治疗机发送过来的命令信号和水杯是否存在的信号
void SONIC_Signal_detect(Typedef_SONIC *sonic)
{
/*********************************************************************/
	//CMD_BIT0信号检测
	if(CMD_CUP_BIT0 == 0){
		SONIC.cmd.detect.counter_bit0[0] ++;
		SONIC.cmd.detect.counter_bit0[1] = 0;
		if(SONIC.cmd.detect.counter_bit0[0] > 500){
			SONIC.cmd.detect.counter_bit0[0] = 0;
			SONIC.cmd.detect.bit[0] = 0;
        }
	}else{
		SONIC.cmd.detect.counter_bit0[1] ++;
		SONIC.cmd.detect.counter_bit0[0] = 0;
		if(SONIC.cmd.detect.counter_bit0[1] > 500){
			SONIC.cmd.detect.counter_bit0[1] = 0;
			SONIC.cmd.detect.bit[0] = 1;
		}
	}
	//CMD_BIT1信号检测
	if(CMD_CUP_BIT1 == 0){
		SONIC.cmd.detect.counter_bit1[0] ++;
		SONIC.cmd.detect.counter_bit1[1] = 0;
		if(SONIC.cmd.detect.counter_bit1[0] > 500){
			SONIC.cmd.detect.counter_bit1[0] = 0;
			SONIC.cmd.detect.bit[1] = 0;
		}
	}else{
		SONIC.cmd.detect.counter_bit1[1] ++;
		SONIC.cmd.detect.counter_bit1[0] = 0;
		if(SONIC.cmd.detect.counter_bit1[1] > 500){
			SONIC.cmd.detect.counter_bit1[1] = 0;
			SONIC.cmd.detect.bit[1] = 1;
		}
	}
/*********************************************************************/	
	
/*********************************************************************/
	//是否存在杯子的信号检测
	if(IS_PHYSIC_CUP_INPUT == 0){
		SONIC.physiccup.counter[0] ++;
		SONIC.physiccup.counter[1] = 0;
		if(SONIC.physiccup.counter[0] > 80){
			SONIC.physiccup.counter[0] = 0;
			SONIC.physiccup.exist = 1;
		}
	}else{
		SONIC.physiccup.counter[1] ++;
		SONIC.physiccup.counter[0] = 0;
		if(SONIC.physiccup.counter[1] > 80){
			SONIC.physiccup.counter[1] = 0;
			SONIC.physiccup.exist = 0;
		}
	}
/*********************************************************************/
}
void SONIC_get_cmd(Typedef_SONIC *sonic)
{
    u16 static err[4] = {0};
	switch(sonic->cmd.detect.bit[0]){
		case 0:
			if(sonic->cmd.detect.bit[1] == 0){
                err[0] ++;err[1] = 0;
                err[2] = 0;err[3] = 0;
                if(err[0] >= 30){
                    sonic->cmd.cmd = SONIC_CMD_00;
                    err[0] = 0;
                }
            }
			else {
                err[0] = 0;err[1] ++;
                err[2] = 0;err[3] = 0;
                if(err[1] >= 30){
                    sonic->cmd.cmd = SONIC_CMD_10;
                    err[1] = 0;
                }
            }
		break;
		case 1:
			if(sonic->cmd.detect.bit[1] == 0){
                err[0] = 0;err[1] = 0;
                err[2] ++;err[3] = 0;
                if(err[2] >= 30){
                    sonic->cmd.cmd = SONIC_CMD_01;
                    err[2] = 0;
                } 
            }
			else {
                err[0] = 0;err[1] = 0;
                err[2] = 0;err[3] ++;
                if(err[3] >= 30){
                    sonic->cmd.cmd = SONIC_CMD_11;
                    err[3] = 0;
                }
            }
		break;
	}
}
void SONIC_cmd_parse(Typedef_SONIC *sonic)
{
	SONIC_get_cmd(sonic);
	/*********************************************/
	/*********************************************/
	switch(sonic->cmd.cmd){
		case SONIC_CMD_00://00为不工作状态,即为一般的定时模式
			FLAG.setcup = 0;
			FLAG.sendplus = 0;
			FLAG.cupsetted = 0;
		break;
		case SONIC_CMD_01://平时一般性的检测，主要检测是否需要加水等等
			FLAG.cupsetted = 0;
			FLAG.setcup = 0;	
		
			/**************************************/
// 			if(SONIC.physiccup.exist && TIMEPARAM.level.level != SONIC_ERROR_VALUE){
// 				FLAG.sendplus = 1;  //液位高度检测标志
// 			}else
// 				FLAG.sendplus = 0;
			/**************************************/
        
        
		//前期调试时暂时设置为1
			FLAG.sendplus = 1;
		break;
		
		case SONIC_CMD_11://强制加水状态,设置也为10
			FLAG.cupsetted = 0;//控制在设置完成时只进行一次数据保存
			FLAG.setcup = 0;
		
		/*********************************************/
		//只有杯子存在时才能进行强制加水
// 			if(SONIC.physiccup.exist && TIMEPARAM.level.level != SONIC_ERROR_VALUE){
// 				FLAG.sendplus = 1;  //液位高度检测标志
// 			}else
// 				FLAG.sendplus = 0;
		/*********************************************/
        
        
		//前期调试时暂时设置为1
			FLAG.sendplus = 1;
		break;
		
		case SONIC_CMD_10://设置完成状态，可以对参数进行保存
			if(SONIC.physiccup.exist == 1){//若水杯不存在，保存无效
				if(FLAG.cupsetted == 0)
				{
					FLAG.setcup=1;
					FLAG.cupsetted = 1;
				}
			}else{
				FLAG.setcup = 0;
				FLAG.cupsetted = 0;
			}
		break;
	}
}
void Sonic_cupset(void)
{
    u32 cupcount = TIMEPARAM.level.level;
    if(FLAG.setcup){
        if(SONIC.physiccup.exist && FLAG.cupsetted){
            DATAFLASH.save(OFFSET_CUP,cupcount,DF_PAGE1);
            TIMEPARAM.level.cup = cupcount;
            FLAG.setcup = 0; 
        }
    }
}
void SONIC_send_cmd(Typedef_SONIC *sonic)
{
	/******************************************************/
	//判断是否加水的条件
	if(FLAG.level_get == 0 && SONIC.physiccup.exist)//液位没到,并且有杯子存在的情况通知治疗机加水
		SONIC.send.send_cmd = 1;//表示液位没有到位，可以加水
	else
		SONIC.send.send_cmd = 0;//水杯不存在或液位已到位，不需加水
	/******************************************************/
	//发送是否加水信息到治疗机控制板
	if(SONIC.send.send_cmd)
		OUTPUT_CUP_FULL = 1;
	else
		OUTPUT_CUP_FULL = 0;
	//是否存在水杯输出
	if(SONIC.physiccup.exist){
        if(TIMEPARAM.level.level == SONIC_ERROR_VALUE){
            if(sonic->cmd.cmd == SONIC_CMD_00)
                OUTPUT_CUP_EXIST = 1;
            else
                OUTPUT_CUP_EXIST = 0;
        }else OUTPUT_CUP_EXIST = 1;
    }
	else
		OUTPUT_CUP_EXIST = 0;
}
void SONIC_overall_control(void)
{
    if(TIMEPARAM.level.level < TIMEPARAM.level.cup + 30){
        FLAG.level_get = 1;
    }else FLAG.level_get = 0;
}

/**********************************************************************/

/**********************************************************************/
void DATAFLASH_init(u32 dfba_startaddr)
{
    DATAFLASH.init(dfba_startaddr);
    if(FLAGVALUE != DATAFLASH.read(OFFSET_FLAG,DF_PAGE1))
    {
        TIMEPARAM.level.cup = 400;
        /**********************************************************/
        DATAFLASH.erase(DF_PAGE1);
        DATAFLASH.write(DF_PAGE1+OFFSET_CUP,TIMEPARAM.level.cup);
        /***************************************************************/
        DATAFLASH.write(DF_PAGE1+OFFSET_FLAG,FLAGVALUE);     
    }
    else
    {
        TIMEPARAM.level.cup = DATAFLASH.read(OFFSET_CUP,DF_PAGE1);
        /**************************************************************/
    }
}
/**********************************************************************/

