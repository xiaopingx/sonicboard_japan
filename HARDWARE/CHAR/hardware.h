#ifndef __HARDWARE_CHAR_H
#define __HARDWARE_CHAR_H
#include "sys.h"
#include "gpio.h"
#define BEEP P06
/*--------I/O设置----*/
#define LED   			        P34        
#define INBCUP                  P34
#define TRIG                    P51
/******************************************************/
//治疗机与超声波板的通信
#define CMD_CUP_BIT3			P35
#define CMD_CUP_BIT2   	 		P07           
#define CMD_CUP_BIT1  			P25 
#define CMD_CUP_BIT0   			P24 
/******************************************************/
#define IS_PHYSIC_CUP_INPUT		P12//杯子是否存在检测
#define IS_FULL_LEVLE_INPUT		P15//水位是否加到预定值
/******************************************************/

/******************************************************/
#define OUTPUT_CUP_EXIST	CMD_CUP_BIT3
#define OUTPUT_CUP_FULL		CMD_CUP_BIT2
#define CUP_EXIST			0
#define CUP_DOESNT_EXIST	1
#define CUP_ADD_WATER		1
#define CUP_STOP_WATER		0
/******************************************************/
//在FLASH中的偏移位置
#define OFFSET_CUP              0X00
#define OFFSET_FLAG             100
#define FLAGVALUE             	0X9A113A02
/******************************************************/
#define SONIC_ERROR_VALUE       0XBBBBAAAA
#define FIRST_FILTER_TIMES      11
#define SECOND_FILTER_TIMES     20
/******************************************************/

/******************************************************/

/******************************************************/
/******************************************************/
typedef struct
{   
	u8 level_get;      //液位没到标志
	u8 setcup;   //设置加水标志
	u8 sendplus; //是否发出电平标志 
//    u8 set;
	u8 cupsetted;
    /************************/
    /************************/
}TypeDef_FLAG;
extern TypeDef_FLAG FLAG;
/********************************************/
typedef struct
{
    struct filter{
        u32 count[SECOND_FILTER_TIMES];
        u32 precap;
        u8 index;
        u8 capfinish;
        u8 filtered:1;
    }pre;
    struct overtime{
        u32 counter;
        u8 flag;
    }timeout;
    struct level{
        u32 cup; 
        u32 level;
    }level;
}Typedef_TIMEPARAM;
extern Typedef_TIMEPARAM TIMEPARAM;
/********************************************/
#define SONIC_CMD_00		0
#define SONIC_CMD_01		1
#define SONIC_CMD_10		2
#define SONIC_CMD_11		3

#define CUP_STATE_TIME_MANNUL_SETING	SONIC_CMD_00
#define CUP_STATE_SONIC					SONIC_CMD_01
#define CUP_STATE_SETING				SONIC_CMD_11
#define CUP_STATE_SET_FINISH			SONIC_CMD_10
typedef struct
{
	struct{
		u8 cmd;
		struct{
			u16 counter_bit0[2];
			u16 counter_bit1[2];
			u8 bit[2];
			u16 counter_bit1bit0[4];
		}detect;
	}cmd;
	struct{
		u8 exist:1;
		u16 counter[2];
		u8 getlevel_delay:1;
		u16 getlevel_delaycount;
	}physiccup;
	struct{
		u8 send_cmd;
	}send;
	struct{
		u8 ocurr:1;
	}interrupt;
	
}Typedef_SONIC;
extern Typedef_SONIC SONIC;





/********************************************************/
void SONIC_Signal_detect(Typedef_SONIC *sonic);
void SONIC_send_cmd(Typedef_SONIC *sonic);
void SONIC_cmd_parse(Typedef_SONIC *sonic);
void SONIC_get_cmd(Typedef_SONIC *sonic);
void IOCUP_Init(void);
void YWJCYES(void);
void DATAFLASH_init(u32 dfba_startaddr);

void Sonic_cappre_filter(void);
void TIMER0_start_cap(void);
void TIMER0_stop_cap(void);
void Sonic_cappre_finish_clear(void);
void SONIC_overall_control(void);
void Sonic_timeout(void);
void Sonic_cupset(void);

#endif

